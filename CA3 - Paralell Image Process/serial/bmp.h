#ifndef BMP_H
#define BMP_H
#include <fstream>
#include <iostream>
#include <vector>
#include <cstring>

using namespace std;

typedef int LONG;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef unsigned char  rgb_type ;

#pragma pack(push, 1)
typedef struct tagBITMAPFILEHEADER {
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;
#pragma pack(pop)

struct RGB {
    
    rgb_type red;
    rgb_type green;
    rgb_type blue;
    RGB(rgb_type r, rgb_type g, rgb_type b) {
        red = r;
        green = g;
        blue = b;
    }
    RGB() {
    }
};


class BMP {
private:    
    int rows;
    int cols;
    char* buffer;
    int bufferSize;
    LONG height;
    LONG width;
    streampos file_length;
    vector<vector<RGB>> pixels;

public:
    bool fillAndAllocate(const char* fileName); 
    void getPixelsFromBMP24();
    void writeOutBmp24(const char* nameOfFileToCreate);
    void mirror_revert();
    void blur_image();
    void purple_filter();
    void add_line();
    ~BMP();
};

#endif