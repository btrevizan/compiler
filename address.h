#ifndef COMPILER_ADDRESS_H
#define COMPILER_ADDRESS_H

#define RFP_START_VALUE 2048
#define RSP_START_VALUE 2048
#define RBSS_START_VALUE 1024

int offset_rsp;

int get_global_offset(int type, int array_len);
void set_local_offset(int offset);
int get_local_offset(int type, int array_len);
int get_current_local_offset();

#endif //COMPILER_ADDRESS_H
