#include "bmp.h"
#include <algorithm>
#include <pthread.h>

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

    get_pixels_from_bmp_24();
    return true;
}




void BMP::get_pixels_thread(int start_row , int end_row){
    for (int i = start_row; i < end_row; i++) {
        for (int j = 0; j <cols; j++) {
            for (int k = 0; k < 3; k++) {
                int count = i * cols * 3 + (cols - j - 1) * 3 + k  + 1;
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
            }
        }
    }
}

void * get_pixels_runner(void * arg){
    struct thread_args* thread_args = static_cast<struct thread_args*>(arg);
    thread_args->bmp->get_pixels_thread(thread_args->start_row,thread_args->end_row);
    pthread_exit(NULL);
}

void BMP::get_pixels_from_bmp_24() {
    int row_per_thread = rows / MAX_THREAD;
    pthread_t threads[MAX_THREAD];
    thread_args thread_args[MAX_THREAD];
    int count = 1;

    for (int i = 0; i < MAX_THREAD; i++) {
        int start_row = (i == 0)? 0 :  i * row_per_thread ;
        int end_row = (i == MAX_THREAD - 1) ? rows - 1 : start_row + row_per_thread;

        thread_args[i].start_row = start_row ;
        thread_args[i].end_row = end_row ;
        thread_args[i].bmp = this;

        count += (row_per_thread * (cols * 3));


        pthread_create(&threads[i], NULL, get_pixels_runner, static_cast<void*>(&thread_args[i]));
    }
    for (int i = 0; i < MAX_THREAD; i++) {
        pthread_join(threads[i], NULL);
    }

}

void BMP::write_bmp_thread(int start_row , int end_row){
    for (int i = start_row; i <= end_row; i++) {
        for (int j = 0; j <cols; j++) {
            for (int k = 0; k < 3; k++) {  
                int count = i * cols * 3 + (cols - j) * 3 + k + 1 ;
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
            }
        }
    }
}

void * write_bmp_runner(void * arg){
    struct thread_args* thread_args = static_cast<struct thread_args*>(arg);
    thread_args->bmp->write_bmp_thread(thread_args->start_row,thread_args->end_row);
    pthread_exit(NULL);
}

void BMP::writeOutBmp24(const char* nameOfFileToCreate) {
    ofstream file(nameOfFileToCreate);
    if (!file) {
        cout << "Failed to write " << nameOfFileToCreate << endl;
        return;
    }
    int row_per_thread = rows / MAX_THREAD;
    pthread_t threads[MAX_THREAD];
    thread_args thread_args[MAX_THREAD];
    int count = 1;

    for (int i = 0; i < MAX_THREAD; i++) {
        int start_row = (i == 0)? 0 :  i * row_per_thread ;
        int end_row = (i == MAX_THREAD - 1) ? rows - 1 : start_row + row_per_thread;

        thread_args[i].start_row = start_row ;
        thread_args[i].end_row = end_row ;
        thread_args[i].bmp = this;

        count += (row_per_thread * (cols * 3));


        pthread_create(&threads[i], NULL, write_bmp_runner, static_cast<void*>(&thread_args[i]));
    }
    for (int i = 0; i < MAX_THREAD; i++) {
        pthread_join(threads[i], NULL);
    }

    file.write(buffer, bufferSize);
}


void BMP::mirror_thread(int start_row , int end_row){
    for (int row = start_row; row < end_row; row++) {
        for (int col = 0; col < width; ++col) {
            swap(pixels[row][col], pixels[height- 1 - row][col]);
        }
    }
}

void * mirror_runner(void * arg){
    struct thread_args* thread_args = static_cast<struct thread_args*>(arg);
    thread_args->bmp->mirror_thread(thread_args->start_row,thread_args->end_row);
    pthread_exit(NULL);
}

void BMP::mirror_revert() {
    int row_per_thread = rows / MAX_THREAD;
    pthread_t threads[MAX_THREAD];
    thread_args thread_args[MAX_THREAD];

    for (int i = 0; i < MAX_THREAD; i++) {
        int start_row = (i == 0)? 0 :  i * row_per_thread ;
        int end_row = (i == MAX_THREAD - 1) ? rows - 1 : start_row + row_per_thread;

        thread_args[i].start_row = start_row / 2 ;
        thread_args[i].end_row = end_row / 2;
        thread_args[i].bmp = this;
        pthread_create(&threads[i], NULL, mirror_runner, static_cast<void*>(&thread_args[i]));
    }
    for (int i = 0; i < MAX_THREAD; i++) {
        pthread_join(threads[i], NULL);
    }

}

