#ifndef VN100_H
#define VN100_H

#include "SerialDevice.h"

class VN100 : public SerialDevice
{
public:

    VN100(string config_path) : SerialDevice(config_path){};
    char Start(string foldername);
    void threadedFunction();
    static void* entryPoint(void*);
    int parseLine(string s, char firstchar, char lastchar);
    char parseBlock(string s, char firstchar);
    char timeInsert(string s);
    char trimData(string s);



private:

    char firstBlock[1000];
    char lastBlock[1000];

};

#endif
