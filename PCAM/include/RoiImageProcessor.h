#pragma once
#include "ofThread.h"
#include "DataProcessor.h"
#include "CameraImage.h"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/imgcodecs/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "Poco/String.h"

#include "Poco/Path.h"
#include "Poco/File.h"

using namespace cv;
using namespace Poco::Util;
using Poco::toLower;

class RoiImageProcessor :
	public DataProcessor
{
public:
	RoiImageProcessor(AbstractConfiguration * cfg);
	~RoiImageProcessor(void);
	void updateDataDir(string dataDir);
	void processDataEntry(DataEntry* entry);
	void setSaveToDisk(bool save);
    void updateShrinkDir(string shrinkDir);
	int getTotalRois();
	int getRecentRoiCount();
	bool isBusy();
	bool isFull();
	string status();

private:

	bool busy;

	string dataDir;
	bool saveToDisk;
	
	bool checkArea;
	bool checkMal;

	string shrinkDir;
    string networkDir;
	int shrinkThreshold;
    int shrinkSubSample;
	bool saveShrink;
    bool saveNetworkImage;
    
    unsigned int networkImageIndex;
    unsigned int numNetworkImages;
    unsigned int networkSubSample;

    unsigned int rawImgWidth;
    unsigned int rawImgHeight;
    unsigned int rawImgOffsetX;
    unsigned int rawImgOffsetY;
	unsigned int binningX;
	unsigned int binningY;
    int downsampleFactor;
    int bwThreshold;
    int sel7Size;
    int sel3Size;
    int laplaceSize;
	double minContourArea;
    double minArea;
    double maxArea;
	double minMal;
	double maxMal;
    int roiPad;
    int cannyLow;
    int cannyHigh;
    int cannyKernelSize;
    int maxRoisPerImage;
	int statusFilterSize;
	int pixelFormat;
    string procMethod;
	string cameraName;
	
	int totalRois;
	int recentRoiCount;

    // Vectors for contour finding in images
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    Mat sel7;
    Mat sel3;

    // Allocate mats based on raw and downsample sizes
    int dsWidth;
    int dsHeight;

    // // 16 bit mats for filtering
    Mat dsImg;
    Mat thImg;

    // 8-bit mats for binary ops
    Mat thImg1;
    Mat bw1;
    Mat bw2;
    Mat subSampleImg;
    Mat subSampleThresh;
    
    // Misc variables for processing and saving rois
    int roiX, roiY, roiW, roiH;
    char filename[2048];
    char logString[2048];
    int logTimer;
	
	// Timers
	Poco::Stopwatch procTimer;
	Poco::Stopwatch elapsedTimer;
	
	// Vectors for data stats
	queue<double> areaHistory;
	queue<double> countHistory;
	queue<double> allCountHistory;
	queue<double> malHistory;
	queue<double> maxAreaHistory;
	queue<double> maxMalHistory;


};


