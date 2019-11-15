#include "lexical.h"
#include "code.h"
#include "iloc.h"
#include "backpatching.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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

CodeList* init_codelist() {
    CodeList* codelist = malloc(sizeof(CodeList));

    codelist->begin = NULL;
    codelist->end = NULL;

    return codelist;
}

void add_dummy(CodeList* codelist) {
    add_op(codelist, init_dummy());
}

void add_op(CodeList* codelist, Operation* op) {
    Code* new_code = init_code();

    new_code->operation = op;
    new_code->prev = codelist->end;
    new_code->next = NULL;

    if(codelist->begin == NULL) {
        codelist->begin = new_code;
    }

    codelist->end->next = new_code;
    codelist->end = new_code;
}

CodeList* concat_code(CodeList* c1, CodeList* c2) {
    CodeList* codelist = malloc(sizeof(codelist));

    c1->end->next = c2->begin;
    c2->begin->prev = c1->end;

    codelist->begin = c1->begin;
    codelist->end = c2->end;

    return codelist;
}

int is_array(Lexeme* lex) {
	return lex->token_type == TK_VC;
}

// Loads immeadiate value into register
char* load_imm(CodeList* codelist, int value) {
	char *temp = get_register();

	add_op(codelist, init_op_ldc("loadI", temp, value));

	return temp;
}

// Load from memory
char *load_mem(CodeList* codelist, char* base_reg, int relative_addr) {
	char* temp = get_register();

    Operation *op = init_op_rrc("loadAI", base_reg, temp, relative_addr);
    add_op(codelist, op);

    return temp;
}

// Load array from memory
char *load_mem_array(CodeList* codelist, char* base_reg, char* index_reg) {
	char* temp = get_register();

	Operation *op = init_op_rrr("loadA0", base_reg, index_reg, temp);
    add_op(codelist, op);

    return temp;
}

char* calculate_address(CodeList* codelist, Symbol* s,  Node* id) {
	Node *current = id->children[1];
	char* address = load_index(codelist, s, id);
	Dim *dimension = s->dimension->next;
	char *address_x_width, *index;

	current = current->children[0];
	while(current != NULL) {
		address_x_width = get_register();
		add_op(codelist, init_op_rrc("multI", address, address_x_width, dimension->size));

		index = load_index(codelist, s, current);
		address = get_register();
		add_op(codelist, init_op_rrr("add", index, address_x_width, address));

		current = current->children[0];
		dimension = dimension->next;
	}

	char *addr_reg = get_register();
	add_op(codelist, init_op_rrc("multI", address, addr_reg, get_type_size(s->type)));

	return addr_reg;
}

char* load_index(CodeList* codelist, Symbol* s, Node* id) {
	char* temp;

	switch(id->value->token_type){
		case TK_ID:
			temp = load_mem(codelist, s->base, s->address);
			break;
		case TK_VC:
			temp = load_mem_array(codelist, s->base, calculate_address(codelist, s, id));
			break;
		case TK_LT:
			temp = load_imm(codelist, id->value->token_value.integer);
			break;
		default:
			fprintf(stderr, "The token type %d is invalid.\n", id->value->token_type);
	}

	return temp;
}

void load(CodeList* codelist, Stack* scope, Node* id) {
    Symbol* symbol = search(scope, id->value->token_value.string);

    if(is_array(id->value)){
    	id->temp = load_mem_array(codelist, symbol->base, calculate_address(codelist, symbol, id));
	} else {
		id->temp = load_mem(codelist, symbol->base, symbol->address);
	}
}

Operation* store_imm_or_reg(CodeList* codelist, Node* expr, Symbol *symbol){
	Operation *op;

    if(expr->temp != NULL){
    	// expression isn't a literal
	    op = init_op_rrc("storeAI", expr->temp, symbol->base, symbol->address);
	    op->type = OP_STC;
	} else {
		// expression is a literal
		char *lit_register = load_imm(codelist, expr->value->token_value.integer);
		op = init_op_rrc("storeAI", lit_register, symbol->base, symbol->address);
	    op->type = OP_STC;
	}

	return op;
}

void store_assign(CodeList* codelist, Stack* scope, Lexeme* id, Node* expr) {
	Operation *op;
    Symbol* symbol = search(scope, id->token_value.string);
    op = store_imm_or_reg(codelist, expr, symbol);

    add_op(codelist, op);
}

void store(CodeList* codelist, Stack* scope, Node* id, Node* expr) {
	Operation *op;
    Symbol* symbol = search(scope, id->value->token_value.string);
    char* final_address;

    if(is_array(id->value)){
    	final_address = calculate_address(codelist, symbol, id);
	} else {
		op = store_imm_or_reg(codelist, expr, symbol);
	}

    add_op(codelist, op);
}

void nop(CodeList* codelist) {
    Operation* op = init_nop();
    add_op(codelist, op);
}

void numeric(char* op_name, Node* expr1, Node* expr2, Node* op) {
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

    op->codelist = concat_code(expr1->codelist, expr2->codelist);
    add_op(op->codelist, operation);
}

