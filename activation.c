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
        op = init_op_rrc("storeAI", current->temp, "rsp", offset);
        op->type = OP_STC;
        add_op(codelist, op);
        offset += get_type_size(current->type);

        current = current->n_children ? current->children[current->n_children-1] : NULL;
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
    op = init_op_rrc("storeAI", return_temp, "rsp", offset);
    op->type = OP_STC;
    add_op(codelist, op);
}

void write_dynamic_link(ActivationRecord* ar, CodeList **codelist_p) {
    Operation *op;
    CodeList *codelist = *codelist_p;
    int offset = ar->arguments_offset + get_type_size(ar->return_type);

    // TODO: maybe can be done in the calee part, so not to be repeated constantly
    // Save dynamic link
    op = init_op_rrc("storeAI", "rfp", "rsp", offset);
    op->type = OP_STC;
    add_op(codelist, op);
}

void write_static_link(ActivationRecord* ar, CodeList **codelist_p){
    Operation *op;
    CodeList *codelist = *codelist_p;
    int offset = ar->arguments_offset + get_type_size(ar->return_type) + 4;

    // Static link is always zero, because we can't have nested functions
    int static_link = 0;

    // Save static link
    char* static_temp = load_imm(codelist, static_link);
    op = init_op_rrc("storeAI", static_temp, "rsp", offset);
    op->type = OP_STC;
    add_op(codelist, op);
}

void write_return_addr(ActivationRecord* ar, CodeList **codelist_p) {
    Operation *op;
    CodeList *codelist = *codelist_p;
    int offset = ar->arguments_offset + get_type_size(ar->return_type) + 8;

    // Save return address
    char* pc_return = get_register();
    // 3 for the add and store of the return address and the jump
    add_op(codelist, init_op_rrc("addI", "rpc", pc_return, 3));
    op = init_op_rrc("storeAI", pc_return, "rsp", offset);
    op->type = OP_STC;
    add_op(codelist, op);
}

char* load_arguments(ActivationRecord* ar, CodeList **codelist_p, Param* param_list) {
    Param *current = param_list;
    int offset = 0;

    while(current != NULL){
        offset += get_type_size(current->symbol->type);
    }

    ar->arguments_offset = offset;
}

char* load_return_value(ActivationRecord* ar, CodeList **codelist_p) {
    // PS: if this can just use RSP, we need to restore it inside the function call return sequence
    char* temp = get_register();
    CodeList* codelist = *codelist_p;

    Operation *op = init_op_rrc("loadAI", "rsp", temp, ar->arguments_offset);
    add_op(codelist, op);  

    return temp;
}

char* load_dynamic_link(ActivationRecord* ar, CodeList **codelist_p) {
    char* temp = get_register();
    int offset =  ar->arguments_offset + get_type_size(ar->return_type);
    CodeList* codelist = *codelist_p;

    Operation *op = init_op_rrc("loadAI", "rsp", temp, offset);
    add_op(codelist, op);  

    return temp;
}

char* load_static_link(ActivationRecord* ar, CodeList **codelist_p) {
    char* temp = get_register();
    int offset =  ar->arguments_offset + get_type_size(ar->return_type) + 4;
    CodeList* codelist = *codelist_p;

    Operation *op = init_op_rrc("loadAI", "rsp", temp, offset);
    add_op(codelist, op);  

    return temp;
}

char* load_return_addr(ActivationRecord* ar, CodeList **codelist_p) {
    char* temp = get_register();
    int offset =  ar->arguments_offset + get_type_size(ar->return_type) + 8;
    CodeList* codelist = *codelist_p;

    Operation *op = init_op_rrc("loadAI", "rsp", temp, offset);
    add_op(codelist, op);  

    return temp;
}

void setup_code_start(Node* tree, Stack* scope) {
    CodeList *codelist = init_codelist();
    Symbol* s = search(scope, "main");

    if(s == NULL){
        fprintf(stderr, "MAIN_NOT_FIND_ERROR. A main function must be defined.\n");
        exit(MAIN_NOT_FIND_ERROR);
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

    // TODO: add the rest of the call sequence here?

    // Put body code into function node's code
    if(body != NULL)
        function->codelist = concat_code(codelist, body->codelist);
    else
        // Empty program
        function->codelist = codelist;

    if(strcmp(function->value->token_value.string, "main") != 0){
        // TODO: add return sequence here?
    } else {
        // It's the main function, there's no return
        add_op(function->codelist, init_halt());
    }
}

void setup_call(Stack* scope, Node* function, Node* args) {
    Operation* op; 

    CodeList* codelist = init_codelist();
    int offset = 0, return_offset;
    Symbol* s = search(scope, function->value->token_value.string);
    ActivationRecord *ar = init_ar(function->type);

    write_arguments(ar, &codelist, args);
    write_dynamic_link(ar, &codelist);
    write_static_link(ar, &codelist);
    write_return_addr(ar, &codelist);

    // Jump to function
    add_op(codelist, jump("jumpI", s->base));

    if(s->type != TYPE_NAN){
        // The function is non-void
        // PS: if this can just use RSP, we need to restore it inside the function call return sequence
        function->temp = load_return_value(ar, &codelist);    
    }

    function->codelist = codelist;
}