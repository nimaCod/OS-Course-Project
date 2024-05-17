#ifndef NET_H
#define NEt_H

#include "cli.h"

void broadcast_message(Connection udp, char *msg);

int send_on_tcp(int port, const char *msg);

Connection *open_udp(int udp_port);

Connection *create_tcp(int port);

Connection *choose_tcp_port();

int connect_tcp(uint16_t port);

int accept_client(int fd);

void user_exists_alarm_handle(int signal);

int username_exists(Connection udp, Connection tcp, char *username);

char *get_username(Connection *udp, Connection *tcp);
#endif