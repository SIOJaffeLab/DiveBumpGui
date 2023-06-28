#ifndef SBE50_H
#define SBE50_H

#include "SerialDevice.h"

class SBE50 : public SerialDevice
{
public:

    SBE50(string config_path) : SerialDevice(config_path) {};
    SBE50(string name, string port, int baud, bool display, bool trim) : SerialDevice(name, port, baud, display, trim) {};
    char Start(string foldername);
    char threadedFunction();
    static void* entryPoint(void*);
    int parseData(string s, char firstchar, char lastchar);
    char timeInsert(string s);
    string parseBuf;
};

#endif


