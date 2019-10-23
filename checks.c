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

// Throws ERR_VARIABLE, ERR_VECTOR and ERR_FUNCTION
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