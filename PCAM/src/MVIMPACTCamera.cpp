#include "MVIMPACTCamera.h"


MVIMPACTCamera::MVIMPACTCamera()
{
	
}

MVIMPACTCamera::~MVIMPACTCamera() {
	
}

//-----------------------------------------------------------------------------
MVIMPACTCamera::MVIMPACTCamera(AbstractConfiguration * cfg)
//-----------------------------------------------------------------------------
{
    
	
	camFrameRate = cfg->getDouble("Camera.FrameRate",1);
	camExposureTime = cfg->getDouble("Camera.ExposureTime",0.05);
	camGain = cfg->getDouble("Camera.Gain",0.0);
	rawImgOffsetX = cfg->getInt("Camera.OffsetX");
	rawImgOffsetY = cfg->getInt("Camera.OffsetY");
	rawImgWidth = cfg->getInt("Camera.ImageWidth",3376);
	rawImgHeight = cfg->getInt("Camera.ImageHeight",2704);
	horzDecim = cfg->getInt("Camera.HorzDecimation",1);
	vertDecim = cfg->getInt("Camera.VertDecimation",1);
	horzBinning = cfg->getInt("Camera.HorzBinning",1);
	vertBinning = cfg->getInt("Camera.VertBinning",1);
	memoryChannel = cfg->getInt("Camera.MemoryChannel",1);
	restoreSettings = cfg->getBool("Camera.RestoreSettings",false);
	strobe1Delay = cfg->getDouble("Camera.Strobe1Delay",0.0);
	strobe2Delay = cfg->getDouble("Camera.Strobe2Delay",0.0);
	strobe1Duration = cfg->getDouble("Camera.Strobe1Duration",0.006);
	strobe2Duration = cfg->getDouble("Camera.Strobe2Duration",0.006);
	strobe1Enable = cfg->getBool("Camera.Strobe1Enable",true);
	strobe2Enable = cfg->getBool("Camera.Strobe2Enable",true);
	imgPixelFormat = cfg->getString("Camera.ImagePixelFormat","BayerRG16");
	redGain = cfg->getDouble("Camera.RedChannelGain",1.3);
	blueGain = cfg->getDouble("Camera.BlueChannelGain",2.4);
    
    grabEnable = true;
    
    cameraThreadRunning = false;
    
    this->cfg = cfg;
}

void MVIMPACTCamera::setCameraName(string name) {
	this->cameraName = name;
}

void MVIMPACTCamera::addHandler(DataProcessor * handler) {
	this->handler = handler;
}

void MVIMPACTCamera::startAcquisition() {
	this->startThread();
}

void MVIMPACTCamera::stopAcquisition() {
	cameraClosed = false;
	this->stopThread();
	while(!cameraClosed)
		usleep(10000);
}

void MVIMPACTCamera::updateDataDir(string dataDir) {
	this-> dataDir = dataDir;
}

void MVIMPACTCamera::setGain(int gain) {
    
    mvIMPACT::acquire::GenICam::AnalogControl anlgCtrl (pDev);
    anlgCtrl.gain.write(gain);
    
}

void MVIMPACTCamera::setExposure(int exposureTime) {
    
    //mvIMPACT::acquire::GenICam::AcquisitionControl ac (pDev);
    //ac.exposureTime.write(exposureTime); // in microseconds 
    
}

void MVIMPACTCamera::setBinning(int binning) {
    FunctionInterface fi( pDev );
    mvIMPACT::acquire::GenICam::ImageFormatControl ifc( pDev );
    fi.acquisitionStop();
    ifc.binningHorizontal.write(binning);
    ifc.binningVertical.write(binning);
    fi.acquisitionStart();
}

bool MVIMPACTCamera::isRunning() {
    return cameraThreadRunning;
}

