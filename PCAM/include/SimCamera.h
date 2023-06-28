#include <stdio.h>
#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <errno.h>
#include <time.h>
#include "ofThread.h"
#include "Camera.h"
#include "Poco/Stopwatch.h"
#include "Poco/Timestamp.h"
#include "Poco/Path.h"
#include "Poco/File.h"

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"


using namespace std;
using Poco::Timestamp;
using Poco::Stopwatch;
using namespace cv;

	
class SimCamera : public Camera
{
	public:
		SimCamera();
		SimCamera(AbstractConfiguration * cfg);
		~SimCamera();
		void threadedFunction();
		void startAcquisition();
		void stopAcquisition();
		void generateImages();
		void updateDataDir(string dataDir);
		void addHandler(DataProcessor * handler);
		void setCameraName(string name);
		
	private:
	
		// vector of images for simulation
        vector<Mat> simulationImages;
		bool simulatorMode;
		bool saveSimImages;
        int simulatorImages;
        int simulatorImageRois;
		int frameNumber;
		int maxRadius;
		bool motionSim;
		double motionSimRate;
		unsigned int binningX;
		unsigned int binningY;
		
		
};