#include "iloc.h"
#include <stdlib.h>

Operation* init_op_rrr(char* op, char* arg1, char* arg2, char* arg3) {
    Operation* operation = malloc(sizeof(Operation));

    operation->op = op;
    operation->arg1 = arg1;
    operation->arg2 = arg2;
    operation->arg3 = arg3;
    operation->nargs = 3;

    return operation;
}

Operation* init_op_rrc(char* op, char* arg1, char* arg2, int argI) {
    Operation* operation = malloc(sizeof(Operation));

    operation->op = op;
    operation->arg1 = arg1;
    operation->arg2 = arg2;
    operation->arg3 = NULL;
    operation->argI = argI;
    operation->nargs = 3;

    return operation;
}

Operation* init_op_rr(char* op, char* arg1, char* arg2) {
    Operation* operation = malloc(sizeof(Operation));

    operation->op = op;
    operation->arg1 = arg1;
    operation->arg2 = arg2;
    operation->arg3 = NULL;
    operation->nargs = 2;

    return operation;
}

Operation* init_op_r(char* op, char* arg1) {
    Operation* operation = malloc(sizeof(Operation));

    operation->op = op;
    operation->arg1 = arg1;
    operation->arg2 = NULL;
    operation->arg3 = NULL;
    operation->nargs = 1;

    return operation;
}

Operation* init_nop() {
    Operation* operation = malloc(sizeof(Operation));

    operation->op = "nop";
    operation->arg1 = NULL;
    operation->arg2 = NULL;
    operation->arg3 = NULL;
    operation->nargs = 0;

    return operation;
}

void destroy_op(Operation* op) {
    if(op == NULL) return;

    if(op->arg1) free(op->arg1);
    if(op->arg2) free(op->arg2);
    if(op->arg3) free(op->arg2);

    free(op->op);
    free(op);

    op = NULL;
}

