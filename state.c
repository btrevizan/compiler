#include "state.h"
#include "code.h"
#include "iloc.h"
#include <stdlib.h>
#include <string.h>

extern State state;

void add_register(char* name) {
    state.size += 1;

    if(state.size == 1) state.reg = malloc(sizeof(Register*));
    else state.reg = realloc(state.reg, sizeof(Register*) * state.size);

    Register* reg = malloc(sizeof(Register));
    reg->name = strdup(name);
    reg->offset = state.current_offset;

    state.current_offset += 4;

    int index = state.size - 1;
    state.reg[index] = reg;
}

void save_state(CodeList* codelist, int state_machine_offset) {
    Operation* save_reg;

    for(int i = 0; i < state.size; i++) {
        save_reg = init_op_stc("storeAI", state.reg[i]->name, "rfp", state.reg[i]->offset + state_machine_offset);
        add_op(codelist, save_reg);
    }
}

void resume_state(CodeList* codelist, int state_machine_offset) {
    Operation* load_reg;

    for(int i = 0; i < state.size; i++) {
        load_reg = init_op_rrc("loadAI", "rfp", state.reg[i]->name, state.reg[i]->offset + state_machine_offset);
        add_op(codelist, load_reg);
    }
}

void clear_state() {
    for(int i = 0; i < state.size; i++)
        free(state.reg[i]);

    if(state.size > 0)
        free(state.reg);

    state.size = 0;
    state.current_offset = 0;
}