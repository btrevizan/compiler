#ifndef ACTIVATION_GEN_H
#define ACTIVATION_GEN_H

#include <string.h>
#include "tree.h"
#include "stack.h"
#include "table.h"

#define RFP_START_VALUE 2048
#define RSP_START_VALUE 2048
#define RBSS_START_VALUE 1024

typedef struct activation_record
{
	int return_type;		// type of the returned value
	int size;
    int local_var_offset;	// space taken by local variables
    int return_addr_offset;
    int pc_addr_offset;
    int machine_state_offset;
    int static_link_offset;
    int dynamic_link_offset;
    int return_value_offset;
    int arguments_offset;	// space taken by arguments
} ActivationRecord;

ActivationRecord* init_ar(int return_type);

void setup_code_start(Node* tree, Stack* scope);
void setup_function(Stack* scope, Node* function, Node* body, Param* params);
void setup_call(Stack* scope, Node* function, Node* args);

int get_local_var_offset(Param* params, RegList* r);
int get_machine_state_offset(Param* params);
int get_pc_addr_offset(Param* params);
int get_return_addr_offset(Param* params);
int get_static_link_offset(Param* params);
int get_dynamic_link_offset(Param* params);
int get_return_value_offset(Param* params);
int get_arguments_offset();

int local_var_size(Table* body);
int get_ra_size(Table* body, Param* params, RegList* r);
void set_ra_size(Stack* scope, char* function, RegList* r);

#endif //ACTIVATION_GEN_H