#include "Image.h"

Image::Image(int rawImgHeight, int rawImgWidth, unsigned char * data, int totalBytes, int format) {

    this->height = rawImgHeight;
    this->width = rawImgWidth;
    this->pData = new unsigned char[totalBytes];
    memcpy(this->pData,data,totalBytes);
    this->blockId =  time(NULL);
    this->bytesPerPixel = format;


}

Image::~Image() {
    if (pData != NULL)
        delete pData;
}

int Image::GetCols() {
    return width;
}

int Image::GetRows() {
    return height;
}

void Image::SetBlockId(unsigned int id) {
    blockId = id;
}

unsigned int Image::GetBlockId() {
    return blockId;
}

int Image::GetReceivedDataSize() {
    return width*height*bytesPerPixel;
}

unsigned char * Image::GetData() {
    return pData;
}

Image::Image(const Image& obj) {
    this->height = obj.height;
    this->width = obj.width;
    int totalBytes = obj.width*obj.height*obj.bytesPerPixel;
    this->pData = new unsigned char[totalBytes];
    memcpy(this->pData,obj.pData,totalBytes);
    this->blockId =  obj.blockId;
    this->bytesPerPixel = obj.bytesPerPixel;
}

Image& Image::operator=(const Image& obj) {
    this->height = obj.height;
    this->width = obj.width;
    int totalBytes = obj.width*obj.height*obj.bytesPerPixel;
    this->pData = new unsigned char[totalBytes];
    memcpy(this->pData,obj.pData,totalBytes);
    this->blockId =  obj.blockId;
    this->bytesPerPixel = obj.bytesPerPixel;

    return *this;
}