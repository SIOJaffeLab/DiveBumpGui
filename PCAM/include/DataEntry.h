#pragma once

#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <fstream>

using namespace std;

class DataEntry {

public:

    DataEntry();
    DataEntry(string name);
    DataEntry(string name,unsigned char * data,unsigned int sizeInBytes);
	virtual ~DataEntry();
    DataEntry(const DataEntry& other);
    DataEntry& operator=(const DataEntry& rhs);
	virtual void allocateAndCopyData(unsigned char * data,unsigned int sizeInBytes);
	virtual bool writeToFile(ofstream& outputFile);
	virtual string info();
	virtual unsigned char * getDataPointer();
	virtual unsigned int    getDataSizeInBytes();

	bool bAllocated;

protected:
    string name;
    unsigned int sizeInBytes;
    unsigned char * data;

};

