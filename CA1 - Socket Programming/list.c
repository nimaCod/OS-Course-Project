#include "cli.h"
#include "list.h"
// resturant list functions 
RestaurantList create_resturant_list() {
    RestaurantList list;
    list.restaurants = (resturant *)malloc(sizeof(resturant));
    list.size = 0;
    list.capacity = 1;
    return list;
}

void add_restaurant(RestaurantList *list, resturant restaurant) {
    // Check if the restaurant already exists
    for (int i = 0; i < list->size; i++)
    {
        if (strcmp(list->restaurants[i].username, restaurant.username) == 0) {
            return;
        }
    }

    if (list->size >= list->capacity) {
        list->capacity *= 2;
        list->restaurants = realloc(list->restaurants, list->capacity * sizeof(restaurant));   
        }
    
    list->restaurants[list->size] = restaurant;
    list->size++;
}

void remove_resturant(RestaurantList* list, const char* username) {
    int index = -1;

    // Find the index of the restaurant with the specified name
    for (int i = 0; i < list->size; i++) {
        if (strcmp(list->restaurants[i].username, username) == 0) {
            index = i;
            break;
        }
    }

    // If the restaurant is found, remove it from the list
    if (index != -1) {
        // Shift the elements after the removal index to the left
        for (int i = index; i < list->size - 1; i++) {
            list->restaurants[i] = list->restaurants[i + 1];
        }

        list->size--;

    } 
}


void free_resturant_list(RestaurantList *list) {
    free(list->restaurants);
    list->size = 0;
    list->capacity = 0;
}

void print_resturant_list(RestaurantList list)
{
    for (int i = 0; i < list.size; i++)
    {
        char out[MAX_BUFF];
        sprintf(out,COLOR_BLUE "%s " COLOR_MAGENTA "%d\n" COLOR_RESET, list.restaurants[i].username, list.restaurants[i].tcp_port);
        write(STDOUT_FILENO, out, _len(out));
    }
}
// res_open list functions
ResOpenList create_res_open_list() {
    ResOpenList list;
    list.res_opens = (ResOpen *)malloc(sizeof(ResOpen));
    list.size = 0;
    list.capacity = 1;
    return list;
}

void add_res_open(ResOpenList *list, ResOpen res_open) {
    // Check if the restaurant already exists
    for (int i = 0; i < list->size; i++) {
        if (strcmp(list->res_opens[i].name, res_open.name) == 0) {
            return;
        }
    }

    if (list->size >= list->capacity) {
        list->capacity *= 2;
        list->res_opens = (ResOpen *)realloc(list->res_opens, list->capacity * sizeof(ResOpen));
    }
    
    list->res_opens[list->size] = res_open;
    list->size++;
}

void free_res_open_list(ResOpenList *list) {
    free(list->res_opens);
    list->size = 0;
    list->capacity = 0;
}



