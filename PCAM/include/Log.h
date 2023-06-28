#include <string>

#include "Poco/Logger.h"
#include "Poco/ConsoleChannel.h"
#include "Poco/SimpleFileChannel.h"
#include "Poco/AutoPtr.h"
#include "Poco/PatternFormatter.h"
#include "Poco/FormattingChannel.h"


using Poco::Logger;
using Poco::AutoPtr;
using Poco::SimpleFileChannel;
using Poco::ConsoleChannel;
using Poco::PatternFormatter;
using Poco::FormattingChannel;
using Poco::Message;


using namespace std;

class Log 
{
	public:
		Log(string outputLog,bool quietLog,bool verboseLog);
		~Log();
		void info(string msg);
		void error(string msg);
		void fatal(string msg);
		void fileNotice(string msg);
	private:
		AutoPtr<SimpleFileChannel> pChannel;
		AutoPtr<PatternFormatter> pPF;
		AutoPtr<FormattingChannel> pFC;
		Logger * fileLogger;
		Logger * consoleLogger;
};