#include <iostream>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <fstream>
#include <cstring>
#include <time.h>
#include "bills.h"
#include "logger.h"

#define YEAR_CSV_INDEX 0
#define MONTH_CSV_INDEX 1
#define MAX_HOUR 6

using namespace std;

logger logg;

Bills::Bills(const string& file_path) {
    csv_path=file_path.c_str();
    load_csv_bill(file_path); // Load bill data from a CSV file (change "filename.csv" to your desired filename)
}

void  Bills::load_csv_bill(const string fileName){
    ifstream file(fileName);

    // Ignore the first line
    string line;
    getline(file, line);

    while (getline(file, line)) {
    vector<string> row;

    for (int i = 0; i < line.size(); i++) {
        if (line[i] == ',') {
        row.push_back(line.substr(0, i));
        line = line.substr(i + 1);
        i = -1;
        }
    }

    row.push_back(line);
    data.push_back(row);
    }
    // cout<<"CSV Loaded For: "<<csv_path<<endl;
}

int Bills::find_busy_hour(int start_index) {
    int busy_hour=0;
    int maxSum = 0;
    int this_hour = 0;

    for(int k = 3;k < data[1].size();k++){ // for each row
        int sum_for_this_hour = 0;
        for(int i = start_index; i < start_index+30; i++) { // we get all collumns
            sum_for_this_hour += stoi(data[i][k]);
        }
        if (sum_for_this_hour > maxSum) {
            maxSum = sum_for_this_hour;
            this_hour = k-3;
            busy_hour = this_hour;
        }
    }    
    return busy_hour; 
}

vector<MonthlyBill*> Bills::fill_monthly_bill(){
    vector<MonthlyBill*> monthly_bills; // Store the bill data
    int sum_prev=0;
    for(int i=0;i<data.size();i+=30){
        // vector<string>temp_data=data[i];
        MonthlyBill *monthly_bill=(MonthlyBill*)malloc(sizeof(MonthlyBill));
        int busy_hour = find_busy_hour(i);
        monthly_bill->busy_hour = busy_hour;
        monthly_bill->year = stoi(data[i][YEAR_CSV_INDEX]);
        monthly_bill->month = stoi(data[i][MONTH_CSV_INDEX]);
        monthly_bill->resource =0;
        monthly_bill->resource_behineh=0;
        monthly_bill->resource_busy=0;
        monthly_bill->resource_price=0;
        monthly_bill->sum_prev=sum_prev;

        for(int j=i;j<30+i;j++){    
            for(int k=0;k<MAX_HOUR;k++){
                if(k==busy_hour){
                    monthly_bill->resource_busy += stoi(data[i][k+3]);
                } else{
                    monthly_bill->resource += stoi(data[i][k+3]);
                }

            }
        }
        monthly_bills.push_back(monthly_bill);
        sum_prev=monthly_bill->resource+monthly_bill->resource_busy;
    }
    return monthly_bills;
}

int main(int argc, char const *argv[]) {

    string csv_path=argv[1];
    logg.log("inside bill function for: "+ csv_path+"\n");
    int fd[2];
    fd[0] = atoi(argv[2]);
    fd[1] = atoi(argv[3]);

    close(fd[0]);

    Bills bill=Bills(csv_path);
    vector<MonthlyBill*> monthly_bills = bill.fill_monthly_bill();

    string serialized_data = serialize_monthly_bills(monthly_bills);
    
    write(fd[1], serialized_data.c_str(),MSG_BUFF_SIZE);
    close(fd[1]);

    logg.log("exiting bill function for: " + csv_path+"\n");

    exit(0);
}

