#include <iostream>
#include "serialib.h"
#include <fstream>
#include <string>
#include <sstream>
#include <pthread.h>

class deviceReader
{

public:

	deviceReader();
    deviceReader(const char path, unsigned int baud);
    ~deviceReader();

    char Start();
    char Stop();
    char GetData();
    int CreateThreads();

protected:
    char path;
    unsigned int baud;


private:


};
