#ifndef COMPILER_STACK_H
#define COMPILER_STACK_H

#include "symbol_table.h"

typedef struct item {
    struct item* next;
    struct item* prev;
    Table* value;
} Item;

typedef struct stack {
    Item* top;
    int size;
} Stack;

Stack* scope;

Stack* init_stack();
Table* peek(Stack* stack);
void push(Stack* stack, Table* value);
Table* pop(Stack* stack);
void destroy_stack(Stack* stack);
Symbol* search(Stack* stack, const char* key);

#endif //COMPILER_STACK_H
