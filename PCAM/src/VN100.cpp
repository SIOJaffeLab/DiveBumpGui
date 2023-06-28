#include "VN100.h"

//Creates a folder and text file with a unique timestamp id
//Opens serial port
//Starts data logging thread
char VN100::Start(string foldername)
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
    errno = pthread_create(&thread, 0, VN100::entryPoint, this);
    if (errno)
    {
         cout << "ERROR; return code from pthread_create() is " << errno << endl;
         return errno;
    }
    return 1;
}

//Allows the VN100 to enter a thread within its own class
void *VN100::entryPoint(void* pthis)
{
    reinterpret_cast<VN100 *>(pthis)->threadedFunction();
	return pthis;
}

//VN100 thread: grabs 137 bytes of data (amount of bytes in one line) from the device at a time
//tempBuf gets written to the data file after a timestamp is inserted
//parseBuf is outputted to the console after a complete line has been extracted
//microsecond timer is subtracted from uSleep to have consistent sleep times
void VN100::threadedFunction()
{
    //Initiate microsecond timer and millisecond timestamp variables
    long int startMicros = 0;
    long int stopMicros = 0;
    long int micros = 0;
    string msTime;

    //Initiate boolean variables
    quit = 0;
    bNew = false;

    int bAvail;

    cout << "Succesfully started " << this->name << " thread!" << endl;

    //Reads 137 bytes (bytes in one complete data line) to buffer
    //While loop continues until user sends the stop command and sets the quit boolean to true
    while(!quit)
    {
        //Microsecond timer start value
        startMicros = getMicros();

        //Check how many bytes are available on the device
        bAvail = device.Available();

        //Perform operations if at least a whole lines worth of bytes are available
        if (bAvail > 136)
        {
            //cout << bAvail << endl;

            //Read whole lines worth of bytes from the device
            device.Read(buffer, 136, 0);

            //cout << buffer << endl;
            tempBuf = buffer;
            parseBuf += buffer;
            //cout << tempBuf;

            //Inserts timestamp and writes data to .txt file. Checks to see if write was successful
            timeInsert(tempBuf);
            if (file.bad())
                cout << name << " failed to write to file." << endl;

            //Parse a complete line of data to be outputted to the console if DISPLAY_DATA=Y
            if (bDisplay)
                parseLine(parseBuf, '$', '\n');

            //Clears the buffer array
            memset(&buffer[0], 0, sizeof(buffer));
        }
        //Stop value of microsecond timer
        stopMicros = getMicros();

        //Microseconds elapsed since beginning of the while loop
        micros = stopMicros - startMicros;

        //Amount of time to sleep per loop
        //Sleeps for 4ms if microsecond timer returns > 4000 or < 0
        if (micros > 4000 || micros < 0)
            usleep ((4 * 1000));
        else
            usleep((4 * 1000) - micros);
    }

    // Close device and file
    device.Close();
    file.close();
    cout << name << " successfully closed!" << endl;

    //Tells Device::Stop() that it is safe to exit the thread
    bStop = true;
    pthread_exit(0);
}

//Parses data to find the most recent, complete line of data


//Sets bNew to true if a new line of data was extracted
int VN100::parseLine(string s, char firstchar, char lastchar)
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
                    if (bTrim)
                        trimData(s);
                    else
                        rawData = s;

                    //Sets bNew to true if a new line of data was extracted
                    bNew = true;
                    //cout << rawData;

                    s.clear();
                    parseBuf.clear();

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
char VN100::timeInsert(string s)
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
            msTime = "$TIME," + DataTimestamp(time(0), true) + ",";
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

char VN100::trimData(string s)
{
    int array_size = 15;
    vector<string> dataBits (array_size);
    std::fill(dataBits.begin(), dataBits.end(), "0");
    //cout << dataBits[0] << endl;
    int i = 0;
    int length = 0;
    string::iterator it;
    for (it = s.begin(); it < s.end(); it++)
    {
        if ((*it==',')&&(i<14)) {
            if (s.size() > 0) {
                length = it - s.begin();
                //cout << "post length" << endl;
                //cout << i << endl;
                dataBits[i] = s.substr(0, length);
                //cout << "post substr" << endl;
                s.erase(0, length+1);
                //cout << "post erase" << endl;
                i++;
                it = s.begin();
            }
        }
    }
    /*for (int i = 0; i < array_size; i++)
    {
        if (dataBits[i].size() <= 0)
            dataBits[i] = '\0';
    }
    cout << "post padding" << endl;*/

    rawData = dataBits[0] + "," + dataBits[1] + "," + dataBits[2] + "," + dataBits[3] + "," + dataBits[7] + "," + dataBits[8] + "," + dataBits[9] + "\n";
    //cout << "post raw data" << endl;
    //cout << rawData;
    return 1;
}




