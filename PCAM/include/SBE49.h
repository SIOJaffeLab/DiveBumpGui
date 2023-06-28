#ifndef SBE49_H
#define SBE49_H

#include "SerialDevice.h"
#include "Poco/NumberParser.h"
#include "SBE49Params.h"
#include <math.h>

class SBE49 : public SerialDevice
{
public:

    SBE49(string config_path) : SerialDevice(config_path) {};
    SBE49(string name, string port, int baud, bool display, bool trim) : SerialDevice(name, port, baud, display, trim) {};
    char Start(string foldername);
    char threadedFunction();
    static void* entryPoint(void*);
    int parseData(string s, char firstchar, char lastchar);
    string setTime();
    string setDate();
    char timeInsert(string s);
    void parseSBE49String(string s, double &sbe49Temp, double &sbe49Depth, double &sbe49Cond);
};

#endif

