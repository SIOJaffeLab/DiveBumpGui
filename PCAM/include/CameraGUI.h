#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "Poco/Path.h"
#include "Poco/Util/AbstractConfiguration.h"
#include "Poco/Util/XMLConfiguration.h"
#include "Poco/SAX/SAXException.h"

#include "DataProcessor.h"

#include "cvui.h"
#include "EnhancedWindow.h"
#include "BUMPControl.h"
#include "Camera.h"
#include "MVIMPACTCamera.h"

using namespace cv;
using namespace std;

using namespace Poco::Util;
using Poco::AutoPtr;

class CameraGUI : public DataProcessor
{
public:
	CameraGUI(const char * config);
	~CameraGUI();
	void update(Mat inputImage);
    void readConfigFile(string filename);
    void updateControl();
    void doFocalStack();
    void setControl(BUMPControl *);
    void setCamera(Camera * camera);
    bool quitPressed();
    void updateConfig();
    void drawCounter(const char * text, int * value, int min, int max, int inc=-1);
    void applyConfig();
    int getStep(int value);
    void threeRowExclusive(bool *b1, char * t1, bool *b2, char * t2, bool *b3, char * t3);
    
    bool toggleTrigger;
    bool updateSettings;
    
    bool saveRaw;
    bool saveFrame;
    
    int exposureTime;
    int binning;
    int gain;
    
private:
    //helpers
    void HandleESC();
    void HandlePositionIncrement();
    void HandlePositionDecrement();
    void HandleGainIncrement();
    void HandleGainDecrement();
    void HandlePAM();
    void HandleScan();
    void HandleVideo();
    
    void HandleToggleWhite();
    void HandleWhiteIncrement();
    void HandleWhiteDecrement();
    void HandleToggleSat();
    void HandleSatIncrement();
    void HandleSatDecrement();
    void HandleREC();
    
    int S(const int& x) const;
	
private:
    //TODO: needs de krufting, much of this is unused for dive version.
    
    BUMPControl * bumpControl;
    
    Camera * bumpCamera;

    bool validConfig;
    
    string guiName;
    string controlWinName;
    string configName;
    
    string lastSequence;
    
    double scaleGUI;
    double fontSize;
    EnhancedWindow commands;
    EnhancedWindow strobe;
    EnhancedWindow camera;
    EnhancedWindow focalStack;
    EnhancedWindow sequence;
    EnhancedWindow settings;
    EnhancedWindow statusWin;
    EnhancedWindow buttons;
    
    int guiImageWidth;
    int guiImageHeight;
    
    // Close match to the bump-control UI
    int satDur;
    int satPow;
    int measDur;
    int measPow;
    int colorDur;
    
    bool whiteFlash;
    bool measFlash;
    bool satFlash;
    
    bool x1Binning;
    bool x2Binning;
    bool x4Binning;
    
    int actPos;
    int minSweep;
    int maxSweep;
    
    int frameRate;
        
    int triggerType;
    
    bool triggerEnable;
    bool ambientOn;
    Mat displayImg;
    Mat dispRoi;
    Mat controlWin;
    Rect rectangle;
    Point anchor;
    Rect roi;
    bool working;
    bool shouldQuit;
    
    bool roiSelected;
};