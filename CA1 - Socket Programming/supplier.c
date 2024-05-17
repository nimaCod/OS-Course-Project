// #include "net.h"

// void handle_cli_customer(costumer cost, char *buff)
// {
//     char *command = strtok(buffer, " \n");


//      if (_match(buffer, AVAILABLE_RESTURANTS)) {
//         printf("broadcasting available_resturants\n");
//         char msg[MAX_BUFF];
//         sprintf(msg, AVAILABLE_RESTURANTS " %s", cost.username);
//         broadcast_message(*cost.udp, AVAILABLE_RESTURANTS);
//     }
// }

// void handle_udp_customer(costumer cost, char *buff){
//     char *command = strtok(buffer, " \n");
//     if (_match(command, IS_USERNAME_EXIST)) {
//         char *username = strtok(NULL, " \n");
//         if (strcmp(username, cost.username) == 0) {
//             char *port = strtok(NULL, " \n");
//             if(!send_on_tcp(ntohs(atoi(port)), USER_ALREADY_EXIST)){
//                 write(STDOUT_FILENO, USER_ALREADY_EXIST_TCP_FAIL, _len(USER_ALREADY_EXIST_TCP_FAIL));
//             }
//         }
//     } 
// }

// void handle_incoming(costumer cost)
// {
//     print_customer_cli();

//     fd_set master_set, working_set;
//     FD_ZERO(&master_set);
//     FD_SET(cost.udp->fd, &master_set);
//     FD_SET(STDIN_FILENO, &master_set);
//     char buff[MAX_BUFF] = {0};

//     int max_fd = (cost.udp->fd > STDIN_FILENO) ? cost.udp->fd : STDIN_FILENO;
//     while (1)
//     {
//         working_set = master_set;
//         select(max_sd + 1, &working_set, NULL, NULL, NULL);

//         if (FD_ISSET(STDIN_FILENO, &working_set))
//         { // handle cli input
//             read(STDIN_FILENO, buff, MAX_BUFF);
//             char *command = strtok(buff, " \n");
//             if (command == NULL)
//                 continue;
//             handle_cli_customer(buff);
//         }
//         else if (FD_ISSET(cost.udp->fd, &working_set))
//         { // handle incoming udp
//             recv(rest.udp->fd, buff, MAX_BUFF, 0);
//             handle_udp_customer(rest, buff);
//         }
//         else {
//             for (int i = 0; i <= max_sd; i++) {
//                 if (FD_ISSET(i, &working_set)) {
//                     if (i == rest.tcp->fd) {
//                         new_socket = accept_client(rest.tcp->fd);
//                         FD_SET(new_socket, &master_set);
//                         if (new_socket > max_sd)
//                             max_sd = new_socket;
//                     } else { // client sending msg
//                         int bytes_received;
//                         bytes_received = recv(i, buff, 1024, 0);
//                         if (bytes_received == 0) { // EOF
//                             close(i);
//                             FD_CLR(i, &master_set);
//                             continue;
//                         }
//                         printf("client %d: %s\n", i, buff);
//                     }
//                 }
//             }
//         }
//         memset(buff, 0, MAX_BUFF);
//     }
// }

// int main(int argc, char const *argv[])
// {
//     costumer cost;
//     char buff[MAX_BUFF] = {0};

//     if (argc < 2)
//     { // Check if we have port as argument
//         write(STDOUT_FILENO, NO_PORT, _len(NO_PORT));
//         return 1;
//     }

//     cost.tcp = choose_tcp_port();
//     cost.udp = open_udp(atoi(argv[1]));
//     cost.username = get_username(cost.udp, cost.tcp);
//     char *welcome = malloc(MAX_BUFF);
//     sprintf(welcome, COLOR_GREEN "Welcome custumer %s.Your are on tcp port: %d.\n", cost.username, htons(cost.tcp->addr.sin_port));
//     handle_incoming(cost);

//     return 0;
// }
