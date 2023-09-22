#include "PCAM.h"

class input;
bool PCAM::signalExit = false;

void PCAM::setSignal() {
    signal(SIGINT,this->signalHandler);
}
void PCAM::signalHandler(int signum) {
    signalExit = true;
}


// PCAM constructor, read config, enumerate cameras and open first camera found
PCAM::PCAM(const char * config) {

    signalExit = false;

    //ctor
    frameCounter = 0;
    validCamera = false;
    validConfig = false;
    useOverflowDataPath = false;
    rawFileWriter = NULL;
    
    rawFrame = new CameraImage();
    
    bumpControl = NULL;

    // Read the config file
    string configName(config);
    readConfigFile(configName);
	
	if (validConfig) {
		
    
		// Call the start script with data directory path
		string cmd = startScriptPath + " " + dataDir->toString() + " &";
		log->info("Running start script: " + cmd);
		int r = system(cmd.c_str());
		if (r) {};

		// if in simulation mode, build a set of images with random ellipse rois
		if (simulatorMode) {
			log->info("Generating simulation images...");
			cam = new SimCamera(cfg);
			cam->updateDataDir(dataDir->toString());
			((SimCamera*)cam)->generateImages();
		}
		//otherwise create the physical camera here
		else {
			cam = new MVIMPACTCamera(cfg);
		}

		setSignal();
		// If we made it here then we have a valid camera
		validCamera = true;
		
		int dsWidth = rawImgWidth/downsampleFactor;
		int dsHeight = rawImgHeight/downsampleFactor;
		
		displayImg = Mat( rawImgWidth, rawImgHeight, CV_8UC1);
		
	}
}

PCAM::~PCAM() {
    
}

bool PCAM::haveValidCamera() {
    return validCamera || simulatorMode;
}

bool PCAM::haveValidConfig() {
    return validConfig;
}

// Print the flycapture build and PCAM build info
void PCAM::PrintBuildInfo()
{

    // Print camera info here

    char timeStamp[512];
    sprintf( timeStamp, "PCAM build date: %s %s", __DATE__, __TIME__ );
	if (validConfig)
		log->info( timeStamp );
	else
		printf("%s",timeStamp);

}

// Print the Camera information
void PCAM::PrintCameraInfo()
{
    //char output[512];
    log->info("Camera Info");

}

void PCAM::saveRawFrame() {
    
    char filename[256];
    
    if (!rawFrame->bAllocated)
        return;
    
    // Convert the pixel data from Flycapture to opencv
    int cvFmt = CV_16UC1;
    if (this->pixelFormat == 2) {
        cvFmt = CV_16UC1;
    
    }
    else {
        cvFmt = CV_8UC1;
    }
    
    unsigned int rowBytes = (double)rawFrame->getDataSizeInBytes()/(double)rawFrame->height;
    Mat opencvImg = Mat(rawFrame->height, 
                    rawFrame->width, 
                    cvFmt, 
                    rawFrame->getDataPointer(),
                    rowBytes );
    
    cv::demosaicing(opencvImg,opencvImg,cv::COLOR_BayerRG2RGB);
    
    // Create the file name for the ROI including time, and bounding box info
    sprintf( filename, "%s-%llu-%llu-%06d-%03d-%03d.tif",
        cameraName.c_str(), rawFrame->systemMicros, rawFrame->cameraMicros, rawFrame->frameNumber, rawFrame->position, rawFrame->flashtype);
    Poco::Path outputFile(dataDir->toString(),filename);
    
    
    
    log->info(outputFile.toString());
    
    // Write the original pixels in the BB from the raw image to the file
    imwrite(outputFile.toString().c_str(),opencvImg);
    
}

