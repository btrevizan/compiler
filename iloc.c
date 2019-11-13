#include "iloc.h"
#include <stdlib.h>
#include <stdio.h>

Operation* init_op_rrr(char* op, char* arg1, char* arg2, char* arg3) {
    Operation* operation = malloc(sizeof(Operation));

    operation->op = op;
    operation->arg1 = arg1;
    operation->arg2 = arg2;
    operation->arg3 = arg3;
    operation->type = OP_RRR;

    return operation;
}

Operation* init_op_rrc(char* op, char* arg1, char* arg2, int argI) {
    Operation* operation = malloc(sizeof(Operation));

    operation->op = op;
    operation->arg1 = arg1;
    operation->arg2 = arg2;
    operation->arg3 = NULL;
    operation->argI = argI;
    operation->type = OP_RRC;

    return operation;
}

Operation* init_op_crr(char* op, char* arg1, char* arg2, int argI) {
    Operation* operation = init_op_rrc(op, arg1, arg2, argI);
    operation->type = OP_CRR;

    return operation;
}

Operation* init_op_rr(char* op, char* arg1, char* arg2) {
    Operation* operation = malloc(sizeof(Operation));

    operation->op = op;
    operation->arg1 = arg1;
    operation->arg2 = arg2;
    operation->arg3 = NULL;
    operation->type = OP_RR;

    return operation;
}

Operation* init_op_r(char* op, char* arg1) {
    Operation* operation = malloc(sizeof(Operation));

    operation->op = op;
    operation->arg1 = arg1;
    operation->arg2 = NULL;
    operation->arg3 = NULL;
    operation->type = OP_R;

    return operation;
}

Operation* init_op_ldc(char* op, char* arg1, int argI) {
    Operation* operation = malloc(sizeof(Operation));

    operation->op = op;
    operation->arg1 = arg1;
    operation->arg2 = NULL;
    operation->arg3 = NULL;
    operation->argI = argI;
    operation->type = OP_LDC;

    return operation;
}

Operation* init_dummy() {
    Operation* operation = malloc(sizeof(Operation));

    operation->op = "dummy";
    operation->arg1 = NULL;
    operation->arg2 = NULL;
    operation->arg3 = NULL;
    operation->type = OP_DUMMY;

    return operation;
}

Operation* init_nop() {
    Operation* operation = malloc(sizeof(Operation));

    operation->op = "nop";
    operation->arg1 = NULL;
    operation->arg2 = NULL;
    operation->arg3 = NULL;
    operation->type = OP_NOP;

    return operation;
}

Operation* init_op_label(char* label) {
    Operation* operation = malloc(sizeof(Operation));

    operation->op = malloc(10);
    snprintf(operation->op, 10, "%s:", label);

    operation->arg1 = NULL;
    operation->arg2 = NULL;
    operation->arg3 = NULL;
    operation->type = OP_LABEL;

    return operation;
}

char* op2str(Operation* op) {
    if(op == NULL) return "";

    char* result;
    int n = 50;

    switch (op->type) {
        case OP_JMP:
            snprintf(result, n, "%s -> %s", op->op, op->arg1);
            break;

        case OP_CMP:
            snprintf(result, n, "%s %s, %s -> %s", op->op, op->arg1, op->arg2, op->arg3);
            break;

        case OP_CBR:
            snprintf(result, n, "%s %s -> %s, %s", op->op, op->arg1, op->arg2, op->arg3);
            break;

        case OP_LDC:
            snprintf(result, n, "%s %d => %s", op->op, op->argI, op->arg1);
            break;

        case OP_STC:
            snprintf(result, n, "%s %s => %s, %d", op->op, op->arg1, op->arg2, op->argI);
            break;

        case OP_STR2:
            snprintf(result, n, "%s %s => %s, %s", op->op, op->arg1, op->arg2, op->arg3);
            break;

        case OP_RRR:
            snprintf(result, n, "%s %s, %s => %s", op->op, op->arg1, op->arg2, op->arg3);
            break;

        case OP_RRC:
        case OP_CRR:
            snprintf(result, n, "%s %s, %d => %s", op->op, op->arg1, op->argI, op->arg2);
            break;

        case OP_RR:
            snprintf(result, n, "%s %s => %s", op->op, op->arg1, op->arg2);
            break;

        case OP_DUMMY:
        case OP_NOP:
            snprintf(result, n, "%s", op->op);
            break;

        default:
            snprintf(result, n, "OP NOT DEFINED");
    }

    return result;
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

