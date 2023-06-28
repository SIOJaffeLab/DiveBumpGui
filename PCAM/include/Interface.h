#ifndef INTERFACE
#define INTERFACE

#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "VN100.h"
#include "SBE39.h"
#include "Battery.h"
#include "PA200.h"
#include "UUV.h"

using namespace std;

class Interface
{

public:

    Interface();
    ~Interface();

    char Setup();
    char Start();
    char Loop ();
    char cmdInterpreter();
    char getData();
    char input_wait();
    char configRead(string config_path);
    char Menu();
    char getInput(char input[]);
    char intConvert(char value[], int range_min, int range_max, int &result);
    char deviceConfig(Device *device, string config_path, bool trim);
    char interfaceConfig(string config_path);

    string createFolder();
    string timestamp(long timestamp);

protected:

    int loop_time;

    VN100* vn100;
    SBE39* sbe39;
    Battery* batt1;
    PA200* pa200;
    UUV* uuv;

    string newdata;
    string olddata;

    string get_selfpath();
    string selfpath;

};

#endif
