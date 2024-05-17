#ifndef LIST_H
#define LIST_H
#include "cli.h"

RestaurantList create_resturant_list();

void add_restaurant(RestaurantList *list, resturant restaurant);

void remove_resturant(RestaurantList *list, const char *username);

void free_resturant_list(RestaurantList *list);

void print_resturant_list(RestaurantList list);

ResOpenList create_res_open_list();

void add_res_open(ResOpenList *list, ResOpen res_open);

void free_res_open_list(ResOpenList *list);

#endif