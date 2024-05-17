#include "net.h"
#include "list.h"
enum status
{
    IDLE,
    SHOW_RES,
    ORDER_FOD
};
int stat = IDLE;

void handle_show_resturants(int signal)
{
    write(STDOUT_FILENO, LINE_OF_DASH, _len(LINE_OF_DASH));
    write(STDOUT_FILENO, SHOW_RESSTURATS, _len(SHOW_RESSTURATS));
    alarm(0);
}
void handle_order_food_alarm(int signal){
    clear_cli();
    write(STDOUT_FILENO, "order food timed out.\n",23);
}

void handle_cli_customer(const costumer *cost, char *buff)
{
    char *command = strtok(buff, " \n");
    if (_match(command, "show_resturants"))
    {
        char msg[MAX_BUFF]; 
        sprintf(msg, AVAILABLE_RESTURANTS " %s %d", cost->username, htons(cost->tcp->addr.sin_port));
        broadcast_message(*cost->udp, msg);
        alarm(USER_EXIST_DELAY);
        signal(SIGALRM, handle_show_resturants);
        stat = SHOW_RES;
        return;
    }
    else if (_match(command, ORDER_FOOD))
    {
        print_get_input();
        write(STDOUT_FILENO, "Please enter name of food: ", _len("Please enter name of food: "));
        char *food_name = malloc(MAX_BUFF);
        read(STDIN_FILENO, food_name, MAX_BUFF);

        print_get_input();
        write(STDOUT_FILENO, "Please enter port number of resturant: ", _len("Please enter port number of resturant: "));
        char *port = malloc(MAX_BUFF);
        read(STDIN_FILENO, port, MAX_BUFF);

        print_get_input();
        write(STDOUT_FILENO, WAITING_RESTURANT_RESPONSE, _len(WAITING_RESTURANT_RESPONSE));
        char *msg = malloc(MAX_BUFF);
        sprintf(msg, ORDER_FOOD "%s ", cost->username);
        send_on_tcp(htons(atoi(port)), msg);
        alarm(30);
        signal(SIGALRM, handle_order_food_alarm);
        return;
    }
    write(STDOUT_FILENO, "\n", 2);
    print_customer_cli();
}

void handle_udp_customer(costumer *cost, char *buff)
{
    char *command = strtok(buff, " \n");
    if (_match(command, RES_CLOSE))
    {
        char *username = strtok(NULL, " \n");
        int port = atoi(strtok(NULL, " \n"));
        remove_resturant(&cost->rest_list, username);
        char *out = malloc(MAX_BUFF);
        clear_cli();
        sprintf(out, COLOR_CYAN RES_CLOSE " %s %d\n", username, port);
        write(STDOUT_FILENO, out, _len(out));
        print_customer_cli();
        return;
    }
    else if (_match(command, OPEN_RESTURANT))
    {
        clear_cli();
        char *username = strtok(NULL, " \n");
        int port = atoi(strtok(NULL, " \n"));
        resturant *rest = malloc(sizeof(resturant));
        rest->username = username;
        rest->tcp_port = port;
        add_restaurant(&cost->rest_list, *rest);
        char msg[MAX_BUFF];
        clear_cli();
        sprintf(msg, "resturant opened %s on port %d\n", username, port);
        write(STDOUT_FILENO, msg,_len(msg));
        print_customer_cli();
        return;
    }
    char *username = strtok(NULL, " \n");
    char *port = strtok(NULL, " \n");

    if (atoi(port)== htons(cost->tcp->addr.sin_port)) { // avoid reciveing my own messeges
        return;
    }


    if (_match(command, IS_USERNAME_EXIST)) {
        if (strcmp(username, cost->username) == 0) {
            if(!send_on_tcp(htons(atoi(port)), USER_ALREADY_EXIST)){
                write(STDOUT_FILENO, USER_ALREADY_EXIST_TCP_FAIL, _len(USER_ALREADY_EXIST_TCP_FAIL));
            }
        }       
    }
}

void handle_incoming(costumer *cost) {
    print_customer_cli();

    fd_set master_set, working_set;
    FD_ZERO(&master_set);
    FD_SET(cost->udp->fd, &master_set);
    FD_SET(STDIN_FILENO, &master_set);

    char *buff = malloc(MAX_BUFF);

    int max_fd = (cost->udp->fd > STDIN_FILENO) ? cost->udp->fd : STDIN_FILENO;
    while (1)
    {
        working_set = master_set;
        int action = select(max_fd + 1, &working_set, NULL, NULL, NULL);
        if (action < 0)
        {
            if (stat == SHOW_RES)
            {
                clear_cli();
                print_resturant_list(cost->rest_list);
                write(STDOUT_FILENO, LINE_OF_DASH, _len(LINE_OF_DASH));
                print_customer_cli();
                stat = IDLE;
            }else if (stat==ORDER_FOD){
                clear_cli();

            }
        }

        if (FD_ISSET(STDIN_FILENO, &working_set))
        { // handle cli input
            read(STDIN_FILENO, buff, MAX_BUFF);
            char *command = strtok(buff, " \n");
            if (command == NULL)
                continue;
            handle_cli_customer(cost, buff);
        }
        else if (FD_ISSET(cost->udp->fd, &working_set))
        { // handle incoming udp
            recv(cost->udp->fd, buff, MAX_BUFF, 0);
            handle_udp_customer(cost, buff);
        }
        else
        {
            for (int i = 0; i <= max_fd; i++)
            {
                if (FD_ISSET(i, &working_set))
                {
                    if (i == cost->tcp->fd)
                    {
                        int new_socket = accept_client(cost->tcp->fd);
                        FD_SET(new_socket, &master_set);
                        if (new_socket > max_fd)
                            max_fd = new_socket;
                    }
                    else
                    { // client sending msg
                        int bytes_received;
                        bytes_received = recv(i, buff, 1024, 0);
                        if (bytes_received == 0)
                        { // EOF
                            close(i);
                            FD_CLR(i, &master_set);
                            continue;
                        }
                        char *message = strtok(buff, " \n");
                        if (strcmp(message, OPEN_RESTURANT) == 0)
                        {
                            resturant rest;
                            strcpy(rest.username , strtok(NULL, " \n"));
                            rest.tcp_port = atoi(strtok(NULL, " \n"));
                            add_restaurant(&cost->rest_list, rest);
                        }
                    }
                }
            }
        }
        memset(buff, 0, MAX_BUFF);
    }
}

int main(int argc, char const *argv[]) {
    costumer *cost = malloc(sizeof(costumer));
    // char buff[MAX_BUFF] = {0};

    if (argc < 2) { // Check if we have port as argument
        write(STDOUT_FILENO, NO_PORT, _len(NO_PORT));
        return 1;
    }

    cost->tcp = choose_tcp_port();
    cost->udp = open_udp(atoi(argv[1]));
    strcpy(cost->username,get_username(cost->udp, cost->tcp));
    // cost->username = get_username(cost->udp, cost->tcp);
    cost->rest_list = create_resturant_list();

    char *welcome = malloc(MAX_BUFF);
    sprintf(welcome, COLOR_GREEN "Welcome custumer %s.Your are on tcp port: %d.\n", cost->username, htons(cost->tcp->addr.sin_port));
    write(STDOUT_FILENO, welcome, _len(welcome));
    handle_incoming(cost);

    return 0;
}
