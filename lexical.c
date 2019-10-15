#include "lexical.h"
#include <stdlib.h>
#include <stdio.h>

void delete_lexeme(Lexeme* lexeme) {
    if(lexeme != NULL) {
        if(lexeme->token_type != TK_SC)
            if(lexeme->literal_type == LT_NAL || lexeme->literal_type == LT_STRING) {
                free(lexeme->token_value.string);
            }

        free(lexeme);
    }
}