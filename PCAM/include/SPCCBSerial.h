#ifndef SPCCBSerial_H
#define SPCCBSerial_H

#include "SerialDevice.h"

class SPCCBSerial : public SerialDevice
{
public:

    SPCCBSerial(string config_path) : SerialDevice(config_path) {};
    SPCCBSerial(string name, string port, int baud, bool display, bool trim) : SerialDevice(name, port, baud, display, trim) {};
    char Start(string foldername);
    void startClean();
    void stopClean();
    char threadedFunction();
    static void* entryPoint(void*);
    int parseData(string s, char firstchar, char lastchar);
    string setTime();
    string setDate();
    char timeInsert(string s);

    bool startclean;
    bool stopclean;
};

#endif

