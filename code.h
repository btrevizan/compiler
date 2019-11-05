#ifndef CODE_GEN_H
#define CODE_GEN_H

#include <stdlib.h>
#include <stdio.h>

#define MAX_LENGTH_LABEL 8

char* get_register();
char* get_label();

void gera_codigo(void* arvore);

#endif //CODE_GEN_H
