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

typedef struct code_t {
    Operation* operation;
    struct code_t* next;
    struct code_t* prev;
} Code;

int offset_rfp;

char* get_register();
char* get_label();

int get_global_offset(int type, int array_len);
void set_local_offset(int offset);
int get_local_offset(int type, int array_len);
int get_current_local_offset();

Code* init_code();
void add_dummy(Code* code);
void add_op(Code* code, Operation* op);
void concat_code(Code* c1, Code* c2);

void load(Code* code, Stack* scope, Node* id);
void store(Code* code, Stack* scope, Node* id, Node* expr);
void store_assign(Code* code, Stack* scope, Lexeme* id, Node* expr);

void nop(Code* code);
void binop(Code* code, char* op_name, Node* expr1, Node* expr2, Node* op);
void cmp(Code* code, char* op_name, Node* expr1, Node* expr2, Node* op);

//void jump(char* op, char* r1);
//void conversion(char* op, char* r1, char* r2);

char* load_index(Code* code, Symbol* s, Node* id);

void destroy_code(Code* code);
void destroy_code_list(Code* code);

#endif //CODE_GEN_H
