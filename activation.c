#include "activation.h"
#include "errors.h"
#include "code.h"

ActivationRecord* init_ar(int return_type){
    ActivationRecord *ar = malloc(sizeof(ActivationRecord));
    ar->return_type = return_type;
    return ar;
}

void write_arguments(ActivationRecord* ar, CodeList **codelist_p, Node *args) {
    Operation *op;
    CodeList *codelist = init_codelist();
    int offset = 0; // arguments are the first elements

    // Pass all the arguments to new function
    Node* current = args;
    while(current != NULL) {
        codelist = concat_code(codelist, current->codelist);

        // Put argument into new activation record
        op = init_op_stc("storeAI", current->temp, "rsp", offset);
        add_op(codelist, op);
        offset += get_type_size(current->type);

        if(current->n_children != 0){
            if ((current->value->token_type == TK_SC  || current->value->token_type == TK_OC ) && current->n_children < 3){
                current = NULL;
                continue;
            }
            current = current->children[current->n_children - 1];
        }
        else current = NULL;
    }

    *codelist_p = concat_code(*codelist_p, codelist);

    // Save the space occupied by the arguments
    ar->arguments_offset = offset;
}

void write_return_value(ActivationRecord* ar, CodeList **codelist_p, char* return_temp) {
    Operation *op;
    CodeList *codelist = *codelist_p;
    int offset = ar->arguments_offset; // arguments are the first elements

     // Save return value
    op = init_op_stc("storeAI", return_temp, "rsp", offset);
    add_op(codelist, op);
}

void write_dynamic_link(ActivationRecord* ar, CodeList **codelist_p) {
    Operation *op;
    CodeList *codelist = *codelist_p;
    int offset = ar->dynamic_link_offset;

    // Save dynamic link
    op = init_op_stc("storeAI", "rfp", "rsp", offset);
    add_op(codelist, op);
}

void write_static_link(ActivationRecord* ar, CodeList **codelist_p){
    Operation *op;
    CodeList *codelist = *codelist_p;
    int offset = ar->static_link_offset;

    // Static link is always zero, because we can't have nested functions
    int static_link = 0;

    // Save static link
    char* static_temp = load_imm(codelist, static_link);
    op = init_op_stc("storeAI", static_temp, "rsp", offset);
    add_op(codelist, op);
}

void write_return_addr(ActivationRecord* ar, CodeList **codelist_p) {
    Operation *op;
    CodeList *codelist = *codelist_p;
    int offset = ar->return_addr_offset;

    // Save return address
    char* pc_return = get_register();
    // 3 for the add and store of the return address and the jump
    add_op(codelist, init_op_rrc("addI", "rpc", pc_return, 3));
    op = init_op_stc("storeAI", pc_return, "rsp", offset);
    add_op(codelist, op);
}

char* load_return_value(ActivationRecord* ar, CodeList **codelist_p) {
    char* temp = get_register();
    CodeList* codelist = *codelist_p;

    Operation *op = init_op_rrc("loadAI", "rsp", temp, ar->arguments_offset);
    add_op(codelist, op);  

    return temp;
}

char* load_dynamic_link(ActivationRecord* ar, CodeList **codelist_p) {
    char* temp = get_register();
    int offset =  ar->dynamic_link_offset;
    CodeList* codelist = *codelist_p;

    Operation *op = init_op_rrc("loadAI", "rsp", temp, offset);
    add_op(codelist, op);  

    return temp;
}

char* load_static_link(ActivationRecord* ar, CodeList **codelist_p) {
    char* temp = get_register();
    int offset =  ar->static_link_offset;
    CodeList* codelist = *codelist_p;

    Operation *op = init_op_rrc("loadAI", "rsp", temp, offset);
    add_op(codelist, op);  

    return temp;
}

char* load_return_addr(ActivationRecord* ar, CodeList **codelist_p) {
    char* temp = get_register();
    int offset =  ar->return_addr_offset;
    CodeList* codelist = *codelist_p;

    Operation *op = init_op_rrc("loadAI", "rsp", temp, offset);
    add_op(codelist, op);  

    return temp;
}

