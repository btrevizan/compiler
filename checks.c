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
    int res = implicit_conversion_check(expected, given);
    if(res == ERR_STRING_TO_X){
        printf("ERR_STRING_TO_X. Cannot convert %d to string.\n", given->type);
        exit(ERR_STRING_TO_X);
    } else if (res == ERR_CHAR_TO_X){
        printf("ERR_CHAR_TO_X. Cannot convert %d to char.\n", given->type);
        exit(ERR_CHAR_TO_X);
    } else if (res == ERR_WRONG_TYPE){
        printf("ERR_WRONG_TYPE. Expecting type %d, but %d was given.\n", expected, given->type);
        exit(ERR_WRONG_TYPE);
    }
}

int implicit_conversion_check(int expected, Node* given) {
    if(expected == given->type) return 0;

    if(given->type == TYPE_STRING) {
        return ERR_STRING_TO_X;
    }

    if(given->type == TYPE_CHAR) {
        return ERR_CHAR_TO_X;
    }

    if(expected == TYPE_INT) {

        given->type = TYPE_INT;
        given->value->literal_type = LT_INT;

        if(given->type == TYPE_FLOAT)
            given->value->token_value.integer = (int) given->value->token_value.real;

    } else if(expected == TYPE_FLOAT) {

        given->type = TYPE_FLOAT;
        given->value->literal_type = LT_FLOAT;
        given->value->token_value.real = (float) given->value->token_value.integer;

    } else if(expected == TYPE_BOOL) {

        given->type = TYPE_BOOL;
        given->value->literal_type = LT_BOOL;

        if(given->type == TYPE_INT)
            given->value->token_value.integer = given->value->token_value.integer > 0 ? 1 : 0;
        else  // float
            given->value->token_value.integer = given->value->token_value.real > 0 ? 1 : 0;

    } else {
        return ERR_WRONG_TYPE;
    }

    return 0;
}

// Needs still to throw ERR_WRONG_PAR_INPUT and ERR_WRONG_PAR_OUTPUT
void check_type(int operation, Node* node) {

    if(operation == ARITH_OP) {
        if(node->type == TYPE_INT) return;
        if(node->type == TYPE_FLOAT) return;

        implicit_conversion(TYPE_INT, node);  // try implicit conversion expecting that node->type = TYPE_BOOL
    } else if(operation == BOOL_OP) {
        implicit_conversion(TYPE_BOOL, node);
    } else if(operation == INPUT_OP) {
        if(node->value->token_type != TK_ID){
            printf("ERR_WRONG_PAR_INPUT. Input parameter must be an identifier.\n");
            exit(ERR_WRONG_PAR_INPUT);
        }
    } else if(operation == OUTPUT_OP) {
        Node* args_list = node;

        while(args_list != NULL) {
            if(args_list->value->literal_type == LT_CHAR) {
                printf("ERR_WRONG_PAR_OUTPUT. Output parameter must be either a string literal or an expression.\n");
                exit(ERR_WRONG_PAR_OUTPUT);
            }

            if(args_list->n_children != 0)
                args_list = args_list->children[args_list->n_children - 1];
            else args_list = NULL;
        }
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
        // Try to cast it to an acceptable type
        if(implicit_conversion_check(function->type, expr_node)){
            printf("ERR_WRONG_PAR_RETURN. Expecting type %d, but %d was given.\n", function->type, expr_node->type);
            exit(ERR_WRONG_PAR_RETURN);
        }
    }   
}

void check_args(Stack *scope, Node *id, Node *args) {
    Symbol *function = search(scope, id->value->token_value.string);
    Param* params_list = function->args;
    Node* args_list = args;
    int seen_count = 0; // nb of arguments we've already checked

    while(params_list != NULL){
        // Check if there's still arguments left to check
        if(args_list == NULL) {
            if(function->args_number != seen_count) {
                printf("ERR_MISSING_ARGS. Expecting %d arguments, but %d were given.\n", function->args_number, seen_count);
                exit(ERR_MISSING_ARGS);
            } else {
                break;
            } 
        }

        // Check type
        if(params_list->symbol->type != args_list->type){
            // Try to cast it to an acceptable type
            if(implicit_conversion_check(params_list->symbol->type, args_list)){
                printf("ERR_WRONG_TYPE_ARGS. Expecting type %d, but %d was given.\n", params_list->symbol->type, args_list->type);
                exit(ERR_WRONG_TYPE_ARGS);
            }
        }

        // Go to next argument and to next parameter
        params_list = params_list->next;
        if(args_list->n_children != 0)
            args_list = args_list->children[args_list->n_children - 1];
        else args_list = NULL;
        seen_count++;
    }

    // There are no more parameters, but there are still arguments
    if( args_list != NULL ) {
        // Count how many arguments were passed in excess
        while(args_list != NULL) {
            if(args_list->n_children != 0)
                args_list = args_list->children[args_list->n_children - 1];
            else args_list = NULL;
            seen_count++;
        }
        
        printf("ERR_EXCESS_ARGS. Expecting %d arguments, but %d were given.\n", function->args_number, seen_count);
        exit(ERR_EXCESS_ARGS);
    }
}