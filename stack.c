#include "stack.h"
#include "symbol_table.h"

Stack* init_stack() {
    Stack* stack = malloc(sizeof(stack));
    stack->top = NULL;
    stack->size = 0;

    return stack;
}

Table* peek(Stack* stack) {
    if(stack == NULL) return NULL;
    if(stack->top == NULL) return NULL;
    
    return stack->top->value;
}

void push(Stack* stack, Table* value) {
    if(stack == NULL) return;

    Item* item = malloc(sizeof(Item));
    item->value = value;
    item->prev = NULL;

    item->next = stack->top;
    if(stack->top != NULL) stack->top->prev = item;

    stack->top = item;
    stack->size++;
}

Table* pop(Stack* stack) {
    if(stack == NULL) return NULL;
    if(stack->top == NULL) return NULL;

    Item* item = stack->top;
    Table* table = item->value;

    stack->top = stack->top->next;
    if(stack->top != NULL) stack->top->prev = NULL;

    stack->size--;

    free(item);
    return table;
}

void destroy_stack(Stack* stack) {
    if(stack == NULL) return;

    Table* table;
    while(stack->top != NULL) {
        table = pop(stack);
        delete_table(table);
    }

    free(stack);
}