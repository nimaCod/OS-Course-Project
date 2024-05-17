#include "serialize.h"

using namespace std;

string serialize_monthly_bills(const vector<MonthlyBill*>& bills) {
    string serialized;
    serialized += to_string(bills.size()) + ";";

    for (int i=0; i<bills.size(); i++) {
        serialized += to_string(bills[i]->year) + ",";
        serialized += to_string(bills[i]->month) + ",";
        serialized += to_string(bills[i]->resource) + ",";
        serialized += to_string(bills[i]->resource_busy) + ",";
        serialized += to_string(bills[i]->resource_price) + ",";
        serialized += to_string(bills[i]->resource_behineh) + ",";
        serialized += to_string(bills[i]->busy_hour) + ",";
        serialized += to_string(bills[i]->sum_prev) + ";" ;
    }

    return serialized;
}

vector<vector<MonthlyBill*>> deserialize_monthly_bills(const string& serialized) {
    vector<vector<MonthlyBill*>> all_3_bills;

    size_t start = 0;
    size_t end = 0;

    for(int i=0;i<3;i++){   
        vector<MonthlyBill*> bills;
        int pos=serialized.find(';', start);
        int  num_bill = stoi(serialized.substr(0, pos));
        string token = serialized.substr(pos + 1);
        end =pos;
        start = pos +1 ;

        for(int j=0;j<num_bill;j++) {
            end = serialized.find(';', start); 
            token = serialized.substr(start, end - start);

            size_t pos = 0;
            MonthlyBill* bill = new(MonthlyBill);

            pos = token.find(',');
            bill->year = stoi(token.substr(0, pos));
            
            token = token.substr(pos + 1);
            pos = token.find(',');
            bill->month = stoi(token.substr(0, pos));

            token = token.substr(pos + 1);
            pos = token.find(',');
            bill->resource = stoi(token.substr(0, pos));

            token = token.substr(pos + 1);
            pos = token.find(',');
            bill->resource_busy = stoi(token.substr(0, pos));

            token = token.substr(pos + 1);
            pos = token.find(',');
            bill->resource_price = stoi(token.substr(0, pos));

            token = token.substr(pos + 1);
            pos = token.find(',');
            bill->resource_behineh = stoi(token.substr(0, pos));

            token = token.substr(pos + 1);
            pos = token.find(',');
            bill->busy_hour = stoi(token.substr(0, pos));

            token = token.substr(pos + 1);
            bill->sum_prev = stoi(token);
            bills.push_back(bill);

            start = end + 1;
        }
    all_3_bills.push_back(bills);    
    }
    return all_3_bills;
}

