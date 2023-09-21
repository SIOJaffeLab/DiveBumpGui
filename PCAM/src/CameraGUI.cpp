#include "CameraGUI.h"

#define CVUI_IMPLEMENTATION
#include "cvui.h"

const int MIN_DURATION = 50; //microseconds
const int MAX_DURATION = 20000; //microseconds
const int STEP_DURATION = 50; //microseconds

const int MAIN_MENU_BINNING = 2;
const int DATA_MENU_BINNING = 1;

CameraGUI::CameraGUI(const char * config) {
    ActiveControlWindow = 0;
    
    //PAM parameters
    pamExposure = 500;
    pamPower1 = 6;
    pamIterations = 100;
    pamPower2 = 100;

    //Scan parameters
    scanWhiteDuration = 200;
    scanSatDuration = 500;
    
    //Video parameters
    videoWhiteDuration = 200;
    videoSatDuration = 500;
    
    //Defaults
	shouldQuit = false;
    saveFrame = false;
    bumpControl = NULL;
    toggleTrigger = false;
    updateSettings = false;
    triggerEnable = true;
    ambientOn = false;
    //TODO: probably want to update these?
    satPow = 50;
    measPow = 50;
    satDur = 500;
    measDur = 5000;
    colorDur = 100;
    triggerType = 1;
    frameRate = 20;
    actPos = 450;
    minSweep = 450;
    maxSweep = 650;
    binning = 1;
    whiteFlash = false;
    measFlash = false;
    satFlash = false;
    x1Binning = false;
    x2Binning = true;
    x4Binning = false;
    gain = 0;
    exposureTime = 10000;
    saveRaw = false;
    
    scaleGUI = 2.0;
    fontSize = 2.0;
    
    // Read the config file
    configName = string(config);
    readConfigFile(configName);
    
	if (validConfig) {
	
        controlWin = Mat(Size(S(800), S(400)), CV_8UC3);
        cvui::init(controlWinName);
        cv::moveWindow(controlWinName, 0, 0);
        //settings = EnhancedWindow()
        working = false;
        rectangle = Rect(0, 0, 0, 0);
        roiSelected = false;
        roi = Rect(0, 0, S(10), S(10));
        
        // Set the flash type
        if (triggerType == 0)
            satFlash = true;
        if (triggerType == 1)
            whiteFlash = true;
        if (triggerType == 2)
            measFlash = true;
    }
}

CameraGUI::~CameraGUI() {
    // Save out the config
    updateConfig();
    ((XMLConfiguration*)cfg)->save(configName.c_str());
    
}

bool CameraGUI::quitPressed() {
    return shouldQuit;
}

void CameraGUI::setControl(BUMPControl * ctr) {
    bumpControl = ctr;
    updateControl();
}

void CameraGUI::setCamera(Camera * cam) {
    bumpCamera = cam;
}

void CameraGUI::updateControl() {
    if (bumpControl != NULL) {
        stringstream ss;
        ss << "!,BUMP," 
            << triggerEnable << ","
            << triggerType << ","
            << satPow << ","
            << satDur << ","
            << measPow << ","
            << measDur << ","
            << colorDur << ","
            << frameRate;
        bumpControl->quickSend(ss.str());
    }
}

void CameraGUI::doFocalStack() {
    if (bumpControl != NULL) {
        stringstream ss;
        ss << "!,FOCAL,"
            << minSweep << ","
            << maxSweep;
        bumpControl->quickSend(ss.str());
    }
}

void CameraGUI::threeRowExclusive(  bool *b1, char * t1, 
                                    bool *b2, char * t2,
                                    bool *b3, char * t3) 
{
    bool lastb1 = *b1;
    bool lastb2 = *b2;
    bool lastb3 = *b3;
    cvui::beginRow();
    cvui::checkbox(t1, b1);
    cvui::checkbox(t2, b2);
    cvui::checkbox(t3, b3);
    cvui::endRow();
    if (!lastb1 && *b1) {
        *b1 = true;
        *b2 = false;
        *b3 = false;
    }
    if (!lastb2 && *b2) {
        *b1 = false;
        *b2 = true;
        *b3 = false;
    }
    if (!lastb3 && *b3) {
        *b1 = false;
        *b2 = false;
        *b3 = true;
    }
}

