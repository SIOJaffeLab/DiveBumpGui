/**
 * @file PCAM.h
 * @brief Main Camera Interface Class
 *
 * PCAM wraps a host of camera and image processing functions into
 * a common command line interface tool with logging, multithreaded
 * image processing and raw data recording, and configurable user
 * commands.
 * 
 * PCAM is derived from @ref DataProcessor but does not use the typical
 * threaded model. Instead, PCAM fires off threads to handle image
 * acquisition, processing, and data storage alogn with serial comms.
 *
 * @author pldr
 * @copyright 2020 Scripps Institution of Oceanography
 */
#pragma once

#include <queue>
#include "ofThread.h"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <time.h>
#include "kbhit.h"
#include <math.h>
#include "Poco/Util/AbstractConfiguration.h"
#include "Poco/Util/XMLConfiguration.h"
#include "Poco/SAX/SAXException.h"
#include "Poco/Stopwatch.h"
#include "Poco/NumberParser.h"
#include "Poco/Path.h"
#include "Poco/File.h"
#include "Poco/AutoPtr.h"
#include "Poco/DateTime.h"
#include "Poco/DateTimeFormatter.h"
#include "Poco/Timestamp.h"
#include "Poco/String.h"
#include "Poco/Util/SystemConfiguration.h"
#include <iostream>
#include <exception>
#include <fstream>
#include <linux/reboot.h>

#include <sys/reboot.h>
#include <poll.h>
#include <signal.h>

#include "Image.h"
#include "DataProcessor.h"
#include "RoiImageProcessor.h"
#include "RawCameraFileWriter.h"
#include "CameraImage.h"
#include "Camera.h"
#include "SimCamera.h"
#include "MVIMPACTCamera.h"

#include "SerialDevice.h"
#include "BUMPControl.h"

#include "CameraGUI.h"

#define MAX_SERIAL_DEVICES 10 /**< The maximum number of serial devies to allow */


using namespace std;
using namespace cv;
using namespace cuda;
using namespace Poco::Util;
using Poco::AutoPtr;
using Poco::DateTime;
using Poco::DateTimeFormatter;
using Poco::PatternFormatter;
using Poco::FormattingChannel;
using Poco::toLower;
using Poco::Util::SystemConfiguration;
using Poco::Timestamp;

/**
 * @brief Camera interface, processing, and control
 *
 */
class PCAM : public DataProcessor
{
    public:

        /**
         * @brief a wrapper around POCO Logging
         */
        enum LOG_TYPE
        {
            INFO,
            WARN,
            ERROR
        };

        /**
         * @brief PCAM consturctor with config file
         *
         * This constructor takes a string representing the config file
         * path and attenmpts to read the xml and set variables accordingly
         *
         * @param config The path to the config file
         */
        PCAM(const char * config);
        
        /**
         * @brief PCAM destructor
         */
        ~PCAM();
        
        /**
         * @brief Print build information about the software.
         */
        void PrintBuildInfo();
        
        /**
         * @brief Prints camera info
         *
         * In most cases, the camera class prints this info,
         * so this function only prints "Camera Info".
         */ 
        void PrintCameraInfo();

        /**
         * @brief Run a single camera instance without GUI
         */
        int RunSingleCamera();

        /**
         * @brief Run a single camera instance with GUI
         */
        int RunCameraGui();

        /**
         * @brief Updates the internal image counter
         */
        int updateImageCount();

        /**
         * @brief Add a new data entry to the processing queue
         *
         * This function overrides the default @ref DataProcessor
         * @ref DataProcessor::addEntry() function. 
         *
         * It immediately converts the entry to a @ref CameraImage and
         * passes it to several threaded @ref DataProcessor objects.
         *
         * If saving raw frames is enabled, a copy of the @ref CameraImage
         * is passed to the @ref RawCameraFileWriter object.
         * 
         * If processing frames is enabled, the original @ref CameraImage
         * is pasded to the first @ref RoiImageProcessor that is not busy
         *
         * @attention This function depends on @RoiImageProcessor and
         * @ref RawCameraFileWriter to delete the @ref DataEntry objects
         *
         * @param pImg The @ref DataEntry to add
         *
         */
        void addEntry(DataEntry* pImg);

        /**
         * @brief Loads an xml file from disk and parses it, setting variables
         *
         * This is the main configuration function for the class. It uses 
         * POCO XML parsing functions to extract all config info from a file
         *
         * If the file has any XML parse errors, this function will abort 
         * and signal the application to exit
         *
         * @param filename The string presentation of the path to open
         */
        void readConfigFile(string filename);

        /**
         * @brief Returns true when there is a valid camera
         */
        bool haveValidCamera();
        
        /**
         * @brief Returns true when there is a valid camera config
         */
        bool haveValidConfig();
        
        /**
         * @brief Save the last acquired frame to a Tiff file
         */
         void saveRawFrame();

        /**
         * @brief Setup @ref PCAM::signalHandler to receive SIGINT
         */
        void setSignal();

        /**
         * @brief Upon receiving SIGINT, set @ref PCAM::signalExit to true
         */
        static void signalHandler(int signum);
        
        /**
         * @brief static app exit bool
         */
        static bool signalExit;

    private:

        /**
         * @brief The pointer to the camera instance
         */
        Camera * cam;               
        
