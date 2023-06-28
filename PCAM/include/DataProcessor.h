#pragma once

#include <queue>
#include <sstream>
#include <time.h>
#include <unistd.h>
#include "DataEntry.h"
#include "ofThread.h"
#include "Poco/Util/AbstractConfiguration.h"
#include "Poco/Util/XMLConfiguration.h"
#include "Poco/Stopwatch.h"
#include "Poco/Timestamp.h"
#include "Log.h"

using Poco::Util::XMLConfiguration;
using Poco::Util::AbstractConfiguration;
using Poco::Timestamp;


using namespace std;

class DataProcessor : public ofThread {

public:
	DataProcessor();
    DataProcessor(AbstractConfiguration * cfg);
    ~DataProcessor();
	virtual void addEntry(DataEntry* entry);
	virtual void setThreadDelay(unsigned int delay);
	void updateDataDir(string dataDir);
    virtual string status();

protected:
	AbstractConfiguration * cfg;
    virtual void threadedFunction();
    virtual void processDataEntry(DataEntry* entry);
    queue<DataEntry*> dataQueue;
	int threadDelay;
	unsigned int maxQueueSize;
	bool finished;
	Log * log;
	string dataDir;

};

