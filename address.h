#ifndef COMPILER_ADDRESS_H
#define COMPILER_ADDRESS_H

int offset_rsp;

int get_global_offset(int type, int array_len);
void set_local_offset(int offset);
int get_local_offset(int type, int array_len);
int get_current_local_offset();

#endif //COMPILER_ADDRESS_H