        int frameCounter;         /**< Count the number of frames received */
		int processFrameCounter;  /**< Count the number of frames processed */    

        // General use variables
        unsigned int imageCnt;      /**< Count the number of images */
        bool validConfig;           /**< True when a config file parsed successfully */
        bool validCamera;           /**< True when we have a valid cam instance */ 
        int totalRois;              /**< Counter of total number of ROIs */
        int currentRoisPerMinute;   /**< Estimate of ROIs detected per minute */
        bool useOverflowDataPath;   /**< when true, save to overflow path */
        vector<int> roisPerMinute;  /**< History of ROIs/min */

        // Threaded processors
        vector<RoiImageProcessor*> imageProcessors; /**< list of iamge processor pointers */
        vector<SerialDevice*> serialDevices; /**< List of serial device pointers */
        RawCameraFileWriter * rawFileWriter; /**< The raw image writer pointer */
        CameraImage * rawFrame;              /**< Hold the last aqcquired frame */

        // Clock ticks for timing events
        Poco::Stopwatch stopWatch;          /**< A general stopwatch */
        Poco::Stopwatch cleaningTimer;      /**< Camera cleaning timer */
        Poco::Stopwatch elapsedCleaning;    /**< Time between cleanings timer */
        Poco::Stopwatch generalTimer;       /**< General timer */
        Poco::Stopwatch roisPerMinuteTimer; /**< Minute timer for counting ROIs */
        Poco::Stopwatch backupTimer;        /**< Data backup timer */

        // Config settings
        unsigned int maxFrames;         /**< Maximum number of frames to aqcuire */
        unsigned long frameRateMod;     /**< Used for sub-1Hz frame rates */
        unsigned long frameRateCount;   /**< Frame rate counter */
        bool saveRawImages;             /**< When true raw images will be saved */
        bool doRoiProcessing;           /**< When true, detect ROIs in images */
        int nImageProcessors;           /**< The number of Roi processors to use */
        bool tarSubDirs;                /**< When true, tar the roi subdirectories */
        string cameraName;              /**< The name of the camera */
        int cameraStartDelay;           /**< Wait this many seconds before running app */
        int cameraMaxRunTime;           /**< After this many seconds, exit app */
		int cameraFrameRate;            /**< The camera frame rate reuested */
		int processFrameRate;           /**< The rate to process camera frames at */  
        int logInterval;                /**< The time in seconds between status info */           
        bool quietMode;                 /**< Set the log level to error only */
        bool verboseLog;                /**< Set the log level to debug */
        string procMethod;              /**< The edge detection method to use */
        bool serialMaster;              /**< When true, act as the serial master */
        bool mergeSerialStrings;        /**< When true, combine all serial device data */
        int maxFilesystemPercentFull;   /**< The max fullness of the data before before using overflow */
        bool backupData;                /**< when true, periodically backup data */
        string backupDataPath;          /**< The path to backup data to */
        string guiConfigPath;           /**< The path to the gui config file */
        string shutdownCmd;             /**< The system command to execute to shutdown */
        int backupPeriod;               /**< The number of minutes between backups */
        bool simulatorMode;             /**< When true use @ref SimCamera */
        int simulatorImages;            /**< The number of novel frames to simulate */
        int simulatorImageRois;         /**< The number of ROIs per frame in simulation */
        int roisPerDir;                 /**< The number of ROIs per subdirectory */
        bool saveShrink;                /**< when true, save a highly compressed version of the raw data */
        string startScriptPath;         /**< Path to the script to run when starting the app */
        string endScriptPath;           /**< Path to the script to run when ending the app */
		
		int pixelFormat;                /**< The pixel format to set the camera to */
		int rawImgHeight;               /**< The raw image height to request from the camera */
		int rawImgWidth;                /**< The raw image width to request from the camera */  
		unsigned int binningX;          /**< The horzontal binning to set the camera to */
		unsigned int binningY;          /**< The vertical binning to set the camera to */
		int downsampleFactor;           /**< The downsample factor before processing ROIs */
		Mat displayImg;                 /**< The mat to use for diaplying images in the GUI */


        // Command strings
        string startCmd;                /**< The command to start the acquisition */
        string stopCmd;                 /**< The command to stop the acquisition */
        string haltCmd;                 /**< The command to halt (stop & shutdown) */
        string getTimeCmd;              /**< Get the system time YYYYMMDDhhmmss*/
        string setTimeCmd;              /**< Set the system time YYYYMMDDhhmmss*/
        string abortCmd;                /**< Leave the app before starting the acquisition */
        
        // Pointer to BUMP Control
        BUMPControl * bumpControl;      /**< Pointer to the camera microcontroller interface */

        // File paths and settings
        Poco::Path * dataDir;           /**< Directory to save data to */
        Poco::Path * shrinkDir;         /**< Name of the compressed data subdir */
        Poco::Path * dataPath;          /**< Path to the data directory */
        Poco::Path * shrinkPath;        /**< Path to the compressed data directory */
        Poco::File * dataDirFile;       /**< File reference to the data directory */
        Poco::File * shrinkDirFile;     /**< File reference to the compressed data directory */
        Poco::File * dataPathFile;      
        Poco::File * shrinkPathFile;

        ofstream outFile;               /**< variable used to creating output files */
};
