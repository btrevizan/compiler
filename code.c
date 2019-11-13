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

void add_dummy() {
    add_op(init_dummy());
}

void add_op(Operation* op) {
    Code* new_code = init_code();

    new_code->operation = op;
    new_code->prev = instr_list;
    new_code->next = NULL;

    instr_list->next = new_code;
    instr_list = new_code;
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

void make_code_load(Stack* scope, Node* id) {
    Symbol* symbol = search(scope, id->value->token_value.string);

    id->temp = get_register();
    Operation *op = init_op_rrc("loadAI", symbol->base, id->temp, symbol->address);

    add_op(op);
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
		add_op(op);

		op = init_op_rrc("storeAI", lit_register, symbol->base, symbol->address);
	    op->type = OP_STC;
	}

	return op;
}

void make_code_store_assign(Stack* scope, Lexeme* id, Node* expr) {
	Operation *op;
    Symbol* symbol = search(scope, id->token_value.string);
    op = store_imm_or_reg(expr, symbol);

    add_op(op);
}

void make_code_store(Stack* scope, Node* id, Node* expr) {
	Operation *op;
    Symbol* symbol = search(scope, id->value->token_value.string);
    char* final_address;

    if(is_array(id->value)){
	    op = store_imm_or_reg(expr, symbol);
	} else {
		final_address = calculate_address(symbol, id);
	}

    add_op(op);
}

void make_code_nop() {
    Operation* op = init_nop();
    add_op(op);
}

void make_code_binop(char* op, Node* a, Node* b) {
    char* r = get_register();
    Operation* operation;

    if(b->temp != NULL) {  // b is not a literal
        operation = init_op_rrr(op, a->temp, b->temp, r);
        add_op(operation);
    } else {
        char *opi = malloc(10);
        snprintf(opi, 10, "%sI", op);

        operation = init_op_rrc(opi, a->temp, r, b->value->token_value.integer);
        add_op(operation);

        if (strcmp(op, "sub") == 0 || strcmp(op, "div") == 0) {
            char *ropi = malloc(10);
            snprintf(ropi, 10, "r%s", opi);

            Operation *r_operation = init_op_crr(ropi, a->temp, get_register(), b->value->token_value.integer);
            add_op(r_operation);
        }
    }
}

void make_code_conversion(char* op, char* r1, char* r2) {
    Operation* operation = init_op_rr(op, r1, r2);
    add_op(operation);
}

void make_code_flow_control(char* op, char* r1, char* r2, char* r3, int type) {
    Operation* operation = init_op_rrr(op, r1, r2, r3);
    operation->type = type;

    add_op(operation);
}

void make_code_jump(char* op, char* r1) {
    Operation* operation = init_op_r(op, r1);
    operation->type = OP_JMP;

    add_op(operation);
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