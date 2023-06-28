#include "Log.h"        
		
		
Log::Log(string outputLog,bool quietLog = false,bool verboseLog = false)
{
		
	pChannel = new SimpleFileChannel();
	pPF = new PatternFormatter();
	pPF->setProperty("pattern", "%Y-%m-%d %H:%M:%S : %t");
	pFC = new FormattingChannel(pPF, pChannel);
	pChannel->setProperty("path", outputLog);
	pChannel->setProperty("rotation", "10 M");

	// Logger for the console
	consoleLogger = &(Logger::get("_ConsoleLogger"));
	AutoPtr<ConsoleChannel> pCC(new ConsoleChannel);
	consoleLogger->setChannel(pCC);

	//Logger for the log file
	fileLogger = &(Logger::get("_FileLogger")); // inherits root channel
	fileLogger->setChannel(pFC);

	if (quietLog) {
		consoleLogger->setLevel(Message::PRIO_ERROR);
	}
	else
		consoleLogger->setLevel(Message::PRIO_INFORMATION);

	if (verboseLog) {
		fileLogger->setLevel(Message::PRIO_TRACE);
	}
	else
		fileLogger->setLevel(Message::PRIO_INFORMATION);
		
}

Log::~Log() {
	
}

void Log::info(string msg) {
	consoleLogger->information(msg);
	fileLogger->information(msg);
}

void Log::error(string msg) {
	consoleLogger->error(msg);
	fileLogger->error(msg);
}

void Log::fatal(string msg) {
	consoleLogger->fatal(msg);
	fileLogger->fatal(msg);
}

void Log::fileNotice(string msg) {
	fileLogger->notice(msg);
}