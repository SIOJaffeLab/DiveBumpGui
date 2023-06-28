#include "SBE49.h"

//Creates a folder and text file with a unique timestamp id
//Opens serial port
//Starts data logging thread
char SBE49::Start(string foldername)
{
    signed int errno;

    //Sets bStop to false to prevent the program from quitting before all the devices/files have closed
    bStop = false;

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

    //Starts the data logging thread
    pthread_t thread;
    errno = pthread_create(&thread, 0, SBE49::entryPoint, this);
    if (errno)
    {
         stringstream ss;
         ss << name << " ERROR: pthread_create() --- return code from pthread_create() is " << errno;
         errorMsgs.push_back(ss.str());
         return errno;
    }
    return 1;
}

//Allows the SBE49 to enter a thread within its own class
void *SBE49::entryPoint(void* pthis)
{
    reinterpret_cast<SBE49 *>(pthis)->threadedFunction();
    return pthis;
}

//SBE49 thread: grabs all the bytes available on a device's buffer and writes them to a data file
//msTimestamp is inserted at the beginning of each data line
//Calls parseData to extract one complete line of data from tempBuf to be outputted to the console
char SBE49::threadedFunction()
{
    quit = false;
    int bAvail;
    bNew = false;
    bool fileAlreadyBad = false;
    //int ms = 100;

    device.WriteString("Start\r\n"); // May not be needed but just in case
    // Posibly start pump here too?

    //cout << "Succesfully started " << this->name << " thread!" << endl;

    //Reads all bytes available on device every 100 ms
    //Writes data out to the console and a file
    while(!quit)
    {
        //Check how many bytes are available on the device
        bAvail = device.Available();
        //cout << bAvail << endl;
        //Make sure at least one line's worth of bytes are available
        if (bAvail > 22)
        {
            //Read all bytes available on device to buffer
            device.Read(buffer, bAvail, 0);
            //cout << buffer << endl;
            //Get the timestamp
            //msTime = "\n$TIME," + DataTimestamp(time(0), true);

            //cout << buffer;
            tempBuf = buffer;
            parseBuf += buffer;
            //cout << parseBuf;

            // Write time and string to file
            timeInsert(tempBuf);
            if (file.bad() && !fileAlreadyBad) {
                fileAlreadyBad = true;
                stringstream ss;
                ss << name << " ERROR: file output --- return code from file.bad() is " << file.bad();
                errorMsgs.push_back(ss.str());
            }

            //Parse a complete line of data to be outputted to the console if DISPLAY_DATA=Y
            if (bDisplay)
                parseData(parseBuf, '\n', '\n');

            //Clears the buffer array
            memset(&buffer[0], 0, sizeof(buffer));
        }

        //Amount of time to sleep per loop
        usleep(10 * 1000);
    }

    //Stops SBE49 logging
    //device.WriteString("stop\r\n");
    sleep(1);
    // Close device and file
    device.Close();
    file.close();
    //cout << name << " successfully closed!" << endl;

    //Sets the boolean to exit the while loop inside Device::Stop()
    bStop = true;
    pthread_exit(0);
}





//Parses data to find the most recent, complete line of data
int SBE49::parseData(string s, char firstchar, char lastchar)
{

    // Example raw string: 051E8409E6EE08262E5562
    for (string::iterator it = s.end(); it >= s.begin(); --it)
    {
        //Searches backwards for lastchar and removes everything after it
        if (*it == lastchar)
        {
            s.erase(it, s.end());

            for (string::iterator it = s.end(); it >= s.begin(); --it)
            {
                //Searches backwards for firstchar and removes everything before it
                if (*it == firstchar)
                {
                    s.erase(s.begin(), it+1);
                    //rawData = name + ", " + s;
                    rawData = s.substr(0,23);
                    //Sets bNew to true if a new line of data was extracted
                    bNew = true;
                    //cout << rawData;
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

//Inserts a timestamp at the beginning of every data line
//Returns 1 if newline was found and timestamp was inserted
//Returns 0 if no newline was found
char SBE49::timeInsert(string s)
{
    int length;
    int pos;

    //Searches for the first newline character and inserts a timestamp after
    for (string::iterator it = s.begin(); it < s.end() ; ++it)
    {
        if (*it == '\n')
        {
            length = it - s.begin();
            file.write(s.substr(0, length).c_str(), length);

            //Writes the timestamp after the first newline character to the data file
            msTime = DataTimestamp(time(0), true) + " : ";
            file.write(msTime.c_str(), msTime.size());

            //Writes the remaining data to the data file
            pos = length;
            length = s.end() - it;
            file.write(s.substr(pos, length).c_str(), length);
            return 1;
        }
    }

    //Writes the entire string to the data file if no newline was found
    file.write(s.c_str(), s.size());
    return 0;
}

void SBE49::parseSBE49String(string s, double &sbe49Temp, double &sbe49Depth, double &sbe49Cond) {

    //cout << s.length() <<endl;
    if (s.length() != 23)
        return;

    Poco::NumberParser np;

    // Get the hex string from the bulk string
    string tempHex = s.substr(0,6);
    string condHex = s.substr(6,6);
    string presHex = s.substr(12,6);
    string ptcompHex = s.substr(18,4);

    unsigned tempRaw;
    if (!np.tryParseHex(tempHex,tempRaw))
        return;
    unsigned condRaw;
    if (!np.tryParseHex(condHex,condRaw))
        return;
    unsigned presRaw;
    if (!np.tryParseHex(presHex,presRaw))
        return;
    unsigned ptcompRaw;
    if (!np.tryParseHex(ptcompHex,ptcompRaw))
        return;



    // Temperature
    double mv = ((double)tempRaw - 524288)/1.6e7;
    double r = (mv*2.295e10+9.216e8)/(6.144e4-mv*5.3e5);
    sbe49Temp = 1/((TA0+TA1*log(r)+TA2*pow(log(r),2)+TA3*pow(log(r),3))) - 273.15;

    // Pressure
    double y = ((double)ptcompRaw)/13107.0;
    double t = PTEMPA0 + PTEMPA1*y + PTEMPA2*pow(y,2);
    double x = (double)presRaw - PTCA0 - PTCA1*t - PTCA2*pow(t,2);
    double n = x*PTCB0/(PTCB0 + PTCB1*t + PTCB2*pow(t,2));
    double psia = PA0 + PA1*n + PA2*pow(n,2);
    sbe49Depth = (psia - 14.7)*0.689476;

    // Conductivity
    double f = ((double)condRaw)/1000/256;
    sbe49Cond = (G+H*pow(f,2)+I*pow(f,3)+J*pow(f,4))/(1+CTCOR*sbe49Temp+CPCOR*sbe49Depth);
}
