#ifndef COMPILER_CHECKS_H
#define COMPILER_CHECKS_H

#include "tree.h"
#include "stack.h"

// Throws ERR_UNDECLARED
// ERR_DECLARED is thrown by the add_entry
void check_declaration(Stack* stack, Node* id);

// Throws ERR_VARIABLE, ERR_VECTOR and ERR_FUNCTION
void check_usage(Stack* stack, Node* id);

// Throws ERR_MISSING_ARGS, ERR_EXCESS_ARGS and ERR_WRONG_TYPE_ARGS
void check_args(Node* id);

// Throws ERR_WRONG_PAR_RETURN, ERR_WRONG_TYPE, ERR_STRING_TO_X,
// ERR_CHAR_TO_X, ERR_WRONG_PAR_INPUT and ERR_WRONG_PAR_OUTPUT
void check_type(Node* id);

#endif //COMPILER_CHECKS_H
