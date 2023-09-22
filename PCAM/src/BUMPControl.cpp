#include "BUMPControl.h"

//Creates a folder and text file with a unique timestamp id
//Opens serial port
//Starts data logging thread
char BUMPControl::Start(string foldername)
{

    //Sets bStop to false to prevent the program from quitting before all the devices/files have closed
    bStop = false;
    cmdMode = false;
    sequenceRunning = false;
    sequenceFile = "";

    //Creates folder and data file to log to
    createFile(foldername);

    //Tries to open to serial device
    //Displays error number if unsuccessful
    bool portOpen = openPort(comPort, baud) == 1;

    if (!portOpen) {
        stringstream ss;
        ss << name << " ERROR: openPort --- Could not open serial port.";
        errorMsgs.push_back(ss.str());
    }
    
    // force the firmware into data mode
    assertDataMode();
    
    this->startThread();

    return 1;
}

bool BUMPControl::assertDataMode() {
    device.WriteChar('@');
    return true;
}

void BUMPControl::enterCmdMode() {
    if (!cmdMode) {
        lock();
        device.WriteString("*");
        unlock();
        usleep(20*1000);
        cmdMode = true;
        
    }
    
}

void BUMPControl::exitCmdMode() {
    if (cmdMode) {
        lock();
        device.WriteString("@");
        unlock();
        usleep(20*1000);
        cmdMode = false;
    }
    
}

void BUMPControl::sendCommand(string cmd) {
    lock();
    if (!cmdMode) {
        device.WriteString("*");
        usleep(20*1000);
    }
    cout << "Sent :" << cmd << endl;
    device.WriteString(cmd.c_str());
    device.WriteString("\n");
    usleep(10*1000);
    if (!cmdMode) {
        device.WriteString("@");
        usleep(20*1000);
    }
    usleep(200*1000);
    unlock();
}

void BUMPControl::loadSequence(string fileName) {
    if (fileName == "") {
        // get a file name to load from UI
        char filename[1024];
        FILE *f = popen("zenity --title 'Select a sequence file' --file-selection","r");
        char *str = fgets(filename,1024, f);
        if (!str)
            return;
        fileName = filename;
    }
    
    if (fileName.at(fileName.size() -1) == '\n')
        fileName.erase(fileName.size() - 1);
    sequenceFile = fileName;
    std::cout << "Selected : " + fileName << std::endl;
    if (fileName.find(".seq") != std::string::npos) {
        lock();
        std::string line;
        std::ifstream infile(fileName);
        if (infile.is_open()) {
            device.WriteString("*loadsequence\n");
            usleep(50*1000);
            cout << "Sending lines..." << std::endl;
            while (std::getline(infile, line)) {
                
                //std::cout << line << std::endl;
                device.WriteString((line + "\n").c_str());
                usleep(50*1000);
                
            }
            device.WriteString("@");
        }
        infile.close();
        unlock();
    }
}

void BUMPControl::runSequence() {
    if (!this->sequenceRunning) {
        device.WriteString("*runsequence\n");
        this->cmdMode = true;
        this->sequenceRunning = true;
        this->sequenceTimer.reset();
        this->sequenceTimer.start();
    }
}

void BUMPControl::stopSequence() {
    if (this->sequenceRunning) {
        device.WriteChar(27);
        device.WriteChar('*');
        this->cmdMode = false;
        this->sequenceRunning = false;
        this->sequenceTimer.stop();
    }
}

void BUMPControl::quickSend(string cmd) {
    lock();
    std::cout << "Sending quick command: " << cmd << "\n" << std::flush;
    device.WriteString(cmd.c_str());
    device.WriteString("\n");
    usleep(50*1000);
    unlock();
}

string BUMPControl::fmtCommand(string prefix, int val) {
    stringstream ss;
    ss << prefix << " " << val;
    return ss.str().c_str();
}


// interface to MCU functions
void BUMPControl::toggleTrigger() {
    this->sendCommand("toggletrigger");

}

void BUMPControl::setSatPow(int pow) {
    sendCommand(fmtCommand("satpow",pow));
}

void BUMPControl::setSatDur(int dur) {
    sendCommand(fmtCommand("satdur",dur));
}

