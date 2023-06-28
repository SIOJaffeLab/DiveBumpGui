#pragma once

#include "CameraImage.h"
#include "DataProcessor.h"
#include "ofThread.h"
#include "Poco/Util/AbstractConfiguration.h"
#include "Poco/Util/XMLConfiguration.h"
#include "Poco/Stopwatch.h"
#include "Poco/Timestamp.h"

using Poco::Util::XMLConfiguration;
using Poco::Util::AbstractConfiguration;
using Poco::Timestamp;

class Camera : public ofThread
{
	public:
		Camera();
		Camera(AbstractConfiguration * cfg);
		~Camera();
		virtual void startAcquisition() = 0;
		virtual void stopAcquisition() = 0;
		virtual void addHandler(DataProcessor * handler) = 0;
		virtual void updateDataDir(string dataDir) = 0;
		virtual void setCameraName(string name) = 0;
	protected:
		AbstractConfiguration * cfg;
		DataProcessor * handler;
		
		double camFrameRate;
		double camExposureTime;
		double camGain;
		int  rawImgOffsetX;
		int rawImgOffsetY;
		int rawImgWidth;
		int rawImgHeight;
		int memoryChannel;
		bool restoreSettings;
		double strobe1Delay;
		double strobe2Delay;
		double strobe1Duration;
		double strobe2Duration;
		bool strobe1Enable;
		bool strobe2Enable;
		
		
		string dataDir;
		string cameraName;
};