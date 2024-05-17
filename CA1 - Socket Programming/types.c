#include "types.h"

int _len(char const buf[]){
    int buf_len=0;
    while(buf[buf_len]!='\0'){
        buf_len++;
    }
    return buf_len;
}

int _match(char in1[],char in2[]){
    int len2 = _len(in2);
    for (int i = 0; i < len2;i++) {
        if(in1[i] != in2[i]){
            return 0;
        }
    }
    return 1;
}