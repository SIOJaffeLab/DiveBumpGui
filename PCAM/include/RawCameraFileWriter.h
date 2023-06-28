#include "RawFileWriter.h"
#include "Poco/Stopwatch.h"

class RawCameraFileWriter : public RawFileWriter {
public: 
		RawCameraFileWriter();
		RawCameraFileWriter(AbstractConfiguration * cfg);
		~RawCameraFileWriter();
		void processDataEntry(DataEntry * entry);
		void openNewFile();
private:
		unsigned short rawImgWidth;
		unsigned short rawImgHeight;
		unsigned short headerLength;
		unsigned short headerFormat;
		int cameraID;
		unsigned short pixelFormat;
		unsigned short illuminationType;
		float flashDuration;
		float flashDelay;
		float exposureTime;
		float gain;
		float redGain;
		float blueGain;
		unsigned short vertOffset;
		unsigned short horzOffset;
		unsigned char vertBinning;
		unsigned char horzBinning;
		unsigned short binningMode;
		int resvd[4];
		int period;
		Poco::Stopwatch recordingTimer;
		bool recording;
};