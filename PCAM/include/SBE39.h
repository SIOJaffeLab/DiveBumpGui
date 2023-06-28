#ifndef SBE39_H
#define SBE39_H

#include "SerialDevice.h"

class SBE39 : public SerialDevice
{
public:

    SBE39(string config_path) : SerialDevice(config_path) {};
    SBE39(string name, string port, int baud, bool display, bool trim) : SerialDevice(name, port, baud, display, trim) {};
    char Start(string foldername);
    char threadedFunction();
    static void* entryPoint(void*);
    int parseData(string s, char firstchar, char lastchar);
    string setTime();
    string setDate();
    char timeInsert(string s);
};

#endif

