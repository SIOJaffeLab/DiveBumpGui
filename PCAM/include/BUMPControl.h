#ifndef BUMPControl_H
#define BUMPControl_H

#include "SerialDevice.h"
#include "Poco/Stopwatch.h"
#include <fstream>
#include <iomanip>

class BUMPControl : public SerialDevice
{
public:

    BUMPControl(string config_path) : SerialDevice(config_path) {};
    BUMPControl(string name, string port, int baud, bool display, bool trim) : SerialDevice(name, port, baud, display, trim) {};
    char Start(string foldername);
    void threadedFunction();
    int parseData(string s, char firstchar, char lastchar);
    bool assertDataMode();
    string setTime();
    string setDate();
    char timeInsert(string s);
    string fmtCommand(string prefix, int val);
    void toggleTrigger();
    void setTriggerType(int type);
    void setSatPow(int pow);
    void setSatDur(int dur);
    void setMeasPow(int pow);
    void setMeasDur(int dur);
    void setColorDur(int dur);
    string getTemperature();
    string getHumidity();
    string getDepth();
    string getPosition();
    void setFrameRate(int rate);
    void sendCommand(string cmd);
    void quickSend(string cmd);
    void loadSequence(string fileName = "");
    void runSequence();
    void stopSequence();
    void enterCmdMode();
    void exitCmdMode();
    
    // Parameters for control of the system
    int measPow;
    int satPow;
    int measDur;
    int satDur;
    int colorDur;
    int startScan;
    int endScan;
    int speed;
    int pulseWidth;
    int frameRate;
    
    // Sensor readings
    float powerDraw;
    float pressure;
    float humidity;
    float depth;
    float temperature;
    float actuatorPos;
    float triggerType;
    
    bool pauseRead;
    bool cmdMode;
    bool sequenceRunning;
    bool sequenceLoaded;
    
    Poco::Stopwatch sequenceTimer;
    
    string sequenceFile;
};

#endif

