#include "bmp.h"
#include <chrono>
using  namespace std;
using  namespace chrono;

void flip_runner(BMP& image){
    auto start_time = high_resolution_clock::now();
    image.mirror_revert();
    auto end_time = chrono::high_resolution_clock::now();
    duration<double> duration = end_time - start_time;
    cout << "Flip: " << duration.count() * 1000 << " ms" << endl;
}
void blur_runner(BMP& image){
    auto start_time = high_resolution_clock::now();
    image.blur_image();
    auto end_time = chrono::high_resolution_clock::now();
    duration<double> duration = end_time - start_time;
    cout << "Blur: " << duration.count() * 1000 << " ms" << endl;
}

void purple_runner(BMP& image){
    auto start_time = high_resolution_clock::now();
    image.purple_filter();
    auto end_time = chrono::high_resolution_clock::now();
    duration<double> duration = end_time - start_time;
    cout << "Purple: " << duration.count() * 1000 << " ms" << endl;
}

void add_line_runner(BMP& image){
    auto start_time = high_resolution_clock::now();
    image.add_line();
    auto end_time = chrono::high_resolution_clock::now();
    duration<double> duration = end_time - start_time;
    cout << "Lines: " << duration.count() * 1000 << " ms" << endl;
}

int main(int argc, char* argv[]) {
    auto total_start_time = high_resolution_clock::now();

    const char * output_file_name = "output.bmp";
    BMP image;

    auto start_time = high_resolution_clock::now();
    if (!image.fillAndAllocate(argv[1])) {
        cout << "File read error" << endl;
        return 1;
    }
    auto end_time = chrono::high_resolution_clock::now();
    duration<double> duration = end_time - start_time;
    cout << "Read: " << duration.count() * 1000 << " ms" << endl;

    flip_runner(image);
    blur_runner(image);
    purple_runner(image);
    add_line_runner(image); 
    start_time = high_resolution_clock::now();
    image.writeOutBmp24(output_file_name);
    auto end_time2 = chrono::high_resolution_clock::now();
    duration = end_time2 - start_time;
    cout << "Write: " << duration.count() * 1000 << " ms" << endl;

    auto total_end_time = chrono::high_resolution_clock::now();
    duration = total_end_time - total_start_time;
    cout << "Execution: " << duration.count() * 1000 << " ms" << endl;

    // return 0;
    exit(0);
}
