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

typedef struct code_list {
    Code* begin;
    Code* end;
} CodeList;

int offset_rfp;

char* get_register();
char* get_label();

int get_global_offset(int type, int array_len);
void set_local_offset(int offset);
int get_local_offset(int type, int array_len);
int get_current_local_offset();

Code* init_code();
CodeList* init_codelist();
void add_dummy(CodeList* codelist);
void add_op(CodeList* codelist, Operation* op);
CodeList* concat_code(CodeList* c1, CodeList* c2);

void load(CodeList* codelist, Stack* scope, Node* id);
void store(CodeList* codelist, Stack* scope, Node* id, Node* expr);
void store_assign(CodeList* codelist, Stack* scope, Lexeme* id, Node* expr);

void nop(CodeList* codelist);
void numeric(char* op_name, Node* expr1, Node* expr2, Node* op);
void cmp(char* op_name, Node* expr1, Node* expr2, Node* op);
void and(Node* expr1, Node* expr2, Node* op);
void or(Node* expr1, Node* expr2, Node* op);
void if_then_else(Node* if_then, Node* else_block);

//void jump(char* op, char* r1);
//void conversion(char* op, char* r1, char* r2);

char* load_index(CodeList* codelist, Symbol* s, Node* id);

void destroy_code(Code* code);
void destroy_code_list(CodeList* codelist);

#endif //CODE_GEN_H
