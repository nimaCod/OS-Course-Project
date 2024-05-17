#include "net.h"

void broadcast_message(Connection udp, char *msg) {
    sendto(udp.fd, msg, strlen(msg), 0, (struct sockaddr *)&udp.addr, sizeof(udp.addr));
}

int send_on_tcp(int port, const char *msg) {
    int fd;
    struct sockaddr_in server_address;

    fd = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_port = port;
    server_address.sin_addr.s_addr = INADDR_ANY; // inet_addr(LOCAL_HOST);

    if (connect(fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) { 
        write(STDOUT_FILENO,COLOR_RED "Error in connecting to server\n",_len(COLOR_RED "Error in connecting to server\n"));
        return 0;
    }
    send(fd, msg, _len(msg), 0);
    close(fd);

    return 1;
}

Connection *open_udp(int udp_port) {
    Connection *udp = malloc(sizeof(Connection));
    udp->fd = socket(PF_INET, SOCK_DGRAM, 0);
    int broadcast = 1;
    int reuseport = 1;
    setsockopt(udp->fd, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
    setsockopt(udp->fd, SOL_SOCKET, SO_REUSEPORT, &reuseport, sizeof(reuseport));

    udp->addr.sin_family = AF_INET;
    udp->addr.sin_port = htons(udp_port);
    inet_pton(AF_INET, BROADCAST_IP, &(udp->addr.sin_addr.s_addr));
    memset(udp->addr.sin_zero, '\0', sizeof(udp->addr.sin_zero));

    bind(udp->fd, (struct sockaddr *)&udp->addr, sizeof(udp->addr));
    write(STDOUT_FILENO, UDP_BIND_SUCCESS, _len(UDP_BIND_SUCCESS));
    return udp;
}

Connection *create_tcp(int port) {
    Connection *tcp = malloc(sizeof(Connection));
    tcp->fd = socket(AF_INET, SOCK_STREAM, 0);

    tcp->addr.sin_family = AF_INET;
    tcp->addr.sin_addr.s_addr = INADDR_ANY;
    tcp->addr.sin_port = htons(port);

    if (bind(tcp->fd, (struct sockaddr *)&tcp->addr, sizeof(tcp->addr)) == -1) {
        tcp->fd = -1;
        return tcp;
    }

    listen(tcp->fd, MAX_CONNECTION);
    return tcp;
}

Connection *choose_tcp_port() {
    int success = 0;
    Connection *tcp;

    while (!success) {
        srand(time(NULL));
        int tcp_port = rand() % (65535 - 1024 + 1) + 1024; // port number can be between 1024 and 65535
        tcp = create_tcp(tcp_port);
        if (tcp->fd >= 0)
        {
            success = 1;
        }
    }
    write(STDOUT_FILENO, TCP_BIND_SUCCESS, _len(TCP_BIND_SUCCESS));
    return tcp;
}

int connect_tcp(uint16_t port)
{
    int fd;
    struct sockaddr_in server_address;

    fd = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(LOCAL_HOST);

    if (connect(fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        clear_cli();
        write(STDOUT_FILENO, TCP_CONNECT_ERROR, _len(TCP_CONNECT_ERROR));
    }

    return fd;
}

int accept_client(int fd)
{
    int client_fd;
    struct sockaddr_in client_address;
    int address_len = sizeof(client_address);

    client_fd = accept(fd, (struct sockaddr *)&client_address, (socklen_t *)&address_len);

    return client_fd;
}

void user_exists_alarm_handle(int signal) {
    write(STDOUT_FILENO, USERNAME_SUCCESS, _len(USERNAME_SUCCESS));
}

int username_exists(Connection udp, Connection tcp, char *username) {
    char *msg = malloc(MAX_BUFF);
    sprintf(msg, IS_USERNAME_EXIST " %s %d", username, ntohs(tcp.addr.sin_port));
    broadcast_message(udp, msg);


    fd_set working_set;
    FD_ZERO(&working_set);
    FD_SET(tcp.fd, &working_set);
    alarm(USER_EXIST_DELAY);
    signal(SIGALRM, user_exists_alarm_handle);

    int action=select(tcp.fd + 1, &working_set, NULL, NULL, NULL);
    alarm(0);

    if (action == -1) { // if sigalarm comes select returns -1 so we have timeout
        return 1;
    }
    if (FD_ISSET(tcp.fd,&working_set)){
        int fd = accept_client(tcp.fd);
        FD_SET(fd, &working_set);
        select(fd + 1, &working_set, NULL, NULL, NULL);

        char *data={0};
        recv(fd, data, MAX_BUFF, 0);
        close(fd);
    }

    return 0; // when we pass from select that means someone made a tcp connection to us so they had username
}

char *get_username(Connection *udp, Connection *tcp) {
    int user_exists = 0;
    char username[MAX_BUFF];
    write(STDOUT_FILENO, GET_USERNAME_MESSAGE, _len(GET_USERNAME_MESSAGE));
    read(STDIN_FILENO, username, MAX_BUFF);
    char *username_toked = strtok(username, " \n");
    if(username != NULL)
        user_exists = username_exists(*udp, *tcp, username_toked);
    while (!user_exists) {
        memset(username, 0, MAX_BUFF);
        memset(username_toked, 0, MAX_BUFF);
        write(STDOUT_FILENO, USERNAME_ALREADY_EXIST, _len(USERNAME_ALREADY_EXIST));
        read(STDIN_FILENO, username, MAX_BUFF);
        username_toked = strtok(username, " \n");
        if(username_toked == NULL){ continue;}
        user_exists = username_exists(*udp, *tcp, username_toked);
        
    }
    return username_toked;
}
