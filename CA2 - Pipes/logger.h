#ifndef LOGGER_H
#define LOGGER_H
#include <string>
#include <vector>
#define LOGGER_PATH "full_log.txt"

// colors
#define COLOR_BLACK "\x1b[30m"
#define COLOR_RED "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_BLUE "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN "\x1b[36m"
#define COLOR_WHITE "\x1b[37m"
#define COLOR_RESET "\x1b[0m"

using namespace std;

class logger {
private:
    int fd;
public:
    logger();
    void log(string msg,bool printable=false,char* color=const_cast<char*>(COLOR_WHITE));
};

#endif