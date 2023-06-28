#include "SerialDevice.h"


//SerialDevice constructor
SerialDevice::SerialDevice(string config_path)
{
    configRead(config_path);
    this->bStop = true;
    this->bNew = false;
}

//SerialDevice constructor with inputs
SerialDevice::SerialDevice(string name, string port, int baud, bool display, bool trim)
{
    this->bDisplay = display;
    this->bTrim = trim;
    this->baud = baud;
    this->name = name;
    this->comPort = port;
    this->bStop = true;
    this->bNew = false;

}
//SerialDevice destructor
SerialDevice::~SerialDevice(){}




//Creates a folder and text file with a unique timestamp id
//Opens serial port
//Starts data logging thread
char SerialDevice::Start(string foldername)
{
    signed int errno;

    //Sets bStop to false to prevent the program from quitting before all the devices/files have closed
    bStop = false;
    bNew = false;

    createFile(foldername);


    //Tries to open to serial device
    //Displays error number if unsuccessful
    bool portOpen = openPort(comPort, baud) == 1;

    if (!portOpen) {
        stringstream ss;
        ss << name << " ERROR: openPort --- return code from openPort is " << portOpen;
        errorMsgs.push_back(ss.str());
    }
    
    this->startThread();

    return 1;
}

string SerialDevice::getErrorMessages() {
    stringstream ss;
    ss.str("");
    for (unsigned int i=0;i < errorMsgs.size();i++) {
        ss << errorMsgs[i];
        if (i < errorMsgs.size()-1)
            ss << ", ";
    }
    errorMsgs.clear();
    return ss.str();
}


//Allows SerialDevice threads to end by setting a boolean to true
//Waits until all devices have successfully quit to exit while loop
void SerialDevice::Stop()
{
    this->stopThread();
}




//Attempts to open the serial port
//Returns 1 if successful
//Returns error number if unsuccessful (refer to serialib.h for error numbers)
char SerialDevice::openPort(string comPort, unsigned int baud)
{
    signed int errno;

    if ((errno = device.Open(comPort.c_str(), baud)) != 1)
    {
        //cout << "There was an error opening  " << this->name << ": " << errno << endl;
        bStop = true;
        return errno;
    }
    else
        return 1;
}



//SerialDevice thread that grabs all the bytes available on a device's buffer and writes them to a file
void SerialDevice::threadedFunction()
{
    string msTime;
    int bAvail;
    bool fileAlreadyBad = false;
    char tempBuffer[10000];
    string buf;

    //Reads all bytes available on device every 100 ms
    //Writes data out to the console and a file
    while(isThreadRunning())
    {
        //Check how many bytes are available on the device
        bAvail = device.Available();
        //cout << bAvail << endl;
        //Make sure at least one line's worth of bytes are available
        if (bAvail > 0)
        {
            //Read all bytes available on device to buffer
            device.Read(tempBuffer, bAvail, 0);
            
            for (int i=0;i < bAvail;i++) {
                if (tempBuffer[i] == '\n') {
                    buf += "\n";
                    file.write(buf.c_str(),buf.length());
                    buf = "";
                }
                else {
                    // append
                    buf += tempBuffer[i];
                }
                    
            }
            
            
        }

        //Amount of time to sleep per loop
        usleep(100 * 1000);
    }

    // Close device and file
    device.Close();
    file.close();

}




//Parses data to find the most recent, complete line of data
int SerialDevice::parseData(string s, char firstchar, char lastchar)
{
    for (string::iterator it = s.end(); it >= s.begin(); --it)
    {
        //Searches backwards for lastchar and removes everything after it
        if (*it == lastchar)
        {
            s.erase(it+1, s.end());

            for (string::iterator it = s.end(); it >= s.begin(); --it)
            {
                //Searches backwards for firstchar and removes everything before it
                if (*it == firstchar)
                {
                    s.erase(s.begin(), it+1);
                    rawData = s;
                    //cout << rawData;

                    //Sets bNew to true if a new line of data was extracted
                    bNew = true;

                    s.clear();
                    tempBuf.clear();
                    return 1;
                }
            }
        }
    }
    s.clear();
    return 0;

}




//Returns the timestamp string used to create unique file and folder names
string SerialDevice::timestamp(long timestamp, bool milliseconds = false)
{
    //Gets current time since epoch
    const time_t rawtime = (const time_t)timestamp;

    struct tm * dt;
    char timestr[30];
    char buffer [30];

    //Converts rawtime into local time
    dt = localtime(&rawtime);

    //Formats time to be human readable
    strftime(timestr, sizeof(timestr), "%m-%d-%Y_%H-%M-%S", dt);
    sprintf(buffer,"%s", timestr);
    std::string stdBuffer(buffer);

    //Adds milliseconds to the timestamp if milliseconds boolean is set to true
    if (milliseconds)
    {
        stdBuffer.append("-");
        stdBuffer.append(mstimestamp());

    }

    //Return the timestamp string
    return stdBuffer;

}

