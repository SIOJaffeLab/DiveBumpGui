#include "SPCIB.h"

//Creates a folder and text file with a unique timestamp id
//Opens serial port
//Starts data logging thread
char SPCIB::Start(string foldername)
{
    signed int errno;

    //Sets bStop to false to prevent the program from quitting before all the devices/files have closed
    bStop = false;
	
	ctrlLine = false;
	sensLine = false;
	sbe49Line = false;

    //Creates folder and data file to log to
    createFile(foldername);
	
	fileAlreadyBad = false;
	
	linesPerData = 3;

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
    errno = pthread_create(&thread, 0, SPCIB::entryPoint, this);
    if (errno)
    {
         stringstream ss;
         ss << name << " ERROR: pthread_create() --- return code from pthread_create() is " << errno;
         errorMsgs.push_back(ss.str());
         return errno;
    }
    return 1;
}

//Allows the SPCIB to enter a thread within its own class
void *SPCIB::entryPoint(void* pthis)
{
    reinterpret_cast<SPCIB *>(pthis)->threadedFunction();
    return pthis;
}

//SPCIB thread: grabs all the bytes available on a device's buffer and writes them to a data file
//msTimestamp is inserted at the beginning of each data line
//Calls parseData to extract one complete line of data from tempBuf to be outputted to the console
char SPCIB::threadedFunction()
{
    quit = false;
    int bAvail;
    bNew = false;
    //int ms = 100;

    //device.WriteString("hello!\n"); // May not be needed but just in case
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
        if (bAvail > 0)
        {

            //Read all bytes available on device to buffer
            device.Read(buffer, bAvail, 0);
            //cout << buffer << endl;
            //Get the timestamp
            //msTime = "\n$TIME," + DataTimestamp(time(0), true);

            tempBuf = buffer;
            parseBuf += buffer;
            //cout << parseBuf;
			
			timeInsert(tempBuf);
            if (file.bad() && !fileAlreadyBad) {
                fileAlreadyBad = true;
                stringstream ss;
                ss << name << " ERROR: file output --- return code from file.bad() is " << file.bad();
                errorMsgs.push_back(ss.str());
            }

            //Parse a complete line of data to be outputted to the console if DISPLAY_DATA=Y
            if (bDisplay)
                parseData(parseBuf, '$', '\n');

            //Clears the buffer array
            memset(&buffer[0], 0, sizeof(buffer));
        }

        //Amount of time to sleep per loop
        usleep(5 * 1000);
    }

    //Stops SPCIB logging
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
int SPCIB::parseData(string input, char firstchar, char lastchar)
{
	if (ctrlLine && sensLine && sbe49Line) {
		rawData = lineData;
		lineData.clear();
		ctrlLine = false;
		sensLine = false;
		sbe49Line = false;
		bNew = true;
	}
	
    // Example raw string: 051E8409E6EE08262E5562
	string s = input;
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
					//cout << s << endl;
					if (s.at(0) == 'C' && !ctrlLine) {
						lineData += "$" + s + "\n";
						ctrlLine = true;
					}
					if (s.at(0) == 'S' && s.at(1) == 'E' && !sensLine) {
						lineData += "$" + s + "\n";
						sensLine = true;
					}
					if (s.at(0) == 'S' && s.at(1) == 'B' && !sbe49Line) {
						lineData += "$" + s + "\n";
						sbe49Line = true;
					}
                    //Sets bNew to true if a new line of data was extracted
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
char SPCIB::timeInsert(string s)
{
	int length;
	int pos;
	
	/*string logString = DataTimestamp(time(0), true) + " : " + s;
	if (logString.at(s.length()-1) != '\n')
		logString = logString+ '\n';
	file.write(logString.c_str(), logString.size());
	*/
    //Searches for the first newline character and inserts a timestamp after
    for (string::iterator it = s.begin(); it < s.end() ; ++it)
    {
        if (*it == '$')
        {
            //Writes data up to the first new line character to the data file
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

    file.write(s.c_str(), s.size());
    
	return 0;
	
}
