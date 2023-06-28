#include "CameraImage.h"


CameraImage::CameraImage(void)
{
	bAllocated = false;
	saveJPEG = true;
}

CameraImage::~CameraImage(void)
{
	if (data != NULL) {
        delete data;
		data = NULL;
	}
	
    sizeInBytes = 0;
	bAllocated = false;
}

CameraImage::CameraImage(const CameraImage& other) {

    // Copy over data if possible
    if (other.data != NULL && other.sizeInBytes != 0) {
        data = new unsigned char[other.sizeInBytes];
        if (data == NULL) {
            // Log error here
            sizeInBytes = 0;
        }
        else {
			bAllocated = true;
            sizeInBytes = other.sizeInBytes;
            memcpy(data,other.data,sizeInBytes);
        }
    }
	// populate stats and members
	this->populateImageStats(other);
}

CameraImage& CameraImage::operator=(const CameraImage& rhs) {
    

    // Copy over data if possible
    if (rhs.data != NULL && rhs.sizeInBytes != 0) {
		if (data != NULL) {
			// Data already allocated
			if (sizeInBytes == rhs.sizeInBytes) {
				// Just memcpy as we already allocated
				memcpy(data,rhs.data,sizeInBytes);
			}
			else {
				delete data;
				data = new unsigned char[rhs.sizeInBytes];
				bAllocated = true;
				memcpy(data,rhs.data,sizeInBytes);
				sizeInBytes = rhs.sizeInBytes;
			}

		}
		else {
			// First allocation
			data = new unsigned char[rhs.sizeInBytes];
			if (data == NULL) {
				// Log error here
				sizeInBytes = 0;
			}
			else {
				bAllocated = true;
				sizeInBytes = rhs.sizeInBytes;
				memcpy(data,rhs.data,sizeInBytes);
			}
		}
    }

	// populate stats and members
	this->populateImageStats(rhs);

    return *this;
}

void CameraImage::populateImageStats(const CameraImage &rhs) {
	this->name = rhs.name;
	this->sizeInBytes = rhs.width*rhs.height*rhs.bytesPerPixel;
	this->systemMicros = rhs.systemMicros;
	this->systemUnixTime = rhs.systemUnixTime;
	this->cameraMicros = rhs.cameraMicros;
	this->width = rhs.width;
	this->height = rhs.height;
	this->bAllocated = rhs.bAllocated;
	this->frameNumber = rhs.frameNumber;
    this->position = rhs.position;
    this->flashtype = rhs.flashtype;
}

bool CameraImage::writeToFile(ofstream& outputFile) {
	
	
	//cout << "In file write -- " << endl;
	if (!outputFile.bad()) {
		//cout << "Writing data: " << sizeInBytes << endl;
		outputFile.write((const char *)&systemUnixTime,sizeof(systemUnixTime));
		outputFile.write((const char *)&systemMicros,sizeof(systemMicros));
		outputFile.write((const char *)&cameraMicros,sizeof(cameraMicros));
		outputFile.write((const char *)&frameNumber,sizeof(frameNumber));
		outputFile.write((const char *)&width,sizeof(width));
		outputFile.write((const char *)&height,sizeof(height));
        outputFile.write((const char *)&position,sizeof(position));
        outputFile.write((const char *)&flashtype,sizeof(flashtype));
		outputFile.write((const char *)data,sizeInBytes);
	}

	return outputFile.bad() != true;
}

string CameraImage::info() {
	return "";
}


