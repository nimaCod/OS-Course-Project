#ifndef BMP_H
#define BMP_H
#include <fstream>
#include <iostream>
#include <vector>
#include <cstring>
#define MAX_THREAD 8
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
    // BMP();
    bool fillAndAllocate(const char* fileName); 
    void get_pixels_from_bmp_24();
    void get_pixels_thread(int start_row , int end_row);
    void writeOutBmp24(const char* nameOfFileToCreate);
    void write_bmp_thread(int start_row , int end_row);
    void mirror_revert();
    void mirror_thread(int start_row , int end_row);
    void blur_image();
    void blur_thread(int start_row , int end_row,vector<vector<RGB>>* blurred_pixels); 
    void purple_filter();
    void purple_filter_thread(int start_row, int end_row); 
    void add_line();
    void add_line_thread(int start_row, int end_row) ;
    ~BMP();
};


struct blur_thread_args {
    BMP *bmp;
    int start_row;
    int end_row;
    vector<vector<RGB>>* blurred_pixels;
};

struct thread_args {
    int start_row;
    int end_row;
    BMP* bmp;
};


#endif