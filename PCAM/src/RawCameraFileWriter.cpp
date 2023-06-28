#include "RawCameraFileWriter.h"

RawCameraFileWriter::RawCameraFileWriter() {
	
}

RawCameraFileWriter::RawCameraFileWriter(AbstractConfiguration * cfg)
{
	counter = 0;
	headerLength = 64;
	headerFormat = 2; // Set to 2 to indicate BUMP data with actuator position in each frame header
	saveToDisk = true;
	framesPerFile = cfg->getInt("App.FramesPerFile",128);
	filePrefix = "RAW-" + cfg->getString("App.CameraName");
	threadDelay = 10000;
	maxQueueSize = cfg->getInt("Processing.MaxQueueSize",60);
	
	// Header values
	cameraID = cfg->getInt("Camera.ModelNumber",2124);
	cameraID = cameraID + (cfg->getInt("Camera.SerialNumber",989) << 16);
	exposureTime = (float)cfg->getDouble("Camera.ExposureTime",0.05);
	flashDuration = exposureTime;
	if(cfg->getString("Camera.ImagePixelFormat","BayerRG16") == "BayerRG16")
		pixelFormat = 2;
	if(cfg->getString("Camera.ImagePixelFormat","BayerRG8") == "BayerRG8")
		pixelFormat = 1;
	gain = cfg->getDouble("Camera.Gain",0.0);
	redGain = cfg->getDouble("Camera.RedChannelGain",1.3);
	blueGain = cfg->getDouble("Camera.BlueChannelGain",2.4);
	horzOffset = cfg->getInt("Camera.OffsetX");
	vertOffset = cfg->getInt("Camera.OffsetY");
	rawImgWidth = cfg->getInt("Camera.ImageWidth",3376);
	rawImgHeight = cfg->getInt("Camera.ImageHeight",2704);
	horzBinning = cfg->getInt("Camera.HorzBinning",1);
	vertBinning = cfg->getInt("Camera.VertBinning",1);
	binningMode = 0;
	if (cfg->getBool("Camera.Strobe1Enable",true)) {
		illuminationType = 0;
		flashDelay = cfg->getDouble("Camera.Strobe1Delay",0.0);
	}
	else {
		illuminationType = 1;
		flashDelay = cfg->getDouble("Camera.Strobe2Delay",0.0);
	}
	
	for (int i=0;i<4;i++)
		resvd[i] = 0;
		
	// Setup for periodic video recording
	period = cfg->getInt("App.VideoPeriod",1200);
	recording = false;
	recordingTimer.reset();
	recordingTimer.start();

	
	
}

void RawCameraFileWriter::processDataEntry(DataEntry* entry) {

	if (period != 0 && recordingTimer.elapsedSeconds() > period) {
		recordingTimer.reset();
		recordingTimer.start();
		recording = true;
	}
	
	if (!saveToDisk) {
		delete entry;
		return;
	}
	if (!recording) {
		delete entry;
		return;
	}
	if (!outputFile.is_open())
		openNewFile();
	entry->writeToFile(outputFile);
	delete entry;
	counter++;
	if (counter % framesPerFile == 0) {
		outputFile.close();
		recording = false;
	}
}


RawCameraFileWriter::~RawCameraFileWriter(void)
{
	if (outputFile.is_open())
		outputFile.close();
}

void RawCameraFileWriter::openNewFile() {
	stringstream ss;
	unsigned long long sysTimeMicros = Timestamp().epochMicroseconds();
	if (!saveToDisk)
		return;

	// Close last output file if open
	if (outputFile.is_open())
		outputFile.close();

	ss << setw(20) << setfill('0') << sysTimeMicros << "-" << setw(5) << setfill ('0') << counter;
	Poco::Path outputFilePath(dataDir,filePrefix + "-" + ss.str() + ".bin");
	
	if (isThreadRunning())
		lock();
	outputFile.open(outputFilePath.toString().c_str(),ios::out | ios::binary);
	if (outputFile.bad())
		cout << "Could not open output file." << endl;
	// Write file header here
	if (!outputFile.bad()) {
		//cout << "Writing data: " << sizeInBytes << endl;
		outputFile.write((const char *)&headerLength,sizeof(headerLength));
		outputFile.write((const char *)&headerFormat,sizeof(headerFormat));
		outputFile.write((const char *)&cameraID,sizeof(cameraID));
		outputFile.write((const char *)&pixelFormat,sizeof(pixelFormat));
		outputFile.write((const char *)&illuminationType,sizeof(illuminationType));
		outputFile.write((const char *)&flashDuration,sizeof(flashDuration));
		outputFile.write((const char *)&flashDelay,sizeof(flashDelay));
		outputFile.write((const char *)&exposureTime,sizeof(exposureTime));
		outputFile.write((const char *)&gain,sizeof(gain));
		outputFile.write((const char *)&redGain,sizeof(redGain));
		outputFile.write((const char *)&blueGain,sizeof(blueGain));
		outputFile.write((const char *)&vertOffset,sizeof(vertOffset));
		outputFile.write((const char *)&horzOffset,sizeof(horzOffset));
		outputFile.write((const char *)&vertBinning,sizeof(vertBinning));
		outputFile.write((const char *)&horzBinning,sizeof(horzBinning));
		outputFile.write((const char *)&binningMode,sizeof(binningMode));
		outputFile.write((const char *)&rawImgHeight,sizeof(rawImgHeight));
		outputFile.write((const char *)&rawImgWidth,sizeof(rawImgWidth));
		outputFile.write((const char *)&resvd,sizeof(resvd));

	}
	if (isThreadRunning())
		unlock();
}