string SerialDevice::DataTimestamp(long timestamp, bool milliseconds = false)
{
    //Gets current time since epoch
    const time_t rawtime = (const time_t)timestamp;

    struct tm * dt;
    char timestr[30];
    char buffer [30];

    //Converts rawtime into local time
    dt = localtime(&rawtime);

    //Formats time to be human readable
    strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S", dt);
    sprintf(buffer,"%s", timestr);
    std::string stdBuffer(buffer);

    //Adds milliseconds to the timestamp if milliseconds boolean is set to true
    if (milliseconds)
    {
        stdBuffer.append(":");
        stdBuffer.append(mstimestamp());

    }

    //Return the timestamp string
    return stdBuffer;

}



//Returns a string with the current millisecond time
string SerialDevice::mstimestamp()
{
    unsigned int ms;
    char buffer[64];

    #ifndef TARGET_WIN32
        struct timeval now;
        gettimeofday( &now, NULL );
        ms = now.tv_usec / 1000;
        sprintf(buffer, "%03d", ms);
        std::string milliseconds(buffer);
        return milliseconds;

    #else
        #if defined(_WIN32_WCE)
        return ((unsigned long long)GetTickCount()) * 1000;
        #else
        return ((unsigned long long)timeGetTime()) * 1000;
        #endif
        #endif
}




//Returns a long int with the current microsecond time
long int SerialDevice::getMicros()
{
    struct timeval now;
    gettimeofday( &now, NULL );
    return now.tv_usec;
}


//Creates a folder and file with a unique timestamp every time the program is run
//Return 1 if the file was opened/created successfully
//Return -1 if there was an error opening the file
char SerialDevice::createFile(string foldername)
{
    //Create the filename with timestamp
    string filename = name + "_" + timestamp(time(0)) + ".txt";

    //Create the filepath
    string filepath = foldername + "/" + filename;


    //cout << filename << endl;

    //Opens an existing .txt file.
    file.open(filepath.c_str());

    //Checks to see if the file was opened
    //If the file did not open successfully, create the file
    //Check to see if the file was created successfully
    if (!file.is_open())
    {
        file.open(filepath.c_str(), ofstream::in | ofstream::out | ofstream::trunc);
        if (!file.is_open())
        {
            stringstream ss;
            ss << name << " ERROR: file creation --- could not open file.";
            errorMsgs.push_back(ss.str());
            return -1;
        }
    }

    return 1;
}




//Opens a device's configuration file specified by config_path
//Reads parameters from the file and assigns them to device variables
//Returns 1 if the config file opened and closed successfully
//Returns 0 if the file didn't open
char SerialDevice::configRead(string config_path)
{
    //cout << "configRead called! Path: " << config_path << endl;
    //Initiates the variables
    string buf;
    ifstream file;
    char arr[50];
    int baud_rate;

    //Opens the configuration file
    file.open(config_path.c_str());

    //Check if the file opened successfully
    if (file.is_open())
    {
        //Parse the file until the end is reached
        while ( file.good() )
        {
            //If NAME= is found in the file, assign the variable to this->name
            getline (file, buf);
            if ((sscanf(buf.c_str(), "NAME=%s", arr)) == 1)
            {
                this->name = arr;
                cout << "Name: " << this->name << endl;;
                memset(&arr[0], 0, sizeof(arr));
            }

            //If PATH= is found in the file, assign the variable to this->comPort
            else if ((sscanf(buf.c_str(), "PATH=%s", arr)) == 1)
            {
                this->comPort = arr;
                cout << "Path: " << this->comPort << endl;;
                memset(&arr[0], 0, sizeof(arr));
            }

            //If BAUD= is found in the file, assign the variable to this->baud
            else if ((sscanf(buf.c_str(), "BAUD=%d", &baud_rate)) == 1)
            {
                this->baud = baud_rate;
                cout << "Baud: " << this->baud << endl;;
            }
            //cout << buf << endl;

            //If DISPLAY_DATA is found in the file, assign the value to bDisplay
            else if ((sscanf(buf.c_str(), "DISPLAY_DATA=%s", arr)) == 1)
            {
                if ((strcmp(arr, "Y") == 0))
                {
                    bDisplay = true;
                    display = "Y";
                }
                else
                {
                    bDisplay = false;
                    display = "N";
                }
                cout << "DISPLAY_DATA: " << display << endl;
                memset(&arr[0], 0, sizeof(arr));
            }

            //If TRIM_DATA is found in the file, assign the value to bTrim
            else if ((sscanf(buf.c_str(), "TRIM_DATA=%s", arr)) == 1)
            {
                if ((strcmp(arr, "Y") == 0))
                {
                    bTrim = true;
                    trim = "Y";
                }
                else
                {
                    bTrim = false;
                    trim = "N";
                }
                cout << "TRIM_DATA: " << trim << endl;
                memset(&arr[0], 0, sizeof(arr));
            }
        }
        file.close();
        return 1;
    }
    else
    {
        cout << "Error opening file! Make sure config file path is correctly set" << endl;
        return 0;
    }
}


