#include "code.h"
#include "iloc.h"
#include <stdlib.h>

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

Code* init_code() {
    Code* code = malloc(sizeof(Code));

    code->operation = NULL;
    code->next = NULL;
    code->prev = NULL;

    return code;
}

Code* add_dummy(Code* code) {
    Code* new_code = init_code();

    new_code->operation = init_dummy();
    new_code->prev = code;
    new_code->next = NULL;

    if(code != NULL)
    	code->next = new_code;

    return new_code;
}

Code* add_op(Code* code, Operation* op) {
    Code* new_code = init_code();

    new_code->operation = op;
    new_code->prev = code;
    new_code->next = NULL;

    if(code != NULL)
    	code->next = new_code;

    return new_code;
}

int is_array(Lexeme* lex) {
	return lex->token_type == TK_VC;
}

// TODO: need
char* calculate_address(Symbol* s,  Node* id){
	Node *current = id->children[1];
	int address = current->value->token_value.integer;
	Dim *dimension = s->dimension->next;

	current = current->children[0];
	while(current != NULL) {
		address *= dimension->size;
		address += current->value->token_value.integer;

		current = current->children[0];
		dimension = dimension->next;
	}

	address *= get_type_size(s->type);
	char *addr_reg = get_register();

	return addr_reg;
}

Code* make_code_load(Stack* scope, Node* id) {
    Symbol* symbol = search(scope, id->value->token_value.string);

    id->temp = get_register();
    Operation *op = init_op_rrc("loadAI", symbol->base, id->temp, symbol->address);

    return add_op(instr_list, op);
}

Operation* store_imm_or_reg(Node* expr, Symbol *symbol){
	Operation *op;

    if(expr->temp != NULL){
    	// expression isn't a literal
	    op = init_op_rrc("storeAI", expr->temp, symbol->base, symbol->address);
	    op->type = OP_STC;
	} else {
		// expression is a literal
		char *lit_register = get_register();
		op = init_op_ldc("loadI", lit_register, expr->value->token_value.integer);
		Code *load_lit = add_op(instr_list, op);
		instr_list = load_lit;

		op = init_op_rrc("storeAI", lit_register, symbol->base, symbol->address);
	    op->type = OP_STC;
	}

	return op;
}

Code* make_code_store_assign(Stack* scope, Lexeme* id, Node* expr) {
	Operation *op;
    Symbol* symbol = search(scope, id->token_value.string);
    op = store_imm_or_reg(expr, symbol);

    return add_op(instr_list, op);
}

Code* make_code_store(Stack* scope, Node* id, Node* expr) {
	Operation *op;
    Symbol* symbol = search(scope, id->value->token_value.string);
    char* final_address;

    if(is_array(id->value)){
	    op = store_imm_or_reg(expr, symbol);
	} else {
		final_address = calculate_address(symbol, id);
	}

    return add_op(instr_list, op);
}

Code* remove_code(Code* code) {
    Code* c;

    if(code->prev == NULL) {
        c = code->next;
        c->prev = NULL;
    } else if(code->next == NULL) {
        c = code->prev;
        c->next = NULL;
    } else {
        c = code->next;

        code->prev->next = code->next;
        code->next->prev = code->prev;

        while(c->next != NULL) c = c->next;
    }

    destroy_code(code);
    return c;
}

void destroy_code(Code* code) {
    destroy_op(code->operation);
    free(code);
}

void destroy_code_list(Code* code) {
    if(code == NULL) return;

    while(code->next != NULL) code = code->next;

    destroy_code_list(code->prev);
    destroy_code(code);

    code = NULL;
}