int CameraGUI::getStep(int value) {
    
    int step = 1;
    while (value / step >= 10) {
        step *= 10;
    }
    
    return step;
    
}

void CameraGUI::drawCounter(const char * text, int * value, int min, int max, int inc) {
    cvui::text(text);
    cvui::space(10); // add 10px of space between UI components
    if (*value < min)
        *value = min;
    if (*value > max)
        *value = max;
    int lastValue = *value;
    int lastStep = getStep(lastValue);
    if (inc > 0) 
        lastStep = inc;
    cvui::counter(value, lastStep);
    //int newStep = getStep(*value);
    if (*value == 0)
        *value = lastValue - lastValue/10;
}

//TODO: passing a copy?
void CameraGUI::update(Mat inputImage) {
    
    Mat controlFrame = controlWin.clone();

    cvui::beginRow(controlFrame, S(0), S(0), S(400), S(800));
    cvui::beginColumn(S(400), S(400));

    //TODO: maybe an extra temporary?
    cv::demosaicing(inputImage,displayImg,cv::COLOR_BayerRG2RGB);
    resize(displayImg, displayImg, Size(S(400), S(400)), 0, 0, INTER_AREA);
    Mat display;
    displayImg.convertTo(display, 16, 1.0/256.0);
    cvui::image(display);
    
    cvui::endColumn();
    
    cvui::beginColumn(S(400), S(400));
    
    if (ActiveControlWindow == 1) {
        //PAM menu
        cvui::beginRow(S(400), S(50));
        cvui::text("PAM menu", fontSize);
        cvui::space(S(100));
        if (cvui::button(S(50), S(50), "X", fontSize)) {HandleXout();}
        cvui::endRow();
        LabelledRow(pamExposure, " Ambient  ", STEP_DURATION, MIN_DURATION, MAX_DURATION);
        LabelledRow(pamPower1, "Meas pwr ", 1, 1, 100);
        LabelledRow(pamIterations, "Sat iter   ", 10, 10, 1000);
        LabelledRow(pamPower2, "Sat pwer ", STEP_DURATION, MIN_DURATION, MAX_DURATION);
        cvui::space(S(1));
        if (cvui::button(S(200), S(50), "Run PAM", fontSize)) {HandleRunPam();}
    } else if (ActiveControlWindow == 2) {
        //SCAN menu
        cvui::beginRow(S(400), S(50));
        cvui::text("SCAN menu", fontSize);
        cvui::space(S(100));
        if (cvui::button(S(50), S(50), "X", fontSize)) {HandleXout();}
        cvui::endRow();
        LabelledRow(scanWhiteDuration, " White Dur ", STEP_DURATION, MIN_DURATION, MAX_DURATION);
        LabelledRow(scanSatDuration,   "   Sat Dur ", STEP_DURATION, MIN_DURATION, MAX_DURATION);
        cvui::space(S(101));
        if (cvui::button(S(200), S(50), "Run SCAN", fontSize)) {HandleRunScan();}
    } else if (ActiveControlWindow == 3) {
        //Video menu
        cvui::beginRow(S(400), S(50));
        cvui::text("VIDEO menu", fontSize);
        cvui::space(S(100));
        if (cvui::button(S(50), S(50), "X", fontSize)) {HandleXout();}
        cvui::endRow();
        LabelledRow(videoWhiteDuration, " White Dur ", STEP_DURATION, MIN_DURATION, MAX_DURATION);
        LabelledRow(videoSatDuration,   "   Sat Dur ", STEP_DURATION, MIN_DURATION, MAX_DURATION);
        cvui::space(S(101));
        if (cvui::button(S(200), S(50), "Run Video", fontSize)) {HandleRunVideo();}
    } else {
        //main menu
        cvui::beginRow(S(400), S(150));
        cvui::space(S(10));
        if (cvui::button(S(50), S(150), "Q", fontSize)) {HandleESC();}

        cvui::space(S(10));
        cvui::beginColumn(S(150), S(150));
        cvui::space(S(10));
        cvui::text("POSITION", fontSize);
        cvui::space(S(10));
        cvui::beginRow(S(150), S(70));
        if (cvui::button(S(70), S(70), "-", fontSize)) {HandlePositionDecrement();}
        if (cvui::button(S(70), S(70), "+", fontSize)) {HandlePositionIncrement();}
        cvui::endRow();
        cvui::space(S(10));
        cvui::text("" + bumpControl->getPosition(), fontSize);
        cvui::endColumn();
        
        cvui::space(S(10));
        cvui::beginColumn(S(150), S(150));
        cvui::space(S(10));
        cvui::text("GAIN", fontSize);
        cvui::space(S(10));
        cvui::beginRow(S(150), S(70));
        if (cvui::button(S(70), S(70), "-", fontSize)) {HandleGainDecrement();}
        if (cvui::button(S(70), S(70), "+", fontSize)) {HandleGainIncrement();}
        cvui::endRow();
        cvui::space(S(10));
        stringstream sgain;
        sgain << gain;
        cvui::text(sgain.str().c_str(), fontSize);
        cvui::endColumn();
        
        cvui::endRow();
        cvui::space(5);
        
        cvui::beginRow(S(400), S(250));
        cvui::beginColumn(S(100), S(250));
        stringstream sequenceElapsed;
        if (bumpControl->sequenceRunning) {
            sequenceElapsed << "" << bumpControl->sequenceTimer.elapsedSeconds();
            cvui::text(lastSequence.c_str(), fontSize);
            cvui::button(S(100), S(80), sequenceElapsed.str().c_str(), fontSize);
            if (cvui::button(S(100), S(80), "CANCEL SEQ", fontSize)) {bumpControl->stopSequence();}
        } else {
            SetBinning(MAIN_MENU_BINNING);
            if (cvui::button(S(100), S(70), "PAM", fontSize)) {HandlePAM();}
            if (cvui::button(S(100), S(70), "SCAN", fontSize)) {HandleScan();}
            if (cvui::button(S(100), S(70), "VIDEO", fontSize)) {HandleVideo();}
        }
        
        cvui::endColumn();
        cvui::beginColumn(S(250), S(250));
        cvui::beginRow();
        if (cvui::button(S(70), S(70), "W", fontSize)) {HandleToggleWhite();}
        if (cvui::button(S(70), S(70), "-", fontSize)) {HandleWhiteDecrement();}
        if (cvui::button(S(70), S(70), "+", fontSize)) {HandleWhiteIncrement();}
        stringstream sWhiteDur;
        sWhiteDur << colorDur;
        cvui::text(sWhiteDur.str().c_str(), fontSize);
        cvui::endRow();
        cvui::beginRow();
        if (cvui::button(S(70), S(70), "S", fontSize)) {HandleToggleSat();}
        if (cvui::button(S(70), S(70), "-", fontSize)) {HandleSatDecrement();}
        if (cvui::button(S(70), S(70), "+", fontSize)) {HandleSatIncrement();}
        stringstream sSatDur;
        sSatDur << satDur;
        cvui::text(sSatDur.str().c_str(), fontSize);
        cvui::endRow();
        if (cvui::button(S(250), S(80), saveRaw ? "Stop REC" : "Start REC", fontSize)) {HandleREC();}
        cvui::endColumn();
        cvui::endRow();
        
    }
    
    cvui::endColumn();
    cvui::endRow();
    
    int status = cvui::iarea(0, 0, S(400), S(400));
    switch (status) {
        case cvui::CLICK:
            roi.x = cvui::mouse().x;
            roi.y = cvui::mouse().y;
            roiSelected = (roi.x + roi.width < S(400)) && (roi.y + roi.height < S(400));
            break;
        default:
            break;
    }
    
    if (roiSelected) {
        cvui::rect(controlFrame, roi.x, roi.y, roi.width, roi.height, 0xff0000);
        auto score = cv::mean(display(roi));
        int scoreR = (int)score[0];
        int scoreG = (int)score[1];
        int scoreB = (int)score[2];
        cvui::rect(controlFrame, S(10), S(8), 700, 60, 0x000000, 0x000000);
        cvui::printf(controlFrame, S(10), S(10), fontSize, 0xffff00, "px mean: %d %d %d", scoreR, scoreG, scoreB);
    }
    
    // clear the control window
    /*Mat controlFrame = controlWin.clone();
    
    cvui::context(controlWinName);
    
    camera.begin(controlFrame);
    if (!camera.isMinimized()) {
        //commands.setMinimized(true);
        //commands.setY(camera.height() + 20);
        int oldExposure = exposureTime;
        drawCounter("Exposure Time (us)",&exposureTime,50,2000000);
        if (oldExposure != exposureTime) {
            ((MVIMPACTCamera*)bumpCamera)->setExposure(exposureTime);
        }
        cvui::space(10); // add 10px of space between UI components
        int oldGain = gain;
        drawCounter("Gain (dB)", &gain, 0,48,1);
        if (gain != oldGain) {
            ((MVIMPACTCamera*)bumpCamera)->setGain(gain);
        }
        cvui::space(10); // add 10px of space between UI components
        cvui::text("Binning");
        cvui::space(10); // add 10px of space between UI components
        int oldBinning = binning;
        //cvui::trackbar(camera.width() - 20, &binning, 1, 4); 
        threeRowExclusive(&x1Binning,"x1",&x2Binning,"x2",&x4Binning,"x4");
        if (x1Binning)
            binning = 1;
        else if (x2Binning)
            binning = 2;
        else if (x4Binning)
            binning = 4;
        if (binning != oldBinning) {
            // stop any recording as the image size will change
            saveRaw = false;
            ((MVIMPACTCamera*)bumpCamera)->setBinning(binning);
        }
    }
    camera.end();
    commands.begin(controlFrame);
    if (!commands.isMinimized()) {
        //camera.setMinimized(true);
        //commands.setY(30);
        drawCounter("Sat Duration (us)",&satDur,50,1000000);
        cvui::space(10); // add 10px of space between UI components
        drawCounter("Sat Power (%)",&satPow,1,100);
        cvui::space(10); // add 10px of space between UI components
        drawCounter("Meas Duration (us)",&measDur,50,1000000);
        cvui::space(10); // add 10px of space between UI components
        drawCounter("Meas Power (%)",&measPow,1,100);
        cvui::space(10); // add 10px of space between UI components
        drawCounter("White Duration (us)",&colorDur,50,1000000);
        cvui::space(10); // add 10px of space between UI components
        drawCounter("Frame Rate (Hz)", &frameRate,1,28,1);
        cvui::space(10);
        cvui::text("Flash Type");
        cvui::space(5);
        cvui::beginRow();
        bool lastWhite = whiteFlash;
        bool lastMeas = measFlash;
        bool lastSat = satFlash;
        cvui::checkbox("WHITE",&whiteFlash);
        cvui::checkbox("MEAS",&measFlash);
        cvui::checkbox("SAT", &satFlash);
        cvui::endRow();
        if (!lastWhite && whiteFlash) {
            triggerType = 1;
            measFlash = false;
            satFlash = false;
        }
        if (!lastMeas && measFlash) {
            triggerType = 2;
            whiteFlash = false;
            satFlash = false;
        }
        if (!lastSat && satFlash) {
            triggerType = 0;
            whiteFlash = false;
            measFlash = false;
        }
        cvui::space(10);
        bool lastAmbientOn = ambientOn;
        cvui::checkbox("Ambient On", &ambientOn);
        if (lastAmbientOn != ambientOn) {
            if (ambientOn)
                bumpControl->sendCommand("ambienton");
            else
                bumpControl->sendCommand("ambientoff");
        }
        cvui::space(10); // add 10px of space between UI components
        updateSettings = cvui::button("Update Settings");
        if (updateSettings)
            updateControl();
            
    }
    commands.end();
    focalStack.begin(controlFrame);
    if (!focalStack.isMinimized()) {
        cvui::text("Position");
        cvui::space(10); // add 10px of space between UI components
        cvui::trackbar(commands.width() - 20, &actPos, 300, 550);
        cvui::space(10); // add 10px of space between UI components
        cvui::text("Min Sweep");
        cvui::space(10); // add 10px of space between UI components
        cvui::trackbar(commands.width() - 20, &minSweep, 300, 550);
        cvui::space(10); // add 10px of space between UI components
        cvui::text("Max Sweep");
        cvui::space(10); // add 10px of space between UI components
        cvui::trackbar(commands.width() - 20, &maxSweep, minSweep, 550);
        cvui::space(10); // add 10px of space between UI components
        cvui::beginRow();
        if (cvui::button("GoTo")) {
            stringstream ss;
            ss << "!,ACT," << actPos;
            bumpControl->quickSend(ss.str());
        }
        if (cvui::button("FWD"))
            bumpControl->quickSend("!,FWD");
        if (cvui::button("RWD"))
            bumpControl->quickSend("!,RWD");
        cvui::endRow();
        cvui::space(5);
        if (cvui::button("Focal Stack"))
            doFocalStack();
        
        
    }
    focalStack.end();
    sequence.begin(controlFrame);
    if (!sequence.isMinimized()) {
        cvui::beginRow();
        if (cvui::button("Load"))
            bumpControl->loadSequence();
        if (cvui::button("Run"))
            bumpControl->runSequence();
        if (cvui::button("Stop"))
            bumpControl->stopSequence();
        cvui::endRow();
        if (bumpControl->sequenceFile != "") {
            cvui::space(10);
            cvui::text("Sequence File:");
            cvui::space(5);
            cvui::text(bumpControl->sequenceFile);
        }
        cvui::space(5);
        

        
    }
    sequence.end();
    buttons.begin(controlFrame);
    if (!buttons.isMinimized()) {
        cvui::checkbox("Save Raw Data",&saveRaw);
        cvui::space(5);
        //cvui::checkbox("TriggerEnable",&triggerEnable);
        cvui::space(2); // add 10px of space between UI components
        cvui::beginRow();
        if (cvui::button("PAM"))
            bumpControl->quickSend("!,PAM");
        if (cvui::button("Save Image"))
            this->saveFrame = true;
        cvui::endRow();
        cvui::space(2); // add 10px of space between UI components
        if (cvui::button("Start Camera"))
            bumpControl->quickSend("!,STARTCAM");
        cvui::space(2); // add 10px of space between UI components
        if (cvui::button("Stop Camera"))
            bumpControl->quickSend("!,STOPCAM");
        cvui::space(2); // add 10px of space between UI components
        shouldQuit = cvui::button("Quit");
    }
    buttons.end();
    statusWin.begin(controlFrame);
    if (!statusWin.isMinimized()) {
        bool tmp = bumpControl->sequenceRunning;
        cvui::checkbox("Sequence Running", &tmp, 0x50AA50);
        cvui::space(5);
        if (bumpControl->sequenceRunning) {
            stringstream sequenceElapsed;
            sequenceElapsed << "Elapsed Seconds: " << bumpControl->sequenceTimer.elapsedSeconds();
            cvui::text(sequenceElapsed.str());
        }
        cvui::space(10);
        tmp = saveRaw;
        cvui::checkbox("Recording", &tmp, 0xAA5050);
        cvui::space(15);
        cvui::text("Temperature: " + bumpControl->getTemperature() + " C");
        cvui::space(7);
        cvui::text("Humidity: " + bumpControl->getHumidity() + " %");
        cvui::space(7);
        cvui::text("Depth: " + bumpControl->getDepth() + " m");
        cvui::space(7);
        cvui::text("Position: " + bumpControl->getPosition() + " DN");
    }
    statusWin.end();*/
    
    cvui::update(controlWinName);
    cv::imshow(controlWinName, controlFrame);
    
    /*cvui::context(guiName);
    
    resize(inputImage,inputImage,Size(),1.0/3, 1.0/3, INTER_NEAREST);
    cv::demosaicing(inputImage,displayImg,cv::COLOR_BayerRG2RGB);
    
    int newHeight = 800;
    int newWidth = (int)inputImage.cols*800/inputImage.rows;
    
    resize(displayImg,displayImg,Size(newWidth, newHeight),INTER_AREA);
    
    // Did any mouse button go down?
    if (cvui::mouse(cvui::DOWN)) {
        // Position the anchor at the mouse pointer.
        anchor.x = cvui::mouse().x;
        anchor.y = cvui::mouse().y;

        // Inform we are working, so the ROI window is not updated every frame
        working = true;
    }

    // Is any mouse button down (pressed)?
    if (cvui::mouse(cvui::IS_DOWN)) {
        // Adjust roi dimensions according to mouse pointer
        int width = cvui::mouse().x - anchor.x;
        int height = cvui::mouse().y - anchor.y;
        
        roi.x = width < 0 ? anchor.x + width : anchor.x;
        roi.y = height < 0 ? anchor.y + height : anchor.y;
        roi.width = std::abs(width);
        roi.height = std::abs(height);

        // Show the roi coordinates and size
        cvui::printf(displayImg, roi.x + 5, roi.y + 5, 0.3, 0xffffff, "(%d,%d)", roi.x, roi.y);
        cvui::printf(displayImg, cvui::mouse().x + 5, cvui::mouse().y + 5, 0.3, 0xffffff, "w:%d, h:%d", roi.width, roi.height);
    }

    // Was the mouse clicked (any button went down then up)?
    if (cvui::mouse(cvui::UP)) {
        // We are done working with the ROI.
        working = false;
    }

    // Ensure ROI is within bounds
    roi.x = roi.x < 0 ? 0 : roi.x;
    roi.y = roi.y < 0 ? 0 : roi.y;
    roi.width = roi.x + roi.width > displayImg.cols ? roi.width + displayImg.cols - (roi.x + roi.width) : roi.width;
    roi.height = roi.y + roi.height > displayImg.rows ? roi.height + displayImg.rows - (roi.y + roi.height) : roi.height;

    // Render the roi
    cvui::rect(displayImg, roi.x, roi.y, roi.width, roi.height, 0xffffff);

    // This function must be called *AFTER* all UI components. It does
    // all the behind the scenes magic to handle mouse clicks, etc.
    cvui::update(guiName);

    // Show everything on the screen
    cv::imshow(guiName, displayImg);
    
    // If the ROI is valid, show it.
    if (roi.area() > 0 && !working) {
        Mat dImg = displayImg(roi);
        resize(dImg,dispRoi,Size(dispRoi.cols,dispRoi.rows),INTER_LINEAR);
        cv::imshow(guiName+"-ROI", dispRoi);
    }*/
    
    waitKey(5); // this is needed or the GUI won't show at all

}

