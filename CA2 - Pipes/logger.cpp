#include "logger.h"
#include "fcntl.h"
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <sys/file.h>

logger::logger(){
    fd = open(LOGGER_PATH,O_CREAT);
    close(fd);
}

void logger::log(string msg,bool printable,char* color){
    fd = open(LOGGER_PATH, O_WRONLY | O_APPEND);
    if (fd == -1) {
        cout << "Failed to open log file!" << endl;
        return;
    }

    // Acquire lock on the log file
    if (flock(fd, LOCK_EX) == -1) {
        cout << "Failed to acquire lock on log file!" << endl;
        close(fd);
        return;
    }

    // Write to the log file
    time_t current_time = time(0);
    char* timestamp = ctime(&current_time);
    timestamp[strlen(timestamp) - 1] = '\0';  // Remove newline character from timestamp

    string log_msg ="[" +  string(timestamp) + "] - " + msg;
    write(fd, log_msg.c_str(), log_msg.length());

    // Release lock and close the log file
    flock(fd, LOCK_UN);
    close(fd);

    if (printable) {
        cout <<color <<log_msg <<COLOR_RESET;
    }
}
