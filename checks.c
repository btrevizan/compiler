#include <stdlib.h>
#include <stdio.h>
#include "checks.h"
#include "stack.h"
#include "error.h"
#include "tree.h"
#include "symbol_table.h"

void check_declaration(Stack* stack, Node* id) {
    if(stack == NULL) return;

    Symbol* symbol = NULL;
    Item* item = stack->top;

    while(item != NULL) {
        symbol = get_entry(item->value, id->value->token_value.string);
        if(symbol != NULL) return;  // found declaration

        item = item->next;
    }

    // From here, symbol is null. Thus, id was not found
    printf("ERR_UNDECLARED: '%s' was not declared.\n", id->value->token_value.string);
    exit(ERR_UNDECLARED);
}