#ifndef COMPILER_BACKPATCHING_H
#define COMPILER_BACKPATCHING_H

typedef struct patch {
    char* arg_ptr;
    struct patch* next;
} Patch;

Patch* makelist(char* label);
Patch* merge(Patch* p1, Patch* p2);
void backpatch(Patch* p, char* label);

#endif //COMPILER_BACKPATCHING_H
