#ifndef TYPES_H
#define TYPES_H

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/in.h> 
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#define MAX_BUFF 1024

/*struct that contains a tcp or upd file descriptor and sockaddr_in*/
typedef struct
{
    int fd;
    struct sockaddr_in addr;
} Connection;

/*struct to store open resturants name and port*/
typedef struct 
{
    char name[MAX_BUFF];
    int port;  
} ResOpen;

/*struct to store list of open resturants*/
typedef struct {
    ResOpen *res_opens;
    int size;
    int capacity;
} ResOpenList;

/*struct for resturant user containing tcp and udp connection and username*/
typedef struct
{
    Connection *tcp;
    Connection *udp;
    int tcp_port;
    char *username;
    ResOpenList res_open_list;
} resturant;

/*struct to store resturant list*/
typedef struct {
    resturant *restaurants;
    int size;
    int capacity;
} RestaurantList;

/*struct to store customer user tcp and udp connection and username*/
typedef struct
{
    Connection *tcp;
    Connection *udp;
    RestaurantList rest_list;
    char username[MAX_BUFF];
} costumer;

/*structw to store supplier tcp and upod and username */
typedef struct
{
    Connection *tcp;
    Connection *udp;
    int tcp_port;
    char username[MAX_BUFF];
} Supplier;

/*struct to store list of suppliers*/
typedef struct {
    Supplier *supplier_list;
    int size;
    int capacity;
} SupplierList;



/*this fuction returns length of a string*/
int _len(char const buf[]);
/*this function checks wheter string1 starts with string2 or not */
int _match(char in1[],char in2[]);

#endif 
