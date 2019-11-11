#ifndef COMPILER_ILOC_H
#define COMPILER_ILOC_H

typedef struct operation {
    char* op;    // operator
    char* arg1;  // argument 1 as char (register or label)
    char* arg2;  // argument 2 as char (register or label)
    char* arg3;  // argument 3 as char (register or label)
    int argI;    // argument as constant
    int nargs;   // number of arguments
} Operation;

Operation* init_op_rrr(char* op, char* arg1, char* arg2, char* arg3);
Operation* init_op_rrc(char* op, char* arg1, char* arg2, int argI);
Operation* init_op_rr(char* op, char* arg1, char* arg2);
Operation* init_op_r(char* op, char* arg1);
Operation* init_nop();

void destroy_op(Operation* op);

#endif //COMPILER_ILOC_H