void setup_code_start(Node* tree, Stack* scope) {
    CodeList *codelist = init_codelist();
    Symbol* s = search(scope, "main");

    if(s == NULL){
        fprintf(stderr, "MAIN_NOT_FOUND_ERROR. A main function must be defined.\n");
        exit(MAIN_NOT_FOUND_ERROR);
    }

    // Setup of base registers
    add_op(codelist, init_op_ldc("loadI", "rfp", RFP_START_VALUE));
    add_op(codelist, init_op_ldc("loadI", "rsp", RSP_START_VALUE));
    add_op(codelist, init_op_ldc("loadI", "rbss", RBSS_START_VALUE));

    // Setup jump to main function
    add_op(codelist, jump("jumpI", s->base));

    tree->codelist = concat_code(codelist, tree->codelist);
}

void setup_function(Stack* scope, Node* function, Node* body, Param* params) {
    Symbol* s = search(scope, function->value->token_value.string);
    CodeList* codelist = init_codelist();

    // Add label for function's first instruction
    add_op(codelist, init_op_label(s->base));

    if(strcmp(function->value->token_value.string, "main") != 0) {
        write_dynamic_link(s->ar, &codelist);  // save caller rfp
        add_op(codelist, init_op_rr("i2i", "rsp", "rfp"));  // update rfp
    }

    add_op(codelist, init_op_crr("addI", "rsp", "rsp", s->ar->size));  // update rsp

    // Put body code into function node's code
    if(body != NULL)
        function->codelist = concat_code(codelist, body->codelist);
    else
        // Empty program
        function->codelist = codelist;

    // Return sequence is added on return command at parser.y
    if(strcmp(function->value->token_value.string, "main") == 0) {
        // It's the main function, there's no return
        add_op(function->codelist, init_halt());
    }
}

void setup_call(Stack* scope, Node* function, Node* args) {
    Operation* op; 

    CodeList* codelist = init_codelist();
    int offset = 0, return_offset;
    Symbol* s = search(scope, function->value->token_value.string);

    write_arguments(s->ar, &codelist, args);
    write_static_link(s->ar, &codelist);
    write_return_addr(s->ar, &codelist);

    // Jump to function
    add_op(codelist, jump("jumpI", s->base));

    if(s->type != TYPE_NAN){
        // The function is non-void
        function->temp = load_return_value(s->ar, &codelist);
    }

    function->codelist = codelist;
}

int get_local_var_offset(Param* params) {
    return get_return_addr_offset(params) + 4;
}

int get_return_addr_offset(Param* params) {
    return get_pc_addr_offset(params) + 4;
}

int get_pc_addr_offset(Param* params) {
    return get_static_link_offset(params) + 4;
}

int get_static_link_offset(Param* params) {
    return get_dynamic_link_offset(params) + 4;
}

int get_dynamic_link_offset(Param* params) {
    return get_return_value_offset(params) + 4;
}

int get_return_value_offset(Param* params) {
    int offset = get_arguments_offset();

    Param* aux = params;
    while(aux != NULL) {
        offset += get_type_size(aux->symbol->type);
        aux = aux->next;
    }

    return offset;
}

int get_arguments_offset() {
    return 0;
}

int local_var_size(Table* body) {
    int offset = 0;

    for(int i = 0; i < body->size; i++) {
        if(body->entries[i] != NULL && body->entries[i] != &DELETED_ENTRY)
            offset += get_type_size(body->entries[i]->value->type);
    }

    return offset;
}

int get_ra_size(Table* body, Param* params) {
    return get_local_var_offset(params) + local_var_size(body);
}

void set_ra_size(Stack* stack, Lexeme* function) {
    Symbol* symbol = get_entry(stack->top->next->value, function->token_value.string);
    symbol->ar->size = get_ra_size(peek(stack), symbol->args);
}