#include <cstring>
#include <time.h>

class Image
{
    public:
        Image(int rawImgHeight, int rawImgWidth, unsigned char * data, int totalBytes, int format);
        ~Image();
        Image(const Image& obj);
        Image& operator=(const Image& obj);
        int GetRows();
        int GetCols();
        unsigned char * GetData();
        unsigned int GetBlockId();
        void SetBlockId(unsigned int id);
        int GetReceivedDataSize();
    private:
        int width;
        int height;
        unsigned int blockId;
        unsigned char * pData;
        int bytesPerPixel;
};