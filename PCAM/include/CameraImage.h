#pragma once
#include "DataEntry.h"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;
using namespace std;

class CameraImage :
	public DataEntry
{
public:
	CameraImage(void);
	virtual ~CameraImage();
	CameraImage(const CameraImage& other);
	CameraImage& operator=(const CameraImage& rhs);
	void populateImageStats(const CameraImage& rhs);
	bool writeToFile(ofstream& outputFile);
	string info();


	unsigned long long systemUnixTime;
	unsigned long long systemMicros;
	unsigned long long cameraMicros;
	unsigned int frameNumber;
	unsigned int width;
	unsigned int height;
	unsigned int bytesPerPixel;
    unsigned int position;
    unsigned int flashtype;
	
	bool saveJPEG;

};