//-----------------------------------------------------------------------------
void MVIMPACTCamera::threadedFunction() 
//-----------------------------------------------------------------------------
{

	int cnt = 0;
	
	DeviceManager devMgr;
    const unsigned int devCnt = devMgr.deviceCount();
    if( devCnt == 0 )
    {
        cout << "No MATRIX VISION device found! Unable to continue!" << endl;
    }
	//pDev = getDeviceFromUserInput( devMgr );
	//pDev = devMgr.getDevice(0);
	pDev = devMgr.getDeviceBySerial("FF"+cfg->getString("Camera.SerialNumber"));
    //pDev = devMgr.getDeviceBySerial("FF000959");
	cout << "Firmware Version: " << pDev->firmwareVersion.read() << endl;
	cout << "Serial Number: " << pDev->serial.read() << endl;
	
	if( pDev )
	{
		// if this device offers the 'GenICam' interface switch it on, as this will
		// allow are better control over GenICam compliant devices
		conditionalSetProperty( pDev->interfaceLayout, dilGenICam );
		// if this device offers a user defined acquisition start/stop behaviour
		// enable it as this allows finer control about the streaming behaviour
		conditionalSetProperty( pDev->acquisitionStartStopBehaviour, assbUser );
	}
	
    try
    {
        pDev->open();
    }
    catch( const ImpactAcquireException& e )
    {
        // this e.g. might happen if the same device is already opened in another process...
        cout << "An error occurred while opening the device " << pDev->serial.read()
             << "(error code: " << e.getErrorCode() << "(" << e.getErrorCodeAsString() << ")). Terminating thread." << endl
             << "Press [ENTER] to end the application..."
             << endl;
		//return;
    }
	
	cout << "Opened camera." << endl;
	
	mvIMPACT::acquire::GenICam::ImageFormatControl ifc( pDev );
	mvIMPACT::acquire::GenICam::AcquisitionControl ac( pDev );
	mvIMPACT::acquire::ImageProcessing imgProc( pDev );
	mvIMPACT::acquire::GenICam::DigitalIOControl io( pDev);
	mvIMPACT::acquire::GenICam::CounterAndTimerControl tmrCtrl (pDev);
	mvIMPACT::acquire::GenICam::AnalogControl anlgCtrl (pDev);
	mvIMPACT::acquire::GenICam::ColorTransformationControl colTrans (pDev);	
	
	cout << "Setting up camera..." << endl;
	
	try {
		
		ifc.decimationHorizontal.write(horzDecim);
		ifc.decimationVertical.write(vertDecim);
		// always set offsets to 0 first before width and height
		// to avoid range errors
        // always set binning to 1 before applying settings to avoid 
        // errors
        ifc.binningHorizontal.write(1);
		ifc.binningVertical.write(1);
		ifc.offsetX.write(0);
		ifc.offsetY.write(0);
		ifc.height.write(rawImgHeight);
		ifc.width.write(rawImgWidth);
		ifc.offsetX.write(rawImgOffsetX);
		ifc.offsetY.write(rawImgOffsetY);
		ifc.pixelFormat.writeS(imgPixelFormat);
		ifc.binningHorizontal.write(horzBinning);
		ifc.binningVertical.write(vertBinning);

		ac.triggerSelector.writeS("FrameStart");
        ac.triggerMode.writeS("On");
        ac.triggerSource.writeS("Line4");
        //ac.acquisitionFrameRateEnable.write(bTrue);
		//ac.acquisitionFrameRate.write(camFrameRate);
		ac.exposureAuto.writeS("Off");
		//ac.exposureTime.write(camExposureTime); // in microseconds 
		
		// Try to turn off all digital gains
		anlgCtrl.gainSelector.writeS("DigitalAll");
		anlgCtrl.balanceRatioSelector.writeS("Red");
		anlgCtrl.balanceRatio.write(1.0);
		anlgCtrl.balanceRatioSelector.writeS("Blue");
		anlgCtrl.balanceRatio.write(1.0);
		anlgCtrl.gain.write(0.0);
		//anlgCtrl.gainAuto.writeS("Off");
		//anlgCtrl.gainAutoBalance.writeS("Off");
		//anlgCtrl.balanceWhiteAuto.writeS("Off");
		//anlgCtrl.blackLevelSelector.writeS("All");
		//anlgCtrl.blackLevel.write(0.0);
		//anlgCtrl.blackLevelAuto.writeS("Off");
		//anlgCtrl.blackLevelAutoBalance.writeS("Off");
		//anlgCtrl.blackLevelSelector.writeS("DigitalAll");
		//anlgCtrl.blackLevel.write(0.0);
		//anlgCtrl.blackLevelAuto.writeS("Off");
		//anlgCtrl.blackLevelAutoBalance.writeS("Off");
		
		
		// Set Analog gains
		anlgCtrl.gainSelector.writeS("AnalogAll");
		anlgCtrl.balanceRatioSelector.writeS("Red");
		anlgCtrl.balanceRatio.write(redGain);
		anlgCtrl.balanceRatioSelector.writeS("Blue");
		anlgCtrl.balanceRatio.write(blueGain);
		anlgCtrl.gain.write(camGain);
		anlgCtrl.gainAuto.writeS("Off");
		//anlgCtrl.gainAutoBalance.writeS("Off");
		anlgCtrl.balanceWhiteAuto.writeS("Off");
		anlgCtrl.blackLevelSelector.writeS("All");
		anlgCtrl.blackLevel.write(0.0);
		anlgCtrl.blackLevelAuto.writeS("Off");
		//anlgCtrl.blackLevelAutoBalance.writeS("Off");
		//anlgCtrl.blackLevelSelector.writeS("DigitalAll");
		//anlgCtrl.blackLevel.write(0.0);
		//anlgCtrl.blackLevelAuto.writeS("Off");
		//anlgCtrl.blackLevelAutoBalance.writeS("Off");
		
		// Turn off all other digital processing
		imgProc.colorProcessing.write(cpmRaw);
		
		
		colTrans.colorTransformationEnable.write(bFalse);

		/*imgProc.LUTEnable.write(bFalse);
		imgProc.colorTwistEnable.write(bFalse);
		imgProc.channelSplitEnable.write(bFalse);
		imgProc.darkCurrentFilterMode.write(dcfmOff);
		imgProc.defectivePixelsFilterMode.write(dpfmOff);
		imgProc.gainOffsetKneeEnable.write(bFalse);
		imgProc.flatFieldFilterMode.write(fffmOff);
		imgProc.formatReinterpreterEnable.write(bFalse);
		imgProc.defectivePixelsFilterMode.write(dpfmOff);
		imgProc.whiteBalanceCalibration.write(wbcmOff);
		*/
		
		

		
		// Set the strobe output lines
		
		// Turn both off initially to make sure we only turn on selected strobe lines
		io.lineSelector.writeS("Line0");
		io.lineInverter.write(bFalse);
		io.lineMode.writeS("Output");
		io.lineSource.writeS("Off");
		io.lineSelector.writeS("Line1");
		io.lineInverter.write(bFalse);
		io.lineMode.writeS("Output");
		io.lineSource.writeS("Off");
		
		
		if (strobe1Enable) {
			tmrCtrl.timerSelector.writeS( "Timer1" );
			tmrCtrl.timerDelay.write( strobe1Delay*1000 );
			tmrCtrl.timerDuration.write( strobe1Duration*1000 );
			tmrCtrl.timerTriggerSource.writeS( "ExposureStart" );
			io.lineSelector.writeS("Line0");
			io.lineSource.writeS("Timer1Active");
		}
		else if (strobe2Enable) {
			tmrCtrl.timerSelector.writeS( "Timer1" );
			tmrCtrl.timerDelay.write( strobe2Delay*1000 );
			tmrCtrl.timerDuration.write( strobe2Duration*1000 );
			tmrCtrl.timerTriggerSource.writeS( "ExposureStart" );
			io.lineSelector.writeS("Line1");
			io.lineSource.writeS("Timer1Active");
		}
		
	}
	catch( const ImpactAcquireException& e )
    {
        // this e.g. might happen if the same device is already opened in another process...
        cout << "An error occurred while configuring the device " << pDev->serial.read()
             << "(error code: " << e.getErrorCode() << "(" << e.getErrorCodeAsString() << ")). Terminating thread." << endl
             << "Press [ENTER] to end the application..."
             << endl;
		//return;
    }
	cout << "Device set up to " << ifc.pixelFormat.readS() << " " << ifc.width.read() << "x" << ifc.height.read() << endl;
	cout << "Device set up to " << ac.acquisitionFrameRate.readS() << "," << ac.exposureAuto.readS() << "," << ac.exposureTime.readS() << endl;

    // establish access to the statistic properties
    Statistics statistics( pDev );
    // create an interface to the device found
    FunctionInterface fi( pDev );

    // pre-fill the capture queue. There can be more than 1 queue for some device, but for this sample
    // we will work with the default capture queue. If a device supports more than one capture or result
    // queue, this will be stated in the manual. If nothing is mentioned about it, the device supports one
    // queue only. Request as many images as possible. If there are no more free requests 'DEV_NO_FREE_REQUEST_AVAILABLE'
    // will be returned by the driver.
    int result = DMR_NO_ERROR;
    SystemSettings ss( pDev );
    const int REQUEST_COUNT = ss.requestCount.read();
	cout << "Request Count: " << REQUEST_COUNT << endl;
    for( int i = 0; i < REQUEST_COUNT; i++ )
    {
        result = fi.imageRequestSingle();
        if( result != DMR_NO_ERROR )
        {
            cout << "Error while filling the request queue: " << ImpactAcquireException::getErrorCodeAsString( result ) << endl;
        }
    }

    // run thread loop
    const Request* pRequest = 0;
    const unsigned int timeout_ms = 8000;   // USB 1.1 on an embedded system needs a large timeout for the first image
    int requestNr = INVALID_ID;
    // This next comment is valid once we have a display:
    // we always have to keep at least 2 images as the display module might want to repaint the image, thus we
    // can't free it unless we have a assigned the display to a new buffer.
    int lastRequestNr = INVALID_ID;
    
	Poco::Stopwatch timer;
	
	manuallyStartAcquisitionIfNeeded( pDev, fi );
	
    cameraThreadRunning = true;
    
	while( isThreadRunning() )
    {
		if (!grabEnable) {
            usleep(10*1000);
            continue;
        }
		// wait for results from the default capture queue
        requestNr = fi.imageRequestWaitFor( timeout_ms );
        if( fi.isRequestNrValid( requestNr ) )
        {
            pRequest = fi.getRequest( requestNr );
            if( pRequest->isOK() )
            {
                ++cnt;
				
				
				// Get image data and send to callback
				CameraImage * pImage = new CameraImage();
				pImage->width = pRequest->imageWidth.read();
				pImage->height = pRequest->imageHeight.read();
				pImage->systemUnixTime = (unsigned long long)time(NULL);
				pImage->systemMicros = Timestamp().epochMicroseconds();
				pImage->cameraMicros = pRequest->infoTimeStamp_us.read();
				pImage->bytesPerPixel = pRequest->imageBytesPerPixel.read();
				pImage->frameNumber = pRequest->infoFrameNr.read();
				pImage->allocateAndCopyData((unsigned char*)pRequest->imageData.read(),pRequest->imageSize.read());
				
				
				
				
				//cout << pRequest->imageSize.read() << "," << pRequest->imageWidth.read() << "," << pRequest->imageHeight.read() << "," << pRequest->imageBytesPerPixel.read() << endl;
				
				
				// Add the image to the processor handler
				handler->addEntry(pImage);
				
                // here we can display some statistical information every 100th image
                //if( cnt % 10 == 0 )
                //{
                //    cout << "Info from " << pDev->serial.read()
                //         << ": " << statistics.framesPerSecond.name() << ": " << statistics.framesPerSecond.readS()
                //         << ", " << statistics.errorCount.name() << ": " << statistics.errorCount.readS()
                //         << ", " << statistics.captureTime_s.name() << ": " << statistics.captureTime_s.readS() << endl;
                //}
            }
            else
            {
                //cout << "Error: " << pRequest->requestResult.readS() << endl;
            }
            if( fi.isRequestNrValid( lastRequestNr ) )
            {
                // this image has been displayed thus the buffer is no longer needed...
                fi.imageRequestUnlock( lastRequestNr );
            }
            lastRequestNr = requestNr;
            // send a new image request into the capture queue
            fi.imageRequestSingle();
			
        }
        else
        {
            // If the error code is -2119(DEV_WAIT_FOR_REQUEST_FAILED), the documentation will provide
            // additional information under TDMR_ERROR in the interface reference (
           cout << "imageRequestWaitFor failed (" << requestNr << ", " << ImpactAcquireException::getErrorCodeAsString( requestNr ) << ", device "<< ")"
                 << ", timeout value too small?" << endl;
        }
		
		//int delay = (int)(threadDelay - timer.elapsed())/1000;
		//if (delay > 0)
		//	usleep(delay*1000);
    }

    // free the last potentially locked request
    if( fi.isRequestNrValid( requestNr ) )
    {
        fi.imageRequestUnlock( requestNr );
    }
    // clear the request queue
    fi.imageRequestReset( 0, 0 );
	
	// Set the strobe output off after acquisition
	//cout << "Turning off strobe output" << endl;
	
	io.lineSelector.writeS("Line0");
	//io.lineInverter.write(mvIMPACT::acquire::TBoolean::bFalse);
	io.lineMode.writeS("Output");
	io.lineSource.writeS("Off");
	io.lineSelector.writeS("Line1");
	//io.lineInverter.write(mvIMPACT::acquire::TBoolean::bFalse);
	io.lineMode.writeS("Output");
	io.lineSource.writeS("Off");

	pDev->close();
	cameraClosed = true;

}