int PCAM::RunCameraGui()
{
    
    // Abort if there is no valid camera or no valid configuration
    if (!validCamera && !simulatorMode)
    {
        log->fatal("No Valid Camera Is Available....Aborting");
        return -1;
    }

    // Abort if there is no valid configuration
    if (!validConfig)
    {
        log->fatal("No Valid Configuration Is Available....Aborting");
        return -1;
    }
    
    // Start any serial devices
    for (unsigned int i=0;i < serialDevices.size();i++) {
        stringstream outBuffer;
        outBuffer << "Starting serial device: " << serialDevices[i]->name << "," << serialDevices[i]->comPort << "," << serialDevices[i]->baud;
        log->info(outBuffer.str());
        outBuffer.str("");
        serialDevices[i]->Start(dataDir->toString().c_str());
        string errMsg = serialDevices[i]->getErrorMessages();
        if (errMsg.length() > 0)
            log->error(errMsg);
    }
    
    // Start the raw file saving thread first to get base directory    
    if (saveRawImages) {
        rawFileWriter = new RawCameraFileWriter(this->cfg);
        rawFileWriter->updateDataDir(dataDir->toString());
        log->info("Created raw camera file writer.");
        rawFileWriter->startThread(true, false);
    }
    
    // Setup the GUI
    CameraGUI cameraGUI(guiConfigPath.c_str());
    
    // Start the camera thread
    cam->addHandler(this);
    cam->startAcquisition();
    
    log->info("waiting for camera to start...");
    while (!((MVIMPACTCamera*)cam)->isRunning()) {
        usleep(100000);
    }
    
    cameraGUI.setControl(bumpControl);
    cameraGUI.setCamera(cam);
    cameraGUI.applyConfig();
    


    try {
        generalTimer.reset();
        generalTimer.start();
        backupTimer.reset();
        backupTimer.start();
        roisPerMinuteTimer.reset();
        roisPerMinuteTimer.start();

        Poco::Stopwatch simTimer;
        Poco::Stopwatch enqTimer;
        Poco::Stopwatch logTimer;
        simTimer.reset();
        simTimer.start();

        logTimer.reset();
        logTimer.start();

        float fpsEstimate = 0.0;
        int lastFrameCount = frameCounter;
        int roiSubDirCount = 0;
        
        bool shouldHalt = false;

        while (generalTimer.elapsedSeconds() < cameraMaxRunTime) {
        
            if (signalExit) 
                break;

            bool shouldBreak = false;
            string input = "";
            if (kbhit_no_buf()) { 
                getline(std::cin,input);
                
                log->fileNotice(input);
                if (toLower(input) == stopCmd) {
                    shouldBreak = true;
                    break;
                }
                if (toLower(input) == haltCmd) {
                    shouldBreak = true;
                    shouldHalt = true;
                    break;
                }
            } 
			if (shouldBreak)
				break;

            
            // Print status info from all of the processing and saving threads here
            if (logTimer.elapsed() >= logInterval*1000000) {
                logTimer.reset();
                logTimer.start();
                
                fpsEstimate = (frameCounter - lastFrameCount)/logInterval;
                lastFrameCount = frameCounter;
                
                // Update total ROI count
                totalRois = 0;
                for (unsigned int i =0;i < imageProcessors.size();i++) {
                    totalRois += imageProcessors[i]->getTotalRois();
                }
                
                // Don't reset roiSubDirCount
                for (unsigned int i =0;i < imageProcessors.size();i++) {
                    roiSubDirCount += imageProcessors[i]->getRecentRoiCount();
                }
                
                // System status output
                stringstream ss;
                ss << "$" << cameraName << "," << generalTimer.elapsedSeconds() << "," << fpsEstimate << "," << processFrameCounter << "," << frameCounter << ",";
                if (saveRawImages && rawFileWriter != NULL)
                    ss << "$RS," + rawFileWriter->status() + ",";
                if (doRoiProcessing) {
                    for (unsigned int i =0;i < imageProcessors.size();i++) {
                        ss << "$PS" << i << "," << imageProcessors[i]->status();
                        if (i < imageProcessors.size())
                            ss << ",";
                
                    }
                }
                log->info(ss.str());
                
                ss.str("");
                for (unsigned int i = 0; i < serialDevices.size();i++) {
                    if (serialDevices[i]->bNew) {
                        string data = serialDevices[i]->rawData;
                        serialDevices[i]->bNew = false;
                        if (data.at(0) != '$')
                            data = "$" + data;
                        ss << data;
                    }
                }
                log->info(ss.str());
            }
            
            // This updates the GUI and bump_control
            cameraGUI.update(displayImg);
            
            // Check is we should save raw data
            if (cameraGUI.saveRaw && rawFileWriter == NULL) {
                // Create a file writer
                rawFileWriter = new RawCameraFileWriter(this->cfg);
                rawFileWriter->updateDataDir(dataDir->toString());
                log->info("Created raw camera file writer.");
                rawFileWriter->startThread(true, false);
            }
            if (!cameraGUI.saveRaw && saveRawImages && rawFileWriter != NULL) {
                // Close any open file
                rawFileWriter->closeOpenFile();
            }
            // Sync up raw recording
            this->saveRawImages = cameraGUI.saveRaw;
            
            // Save frame if requested
            if (cameraGUI.saveFrame) {
                log->info("Saving one raw frame...");
                this->saveRawFrame();
                cameraGUI.saveFrame = false;
            }
 
            
            if (cameraGUI.quitPressed())
                break;

        }
        if (generalTimer.elapsedSeconds() >= cameraMaxRunTime)
            shouldHalt = true;
			
		

    }
    catch (...){
        log->error("Unexpected Error in main UI loop");
    }
    
    cameraGUI.saveRaw = false;
    if (cameraGUI.saveRaw) {
        // Close any open file
        rawFileWriter->closeOpenFile();
    }
    
    cameraGUI.saveRaw = false;
    saveRawImages = false;
    
    
    cv::destroyAllWindows();
    
    // Stop the acquisition
    cam->stopAcquisition();
    
    // Stop the strobe and trigger
    bumpControl->quickSend("!,STOPCAM");

    // Stop the processing thread after all frames are done
    log->info("Waiting for frames to complete...");
    bool allFinished = false;
    while(!allFinished && saveRawImages)
    {
        if (saveRawImages and rawFileWriter != NULL) {
            if (rawFileWriter->isFull()) {
                allFinished = false;
            }
        }    
        const unsigned int millisecondsToSleep = 100;
        usleep(millisecondsToSleep * 1000);
    }
    log->info("Frames complete.");
    
    if (saveRawImages and rawFileWriter != NULL) {
        rawFileWriter->stopThread();
    }
    
    // Stop any serial devices
    stringstream outBuffer;
    for (unsigned int i=0;i < serialDevices.size();i++) {
        outBuffer << "Stopping serial device: " << serialDevices[i]->name << "," << serialDevices[i]->comPort << "," << serialDevices[i]->baud;
        log->info(outBuffer.str());
        outBuffer.str("");
        serialDevices[i]->Stop();
        string errMsg = serialDevices[i]->getErrorMessages();
        if (errMsg.length() > 0)
            log->error(errMsg);
    }

    // Wait for serial devices to stop
    if (serialDevices.size() > 0) 
        usleep(2000 * 1000);

    // Delete and serial devices
    for (unsigned int i=0;i < serialDevices.size();i++) {
        if (serialDevices[i] != NULL)
            delete serialDevices[i];
    }
        
    log->info("Done.");
    
    return 0;
}


