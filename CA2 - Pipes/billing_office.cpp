#include "serialize.h"
#include <iostream>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <fstream>
#include <cstring>
#include <time.h>
#include <sstream>
#include "logger.h"

#define BUSY_PENALTY 1.25
#define BEHINEH_PENLTY 0.75
using namespace std;
#define BILL_ZARIB_CSV "bills.csv"
logger logg;

vector<vector<int>> calculate_bill_zarib(const char* file_name){
    ostringstream csv_path;
    csv_path << file_name << BILL_ZARIB_CSV;
    vector<vector<int>> zaribs;
    string line;
    vector<int> row;
    string word;
    fstream csv_file(csv_path.str(), ios::in);
    if(csv_file.is_open()){
        getline(csv_file,line);
        while(getline(csv_file, line)){
            row.clear();
            stringstream str(line);
            int j = 0;
            while(getline(str, word, ',')){
                if ( j > 1){
                    row.push_back(stoi(word));
                }
                j++;
            }
            zaribs.push_back(row);
        }
    }
    else{
        logg.log("File Not Found",true,COLOR_RED);
        exit(0);
    }
    return zaribs;
} 

void calculate_bills(vector<MonthlyBill*> monthly_bill,vector<vector<int>> zarib,int id){
    if (id==0){
        for(int j=0;j<monthly_bill.size();j++){
            monthly_bill[j]->resource_price  = (monthly_bill[j]->resource + monthly_bill[j]->resource_busy*zarib[id][j]);
        }   
    } 
    if(id==1){
        for(int j=0;j<monthly_bill.size();j++){
            if ((monthly_bill[j]->resource + monthly_bill[j]->resource_busy)< monthly_bill[j]->sum_prev) // we are behineh
                monthly_bill[j]->resource_price  = (monthly_bill[j]->resource + monthly_bill[j]->resource_busy)*BEHINEH_PENLTY*zarib[id][j];
            else
                monthly_bill[j]->resource_price  = (monthly_bill[j]->resource + (monthly_bill[j]->resource_busy)*BUSY_PENALTY)*zarib[id][j];
        }
    }
    if(id==2)
        for(int j=0;j<monthly_bill.size();j++)
            monthly_bill[j]->resource_price  = (monthly_bill[j]->resource + (monthly_bill[j]->resource_busy)*BUSY_PENALTY)*zarib[id][j];
        
}

void handle_messages(char * buff,vector<vector<int>>monthly_bill_zarib,int parent_write_fd){
 string token,building_name;
    size_t pos;
    token =buff;
    pos = token.find('&'); // & delimiter shows building name
    building_name = token.substr(0, pos);
    logg.log("Got Data from building: "+building_name+"\n");
    token = token.substr(pos+1);
    vector<vector<MonthlyBill*>> monthly_bill = deserialize_monthly_bills(token);

    for(int i=0;i<3;i++){
        calculate_bills(monthly_bill[i],monthly_bill_zarib,i);
    }
    
    string serialized_data = building_name +'&'
                                            + serialize_monthly_bills(monthly_bill[0])   
                                            + serialize_monthly_bills(monthly_bill[1])
                                            + serialize_monthly_bills(monthly_bill[2]);
    int byte_recived = write(parent_write_fd, serialized_data.c_str(),MSG_BUFF_SIZE);
    if(byte_recived==-1) logg.log("Could not send data from office to app\n",true,COLOR_RED);
    logg.log("Data for building: "+building_name+" sent to app\n");
}

int main(int argc, char const *argv[]) {
    ostringstream oss;
    const char * path = argv[0];
    int num_of_building= atoi(argv[1]);
    int parent_fd[2];
    parent_fd[0] = atoi(argv[2]);
    parent_fd[1] = atoi(argv[3]);
    close(parent_fd[0]);

    oss << "Inside billing office with num building: " << num_of_building << " fds: " 
        <<parent_fd[0] << " " << parent_fd[1] << " File path:"<<argv[4] <<endl;
    logg.log(oss.str());
    int named_fd=open(NAMED_PIPE,O_RDONLY);
    int handled_buildings=0;
    vector<vector<int>> monthly_bill_zarib = calculate_bill_zarib(argv[4]);

    while (handled_buildings < num_of_building){
        char * buff=(char*)malloc(MSG_BUFF_SIZE);
        int byte_recived = read(named_fd,buff,MSG_BUFF_SIZE);
        if (byte_recived<=0) continue;

        handled_buildings++;
        handle_messages(buff,monthly_bill_zarib,parent_fd[1]);
        free(buff);
    }
    close(named_fd);
    close(parent_fd[1]);
    logg.log("Exiting billing office...\n");
    exit(0);
}
