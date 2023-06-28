#include "DataEntry.h"

DataEntry::DataEntry() {
    data = NULL;
    sizeInBytes = 0;
	bAllocated = false;
    name = "";
}

DataEntry::DataEntry(string name) {
    data = NULL;
    sizeInBytes = 0;
	bAllocated = false;
    this->name = name;
}

DataEntry::DataEntry(string name,unsigned char * data, unsigned int sizeInBytes) {
    this->name = name;
    // Copy over data if possible
    if (data != NULL && sizeInBytes != 0) {
        this->data = new unsigned char[sizeInBytes];
        if (this->data == NULL) {
            // Log error here
            this->sizeInBytes = 0;
        }
        else {
			bAllocated = true;
            this->sizeInBytes = sizeInBytes;
            memcpy(this->data,data,this->sizeInBytes);
        }
    }
}

DataEntry::~DataEntry() {
	if (data != NULL) {
        delete data;
		data = NULL;
	}
	
    sizeInBytes = 0;
	bAllocated = false;
}

DataEntry::DataEntry(const DataEntry& other) {
    // Set the name
    name = other.name;
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
}

DataEntry& DataEntry::operator=(const DataEntry& rhs) {
    // Set the name
    name = rhs.name;
    // Copy over data if possible
    if (rhs.data != NULL && rhs.sizeInBytes != 0) {
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
    return *this;
}

void DataEntry::allocateAndCopyData(unsigned char * data,unsigned int sizeInBytes) {
	// Copy over data if possible
    if (data != NULL && sizeInBytes != 0) {
        this->data = new unsigned char[sizeInBytes];
        if (this->data == NULL) {
            // Log error here
            this->sizeInBytes = 0;
        }
        else {
			bAllocated = true;
            this->sizeInBytes = sizeInBytes;
            memcpy(this->data,data,this->sizeInBytes);
        }
    }
}


bool DataEntry::writeToFile(ofstream& outputFile) {
	if (!outputFile.bad()) 
		outputFile.write((const char *)data,sizeInBytes);
	return outputFile.bad() != true;
}

            
string DataEntry::info() {
	return "Generic Data Entry Object";
}

unsigned char * DataEntry::getDataPointer() {
	return data;
}

unsigned int DataEntry::getDataSizeInBytes() {
	return sizeInBytes;
}

    