int PCAM::updateImageCount() {
    return imageCnt++;
}

void PCAM::addEntry(DataEntry * pImg) {

    CameraImage * procImg = (CameraImage*)pImg;
    bool enqueued = false;
    
    // Copy to temporary storage
    *rawFrame = *(CameraImage*)pImg;
    
    // Set the current actuator position
    procImg->position = (int)(bumpControl->actuatorPos);
    procImg->flashtype = (int)(bumpControl->triggerType);
    
    frameCounter++;
	
	if (true) {
		// Convert the pixel data from Flycapture to opencv
		int cvFmt = CV_16UC1;
		if (this->pixelFormat == 2) {
			cvFmt = CV_16UC1;
        
        }
		else {
			cvFmt = CV_8UC1;
        }
		
		unsigned int rowBytes = (double)procImg->getDataSizeInBytes()/(double)procImg->height;
		Mat opencvImg = Mat(procImg->height, 
						procImg->width, 
						cvFmt, 
						procImg->getDataPointer(),
						rowBytes );
        //consistent size for display only
        cv::resize(opencvImg, opencvImg, cv::Size(1504, 1504));
        
        opencvImg.copyTo(displayImg);
    }
    
    // Must call this before sending procImg to roi processor
    if (saveRawImages && rawFileWriter != NULL) {
        if (!rawFileWriter->isFull()) {
            
            // Note the copy to a new CameraImage is important as the addEntry method
            // takes pointers and the roi processor may already be using procImg
            CameraImage * rawSaveImg = new CameraImage();
            *rawSaveImg = *procImg;
            rawFileWriter->addEntry(rawSaveImg);
        }
        else {
            log->error("Skipped saving raw image, no memory available to queue...");
        }
    }
    
	bool procFrame = frameCounter % (cameraFrameRate/processFrameRate) == 0;
	
    if (doRoiProcessing && procFrame) {
		
		processFrameCounter++;
    
        // Look for idle processor to queue image
        //procImg.allocateAndCopyData(pImage.GetData(),pImage.GetReceivedDataSize());
        for (unsigned int i=0;i<imageProcessors.size();i++) {
            if (!imageProcessors[i]->isBusy()) {
                imageProcessors[i]->addEntry(procImg);
                enqueued = true;
                break;
            }
        }
        // If no processors are free, enque to the first non full processor
        if (!enqueued) {
            for (unsigned int i=0;i<imageProcessors.size();i++) {
                if (!imageProcessors[i]->isFull()) {
                    imageProcessors[i]->addEntry(procImg);
                    enqueued = true;
                    break;
                }
            }
        }
        // Otherwise log and error message
        if (!enqueued) {
            log->error("Skipped processing image, no memory available to queue...");
            // Delete the image as the processor would normally do this
            delete procImg;
        }
    }
    else {
        // delete the allocated image as the roi processor would norally do this
        delete procImg;
    }
    
}

