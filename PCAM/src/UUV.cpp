#include "UUV.h"

//Creates a folder and text file with a unique timestamp id
//Opens serial port
//Starts data logging thread
char UUV::Start(string foldername)
{
    signed int errno;

    //Sets bStop to false to prevent the program from quitting before all the devices/files have closed
    bStop = false;

    //Creates folder and data file to log to
    createFile(foldername);

    //Tries to open to serial device
    //Displays error number if unsuccessful
    errno = openPort(comPort, baud);

    //Starts the data logging thread
    pthread_t thread;
    errno = pthread_create(&thread, 0, UUV::entryPoint, this);
    if (errno)
    {
         cout << "ERROR; return code from pthread_create() is " << errno << endl;
         return errno;
    }
    return 1;
}

//Allows the UUV to enter a thread within its own class
void *UUV::entryPoint(void* pthis)
{
    reinterpret_cast<UUV *>(pthis)->threadedFunction();
	return pthis;
}

//UUV thread: grabs all the bytes available on a device's buffer and writes them to a data file
//msTimestamp is inserted at the beginning of each data line
//Calls parseData to extract one complete line of data from tempBuf to be outputted to the console
char UUV::threadedFunction()
{
    quit = false;
    int bAvail;
    bNew = false;
    int ms = 100;

     //Sets all the parameters and activates logging for the UUV
    usleep(ms*1000);
    device.WriteString("~/uuvControl\n");

    usleep(ms*1000);

    cout << "Succesfully started " << this->name << " thread!" << endl;

    //Reads all bytes available on device every 100 ms
    //Writes data out to the console and a file
    while(!quit)
    {
        //Check how many bytes are available on the device
        bAvail = device.Available();

        //Make sure at least one line's worth of bytes are available
        if (bAvail >= 29)
        {
            //Read all bytes available on device to buffer
            device.Read(buffer, bAvail, 0);

            //Get the timestamp
            //msTime = "\n$TIME," + DataTimestamp(time(0), true);

            //cout << buffer;
            tempBuf = buffer;
            parseBuf += buffer;

            timeInsert(tempBuf);

            //Writes timestamp and data to .txt file. Checks to see if write was successful
            //file.write(msTime.c_str(), msTime.size());
            //file.write(buffer, bAvail);
            if (file.bad())
                cout << name << " failed to write to file." << endl;

            //Parse a complete line of data to be outputted to the console if DISPLAY_DATA=Y
            if (bDisplay)
                parseData(parseBuf, '$', '\n');

            //Clears the buffer array
            memset(&buffer[0], 0, sizeof(buffer));
        }

        //Amount of time to sleep per loop
        usleep(10 * 1000);
    }

    //Stops UUV logging
    //device.WriteString("stop\r\n");
    sleep(1);
    // Close device and file
    device.Close();
    file.close();
    cout << name << " successfully closed!" << endl;

    //Sets the boolean to exit the while loop inside Device::Stop()
    bStop = true;
    pthread_exit(0);
}





//Parses data to find the most recent, complete line of data
int UUV::parseData(string s, char firstchar, char lastchar)
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

string UUV::setTime()
{
      //Gets current time since epoch
    const time_t rawtime = (const time_t)time(0);

    struct tm * dt;
    char timestr[30];
    char buffer [30];

    //Converts rawtime into local time
    dt = localtime(&rawtime);

    //Formats time to be human readable
    strftime(timestr, sizeof(timestr), "%H%M%S", dt);
    sprintf(buffer,"%s", timestr);
    std::string stdBuffer(buffer);

    //Return the timestamp string
    return stdBuffer;
}

string UUV::setDate()
{
      //Gets current time since epoch
    const time_t rawtime = (const time_t)time(0);

    struct tm * dt;
    char timestr[30];
    char buffer [30];

    //Converts rawtime into local time
    dt = localtime(&rawtime);

    //Formats time to be human readable
    strftime(timestr, sizeof(timestr), "%m%d%y", dt);
    sprintf(buffer,"%s", timestr);
    std::string stdBuffer(buffer);

    //Return the timestamp string
    return stdBuffer;
}


//Inserts a timestamp at the beginning of every data line
//Returns 1 if newline was found and timestamp was inserted
//Returns 0 if no newline was found
char UUV::timeInsert(string s)
{
    int length;
    int pos;

    //Searches for the first newline character and inserts a timestamp after
    for (string::iterator it = s.begin(); it < s.end() ; ++it)
    {
        if (*it == '\n')
        {
            //Writes data up to the first new line character to the data file
            length = it - s.begin();
            file.write(s.substr(0, length).c_str(), length);

            //Writes the timestamp after the first newline character to the data file
            msTime = "\n$TIME," + DataTimestamp(time(0), true) + ",";
            file.write(msTime.c_str(), msTime.size());

            //Writes the remaining data to the data file
            pos = length + 1;
            length = s.end() - it - 1;
            file.write(s.substr(pos, length).c_str(), length);
            return 1;
        }
    }

    //Writes the entire string to the data file if no newline was found
    file.write(s.c_str(), s.size());
    return 0;
}

bool UUV::sendCommand(string command) {
    string str(command+"\n"); 
    device.WriteString(str.c_str());
    return true;
}