void CameraGUI::applyConfig() {
    
    updateControl();
    ((MVIMPACTCamera*)bumpCamera)->setGain(gain);
    ((MVIMPACTCamera*)bumpCamera)->setExposure(exposureTime);
    ((MVIMPACTCamera*)bumpCamera)->setBinning(binning);
    
}

void CameraGUI::updateConfig() {
    try {
        cfg->setInt("Gui.TriggerType",triggerType);
        cfg->setBool("Gui.TriggerEnable",triggerEnable);
        cfg->setInt("Gui.SaturationDuration",satDur);
        cfg->setInt("Gui.SaturationPower",satPow);
        cfg->setInt("Gui.MeasurmentDuration",measDur);
        cfg->setInt("Gui.MeasurmentPower",measPow);
        cfg->setBool("Gui.AmbientOn",ambientOn);
        cfg->setInt("Gui.CameraGain",gain);
        cfg->setInt("Gui.CameraExposureTime",exposureTime);
        cfg->setInt("Gui.CameraBinning",binning);
        cfg->setInt("Gui.ActuatorPosition",actPos);
        cfg->setInt("Gui.FrameRate",frameRate);
        
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
}

void CameraGUI::readConfigFile(string filename) {
    
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
        guiName = cfg->getString("Gui.name","BUMP");
        controlWinName = guiName+"-Control";
        guiImageHeight = cfg->getInt("Gui.height",800);
        guiImageWidth = cfg->getInt("Gui.width",800);
        triggerType = cfg->getInt("Gui.TriggerType",0);
        triggerEnable = cfg->getBool("Gui.TriggerEnable",false);
        satDur = cfg->getInt("Gui.SaturationDuration",50);
        satPow = cfg->getInt("Gui.SaturationPower",50);
        measDur = cfg->getInt("Gui.MeasurmentDuration",500);
        measPow = cfg->getInt("Gui.MeasurmentPower",5);
        ambientOn = cfg->getBool("Gui.AmbientOn",true);
        gain = cfg->getInt("Gui.CameraGain",0);
        exposureTime = cfg->getInt("Gui.CameraExposureTime",10000);
        binning = cfg->getInt("Gui.CameraBinning",2);
        actPos = cfg->getInt("Gui.ActuatorPosition",450);
        frameRate = cfg->getInt("Gui.FrameRate",20);
        
        
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

void CameraGUI::HandleESC() {
    bumpControl->quickSend("!,STOPCAM");
    shouldQuit = true;
}

void CameraGUI::HandlePositionIncrement() {
    bumpControl->quickSend("!,FWD");
}
void CameraGUI::HandlePositionDecrement() {
    bumpControl->quickSend("!,RWD");
}


// TODO: max/min values hacked in (gain can be zero?)
void CameraGUI::HandleGainIncrement() {
    if (gain < 48) {
        gain += 1;
        ((MVIMPACTCamera*)bumpCamera)->setGain(gain);
    }
}
void CameraGUI::HandleGainDecrement() {
    if (gain > 0) {
        gain -= 1;
        ((MVIMPACTCamera*)bumpCamera)->setGain(gain);
    }
}

void CameraGUI::HandlePAM() {
    ActiveControlWindow = 1;
    //triggerEnable = false;
    //bumpControl->quickSend("!,STOPCAM");
    //bumpControl->loadSequence("/mnt/NVMEDATA/Sequencetrials/diverguipam1.seq");
    //lastSequence = "PAM1";
    //bumpControl->runSequence();
}
void CameraGUI::HandleScan() {
    ActiveControlWindow = 2;
    //bumpControl->loadSequence("/mnt/NVMEDATA/Sequencetrials/diverguifocal.seq");
    //bumpControl->runSequence();
}
void CameraGUI::HandleVideo() {
    ActiveControlWindow = 3;
    //triggerEnable = false;
    //bumpControl->quickSend("!,STOPCAM");
    //bumpControl->loadSequence("/mnt/NVMEDATA/Sequencetrials/diverguipam3.seq");
    //lastSequence = "PAM3";
    //bumpControl->runSequence();
}

void CameraGUI::HandleToggleWhite() {
    triggerType = 1;
    if (triggerEnable) {
        triggerEnable = false;
        bumpControl->quickSend("!,STOPCAM");
    } else {
        satFlash = false;
        whiteFlash = true;
        measFlash = false;
        triggerEnable = true;
        bumpControl->quickSend("!,STARTCAM");
    }
    updateControl();
}

//modify files that must exist (hard crash otherwise)
void CameraGUI::HandleRunPam() {
    ifstream fil("/mnt/NVMEDATA/AutoSequences/pam.seq");
    if (!fil.good()) {
        cerr << "Error: could not open pam sequence file ... aborting\n" << flush;
        exit(-1);
    }
    
    ofstream out("auto.seq");
    if (!out.good()) {
        cerr << "Error: could not open pam save file ... aborting\n" << flush;
        exit(-1);
    }
    
    string line;
    while (fil >> line) {
        ReplaceAll(line, "[exposure]", IntToString(pamExposure));
        ReplaceAll(line, "[power1]", IntToString(pamPower1));
        ReplaceAll(line, "[iterations]", IntToString(pamIterations));
        ReplaceAll(line, "[power2]", IntToString(pamPower2));
        out << line + "\n";
    }
    fil.close();
    out.close();
    
    RunAutoSequence();
}

void CameraGUI::HandleRunScan() {
    ifstream fil("/mnt/NVMEDATA/AutoSequences/scan.seq");
    if (!fil.good()) {
        cerr << "Error: could not open pam sequence file ... aborting\n" << flush;
        exit(-1);
    }
    
    ofstream out("auto.seq");
    if (!out.good()) {
        cerr << "Error: could not open pam save file ... aborting\n" << flush;
        exit(-1);
    }
    
    string line;
    while (fil >> line) {
        ReplaceAll(line, "[duration1]", IntToString(scanWhiteDuration));
        ReplaceAll(line, "[duration2]", IntToString(scanSatDuration));
        out << line + "\n";
    }
    fil.close();
    out.close();
    
    RunAutoSequence();
}

void CameraGUI::HandleRunVideo() {
    ifstream fil("/mnt/NVMEDATA/AutoSequences/video.seq");
    if (!fil.good()) {
        cerr << "Error: could not open pam sequence file ... aborting\n" << flush;
        exit(-1);
    }
    
    ofstream out("auto.seq");
    if (!out.good()) {
        cerr << "Error: could not open pam save file ... aborting\n" << flush;
        exit(-1);
    }
    
    string line;
    while (fil >> line) {
        ReplaceAll(line, "[duration3]", IntToString(videoWhiteDuration));
        ReplaceAll(line, "[duration4]", IntToString(videoSatDuration));
        out << line + "\n";
    }
    fil.close();
    out.close();
    
    RunAutoSequence();
}

void CameraGUI::RunAutoSequence() {
    ActiveControlWindow = 0;
    SetBinning(DATA_MENU_BINNING);
    saveRaw = true;
    triggerEnable = false;
    bumpControl->quickSend("!,STOPCAM");
    bumpControl->loadSequence("auto.seq");
    lastSequence = "auto";
    bumpControl->runSequence();
}


void CameraGUI::HandleWhiteIncrement() {
    if (colorDur < MAX_DURATION) {
        colorDur += STEP_DURATION;
        updateControl();
    }
}
void CameraGUI::HandleWhiteDecrement() {
    if (colorDur > MIN_DURATION) {
        colorDur -= STEP_DURATION;
        updateControl();
    }
}
void CameraGUI::HandleToggleSat() {
    triggerType = 0;
    if (triggerEnable) {
        triggerEnable = false;
        bumpControl->quickSend("!,STOPCAM");
    } else {
        satFlash = true;
        whiteFlash = false;
        measFlash = false;
        triggerEnable = true;
        bumpControl->quickSend("!,STARTCAM");
    }
    updateControl();
}

void CameraGUI::ReplaceAll(string& str, const string& from, const string& to) {
    if (from.empty()) {
        return;
    }
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}


void CameraGUI::HandleSatIncrement() {
    if (satDur < MAX_DURATION) {
        satDur += STEP_DURATION;
        updateControl();
    }
}
void CameraGUI::HandleSatDecrement() {
    if (satDur > MIN_DURATION) {
        satDur -= STEP_DURATION;
        updateControl();
    }
}

//TODO: toggle I guess?
void CameraGUI::HandleREC() {
    saveRaw = !saveRaw;
}

void CameraGUI::HandleXout() {
    ActiveControlWindow = 0;
}

// Check the binning matches the input and potentially change it.
//Does nothing if the binning matches.
//Stops the recording (flags it to stop) if it has to change.
void CameraGUI::SetBinning(const int& bin) {
    if (bin != binning) {
        binning = bin;
        saveRaw = false;
        ((MVIMPACTCamera*)bumpCamera)->setBinning(binning);
    }
}


string CameraGUI::IntToString(int x) {
    stringstream ss;
    ss << x;
    return ss.str();
}

//modifies the value
void CameraGUI::ChangeInt(int& value, int change, int Min, int Max) {
    int v = value + change;
    if (v >= Min && v <= Max) {
        value = v;
    }
}

// mostly for the sub menus
void CameraGUI::LabelledRow(int& value, string id, int sstep, int low, int high) {
    cvui::beginRow();
    cvui::text(id.c_str(), fontSize);
    if (cvui::button(S(70), S(70), "-", fontSize)) {
        ChangeInt(value, -sstep, low, high);
    }
    if (cvui::button(S(70), S(70), "+", fontSize)) {
        ChangeInt(value, sstep, low, high);
    }
    cvui::text(IntToString(value), fontSize);
    cvui::endRow();
}


//Scale the ui by the built in scaleGUI factor.
//Short (bad) name since this gets called a lot.
int CameraGUI::S(const int& x) const {
    return int(double(x)*scaleGUI);
}