void PCAM::readConfigFile(string filename) {

    validConfig = false;

    // Open the config file, read and parse lines
    Poco::Path configFile(Poco::Path::current(),filename.c_str());
    cfg = new XMLConfiguration;
    try {
        ((XMLConfiguration*)cfg)->load(configFile.toString());
    }
    catch (Poco::FileNotFoundException e) {
        printf("Config file %s not found.\n",filename.c_str());
        return;
    }
	catch (Poco::XML::SAXParseException e) {
		printf("There was an error parsing the config file. Check the XML syntax.\n");
		return;
	}
	
    try {

        // Get config settings

        // Camera
		cameraFrameRate = cfg->getDouble("Camera.FrameRate",1);

        // Application
        saveRawImages = cfg->getBool("App.SaveRawImages",false);
        doRoiProcessing = cfg->getBool("App.DoROIProcessing",true);
        maxFrames = cfg->getInt("App.MaxFrames",20000);
        serialMaster = cfg->getBool("App.SerialMaster",true);
        logInterval = cfg->getInt("App.LogInterval",8);
        cameraName = cfg->getString("App.CameraName","SPC");
        cameraStartDelay = cfg->getInt("App.CameraStartDelay",60);
        cameraMaxRunTime = cfg->getInt("App.CameraMaxRunTime",600);
		processFrameRate = cfg->getDouble("App.ProcessFrameRate",1);
        quietMode = cfg->getBool("App.QuietMode",false);
        verboseLog = cfg->getBool("App.VerboseLog",false);
        mergeSerialStrings = cfg->getBool("App.MergeSerialStrings",true);
        maxFilesystemPercentFull = cfg->getInt("App.MaxFilesystemPercentFull",95);
        simulatorMode = cfg->getBool("App.SimulatorMode",false);
        simulatorImages = cfg->getInt("App.SimulatorImages",10);
        simulatorImageRois = cfg->getInt("App.SimulatorImageROIs",10);
        shutdownCmd = cfg->getString("App.ShutdownCommand","/home/odroid/odroid_halt 5");
        nImageProcessors = cfg->getInt("App.NImageProcessors",2);
        tarSubDirs = cfg->getBool("App.TarSubDirs",true);
        roisPerDir = cfg->getInt("App.RoisPerDir",500);
        
        startScriptPath = cfg->getString("App.StartScriptPath","/home/spc/pcam_start_script");
        endScriptPath = cfg->getString("App.EndScriptPath","/home/spc/pcam_end_script");

        // Commands
        startCmd = cfg->getString("Command.StartCommand","start");
        stopCmd = cfg->getString("Command.StopCommand","stop");
        haltCmd = cfg->getString("Command.HaltCommand","halt");
        getTimeCmd = cfg->getString("Command.GetTimeCommand","time?");
        setTimeCmd = cfg->getString("Command.SetTimeCommand","settime");
        abortCmd = cfg->getString("Command.AbortCommand","abort");
		binningX = cfg->getInt("Camera.HorzBinning",1);
		binningY = cfg->getInt("Camera.VertBinning",1);
		rawImgWidth = cfg->getInt("Camera.ImageWidth",3376)/binningX;
		rawImgHeight = cfg->getInt("Camera.ImageHeight",2704)/binningY;
		if (cfg->getString("Camera.ImagePixelFormat","BayerRG16") == "BayerRG16")
			pixelFormat = 2;
		else
			pixelFormat = 1;
		downsampleFactor = cfg->getInt("Processing.DownsampleFactor",4);

        // Check status of the root sile system and switch to overflow partition if less than maxFilesystemPercentFull is available
        FILE* df_output = popen(("df -h --output=pcent " + cfg->getString("Storage.DataPath","/data/")).c_str(),"r");
        int rootPercentFull;
        if (df_output == NULL) {
            cout << "Could not get file system percent used." << endl;
        }
        else {
            char buffer[1024];
            char* line = fgets(buffer,sizeof(buffer),df_output);
            line = fgets(buffer,sizeof(buffer),df_output);
            if (line != NULL) {
                sscanf(buffer," %d%%",&rootPercentFull);
            }
            pclose(df_output);
        }

        // Check status of the root sile system and switch to overflow partition if less than maxFilesystemPercentFull is available
        df_output = popen(("df -h --output=pcent " + cfg->getString("Storage.OverflowDataPath","/data/")).c_str(),"r");
        int overflowPercentFull;
        if (df_output == NULL) {
            cout << "Could not get file system percent used." << endl;
        }
        else {
            char buffer[1024];
            char* line = fgets(buffer,sizeof(buffer),df_output);
            line = fgets(buffer,sizeof(buffer),df_output);
            if (line != NULL) {
                sscanf(buffer," %d%%",&overflowPercentFull);
            }
            pclose(df_output);
        }

        if (rootPercentFull > maxFilesystemPercentFull) {
            useOverflowDataPath = true;
        }

        if (useOverflowDataPath && overflowPercentFull > maxFilesystemPercentFull) {
            cout << " No space left on file system, aborting.";
            return;
        }

        // Storage
        if (useOverflowDataPath)
            dataPath = new Poco::Path(cfg->getString("Storage.OverflowDataPath","/data/"));
        else
            dataPath = new Poco::Path(cfg->getString("Storage.DataPath","/data/"));
        dataPath->makeAbsolute();
        dataPath->makeDirectory();
        dataPathFile = new Poco::File(dataPath->toString());
        backupData = cfg->getBool("Storage.BackupData",true);
        backupDataPath = cfg->getString("Storage.BackupDataPath");
        backupPeriod = cfg->getInt("Storage.BackupPeriod",1);
        
        guiConfigPath = cfg->getString("App.GUIconfigPath","./gui_config.xml");

        // Create directories if needed
        if (!dataPathFile->exists())
            dataPathFile->createDirectories();

        // Create a new directory using the system time in seconds as the name
        // Get the time
        unsigned long int sec= time(NULL);
        char timeBuf[80];
        sprintf(timeBuf,"%u",(unsigned int)sec);
        dataDir = new Poco::Path(dataPath->toString());
        dataDir->makeAbsolute();
        dataDir->makeDirectory();
        dataDir->pushDirectory(timeBuf);
        dataDirFile = new Poco::File(dataDir->toString());
        if (!dataDirFile->exists())
            dataDirFile->createDirectories();
        
        // Create shrink file dir if requested
        saveShrink = cfg->getBool("SaveShrinkFiles",true);
        if (saveShrink) {
            shrinkDir = new Poco::Path(dataDir->toString());
            shrinkDir->pushDirectory(cfg->getString("Storage.ShrinkDir","shrink"));
            shrinkDirFile = new Poco::File(shrinkDir->toString());
            if (!shrinkDirFile->exists())
                shrinkDirFile->createDirectories();
            cout << shrinkDir->toString() << endl;
        }

        // Setup logging
        Poco::Path outputLog(dataDir->toString(),cameraName+"-system.log");
        log = new Log(outputLog.toString(),quietMode,verboseLog);

        // Log file system fullness
        stringstream ss;
        ss << "Root filesystem is " << rootPercentFull << "% full.";
        log->info(ss.str());
        ss.str("");
        ss << "Overflow filesystem is " << overflowPercentFull << "% full.";
        log->info(ss.str());

        // Serial Devices
        AbstractConfiguration::Keys keys;
        cfg->keys("SerialDevices",keys);
        string dName, dPort, dDisplay, dTrim;
        int dBaud;
        stringstream keyName;
        for (unsigned int i = 0; i < keys.size(); i ++ ){
            if (keys[i] == "SerialDevice") {
                // Get the settings for the serial device
                keyName.str("");
                keyName << "SerialDevices.SerialDevice[" << i << "][@name]";
                dName = cfg->getString(keyName.str(),"");
                keyName.str("");
                keyName << "SerialDevices.SerialDevice[" << i << "][@port]";
                dPort = cfg->getString(keyName.str(),"");
                keyName.str("");
                keyName << "SerialDevices.SerialDevice[" << i << "][@baud]";
                sscanf(cfg->getString(keyName.str(),"").c_str(),"%d",&dBaud);
                keyName.str("");
                keyName << "SerialDevices.SerialDevice[" << i << "][@display]";
                dDisplay = cfg->getString(keyName.str(),"");
                keyName.str("");
                keyName << "SerialDevices.SerialDevice[" << i << "][@trim]";
                dTrim = cfg->getString(keyName.str(),"");


                // Switch on the name and create the objects
                /*if (dName == "SPCIB") {
                    serialDevices.push_back(new SPCIB(dName,dPort,dBaud,dDisplay=="Y" || dDisplay=="y",dTrim=="Y" || dTrim=="y"));
                }
                else if (dName == "SBE39") {
                    serialDevices.push_back(new SBE39(dName,dPort,dBaud,dDisplay=="Y" || dDisplay=="y",dTrim=="Y" || dTrim=="y"));
                }
                else if (dName == "SBE49") {
                    serialDevices.push_back(new SBE49(dName,dPort,dBaud,dDisplay=="Y" || dDisplay=="y",dTrim=="Y" || dTrim=="y"));
                }
                else if (dName == "SBE50") {
                    serialDevices.push_back(new SBE49(dName,dPort,dBaud,dDisplay=="Y" || dDisplay=="y",dTrim=="Y" || dTrim=="y"));
                }
                else if (dName == "PA200") {
                    serialDevices.push_back(new SBE50(dName,dPort,dBaud,dDisplay=="Y" || dDisplay=="y",dTrim=="Y" || dTrim=="y"));
                }
                else if (dName == "SPCCBSerial") {
                    serialDevices.push_back(new SPCCBSerial(dName,dPort,dBaud,dDisplay=="Y" || dDisplay=="y",dTrim=="Y" || dTrim=="y"));
                }
                else if (dName == "SPCSerial") {
                    serialDevices.push_back(new SPCSerial(dName,dPort,dBaud,dDisplay=="Y" || dDisplay=="y",dTrim=="Y" || dTrim=="y"));
                }*/
                if (dName == "BUMPControl") {
                    bumpControl = new BUMPControl(dName,dPort,dBaud,dDisplay=="Y" || dDisplay=="y",dTrim=="Y" || dTrim=="y");
                    serialDevices.push_back(bumpControl);
                }
                else {
                    log->error("Serial Device: " + dName + " not recognizied or supported");
                }

            }
        }

        // Save settings to output file
        Poco::Path outputFile(dataDir->toString(),"config.xml");
        ((XMLConfiguration*)cfg)->save(outputFile.toString().c_str());


        log->info("Read config file.");
        log->info("Data Path: " + dataPath->toString());
        log->info("Data Directory: " + dataDir->toString());


    }
    catch (const std::exception& ex) {
        return;
    }
    catch (const std::string& ex) {
        return;
    }
    catch (...) {
    return;
    }

    validConfig = true;

}
    
