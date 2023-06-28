#ifndef SPCSerial_H
#define SPCSerial_H

#include "SerialDevice.h"

class SPCSerial : public SerialDevice
{
public:

    SPCSerial(string config_path) : SerialDevice(config_path) {};
    SPCSerial(string name, string port, int baud, bool display, bool trim) : SerialDevice(name, port, baud, display, trim) {};
    char Start(string foldername);
    char threadedFunction();
    static void* entryPoint(void*);
    int parseData(string s, char firstchar, char lastchar);
    string setTime();
    string setDate();
    char timeInsert(string s);
};

#endif

