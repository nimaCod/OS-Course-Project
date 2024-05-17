#ifndef BILLS_H
#define BILLS_H

#include <string>
#include <vector>
#include "serialize.h"

using namespace std;

class Bills {
public:
    Bills(const std::string& file_path); // Constructor
    void load_csv_bill(const string fileName); // Load bill data from a CSV file
    int find_busy_hour(int start_index); // Find the busy hour
    vector<MonthlyBill*> fill_monthly_bill();
    

private:
    const char* csv_path;
    vector<vector<string>> data; // Store the bill data
};


#endif
