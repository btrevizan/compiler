#include <stdlib.h>
#include <stdio.h>
#include "lexical.h"
#include "checks.h"
#include "stack.h"
#include "errors.h"
#include "tree.h"
#include "table.h"

void check_declaration(Stack* stack, Node* id) {
    Symbol* symbol = search(stack, id->value->token_value.string);

    if(symbol == NULL) {
        printf("ERR_UNDECLARED: '%s' was not declared.\n", id->value->token_value.string);
        exit(ERR_UNDECLARED);
    }
}

void check_usage(Stack* stack, Node* id) {
    Symbol* symbol = search(stack, id->value->token_value.string);

    switch (symbol->nature) {
        case NATUREZA_IDENTIFICADOR:
            if(id->value->token_type != TK_ID) {
                printf("ERR_VARIABLE. '%s' is a variable.\n", id->value->token_value.string);
                exit(ERR_VARIABLE);
            }
            break;

        case NATUREZA_FUNCAO:
            if(id->value->token_type != TK_FN) {
                printf("ERR_FUNCTION. '%s' is a function.\n", id->value->token_value.string);
                exit(ERR_FUNCTION);
            }
            break;

        case NATUREZA_VETOR:
            if(id->value->token_type != TK_VC) {
                printf("ERR_VECTOR. '%s' is a vector.\n", id->value->token_value.string);
                exit(ERR_VECTOR);
            }
    }
}

void implicit_conversion(int expected, Node* given) {
    if(expected == given->type) return;

    if(given->type == TYPE_STRING && expected != TYPE_STRING) {
        printf("ERR_STRING_TO_X. Cannot convert %d to string.\n", given->type);
        exit(ERR_STRING_TO_X);
    }

    if(given->type == TYPE_CHAR && expected != TYPE_CHAR) {
        printf("ERR_CHAR_TO_X. Cannot convert %d to char.\n", given->type);
        exit(ERR_STRING_TO_X);
    }

    if(expected == TYPE_INT && (given->type == TYPE_FLOAT || given->type == TYPE_BOOL))
        given->type = TYPE_INT;
    else if(expected == TYPE_FLOAT && (given->type == TYPE_INT || given->type == TYPE_BOOL))
        given->type = TYPE_FLOAT;
    else if(expected == TYPE_BOOL && (given->type == TYPE_INT || given->type == TYPE_FLOAT))
        given->type = TYPE_BOOL;
    else {
        printf("ERR_WRONG_TYPE. Expecting type %d, but %d was given.\n", expected, given->type);
        exit(ERR_WRONG_TYPE);
    }
}

// Needs still to throw ERR_WRONG_PAR_INPUT and ERR_WRONG_PAR_OUTPUT
void check_type(int operation, Node* node) {

    if(operation == ARITH_OP) {
        if(node->type == TYPE_INT) return;
        if(node->type == TYPE_FLOAT) return;

        implicit_conversion(TYPE_INT, node);  // try implicit conversion expecting that node->type = TYPE_BOOL
    } else if (operation == BOOL_OP) {
        implicit_conversion(TYPE_BOOL, node);
    }

}

void check_return_type(Stack* scope, Node* expr_node) {
    // Find last function inserted in table
    Item* item = scope->top;
    Symbol* function = NULL;
    Table* table;

    while(item != NULL) {
        table = item->value;

        for(int i = 0; i < table->size; i++) {
            if(table->entries[i] != NULL && table->entries[i]->value->nature == NATUREZA_FUNCAO) {
                function = table->entries[i]->value;
                break;
            }
        }

        item = item->next;
    }

    if(function == NULL) {
        printf("ERROR. 'return' command in global scope.\n");
        exit(-1);
    }

    // Compare function (symbol) type with the expr type in return
    if(function->type != expr_node->type) {
        printf("ERR_WRONG_PAR_RETURN. Expecting type %d, but %d was given.\n", function->type, expr_node->type);
        exit(ERR_WRONG_PAR_RETURN);
    }
}