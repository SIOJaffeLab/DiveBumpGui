#include "deviceReader.h"
#define NUM_THREADS 1

using namespace std;

deviceReader::deviceReader() {
	
}

//constructor
deviceReader::deviceReader(const char path, unsigned int baud)
{

}
//destructor
deviceReader::~deviceReader()
{
}

void *vn100(void*)
{
    int bAvail;
    char buffer[10000];
    serialib device;
    signed int errno;

    // Opens or creates vectorNavData.txt file and sets it up for read/write and append to file (if it already exists)
    ofstream file;
    file.open("vectorNavData.txt", ofstream::in | ofstream::out | ofstream::app);

    // Checks to see if the file was successfully opened/created
    if (!file.is_open())
    {
        cout << "\nThere was an error opening the file: -7" << endl;
        exit(-1);
    }


    //Tries to open to serial device
    //Displays error number if unsuccessful
    if ((errno = device.Open("/dev/ttyUSB0", 921600)) != 1)
    {
        cout << "There was an error opening the device: " << errno << endl;
        exit(-1);
    }
    else if (errno == 1)
        cout << "Connection successful!" << endl;

    //Reads all bytes available on device every 100 ms
    //Writes data out to the console and a file
    usleep(100 * 1000);
    while(true)
    {
        bAvail = device.Available();
        device.Read(buffer, bAvail);
        cout << buffer;
        file.write(buffer, bAvail);
        memset(&buffer[0], 0, sizeof(buffer));
        usleep(100 * 1000);
    }

    file.close();
    pthread_exit(0);
}

int main()
{
    deviceReader devices;

    devices.CreateThreads();


    pthread_exit(0);

}

int deviceReader::CreateThreads()
{
    pthread_t threads[1];
    int rc = pthread_create(&threads[0], 0, &vn100, 0);
    if (rc)
    {
         cout << "ERROR; return code from pthread_create() is " << rc << endl;
         return rc;
    }
    return 0;
}


