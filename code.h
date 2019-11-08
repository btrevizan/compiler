#ifndef CODE_GEN_H
#define CODE_GEN_H

#include <stdlib.h>
#include <stdio.h>
#include "table.h"

#define MAX_LENGTH_LABEL 8
#define NOT_ARRAY -1
#define NOT_DECLARATION -1

int offset_rfp;

char* get_register();
char* get_label();

int get_global_offset(int type, int array_len);
void set_local_offset(int offset);
int get_local_offset(int type, int array_len);
int get_current_local_offset();

void gera_codigo(void* arvore);

#endif //CODE_GEN_H
