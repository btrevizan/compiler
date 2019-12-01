#include "address.h"
#include "table.h"
#include <stdlib.h>
#include <stdio.h>

int get_global_offset(int type, int array_len){
    static int offset_rbss = 0;
    int prev_offset = offset_rbss;

    if(array_len <= 0)
        array_len = 1;

    offset_rbss += get_type_size(type)*array_len;

    return prev_offset;
}

void set_local_offset(int offset){
    offset_rsp = offset;
}

int get_current_local_offset(){
    return offset_rsp;
}

int get_local_offset(int type, int array_len){
    if(array_len <= 0)
        array_len = 1;

    offset_rsp -= get_type_size(type)*array_len;

    return offset_rsp;
}

void set_param_offset(int offset){
    offset_rfp = offset;
}

int get_param_offset(int type, int array_len){
    if(array_len <= 0)
        array_len = 1;

    int prev_offset = offset_rfp;
    offset_rfp += get_type_size(type)*array_len;

    return prev_offset;
}