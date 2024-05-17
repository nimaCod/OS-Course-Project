#include "net.h"
#include "list.h"

void handle_udp(const resturant rest, char *buff) {

    if((buff==NULL) | (_len(buff)==1)) {
        char *out = malloc(MAX_BUFF);
        clear_cli();
        sprintf(out, MALFORMED_UDP "%s\n", buff);
        write(STDOUT_FILENO, out, _len(out));
        print_resturant_cli();
        // free(out);
        return;
    }
    char command[MAX_BUFF];
    char username[MAX_BUFF];
    char port[MAX_BUFF];
    strcpy(command , strtok(buff, " \n"));
    strcpy(username , strtok(NULL, " \n"));
    strcpy(port , strtok(NULL, " \n"));

    if (atoi(port)== htons(rest.tcp->addr.sin_port)) { // avoid reciveing my own messeges
        return;
    }


    if (_match(command, IS_USERNAME_EXIST)) {
        if (strcmp(username, rest.username) == 0) {
            if(!send_on_tcp(htons(atoi(port)), USER_ALREADY_EXIST)){
                write(STDOUT_FILENO, USER_ALREADY_EXIST_TCP_FAIL, _len(USER_ALREADY_EXIST_TCP_FAIL));
            }
        }       
    } else if(_match(command,AVAILABLE_RESTURANTS)){
        char res[MAX_BUFF];
        sprintf(res, OPEN_RESTURANT " %s %d", rest.username, htons(rest.tcp->addr.sin_port));
        send_on_tcp(htons(atoi(port)), res);
    }
}

void handle_cli(char* buff){

}


void resturant_open(const resturant rest, char *buff)
{
    int new_socket, max_sd;
    fd_set master_set, working_set;

    FD_ZERO(&master_set);
    FD_SET(rest.tcp->fd, &master_set); // adding tcp to select
    FD_SET(rest.udp->fd, &master_set); // adding udp to select 
    FD_SET(STDIN_FILENO, &master_set); // adding cli to select
    max_sd = (rest.tcp->fd > rest.udp->fd) ? rest.tcp->fd : rest.udp->fd;
    print_resturant_cli();
    while (1)
    {
        working_set = master_set;
        select(max_sd + 1, &working_set, NULL, NULL, NULL);

        if (FD_ISSET(STDIN_FILENO, &working_set)) { // handle cli input
            read(STDIN_FILENO, buff, MAX_BUFF); 
            char *command = strtok(buff, " \n");
            if(command==NULL){continue;}
            if (_match(command, "break")) {
                char close_res[MAX_BUFF];
                sprintf(close_res, "res_close %s %d", rest.username, rest.tcp_port);
                broadcast_message(*rest.udp, close_res);
                return;
            }
            handle_cli(buff);

        } else if (FD_ISSET(rest.udp->fd, &working_set)) { // handle incoming udp
            recv(rest.udp->fd, buff, MAX_BUFF, 0);
            printf("GOT udp: %s\n", buff);
            handle_udp(rest, buff);
        } else {
            for (int i = 0; i <= max_sd; i++) {
                if (FD_ISSET(i, &working_set)) {
                    if (i == rest.tcp->fd) {
                        new_socket = accept_client(rest.tcp->fd);
                        FD_SET(new_socket, &master_set);
                        if (new_socket > max_sd)
                            max_sd = new_socket;
                    } else { // client sending msg
                        int bytes_received;
                        bytes_received = recv(i, buff, 1024, 0);
                        if (bytes_received == 0) { // EOF
                            close(i);
                            FD_CLR(i, &master_set);
                            continue;
                        }
                    }
                }
            }
        }

        memset(buff, 0, MAX_BUFF);
    }
}

void resturant_close()  {
    char buff[MAX_BUFF];
    fd_set  working_set;

    FD_ZERO(&working_set);
    FD_SET(STDIN_FILENO, &working_set);
    print_resturant_cli();
    while (1) {
        select( 1, &working_set, NULL, NULL, NULL);
        if (FD_ISSET(STDIN_FILENO, &working_set)) {
            read(STDIN_FILENO, buff, MAX_BUFF); 
            char *command = strtok(buff, " \n");
            if(command==NULL){continue;}
            if (_match(command, "start")) {
                char *command2 = strtok(NULL, " \n");
                if (command2==NULL) continue;
                if(_match(command2,"working")){
                    return;
                }
                }
            else {
                write(STDOUT_FILENO,RESTURANT_IS_CLOSED,_len(RESTURANT_IS_CLOSED));
                print_resturant_cli();
            }
        }
        memset(buff, 0, MAX_BUFF);
    }
}

int main(int argc, char const *argv[])
{
    resturant rest;
    char buff[MAX_BUFF] = {0};  

    if (argc < 2)   
    { // Check if we have port as argument
        write(STDOUT_FILENO, NO_PORT, _len(NO_PORT));
        return 1;
    }
    rest.udp = open_udp(atoi(argv[1]));
    rest.tcp = choose_tcp_port();
    rest.tcp_port = htons(rest.tcp->addr.sin_port);
    rest.username = get_username(rest.udp, rest.tcp);

    char resturant_open_msg[MAX_BUFF];
    sprintf(resturant_open_msg, OPEN_RESTURANT " %s %d\n", rest.username, htons(rest.tcp->addr.sin_port));
    char welecom[MAX_BUFF];
    sprintf(welecom, COLOR_GREEN "Welcome %s.Your resturant is open on tcp port: %d.\n", rest.username, htons(rest.tcp->addr.sin_port));
    while (1)
    {
        broadcast_message(*rest.udp, resturant_open_msg);
        write(STDOUT_FILENO, welecom, _len(welecom));
        resturant_open(rest, buff);
        write(STDOUT_FILENO, RESTURANT_CLOSED, _len(RESTURANT_CLOSED));
        resturant_close();
    }
}
