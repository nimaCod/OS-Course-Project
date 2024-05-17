#include "cli.h"

void print_resturant_cli() {
    write(STDOUT_FILENO, RESTURANT_STARTER, _len(RESTURANT_STARTER));
}

void clear_cli() {
    write(STDOUT_FILENO, CLEAR_STDIN, _len(CLEAR_STDIN));
}

void print_customer_cli() {
    write(STDOUT_FILENO, COSTUMER_STARTER, _len(COSTUMER_STARTER));
}

void print_get_input(){
    write(STDOUT_FILENO, GET_INPUT_STARTER,_len(GET_INPUT_STARTER));
}
