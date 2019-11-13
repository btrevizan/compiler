#ifndef COMPILER_BACKPATCHING_H
#define COMPILER_BACKPATCHING_H

#include "tree.h"
#include "code.h"

Code* makelist(char* i);
Code* merge(Code* p1, Code* p2);
void backpatch(Code* p, char* i);

#endif //COMPILER_BACKPATCHING_H
