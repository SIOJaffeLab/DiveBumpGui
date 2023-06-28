#ifndef UUV_H
#define UUV_H

#include "SerialDevice.h"

class UUV : public SerialDevice
{
public:

    UUV(string config_path) : SerialDevice(config_path) {};

    char Start(string foldername);
    char threadedFunction();
    static void* entryPoint(void*);
    int parseData(string s, char firstchar, char lastchar);
    string setTime();
    string setDate();
    char timeInsert(string s);
    bool sendCommand(string command);
};

#endif

