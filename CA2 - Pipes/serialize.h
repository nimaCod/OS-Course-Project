#ifndef SERIALIZE_H
#define SERIALIZE_H
#include <iostream>
#include <string>
#include <vector>
#include "logger.h"


#define MSG_BUFF_SIZE 2048
#define NAMED_PIPE "billing_office_named_pipe"
using namespace std;
typedef struct  {
    int year;
    int month;
    int resource; // sum of all water except busy hours
    int resource_busy;
    int resource_price;
    int resource_behineh;
    int busy_hour;
    int sum_prev;

}MonthlyBill;

string serialize_monthly_bills(const vector<MonthlyBill*>& bills) ;
vector<vector<MonthlyBill*>> deserialize_monthly_bills(const string& serialized) ;


#endif