#include <stdlib.h>
#include <stdio.h>
#include "lexical.h"
#include "checks.h"
#include "stack.h"
#include "error.h"
#include "tree.h"
#include "symbol_table.h"

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
                printf("ERR_VARIABLE. '%s' is a variable.", id->value->token_value.string);
                exit(ERR_VARIABLE);
            }
            break;

        case NATUREZA_FUNCAO:
            if(id->value->token_type != TK_FN) {
                printf("ERR_FUNCTION. '%s' is a function.", id->value->token_value.string);
                exit(ERR_FUNCTION);
            }
            break;

        case NATUREZA_VETOR:
            if(id->value->token_type != TK_VC) {
                printf("ERR_VECTOR. '%s' is a vector.", id->value->token_value.string);
                exit(ERR_VECTOR);
            }
    }
}

void implicit_conversion(int expected, Node* given) {
    if(expected == given->type) return;

    if(given->type == TYPE_STRING && expected != TYPE_STRING) {
        printf("ERR_STRING_TO_X. Cannot convert %d to string.", given->type);
        exit(ERR_STRING_TO_X);
    }

    if(given->type == TYPE_CHAR && expected != TYPE_CHAR) {
        printf("ERR_CHAR_TO_X. Cannot convert %d to char.", given->type);
        exit(ERR_STRING_TO_X);
    }

    if(expected == TYPE_INT && (given->type == TYPE_FLOAT || given->type == TYPE_BOOL))
        given->type = TYPE_INT;
    else if(expected == TYPE_FLOAT && (given->type == TYPE_INT || given->type == TYPE_BOOL))
        given->type = TYPE_FLOAT;
    else if(expected == TYPE_BOOL && (given->type == TYPE_INT || given->type == TYPE_FLOAT))
        given->type = TYPE_BOOL;
    else {
        printf("ERR_WRONG_TYPE. Expecting type %d, but %d was given.", expected, given->type);
        exit(ERR_WRONG_TYPE);
    }
}

// Needs still to throw ERR_WRONG_PAR_RETURN, ERR_WRONG_PAR_INPUT and ERR_WRONG_PAR_OUTPUT
void check_type(int operation, Node* node) {

    if(operation == ARITH_OP) {
        if(node->type == TYPE_INT) return;
        if(node->type == TYPE_FLOAT) return;

        implicit_conversion(TYPE_INT, node);  // try implicit conversion expecting that node->type = TYPE_BOOL
    } else if (operation == BOOL_OP) {
        implicit_conversion(TYPE_BOOL, node);
    }

}