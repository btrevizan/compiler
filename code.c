#include "code.h"

int get_global_offset(int type, int array_len){
	static int offset_rbss = 0;

	if(array_len <= 0) 
		array_len = 1;

	offset_rbss += get_type_size(type)*array_len;
	
	return offset_rbss;
}

void set_local_offset(int offset){
	offset_rfp = offset;
}

int get_current_local_offset(){
	return offset_rfp;
}

int get_local_offset(int type, int array_len){

	if(array_len <= 0) 
		array_len = 1;

	offset_rfp += get_type_size(type)*array_len;
	
	return offset_rfp;
}

char* get_register(){
	static int register_label = 0;	// register allocation, register names are 'r'+register_label
	
	char *s = malloc(sizeof(MAX_LENGTH_LABEL));
	snprintf(s, MAX_LENGTH_LABEL, "r%d", register_label);
	register_label++;

	return s;
}

char* get_label(){
	static int jump_label = 0;	// label for jumps, label names are 'L'+jump_label

	char *s = malloc(sizeof(MAX_LENGTH_LABEL));
	snprintf(s, MAX_LENGTH_LABEL, "L%d", jump_label);
	jump_label++;

	return s;
}