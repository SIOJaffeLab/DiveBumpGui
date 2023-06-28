#include "DataProcessor.h"

DataProcessor::DataProcessor() {
	
}

DataProcessor::DataProcessor(AbstractConfiguration * cfg) {
	this->cfg = cfg;
	threadDelay = 10000;
}

DataProcessor::~DataProcessor() {
    // remove all elements from the queue
    while(!dataQueue.empty()) {
		processDataEntry(dataQueue.front());
		dataQueue.pop();
	}
}

void DataProcessor::setThreadDelay(unsigned int delay) {
	threadDelay = delay;
}

void DataProcessor::addEntry(DataEntry* entry) {
	if (dataQueue.size() < maxQueueSize) {
		lock();
		dataQueue.push(entry);
		unlock();
		
	}
	else {
        delete entry;
		log->error("Queue is full, no more memory to add entries.");
	}
}

void DataProcessor::updateDataDir(string dataDir) {
	this-> dataDir = dataDir;
}

string DataProcessor::status() {
	stringstream output;
	lock();
	output<<"QS: "<<dataQueue.size();
	unlock();
    return output.str();
}

void DataProcessor::threadedFunction() {
	
	Poco::Stopwatch timer;
	
	int delay;
	
	finished = false;

    while( isThreadRunning() ) {

		timer.reset();
		timer.start();

        if (!dataQueue.empty()) {
			DataEntry* entry = dataQueue.front();
			lock();
			dataQueue.pop();
			unlock();
			processDataEntry(entry);
		}

		delay = (int)(threadDelay - timer.elapsed())/1000;
		if (delay > 0)
			usleep(delay*1000);

    }
	
	finished = true;
}

void DataProcessor::processDataEntry(DataEntry* entry) {
	cout << entry->info();
	delete entry;
}
