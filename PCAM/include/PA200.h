#ifndef PA200_H
#define PA200_H

#include "SerialDevice.h"

class PA200 : public SerialDevice
{
public:

    PA200(string config_path) : SerialDevice(config_path) {};
    PA200(string name, string port, int baud, bool display, bool trim) : SerialDevice(name, port, baud, display, trim) {};
    char Start(string foldername);
    char threadedFunction();
    static void* entryPoint(void*);
    int parseData(string s, char firstchar, char lastchar);
    char timeInsert(string s);
    string parseBuf;
};

#endif


