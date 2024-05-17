#include <iostream>
#include <unistd.h>
#include <string>
#include <sys/wait.h>
#include <vector>
#include <time.h>
#include <cstring>
#include <fcntl.h>
#include <sstream>
#include "logger.h"
#include "serialize.h"
#define BILLS_FILE_NAME "./bills"
#define GAS_CSV_FILE_NAME "/Gas.csv"
#define ELECTRICITY_CSV_FILE_NAME "/Electricity.csv"
#define WATER_CSV_FILE_NAME "/Water.csv"

using namespace std;
logger logg;

// Creates bill and returns pid
int create_resource(string resuource_path,int fd[2]){
    int pid=fork();
    if(pid==0){ // child proces
        string file_name=BILLS_FILE_NAME;
        ostringstream oss;


        oss << "Creating new bill:"
             << " filename=" 
             << file_name 
             << " resource_path=" 
             << const_cast<char *>(resuource_path.c_str()) 
             << " reource_fds= " << const_cast<char *>(to_string(fd[0]).c_str()) 
             << " " << const_cast<char *>(to_string(fd[1]).c_str()) 
             << endl;

        logg.log(oss.str());

        char* param[] = {
            const_cast<char*>(file_name.c_str()),
            const_cast<char*>(resuource_path.c_str()),
            const_cast<char*>(to_string(fd[0]).c_str()),
            const_cast<char*>(to_string(fd[1]).c_str()),
            nullptr};      
        
        execv(BILLS_FILE_NAME, param);   
    }

    return pid; // in parent

}

int main(int argc, char *argv[]) {
    const char* building_dir = argv[1];
    ostringstream oss;
    oss << "Inside building function for: "  << argv[1] << endl;
    logg.log(oss.str());

    string gas_path = string(building_dir) + GAS_CSV_FILE_NAME;
    string electricity_path = string(building_dir) + ELECTRICITY_CSV_FILE_NAME;
    string water_path = string(building_dir) + WATER_CSV_FILE_NAME;
    
    // creating child bills ------------------------------------------------------------------------------------    
    int gas_fd[2];
    if(pipe(gas_fd)){logg.log("Could not create unnamed pipe\n",true,COLOR_RED);}
    int gas_pid= create_resource(gas_path,gas_fd);
    close(gas_fd[1]);

    int electricity_fd[2];
    if(pipe(electricity_fd)){logg.log("Could not create unnamed pipe\n",true,COLOR_RED);}
    int electricity_pid = create_resource(electricity_path,electricity_fd);
    close(electricity_fd[1]);

    int water_fd[2];
    if(pipe(water_fd)){logg.log("Could not create unnamed pipe\n",true,COLOR_RED);}
    int water_pid = create_resource(water_path,water_fd); 
    close(water_fd[1]);

    vector<string> all_child_data;
    string all_bills;
    all_bills+= string(building_dir)+ '&'; // add our name to the first of data
    char* buff=(char*)malloc(MSG_BUFF_SIZE);

    // Getting Data of each child ---------------------------------------------------------------------------
    read(gas_fd[0],buff,MSG_BUFF_SIZE);
    close(gas_fd[0]);
    all_bills += buff;
    memset(buff,0,MSG_BUFF_SIZE);

    read(electricity_fd[0],buff,MSG_BUFF_SIZE);
    all_bills += buff;
    close(electricity_fd[0]);
    memset(buff,0,MSG_BUFF_SIZE);

    read(water_fd[0],buff,MSG_BUFF_SIZE);
    close(water_fd[0]);
    all_bills += buff;
    close(water_fd[0]);

    oss.str("");
    oss <<"[..] Sendig frame of data from: " << building_dir <<" to office" <<endl;
    logg.log(oss.str());
    oss.str("");
    int named_fd = open(NAMED_PIPE,O_WRONLY);
    if(named_fd==-1){
        logg.log("Could not open named pipe\n",true,COLOR_RED);
        exit(1);
    }
    write(named_fd,all_bills.c_str(),MSG_BUFF_SIZE);
    close(named_fd);

    waitpid(gas_pid, NULL, 0);
    waitpid(electricity_pid, NULL, 0);
    waitpid(water_pid, NULL, 0);

    oss << "Exiting from building function for: "  << argv[1] << endl;
    logg.log(oss.str());
    oss.clear();
    exit(0);
}
