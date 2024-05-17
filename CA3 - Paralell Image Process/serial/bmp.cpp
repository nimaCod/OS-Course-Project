#include "bmp.h"
#include <algorithm>

BMP::~BMP() {
    delete[] buffer;
}

bool BMP::fillAndAllocate(const char* fileName) {
    ifstream file(fileName);
    if (!file) {
        cout << "File" << fileName << " doesn't exist!" << endl;
        return false;
    }

    file.seekg(0, ios::end);
    file_length = file.tellg();
    file.seekg(0, ios::beg);

    buffer = new char[file_length];
    file.read(&buffer[0], file_length);

    PBITMAPFILEHEADER file_header;
    PBITMAPINFOHEADER info_header;
    file_header = (PBITMAPFILEHEADER)(&buffer[0]);
    info_header = (PBITMAPINFOHEADER)(&buffer[0] + sizeof(BITMAPFILEHEADER));
    rows = info_header->biHeight;
    cols = info_header->biWidth;
    bufferSize = file_header->bfSize;
    height = info_header->biHeight;
    width = info_header->biWidth;
    pixels.resize(rows, vector<RGB>(cols));

    getPixelsFromBMP24();
    return true;
}

void BMP::getPixelsFromBMP24() {
    int count = 1;
    int extra = cols % 4;
    for (int i = 0; i < rows; i++) {
        count += extra;
        for (int j = cols - 1; j >= 0; j--) {
            for (int k = 0; k < 3; k++) {
                switch (k) {
                case 0:
                    pixels[i][j].red =  buffer[bufferSize - count];
                    break;
                case 1:
                    pixels[i][j].green =  buffer[bufferSize - count];
                    break;
                case 2:
                    pixels[i][j].blue =  buffer[bufferSize - count];
                    break;
                }
                count++;
            }
        }
    }
}

void BMP::writeOutBmp24(const char* nameOfFileToCreate) {
    ofstream file(nameOfFileToCreate);
    if (!file) {
        cout << "Failed to write " << nameOfFileToCreate << endl;
        return;
    }
    int count = 1;
    int extra = cols % 4;
    for (int i = 0; i < rows; i++) {
        count += extra;
        for (int j = cols - 1; j >= 0; j--) {
            for (int k = 0; k < 3; k++) {   
                switch (k) {
                    case 0:
                        buffer[bufferSize - count] = pixels[i][j].red;
                        break;
                    case 1:
                        buffer[bufferSize - count] = pixels[i][j].green;
                        break;
                    case 2:
                        buffer[bufferSize - count] = pixels[i][j].blue;
                        break;
                }
                count++;
            }
        }
    }
    file.write(buffer, bufferSize);
}

void BMP::mirror_revert() {

    for (int row = 0; row < height / 2; ++row) {
        for (int col = 0; col < width; ++col) {
            swap(pixels[row][col], pixels[height- 1 - row][col]);
        }
    }
    // This one works much faster
    // reverse(pixels.begin(), pixels.end());
    return;
}

void BMP::blur_image() {
    int kernel[3][3] = {{1, 2, 1},
                        {2, 4, 2},
                        {1, 2, 1}};
    vector<vector<RGB>> blurred_pixels(rows, vector<RGB>(cols));

    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            int sum_red = 0;
            int sum_green = 0;
            int sum_blue = 0;

            for (int x = -1; x <= 1; x++) {
                for (int y = -1; y <= 1; y++) {
                    if(row + x< 0 || row + x >= rows || col + y < 0 || col + y >= cols){    
                        sum_red += pixels[row][col].red * kernel[x + 1][y + 1];
                        sum_green += pixels[row][col].green * kernel[x + 1][y + 1];
                        sum_blue += pixels[row][col].blue * kernel[x + 1][y + 1];
                    } else {

                        sum_red += pixels[row + x][col + y].red * kernel[x + 1][y + 1];
                        sum_green += pixels[row + x][col + y].green * kernel[x + 1][y + 1];
                        sum_blue += pixels[row + x][col + y].blue * kernel[x + 1][y + 1];
                    }
                }
            }
            blurred_pixels[row][col].red = sum_red / 16;
            blurred_pixels[row][col].green = sum_green / 16;
            blurred_pixels[row][col].blue = sum_blue / 16;
        }
    }
    pixels = blurred_pixels;
}


void BMP::purple_filter() {
    for (int i = 0; i < rows; i++) {
        for (int j = cols - 1; j >= 0; j--) {
            int new_red , new_blue, new_green;
            for (int k = 0; k < 3; k++) {   
                switch (k) {
                    case 0:
                        new_red = pixels[i][j].red * 0.5 + pixels[i][j].green * 0.3 + pixels[i][j].blue * 0.5;
                        break;
                    case 1:
                        new_green = pixels[i][j].red * 0.16 + pixels[i][j].green * 0.5 + pixels[i][j].blue * 0.16;
                        break;
                    case 2:
                        new_blue = pixels[i][j].red * 0.6 + pixels[i][j].green * 0.2 + pixels[i][j].blue * 0.8;
                        break;
                }
            }
            new_red = new_red < 0 ? 0 : new_red > 255 ? 255 : new_red;
            new_green = new_green < 0 ? 0 : new_green > 255 ? 255 : new_green;
            new_blue = new_blue < 0 ? 0 : new_blue > 255 ? 255 : new_blue;
            pixels[i][j] = RGB(new_red,new_green,new_blue);
        }
    }
}
void BMP::add_line(){
    RGB white = RGB(255,255,255);
    float zarib = float(cols)/float(rows);
    for(int row = 0; row < rows; row++){
        for(int col = 0; col < cols; col++){
            if(zarib *  row  + col == cols / 2 || zarib *  row  + col == cols || zarib *  row  + col == 3 * cols / 2){
                pixels[row][col] = white;
            }
        }
    }
}