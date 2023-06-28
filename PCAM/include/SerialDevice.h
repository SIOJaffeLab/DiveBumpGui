#ifndef DEVICE
#define DEVICE

#include <iostream>
#include "serialib.h"
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <cstdio>
#include <ctime>
#include <vector>
#include "ofThread.h"

using namespace std;

class SerialDevice : public ofThread
{

public:

    SerialDevice(string config_path);
    SerialDevice(string name, string port, int baud, bool display, bool trim);
    virtual ~SerialDevice();

    virtual char Start(string foldername);
    virtual void Stop();
    char GetData();

    void threadedFunction();
    void dataGrabber();
    int parseData(string s, char firstchar, char lastchar);
    string timestamp(long timestamp, bool milliseconds);
    string DataTimestamp(long timestamp, bool milliseconds);
    string mstimestamp();
    long int getMicros();
    string createFolder();
    char createFile(string foldername);
    char configRead(string config_path);
    char openPort(string comPort, unsigned int baud);
    string getErrorMessages();

    string name;
    string comPort;
    unsigned int baud;
    bool bTrim;
    string trim;
    string display;
    bool bDisplay;

    serialib device;

    string rawData;
    string formattedData;
    bool bNew;

    string foldername;

protected:

    string msTime;

    bool quit;
    bool bStop;


    string parseBuf;
    string tempBuf;
    vector<string> errorMsgs;

    ofstream file;

};

#endif
