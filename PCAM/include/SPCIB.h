#ifndef SPCIB_H
#define SPCIB_H

#include "SerialDevice.h"

class SPCIB : public SerialDevice
{
public:

    SPCIB(string config_path) : SerialDevice(config_path) {};
    SPCIB(string name, string port, int baud, bool display, bool trim) : SerialDevice(name, port, baud, display, trim) {};
    char Start(string foldername);
    char threadedFunction();
    static void* entryPoint(void*);
    int parseData(string s, char firstchar, char lastchar);
    string setTime();
    string setDate();
    char timeInsert(string s);
	int linesPerData;
	int lineCount;
	bool fileAlreadyBad;
	string lineData;
	bool ctrlLine;
	bool sensLine;
	bool sbe49Line;

};

#endif

