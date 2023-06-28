#pragma once
#include "ofThread.h"
#include "DataProcessor.h"
#include "Poco/Path.h"
#include "Poco/File.h"
#include <iomanip>


class RawFileWriter :
	public DataProcessor
{
public:
	RawFileWriter();
	RawFileWriter(AbstractConfiguration * cfg);
	~RawFileWriter(void);
	virtual void processDataEntry(DataEntry* entry);
	void setFileNameIndex(string index);
	void setSaveToDisk(bool save);
	void setFramesPerFile(int nFrames);
	virtual void openNewFile();
	void updateDataDir(string dataDir);
	void closeOpenFile();
	bool isFull();
	string status();

protected:
	string dataDir;
	string filePrefix;
	string filepath;
	string fileNameIndex;
	unsigned int counter;
	unsigned int framesPerFile;
	ofstream outputFile;
	bool saveToDisk;


};