Operation* cbr(char* r1, char* l2, char* l3) {
    Operation* op = init_op_rrr("cbr", r1, l2, l3);
    op->type = OP_CBR;

    return op;
}

void cmp(char* op_name, Node* expr1, Node* expr2, Node* op) {
    op->temp = get_register();

    Operation* operation = init_op_rrr(op_name, expr1->temp, expr2->temp, op->temp);
    operation->type = OP_CMP;

    op->codelist = concat_code(expr1->codelist, expr2->codelist);
    add_op(op->codelist, operation);

    // For backpatching later
    Operation* cbr_op = cbr(expr1->temp, malloc(10), malloc(10));
    add_op(op->codelist, cbr_op);

    op->truelist = makelist(cbr_op->arg2);
    op->falselist = makelist(cbr_op->arg3);
}

Operation* logical(char* op_name, Node* expr1, Node* expr2, Node* op) {
    Operation *operation;
    op->temp = get_register();

    if (expr2->temp == NULL) {

        // expr2 is a literal
        char *opi = malloc(10);
        snprintf(opi, 10, "%sI", op_name);

        operation = init_op_rrc(opi, expr1->temp, op->temp, expr2->value->token_value.integer);

    } else {
        operation = init_op_rrr(op_name, expr1->temp, expr2->temp, op->temp);
    }

    return operation;
}

void and(Node* expr1, Node* expr2, Node* op) {
    // For backpatching
    char* label = get_label();
    backpatch(expr1->truelist, label);

    op->truelist = expr2->truelist;
    op->falselist = merge(expr1->falselist, expr2->falselist);

    // Operator codelist
    Operation* op_label = init_op_label(label);

    CodeList* codelist;
    (*codelist) = *(expr1->codelist);
    add_op(codelist, op_label);

    op->codelist = concat_code(codelist, expr2->codelist);
}

void or(Node* expr1, Node* expr2, Node* op) {
    // For backpatching
    char* label = get_label();
    backpatch(expr1->falselist, label);

    op->falselist = expr2->falselist;
    op->truelist = merge(expr1->truelist, expr2->truelist);

    // Operator codelist
    Operation* op_label = init_op_label(label);

    CodeList* codelist;
    (*codelist) = *(expr1->codelist);
    add_op(codelist, op_label);

    op->codelist = concat_code(codelist, expr2->codelist);
}

Operation* jump(char* op, char* r1) {
    Operation* operation = init_op_r(op, r1);
    operation->type = OP_JMP;

    return operation;
}

void if_then_else(Node* if_then, Node* else_block) {
    Node* expr = if_then->children[0];
    Node* then_block = if_then->children[1];

    char* label_true = get_label();
    backpatch(expr->truelist, label_true);
    Operation* op_label_true = init_op_label(label_true);

    CodeList* codelist;
    (*codelist) = *(expr->codelist);

    add_op(codelist, op_label_true);
    codelist = concat_code(codelist, then_block->codelist);

    char *label_end = get_label();
    Operation *op_label_end = init_op_label(label_end);

    if(else_block != NULL) {
        char *label_false = get_label();
        backpatch(expr->falselist, label_false);
        Operation *op_label_false = init_op_label(label_false);

        add_op(codelist, jump("jumpI", label_end));

        add_op(codelist, op_label_false);
        codelist = concat_code(codelist, else_block->codelist);
    } else {
        backpatch(expr->falselist, label_end);
    }

    // Not necessary, but kept to maintain the pattern
    add_op(codelist, op_label_end);
    add_op(codelist, init_nop());

    if_then->codelist = codelist;
}

void while_do(Node* expr, Node* block, Node* parent) {
    char* label_init = get_label();
    Operation* op_label_init = init_op_label(label_init);

    CodeList* codelist = init_codelist();
    add_op(codelist, op_label_init);
    codelist = concat_code(codelist, expr->codelist);

    char* label_true = get_label();
    Operation* op_label_true = init_op_label(label_true);
    backpatch(expr->truelist, label_true);

    add_op(codelist, op_label_true);
    codelist = concat_code(codelist, block->codelist);
    add_op(codelist, jump("jumpI", label_init));

    char* label_false = get_label();
    Operation* op_label_false = init_op_label(label_false);
    backpatch(expr->falselist, label_false);

    add_op(codelist, op_label_false);
    add_op(codelist, init_nop());

    parent->codelist = codelist;
}

void destroy_code(Code* code) {
    destroy_op(code->operation);
    free(code);
}

void destroy_code_list(CodeList* codelist) {
    if(codelist == NULL) return;

    Code* aux = codelist->begin;
    while(aux != NULL) {
        aux = codelist->begin->next;
        destroy_code(codelist->begin);
    }

    free(codelist);
}

void print_code(Node* node) {
    if(node == NULL) return;
    if(node->codelist == NULL) return;

    Code* code = node->codelist->begin;
    while(code != NULL) {
        printf("%s\n", op2str(code->operation));
        code = code->next;
    }
}