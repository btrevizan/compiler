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

Code* make_code_loadI(Node* literal);
Code* make_code_loadAI(Stack* scope, Node* id);
Code* make_code_store(Stack* scope, Lexeme* id, Node* expr);

Code* make_code_nop();
Code* make_code_binop(char* op, Node* a, Node* b);
Code* make_code_conversion(char* op, char* r1, char* r2);
Code* make_code_flow_control(char* op, char* r1, char* r2, char* r3, int type);
Code* make_code_jump(char* op, char* r1);

Code* make_code_add(Node* a, Node* b);
Code* make_code_sub(Node* a, Node* b);
Code* make_code_mult(Node* a, Node* b);
Code* make_code_div(Node* a, Node* b);
Code* make_code_lshift(Node* a, Node* b);
Code* make_code_rshift(Node* a, Node* b);
Code* make_code_and(Node* a, Node* b);
Code* make_code_or(Node* a, Node* b);
Code* make_code_xor(Node* a, Node* b);

Code* make_code_i2i(char* r1, char* r2);
Code* make_code_c2c(char* r1, char* r2);
Code* make_code_c2i(char* r1, char* r2);
Code* make_code_i2c(char* r1, char* r2);

Code* make_code_jmp(char* r1);
Code* make_code_jmpI(char* l1);
Code* make_code_cbr(char* r1, char* l2, char* l3);
Code* make_code_cmpLT(char* r1, char* r2, char* r3);
Code* make_code_cmpLE(char* r1, char* r2, char* r3);
Code* make_code_cmpEQ(char* r1, char* r2, char* r3);
Code* make_code_cmpGE(char* r1, char* r2, char* r3);
Code* make_code_cmpGT(char* r1, char* r2, char* r3);
Code* make_code_cmpNE(char* r1, char* r2, char* r3);


void destroy_code(Code* code);
void destroy_code_list(Code* code);

#endif //CODE_GEN_H
