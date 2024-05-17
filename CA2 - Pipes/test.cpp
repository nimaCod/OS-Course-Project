#include <iostream>
#include <vector>
#include <unistd.h>
#include <fcntl.h>

using namespace std;
void LoadCSVBill(const string& file_path) {
    int fd = open(file_path.c_str(), O_RDONLY);
    vector<vector<int>> data; // Store the bill data

    string line;
    vector<int> bill;
    char buffer[1];
    while (read(fd, buffer, 1) > 0) {
        if (buffer[0] == ',') {
            bill.push_back(stoi(line));
            line.clear();
        } else if (buffer[0] == '\n') {
            bill.push_back(stoi(line));
            data.push_back(bill);
            bill.clear();
            line.clear();
        } else {
            line += buffer[0];
        }
    }

    close(fd);
}


int main(){
    // bill= Bills('__MACOSX/buildings/Ein_Electricity.csv');
    LoadCSVBill("./__MACOSX/buildings/Ein/._Electricity.csv");
}