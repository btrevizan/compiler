#ifndef COMPILER_ILOC_H
#define COMPILER_ILOC_H

#define OP_DUMMY  790
#define OP_NOP    791
#define OP_RRR    792
#define OP_RRC    793
#define OP_RR     794
#define OP_R      795

#define OP_CMP    796
#define OP_JMP    797
#define OP_CBR    798
#define OP_LDC    799
#define OP_LDR    800
#define OP_STC    801
#define OP_STR1   802
#define OP_STR2   803

typedef struct operation {
    char* op;    // operator
    char* arg1;  // argument 1 as char (register or label)
    char* arg2;  // argument 2 as char (register or label)
    char* arg3;  // argument 3 as char (register or label)
    int argI;    // argument as constant
    int type;    // number of arguments
} Operation;

Operation* init_op_rrr(char* op, char* arg1, char* arg2, char* arg3);
Operation* init_op_rrc(char* op, char* arg1, char* arg2, int argI);
Operation* init_op_rr(char* op, char* arg1, char* arg2);
Operation* init_op_ldc(char* op, char* arg1, int argI);
Operation* init_dummy();
Operation* init_nop();

char* op2str(Operation* op);
void destroy_op(Operation* op);

#endif //COMPILER_ILOC_H
