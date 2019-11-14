#include "lexical.h"
#include "code.h"
#include "iloc.h"
#include <stdlib.h>
#include <string.h>

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

void add_dummy(Code* code) {
    add_op(code, init_dummy());
}

void add_op(Code* code, Operation* op) {
    Code* new_code = init_code();

    new_code->operation = op;
    new_code->prev = code;
    new_code->next = NULL;

    code->next = new_code;
    code = new_code;
}

void concat_code(Code* c1, Code* c2) {
    Code* aux2 = c2;
    while(aux2->prev != NULL) aux2 = aux2->prev;

    c1->next = aux2;
    aux2->prev = c1;

    while(c1->next != NULL) c1 = c1->next;
}

int is_array(Lexeme* lex) {
	return lex->token_type == TK_VC;
}

// Loads immeadiate value into register
char* load_imm(Code* code, int value) {
	char *temp = get_register();

	add_op(code, init_op_ldc("loadI", temp, value));

	return temp;
}

// Load from memory
char *load_mem(Code* code, char* base_reg, int relative_addr) {
	char* temp = get_register();

    Operation *op = init_op_rrc("loadAI", base_reg, temp, relative_addr);
    add_op(code, op);

    return temp;
}

// Load array from memory
char *load_mem_array(Code* code, char* base_reg, char* index_reg) {
	char* temp = get_register();

	Operation *op = init_op_rrr("loadA0", base_reg, index_reg, temp);
    add_op(code, op);

    return temp;
}

char* calculate_address(Code* code, Symbol* s,  Node* id) {
	Node *current = id->children[1];
	char* address = load_index(code, s, id);
	Dim *dimension = s->dimension->next;
	char *address_x_width, *index;

	current = current->children[0];
	while(current != NULL) {
		address_x_width = get_register();
		add_op(code, init_op_rrc("multI", address, address_x_width, dimension->size));

		index = load_index(code, s, current);
		address = get_register();
		add_op(code, init_op_rrr("add", index, address_x_width, address));

		current = current->children[0];
		dimension = dimension->next;
	}

	char *addr_reg = get_register();
	add_op(code, init_op_rrc("multI", address, addr_reg, get_type_size(s->type)));

	return addr_reg;
}

char* load_index(Code* code, Symbol* s, Node* id) {
	char* temp;

	switch(id->value->token_type){
		case TK_ID:
			temp = load_mem(code, s->base, s->address);
			break;
		case TK_VC:
			temp = load_mem_array(code, s->base, calculate_address(code, s, id));
			break;
		case TK_LT:
			temp = load_imm(code, id->value->token_value.integer);
			break;
		default:
			fprintf(stderr, "The token type %d is invalid.\n", id->value->token_type);
	}

	return temp;
}

void load(Code* code, Stack* scope, Node* id) {
    Symbol* symbol = search(scope, id->value->token_value.string);

    if(is_array(id->value)){
    	id->temp = load_mem_array(code, symbol->base, calculate_address(code, symbol, id));
	} else {
		id->temp = load_mem(code, symbol->base, symbol->address);
	}
}

Operation* store_imm_or_reg(Code* code, Node* expr, Symbol *symbol){
	Operation *op;

    if(expr->temp != NULL){
    	// expression isn't a literal
	    op = init_op_rrc("storeAI", expr->temp, symbol->base, symbol->address);
	    op->type = OP_STC;
	} else {
		// expression is a literal
		char *lit_register = load_imm(code, expr->value->token_value.integer);
		op = init_op_rrc("storeAI", lit_register, symbol->base, symbol->address);
	    op->type = OP_STC;
	}

	return op;
}

void store_assign(Code* code, Stack* scope, Lexeme* id, Node* expr) {
	Operation *op;
    Symbol* symbol = search(scope, id->token_value.string);
    op = store_imm_or_reg(code, expr, symbol);

    add_op(code, op);
}

void store(Code* code, Stack* scope, Node* id, Node* expr) {
	Operation *op;
    Symbol* symbol = search(scope, id->value->token_value.string);
    char* final_address;

    if(is_array(id->value)){
    	final_address = calculate_address(code, symbol, id);
	} else {
		op = store_imm_or_reg(code, expr, symbol);
	}

    add_op(code, op);
}

void nop(Code* code) {
    Operation* op = init_nop();
    add_op(code, op);
}

void binop(Code* code, char* op_name, Node* expr1, Node* expr2, Node* op) {
    Operation* operation;
    op->temp = get_register();

    if(expr2->temp == NULL) {

        // expr2 is a literal
        char *opi = malloc(10);
        snprintf(opi, 10, "%sI", op_name);

        operation = init_op_rrc(opi, expr1->temp, op->temp, expr2->value->token_value.integer);

    } else if (expr1->temp == NULL) {

        // expr1 is a literal
        if (strcmp(op_name, "sub") == 0 || strcmp(op_name, "div") == 0) {

            char *ropi = malloc(10);
            snprintf(ropi, 10, "r%sI", op_name);

            operation = init_op_crr(ropi, expr2->temp, op->temp, expr1->value->token_value.integer);

        } else {

            char *opi = malloc(10);
            snprintf(opi, 10, "%sI", op_name);

            operation = init_op_rrc(opi, expr2->temp, op->temp, expr1->value->token_value.integer);

        }
    } else {
        operation = init_op_rrr(op_name, expr1->temp, expr2->temp, op->temp);
    }

    add_op(code, operation);
}

void conversion(Code* code, char* op, char* r1, char* r2) {
    Operation* operation = init_op_rr(op, r1, r2);
    add_op(code, operation);
}

void cmp(Code* code, char* op_name, Node* expr1, Node* expr2, Node* op) {
    op->temp = get_register();

    Operation* operation = init_op_rrr(op_name, expr1->temp, expr2->temp, op->temp);
    operation->type = OP_CMP;

    add_op(code, operation);
}

void jump(Code* code, char* op, char* r1) {
    Operation* operation = init_op_r(op, r1);
    operation->type = OP_JMP;

    add_op(code, operation);
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