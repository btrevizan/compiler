#ifndef COMPILER_CHECKS_H
#define COMPILER_CHECKS_H

#include "tree.h"
#include "stack.h"

#define ARITH_OP    555
#define BOOL_OP     556

// Throws ERR_UNDECLARED
// ERR_DECLARED is thrown by the add_entry
void check_declaration(Stack* stack, Node* id);

// Throws ERR_VARIABLE, ERR_VECTOR and ERR_FUNCTION
void check_usage(Stack* stack, Node* id);

// Throws ERR_MISSING_ARGS, ERR_EXCESS_ARGS and ERR_WRONG_TYPE_ARGS
void check_args(Stack *scope, Node *id, Node *args);

// Throws ERR_WRONG_PAR_RETURN, ERR_WRONG_TYPE, ERR_STRING_TO_X,
// ERR_CHAR_TO_X, ERR_WRONG_PAR_INPUT and ERR_WRONG_PAR_OUTPUT
void implicit_conversion(int expected, Node* given);
void check_type(int operation, Node* node);
void check_return_type(Stack* scope, Node* node);

#endif //COMPILER_CHECKS_H
