#include "RawFileWriter.h"

RawFileWriter::RawFileWriter() {
	
}

RawFileWriter::RawFileWriter(AbstractConfiguration * cfg)
{
	counter = 0;
	saveToDisk = true;
	framesPerFile = cfg->getInt("App.FramesPerFile",128);
	filePrefix = "RAW-" + cfg->getString("App.CameraName");
	threadDelay = 10000;
	maxQueueSize = cfg->getInt("Processing.MaxQueueSize",60);
	
}


RawFileWriter::~RawFileWriter(void)
{
	if (outputFile.is_open())
		outputFile.close();
}

void RawFileWriter::processDataEntry(DataEntry* entry) {

	if (!saveToDisk) {
		delete entry;
		return;
	}
	if (!outputFile.is_open())
		openNewFile();
	entry->writeToFile(outputFile);
	delete entry;
	counter++;
	if (counter % framesPerFile == 0) {
		outputFile.close();
	}
}

bool RawFileWriter::isFull() {
	return dataQueue.size() >= maxQueueSize;
}

void RawFileWriter::updateDataDir(string dataDir) {
	bool fileOpen = outputFile.is_open();
	if (isThreadRunning())
		lock();
	if (fileOpen)
		closeOpenFile();
	this-> dataDir = dataDir;
	if (isThreadRunning())
		unlock();
}

void RawFileWriter::setSaveToDisk(bool save) {
	saveToDisk = save;
}

void RawFileWriter::setFramesPerFile(int nFrames) {
	framesPerFile = nFrames;
}

void RawFileWriter::openNewFile() {
	stringstream ss;
	unsigned long long sysTimeMicros = Timestamp().epochMicroseconds();

	if (!saveToDisk)
		return;

	// Close last output file if open
	if (outputFile.is_open())
		outputFile.close();

	ss << setw(20) << setfill('0') << sysTimeMicros << "-" << setw(5) << setfill ('0') << counter;
	Poco::Path outputFilePath(dataDir,filePrefix + "-" + ss.str() + ".bin");
	
	if (isThreadRunning())
		lock();
	outputFile.open(outputFilePath.toString().c_str(),ios::out | ios::binary);
	if (outputFile.bad())
		cout << "Could not open output file." << endl;
	if (isThreadRunning())
		unlock();
}

void RawFileWriter::closeOpenFile() {
	// Close last output file if open
	if (outputFile.is_open())
		outputFile.close();
}

void RawFileWriter::setFileNameIndex(string index) {
	fileNameIndex = index;
}

string RawFileWriter::status() {
	stringstream output;
	output << dataQueue.size();
	return output.str();
}

