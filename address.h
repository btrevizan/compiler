#ifndef COMPILER_ADDRESS_H
#define COMPILER_ADDRESS_H

int offset_rsp;
int offset_rfp;

int get_global_offset(int type, int array_len);
void set_local_offset(int offset);
int get_local_offset(int type, int array_len);
int get_current_local_offset();
void set_param_offset(int offset);
int get_param_offset(int type, int array_len);

#endif //COMPILER_ADDRESS_H