void BMP::blur_thread(int start_row , int end_row,vector<vector<RGB>>* blurred_pixels) {
    int kernel[3][3] = {{1, 2, 1},
                        {2, 4, 2},
                        {1, 2, 1}};


    for (int row = start_row; row < end_row; row++) {
        for (int col = 0; col < cols - 1; col++) {
            int sum_red = 0;
            int sum_green = 0;
            int sum_blue = 0;

            for (int x = -1; x <= 1; x++) {
                for (int y = -1; y <= 1; y++) {
                    if(row + x< 0 || row + x >= rows || col + y < 0 || col + y >= cols){    
                        sum_red += pixels[row][col].red * kernel[x + 1][y + 1];
                        sum_green += pixels[row][col].green * kernel[x + 1][y + 1];
                        sum_blue += pixels[row][col].blue * kernel[x + 1][y + 1];
                    } else{
                        sum_red += pixels[row + x][col + y].red * kernel[x + 1][y + 1];
                        sum_green += pixels[row + x][col + y].green * kernel[x + 1][y + 1];
                        sum_blue += pixels[row + x][col + y].blue * kernel[x + 1][y + 1];
                    }
                }
            }
            (*blurred_pixels)[row][col].red = sum_red / 16;
            (*blurred_pixels)[row][col].green = sum_green / 16;
            (*blurred_pixels)[row][col].blue = sum_blue / 16;
        }
    }
}

void * blur_runner(void * arg){
    struct blur_thread_args* thread_args = static_cast<struct blur_thread_args*>(arg);
    thread_args->bmp->blur_thread(thread_args->start_row,thread_args->end_row, thread_args->blurred_pixels);
    pthread_exit(NULL);
}

void BMP::blur_image() {
    int row_per_thread = rows / MAX_THREAD;
    pthread_t threads[MAX_THREAD];
    blur_thread_args thread_args[MAX_THREAD];

    vector<vector<RGB>> blurred_pixels(rows, vector<RGB>(cols));
    blurred_pixels = pixels;

    for (int i = 0; i < MAX_THREAD; i++) {
        int start_row =  i * row_per_thread ;
        int end_row = (i == MAX_THREAD - 1) ? rows - 1 : start_row + row_per_thread;

        thread_args[i].start_row = start_row;
        thread_args[i].end_row = end_row;
        thread_args[i].bmp = this;
        thread_args[i].blurred_pixels = &blurred_pixels;
        
        pthread_create(&threads[i], NULL, blur_runner, static_cast<void*>(&thread_args[i]));   
    }

    for (int i = 0; i < MAX_THREAD; i++) {
        pthread_join(threads[i], NULL);
    }
    pixels = blurred_pixels;

}

void BMP::purple_filter_thread(int start_row, int end_row) {
    for (int i = start_row; i < end_row; i++) {
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

void * purple_runner(void * arg) {
    thread_args   * data = (thread_args*) arg;
    data->bmp->purple_filter_thread(data->start_row,data->end_row);
    pthread_exit(NULL);
}

void BMP::purple_filter() {
    int row_per_thread = rows / MAX_THREAD;
    pthread_t threads[MAX_THREAD];
    thread_args thread_args[MAX_THREAD];

    for (int i = 0; i < MAX_THREAD; i++) {
        int start_row = (i == 0)? 0 :  i * row_per_thread ;
        int end_row = (i == MAX_THREAD - 1) ? rows - 1 : start_row + row_per_thread;

        thread_args[i].start_row = start_row;
        thread_args[i].end_row = end_row;
        thread_args[i].bmp = this;
        pthread_create(&threads[i], NULL, purple_runner, static_cast<void*>(&thread_args[i]));
    }
    for (int i = 0; i < MAX_THREAD; i++) {
        pthread_join(threads[i], NULL);
    }
}


void BMP::add_line_thread(int start_row, int end_row) {
    RGB white = RGB(255,255,255);
    float zarib = float(cols)/float(rows);
    for (int row = start_row; row < end_row; row++) {
        for(int col = 0; col < cols; col++){
            if(zarib *  row  + col == cols / 2 || zarib *  row  + col == cols || zarib *  row  + col == 3 * cols / 2){
                pixels[row][col] = white;
            }
        }
    }
}

void * add_line_runner(void * arg) {
    thread_args* data = (thread_args*) arg;
    data->bmp->add_line_thread(data->start_row,data->end_row);
    pthread_exit(NULL);
}

void BMP::add_line() {
    int row_per_thread = rows / MAX_THREAD;
    pthread_t threads[MAX_THREAD];
    thread_args thread_args[MAX_THREAD];

    for (int i = 0; i < MAX_THREAD; i++) {
        int start_row = (i == 0)? 0 :  i * row_per_thread ;
        int end_row = (i == MAX_THREAD - 1) ? rows - 1 : start_row + row_per_thread;

        thread_args[i].start_row = start_row;
        thread_args[i].end_row = end_row;
        thread_args[i].bmp = this;
        pthread_create(&threads[i], NULL, add_line_runner, static_cast<void*>(&thread_args[i]));
    }
    for (int i = 0; i < MAX_THREAD; i++) {
        pthread_join(threads[i], NULL);
    }
}
