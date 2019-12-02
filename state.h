#ifndef COMPILER_STATE_H
#define COMPILER_STATE_H

#include "code.h"

typedef struct register_t {
    char* name;
    int offset;
} Register;

typedef struct state_t {
    int size;
    int current_offset;
    Register** reg;
} State;

static State state = {0, 0, NULL};

void add_register(char* name);

void save_state(CodeList* codelist, int state_machine_offset);
void resume_state(CodeList* codelist, int state_machine_offset);
void clear_state();

#endif //COMPILER_STATE_H
