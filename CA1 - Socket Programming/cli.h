#ifndef CLI_H
#define CLI_H
#include "types.h"
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
#define CLEAR_STDIN "\033[2K\r"
// cli defult
#define COSTUMER_STARTER COLOR_MAGENTA "costumer>>" COLOR_RESET
#define RESTURANT_STARTER COLOR_MAGENTA "resturant>>" COLOR_RESET
#define GET_INPUT_STARTER COLOR_MAGENTA "-->" COLOR_RESET

// net messages
#define IS_USERNAME_EXIST "is_user_exist"
#define USER_ALREADY_EXIST "user_already_exist"
#define AVAILABLE_RESTURANTS "available_resturants"
#define RES_CLOSE "res_close"
#define RESTURANT_CLOSED COLOR_BLACK "Resturant closed.\n" COLOR_RESET
#define OPEN_RESTURANT "open_resturant"
#define ORDER_FOOD "order_food"
// cli messages
#define GET_USERNAME_MESSAGE COLOR_YELLOW "Please select a unique username:" COLOR_RESET
#define USERNAME_ALREADY_EXIST COLOR_YELLOW "Selected username already exist please select another one:" COLOR_RESET
#define NO_PORT COLOR_RED "No ports specefied!\n" COLOR_RESET
#define TCP_CONNECT_ERROR COLOR_RED "Error in connecting to server.\n" COLOR_RESET
#define UDP_BIND_SUCCESS COLOR_GREEN "User binded to udp port successfuly.\n" COLOR_RESET
#define TCP_BIND_SUCCESS COLOR_GREEN "User binded to tcp port successfuly.\n" COLOR_RESET 
#define USERNAME_SUCCESS COLOR_GREEN "Entered username is unique and added successfuly.\n" COLOR_RESET
#define RESTURANT_OPEN COLOR_GREEN "Resturant opened.\n" COLOR_RESET
#define RESTURANT_IS_CLOSED COLOR_RED "Resturant is closed. To do any action please open the restrant first\n" COLOR_RESET
#define MALFORMED_UDP COLOR_RED "recieved malformed udp message. got: " COLOR_RESET
#define USER_ALREADY_EXIST_TCP_FAIL COLOR_RED "Username exists but could not send on tcp.\n" COLOR_RESET
#define WAITING_RESTURANT_RESPONSE COLOR_BLUE "Waiting for restuant resoponse please wait...." COLOR_RESET
#define SHOW_RESSTURATS "username/port\n"
#define LINE_OF_DASH "------------------------------------------------------\n"
// consts
#define MAX_CONNECTION 4
#define LOCAL_HOST "127.0.0.1"
#define BROADCAST_IP "192.168.5.255"
#define USER_EXIST_DELAY 3

void print_resturant_cli();

void clear_cli();

void print_customer_cli();

void print_get_input();
#endif