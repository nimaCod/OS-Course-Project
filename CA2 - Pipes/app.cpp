#include <iomanip> 
#include <iostream>
#include <unistd.h>
#include <string>
#include <sys/wait.h>
#include <vector>
#include <time.h>
#include <cstring>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include "serialize.h"

#define BUILDING_FILENAME "./building"
#define BILLING_OFFICE_FILENAME "./billing_office"
#define GAS_CSV_FILE_NAME "/Gas.csv"
#define ELECTRICITY_CSV_FILE_NAME "/Electricity.csv"
#define WATER_CSV_FILE_NAME "/Water.csv"

namespace fs = std::filesystem;
logger logg;

// Creates office and returns pid
int create_billing_office(string file_name, int num_of_buildings, int fd[2],string file_path) {
    int pid = fork();
    if (pid == 0) { // child process
        ostringstream oss;
        oss  << "Creating new office:"
             << " filename=" << file_name
             << " num of buildings=" << num_of_buildings
             << " reource_fds= " << fd[0] << " " << fd[1]
             << " File_path= " << file_path
             << endl;
        
        logg.log(oss.str());

        const char* param[] = {
            file_name.c_str(),
            to_string(num_of_buildings).c_str(),
            to_string(fd[0]).c_str(),
            to_string(fd[1]).c_str(),
            file_path.c_str(),
            nullptr
        };
        int r;
        r= execv(file_name.c_str(), const_cast<char**>(param));
        exit(0);
    }

    return pid; // in parent
}

// Creates building and returns pid
int create_building(string file_name, string folder_path){
    int pid=fork();
    if(pid==0){ // child proces
        ostringstream oss;

        oss << "Creating new building:"
             << " filename=" 
             << folder_path 
             << endl;
        logg.log(oss.str());

        char* param[] = {
            const_cast<char*>(file_name.c_str()),
            const_cast<char*>(folder_path.c_str()),
            nullptr};      
        
        execv(file_name.c_str(), param);   
    }

    return pid; // in parent
}

vector<string> get_all_subfolders(string path)
{
    vector<string> sub_folders;

    for (const auto& entry : filesystem::directory_iterator(path))
    {
        if (filesystem::is_directory(entry.path()))
        {
            string building_name = entry.path().filename().string();
            sub_folders.push_back(building_name);
        }
    }

    return sub_folders;
}



void show_resource(vector<MonthlyBill*> resource){
    ostringstream oss;
    oss << endl<<endl << " =================================================================================================="<<endl;
    oss << "|  Year  " << " | "<< "  Month  " << " | "<< "  Mean  " << " | "<< "  Total  " << " | "<< "  Busy Hours  " << " | "<< "  Diff Busy/Mean  " << " | "<< "    Bill    "<<" | "<<endl; 
    for (int i=0;i<resource.size();i++){
        int total = resource[i]->resource + resource[i]->resource_busy;
        int mean = (total)/30;
        oss << "|  " << setw(4) << resource[i]->year << "  " << " | " 
            << setw(7)  << resource[i]->month << "  " << " | " 
            << setw(7)  << mean << "  | " 
            << setw(8)  << total << "  | " 
            << setw(13) << resource[i]->busy_hour << "  | " 
            << setw(17) << (mean - resource[i]->resource_busy/30) << "  | " 
            << setw(12)  << resource[i]->resource_price <<" | " << endl;
    }
    oss << " =================================================================================================="<<endl<<endl;
    logg.log(oss.str(), true, COLOR_CYAN);
}   


int main(int argc, char const *argv[]){
    logg.log("========== App startd ============\n",true,COLOR_GREEN);
    // const char* buildings_dir = "./buildings" + "/";
    const char* buildings_dir = argv[1];
    if(mkfifo(NAMED_PIPE,0777)==-1){// read + write every one
        if(errno!= EEXIST){ // if the file already exists mkfifo returns -1
            logg.log("Could not create named pipe\n",true,COLOR_RED);
            return 1;
        }
    } 

    vector<string> sub_folders =get_all_subfolders(buildings_dir);
    int num_buidlings=sub_folders.size();
    int buildings_pid[num_buidlings];
    int office_fd[2];
    if(pipe(office_fd)==-1){logg.log("Problem in creating pipe\n",true,COLOR_RED);}

    int office_pid = create_billing_office(BILLING_OFFICE_FILENAME, num_buidlings,office_fd,buildings_dir);
    logg.log("Billing Office Created\n",true,COLOR_GREEN);

    for(int i=0;i<num_buidlings;i++){
        buildings_pid[i] = create_building(BUILDING_FILENAME,buildings_dir + sub_folders[i]);
    }
    logg.log("Buildings Created\n",true,COLOR_GREEN);
    char* buff=(char*)malloc(MSG_BUFF_SIZE);
    
    close(office_fd[1]);
    vector<vector<vector<MonthlyBill*>>> all_bills;
    vector<string> all_bills_names;
    int byte_recived;
    int data_recieved=0;
    while (data_recieved < num_buidlings){
        byte_recived= read(office_fd[0],buff,MSG_BUFF_SIZE);
        if(byte_recived==-1){continue;}
        
        size_t pos;
        string token =buff;
        pos = token.find('&'); // & delimiter shows building name
        string building_name = token.substr(0, pos);
        logg.log("Got Data in main from building : "+building_name+"\n");
        token = token.substr(pos+1);
        data_recieved++;
        all_bills.push_back(deserialize_monthly_bills(token));
        all_bills_names.push_back(building_name.substr(building_name.find_last_of('/')+1));
        memset(buff,0,MSG_BUFF_SIZE);
    }
    close(office_fd[0]);


    bool is_prog_end=0;

    while (!is_prog_end) {
        ostringstream oss;
        oss <<"We Have " << num_buidlings << " Buildings"<<endl;
        logg.log(oss.str(),true,COLOR_BLUE);
        for(int i=0;i<num_buidlings;i++){
            logg.log("Building: "+all_bills_names[i]+"\n",true,COLOR_BLUE);
        }
        logg.log("\nTo see data of each resource of building provide input in this format:(to exit type exit :) )\n<BUILDING_NAME>< ><resources> (Possible resources: Gas Electricity Water)\n",true,COLOR_YELLOW);
        string building;
        cin >> building;
        if(building=="exit") break;
        string resource;
        cin >> resource;
        transform(resource.begin(), resource.end(), resource.begin(),
                   [](unsigned char c) { return std::tolower(c); });
        for(int i=0;i<num_buidlings;i++){
            if(building==all_bills_names[i]){
                if(strcmp(resource.c_str(),"gas")==0){
                    show_resource(all_bills[i][0]);
                }
                else if(strcmp(resource.c_str(),"electricity")==0){
                    show_resource(all_bills[i][1]);
                } else if(strcmp(resource.c_str(),"water")==0){
                    show_resource(all_bills[i][2]);
                }
            }
        }
        
    }
    waitpid(office_pid, NULL, 0);
    logg.log("App finished. Logs stored inside full_log.txt\n",true,COLOR_GREEN);

    return 0;
}