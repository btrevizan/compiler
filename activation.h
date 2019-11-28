#ifndef ACTIVATION_GEN_H
#define ACTIVATION_GEN_H

#include <string.h>
#include "tree.h"
#include "stack.h"

#define RFP_START_VALUE 2048
#define RSP_START_VALUE 2048
#define RBSS_START_VALUE 1024

typedef struct activation_record
{
	int return_type;		// type of the returned value
	int arguments_offset;	// space taken by arguments
	int local_var_offset;	// space taken by local variables
} ActivationRecord;

ActivationRecord* init_ar(int return_type);

void setup_code_start(Node* tree, Stack* scope);
void setup_function(Stack* scope, Node* function, Node* body, Param* params);
void setup_call(Stack* scope, Node* function, Node* args);

#endif //ACTIVATION_GEN_H