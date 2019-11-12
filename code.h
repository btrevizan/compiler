#ifndef CODE_GEN_H
#define CODE_GEN_H

#include <stdlib.h>
#include <stdio.h>
#include "table.h"
#include "stack.h"
#include "tree.h"
#include "iloc.h"

#define MAX_LENGTH_LABEL 8
#define NOT_ARRAY -1
#define NOT_DECLARATION -1

typedef struct code {
    Operation* operation;
    struct code* next;
    struct code* prev;
} Code;

Code *instr_list;

int offset_rfp;

char* get_register();
char* get_label();

int get_global_offset(int type, int array_len);
void set_local_offset(int offset);
int get_local_offset(int type, int array_len);
int get_current_local_offset();

Code* init_code();
Code* add_dummy(Code* code);
Code* add_op(Code* code, Operation* op);
Code* remove_code(Code* code);

Code* make_code_load(Stack* scope, Node* id, Code* instr_list);
Code* make_code_store(Stack* scope, Node* id, Node* expr, Code* instr_list);
Code* make_code_store_assign(Stack* scope, Lexeme* id, Node* expr, Code* instr_list);

void destroy_code(Code* code);
void destroy_code_list(Code* code);

#endif //CODE_GEN_H
