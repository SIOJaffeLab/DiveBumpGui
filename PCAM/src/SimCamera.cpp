#include "SimCamera.h"

SimCamera::SimCamera()
{
	
}

SimCamera::~SimCamera() {
	
}

//-----------------------------------------------------------------------------
SimCamera::SimCamera(AbstractConfiguration * cfg)
//-----------------------------------------------------------------------------
{
	
	camFrameRate = cfg->getDouble("Camera.FrameRate",1);
	camExposureTime = cfg->getDouble("Camera.ExposureTime",0.05);
	camGain = cfg->getDouble("Camera.Gain",0.0);
	rawImgOffsetX = cfg->getInt("Camera.OffsetX");
	rawImgOffsetY = cfg->getInt("Camera.OffsetY");
	binningX = cfg->getInt("Camera.HorzBinning",1);
	binningY = cfg->getInt("Camera.VertBinning",1);
	rawImgWidth = cfg->getInt("Camera.ImageWidth",3376)/binningX;
	rawImgHeight = cfg->getInt("Camera.ImageHeight",2704)/binningY;
	memoryChannel = cfg->getInt("Camera.MemoryChannel",1);
	restoreSettings = cfg->getBool("Camera.RestoreSettings",false);
	strobe1Delay = cfg->getDouble("Camera.Strobe1Delay",0.0);
	strobe2Delay = cfg->getDouble("Camera.Strobe2Delay",0.0);
	strobe1Enable = cfg->getBool("Camera.Strobe1Enable",true);
	strobe2Enable = cfg->getBool("Camera.Strobe2Enable",true);
	simulatorMode = cfg->getBool("App.SimulatorMode",false);
	simulatorImages = cfg->getInt("App.SimulatorImages",10);
	simulatorImageRois = cfg->getInt("App.SimulatorImageROIs",10);
	saveSimImages = cfg->getBool("App.SaveSimImages",true);
	
	maxRadius = cfg->getInt("App.SimulatorMaxRadius",100);
	motionSim = cfg->getBool("App.SimulatorMotionSim",true);
	motionSimRate = cfg->getDouble("App.SimulatorMotionRate",10);
	

}

void SimCamera::setCameraName(string name) {
	this->cameraName = name;
}

void SimCamera::addHandler(DataProcessor * handler) {
	this->handler = handler;
}

void SimCamera::startAcquisition() {
	frameNumber = 0;
	this->startThread();
}

void SimCamera::stopAcquisition() {
	this->stopThread();
}

void SimCamera::updateDataDir(string dataDir) {
	this-> dataDir = dataDir;
}

void SimCamera::generateImages() {
		
	char filename[1024];
	
	vector<Point> centers;
	vector<Size> sizes;
	vector<int> angles;
	vector<Point> speeds;
	
	srand(time(NULL));
	
	if (motionSim) {
		for (int j = 0; j < simulatorImageRois; j++ ){
			centers.push_back(Point(rand() % rawImgWidth,rand() % rawImgHeight));
			sizes.push_back(Size(rand() % maxRadius,rand() % maxRadius));
			angles.push_back(rand() % 360);
			speeds.push_back(Point(-motionSimRate + 2*(rand() % (int)motionSimRate)));
		}
		
		for (int i = 0; i < simulatorImages; i++) {
			Mat img = Mat::zeros(rawImgHeight,rawImgWidth,CV_16U);
			for (int j = 0; j < simulatorImageRois; j++ ){
				Point center = centers[j];
				Size s = sizes[j];
				float angle = angles[j];
				ellipse(img,center,s,angle,0,360,Scalar(30*4/0.015625,30*4/0.015625,30*4/0.015625),-1,2);
				
				centers[j] += speeds[j];
				angles[j] += -5 + 2*(rand() % 5);
				speeds[j] += Point(-2 + 2*(rand() % 2),-2 + 2*(rand() % 2));
			}
			Mat noise = img.clone();
			int blurSize = 1 + rand() % 15;
			if (blurSize % 2 != 1)
				blurSize = blurSize + 1;
			GaussianBlur(img,img,Size(blurSize,blurSize),0,0);
			randn(noise,4*4/0.015625,4*4/0.015625);
			img = img + noise;
			simulationImages.push_back(img);
			sprintf(filename,"sim-full-image-%03d.tif",i);
			Poco::Path outputFile(dataDir,filename);
			imwrite(outputFile.toString().c_str(),img);
			
		}
		
	}
	
	else {
	
		for (int i = 0; i < simulatorImages; i++) {
			Mat img = Mat::zeros(rawImgHeight,rawImgWidth,CV_16U);
			for (int j = 0; j < simulatorImageRois; j++ ){
				Point center(rand() % rawImgWidth,rand() % rawImgHeight);
				Size s(rand() % maxRadius,rand() % maxRadius);
				int angle = rand() % 360;
				ellipse(img,center,s,angle,0,360,Scalar(30*4/0.015625,30*4/0.015625,30*4/0.015625),-1,2);
			}
			Mat noise = img.clone();
			int blurSize = 1 + rand() % 15;
			if (blurSize % 2 != 1)
				blurSize = blurSize + 1;
			GaussianBlur(img,img,Size(blurSize,blurSize),0,0);
			randn(noise,4*4/0.015625,4*4/0.015625);
			img = img + noise;
			simulationImages.push_back(img);
			sprintf(filename,"sim-full-image-%03d.tif",i);
			Poco::Path outputFile(dataDir,filename);
			imwrite(outputFile.toString().c_str(),img);
		}
	
	}
}

//-----------------------------------------------------------------------------
void SimCamera::threadedFunction() 
//-----------------------------------------------------------------------------
{

	double threadDelay;
	if (camFrameRate > 0)
		threadDelay = 1000000/camFrameRate;
	else
		threadDelay = 50000;
		
	Poco::Stopwatch timer;
	
	Mat img = Mat::zeros(rawImgHeight,rawImgWidth,CV_16U);
	Mat noise = img.clone();
	int index = 0;
	int dir = 1;
    while( isThreadRunning() )
    {
        
		timer.reset();
		timer.start();
			
		// Get image data and send to callback
				 
		CameraImage * pImage = new CameraImage();
		pImage->width = rawImgWidth;
		pImage->height = rawImgHeight;
		
		pImage->systemUnixTime = (unsigned long long)time(NULL);
		pImage->systemMicros = Timestamp().epochMicroseconds();
		pImage->frameNumber = frameNumber++;
		pImage->bytesPerPixel = 2;
	
		// Determine the index into the simulation based on sim type
		if (motionSim) {
			index += dir;
			if (index >= (int)simulationImages.size()) {
				dir = -1;
				index = (int)(simulationImages.size()-1);
			}
			if (index < 0) {
				dir = 1;
				index = 0;
			}
			
		}	
		else {
			index = rand() % simulationImages.size();
		}
		
		//cout << index << endl;
		
		pImage->allocateAndCopyData((unsigned char*)simulationImages[index].data,rawImgWidth*rawImgHeight*2);
		
		
		//cout << "Test in SimCamera thread." << endl;
		
		// Add the image to the processor handler
		handler->addEntry(pImage);	
		
		int delay = (int)(threadDelay - timer.elapsed())/1000;
		if (delay > 0)
			usleep(delay*1000);

	}

}