void BUMPControl::setMeasPow(int pow) {
    sendCommand(fmtCommand("measpow",pow));
}

void BUMPControl::setMeasDur(int dur) {
    sendCommand(fmtCommand("measdur",dur));
}

void BUMPControl::setColorDur(int dur) {
    sendCommand(fmtCommand("colordur",dur));
}

void BUMPControl::setTriggerType(int trigType) {
    sendCommand(fmtCommand("trigtype",trigType));
}

void BUMPControl::setFrameRate(int rate) {
    sendCommand(fmtCommand("framrate",rate));
}

int BUMPControl::parseData(string s, char firstchar, char lastchar) {
    char * rem;
    char * tok;
    char tmp[s.size() + 1];
    //cout << s << endl;
    strcpy(tmp,s.c_str());
    tok = strtok_r(tmp,",",&rem);
    if (strncmp(tok,"$BUMP",5) == 0) {
        float data[16];
        int max_ind = 0;
        for (int i=0; i < 16; i++) {
            tok = strtok_r(rem,",",&rem);
            if (tok != NULL) {
                data[i] = atof(tok);
                max_ind = i;
            }
            else {
                break;
            }
        }
        if (max_ind == 10) {
            temperature = data[2];
            humidity = data[3];
            pressure = data[4];
            depth = data[7];
            actuatorPos = data[10];
            //triggerType = data[11]; //TODO: not sent by the bump, read elsewhere?
        }
    }
    return 0;
}

string BUMPControl::getTemperature() {
    stringstream ss;
    ss << std::setw(4) << temperature;
    return ss.str();
}

string BUMPControl::getHumidity() {
    stringstream ss;
    ss << std::setw(4) << humidity;
    return ss.str();
}

string BUMPControl::getDepth() {
    stringstream ss;
    ss << std::setw(4) << depth;
    return ss.str();
}

string BUMPControl::getPosition() {
    stringstream ss;
    ss << std::setw(3) << (int)actuatorPos;
    return ss.str();
}

//BUMPControl thread: grabs all the bytes available on a device's buffer and writes them to a data file
//msTimestamp is inserted at the beginning of each data line
//Calls parseData to extract one complete line of data from tempBuf to be outputted to the console
void BUMPControl::threadedFunction()
{
    int bAvail;
    // bool fileAlreadyBad = false;
    char tempBuffer[10000];
    string buf = "";
    
    // Example Data
    //BUMP,1551056140,6303709,34.33,33.91,101163.88,34.51,1050.00,0.37,0.030,12.153,616

    
    while(isThreadRunning())
    {

        //Check how many bytes are available on the device
 
        bAvail = device.Available();

        //cout << bAvail << endl;
        //Make sure at least one line's worth of bytes are available
        if (bAvail > 0)
        {

            //Read all bytes available on device to buffer
            //TODO: lock is not global, so this isn't doing anything unless there are multiple threads running this function --- fix me -- JS 11/30/22
            lock();
            device.Read(tempBuffer, bAvail, 0);
            unlock();
            
            for (int i=0;i < bAvail;i++) {
                if (tempBuffer[i] == '\n') {
                    
                    buf += "\n";
                    this->parseData(buf, '$', '\n');
                    buf = DataTimestamp(time(0), true) + " : " + buf;
                    file.write(buf.c_str(),buf.length());
                    
                    buf = "";
                }
                else {
                    // append
                    buf += tempBuffer[i];
                }
                    
            }
            
            // Check for the end of a sequence
            if (this->sequenceRunning && this->sequenceTimer.elapsedSeconds() > 1) {
                if (buf.find("BUMP:") != std::string::npos && 
                buf.find("runseq") == std::string::npos) {
                    cout << "Sequence ended\n" << flush;
                    this->sequenceRunning = false;
                    this->sequenceTimer.stop();
                    this->device.WriteString("@");
                    this->cmdMode = false;
                }
            }
            
            
        }
        

        //Amount of time to sleep per loop
        usleep(100 * 1000);
    }

    //Stops BUMPControl logging
    //device.WriteString("stop\r\n");
    usleep(250 * 1000);
    // Close device and file
    device.Close();
    file.close();

}
