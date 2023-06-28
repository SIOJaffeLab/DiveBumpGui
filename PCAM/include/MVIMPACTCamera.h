#ifndef MVIMPACTCAMERA_H
#define MVIMPACTCAMERA_H

#include <stdio.h>
#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <errno.h>
#include <time.h>
#include <mvIMPACT_Aquire/mvIMPACT_CPP/mvIMPACT_acquire.h>
#include <mvIMPACT_Aquire/mvIMPACT_CPP/mvIMPACT_acquire_GenICam.h>
#include <mvIMPACT_Aquire/common/exampleHelper.h>
#include "ofThread.h"
#include "Camera.h"


using namespace std;
//using namespace mvIMPACT::acquire; // This corrupts the serial Device namespace

	
class MVIMPACTCamera : public Camera
{
	public:
		MVIMPACTCamera();
		MVIMPACTCamera(AbstractConfiguration * cfg);
		~MVIMPACTCamera();
		void threadedFunction();
		void startAcquisition();
		void stopAcquisition();
		void updateDataDir(string dataDir);
		void addHandler(DataProcessor * handler);
		void setCameraName(string name);
        void setGain(int gain);
        void setExposure(int exposure);
        void setBinning(int binning);
        bool isRunning();
        bool grabEnable;
		
	private:
		mvIMPACT::acquire::DeviceManager devMgr;
		mvIMPACT::acquire::Device* pDev;
		bool cameraClosed;
        bool cameraThreadRunning;
		int horzDecim;
		int vertDecim;
		int horzBinning;
		int vertBinning;
		string imgPixelFormat;
		double redGain;
		double blueGain;
		string cameraName;
		
};

#endif