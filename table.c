#include "table.h"
#include "lexical.h"
#include "errors.h"
#include "code.h"
#include "address.h"
#include "activation.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

int hash(const char* key, const int prime_number, const int length) {
    long hashcode = 0;
    const int key_len = strlen(key);

    for (int i = 0; i < key_len; i++) {
        hashcode += (long) pow(prime_number, key_len - (i + 1)) * key[i];
        hashcode = hashcode % length;
    }

    return (int) hashcode;
}

int get_hash(const char* key, const int length, const int attempt) {
    const int hash_a = hash(key, PRIME_1, length);
    const int hash_b = hash(key, PRIME_2, length);
    return (hash_a + (attempt * (hash_b + 1))) % length;
}

int get_type_size(int type) {
    switch (type) {
        case TYPE_INT:
            return SIZE_INT;

        case TYPE_FLOAT:
            return SIZE_FLOAT;

        case TYPE_CHAR:
            return SIZE_CHAR;

        case TYPE_BOOL:
            return SIZE_BOOL;

        case TYPE_NAN:
            return 0;

        default:
            return -1;
    }
}

int is_prime(int x) {
    if(x % 2 == 0) return 0;

    for(int i = 3; i <= sqrt((float) x); i++)
        if(x % i == 0) return 0;

    return 1;
}

int next_prime(int x) {
    while(is_prime(x) == 0)
        x++;

    return x;
}

void resize(Table* table, int new_size) {
    if(new_size < TABLE_LENGTH) return;

    table->base_size = new_size;
    table->size = next_prime(table->base_size);
    table->entries = realloc(table->entries, table->size * sizeof(Entry*));

    memset((table->entries + table->count), 0, table->size * sizeof(Entry*) - table->count);
}

Table* create_table() {
    return create_sized_table(TABLE_LENGTH);
}

Table* create_sized_table(int size) {
    Table* table = malloc(sizeof(Table));

    table->base_size = size;
    table->size = next_prime(table->base_size);
    table->count = 0;
    table->entries = calloc((size_t) table->size, sizeof(Entry*));

    return table;
}

Entry* create_entry(const char* key, Symbol* value) {
    Entry* entry = malloc(sizeof(Entry));

    entry->key = strdup(key);
    entry->value = value;

    return entry;
}

Symbol* create_symbol(int nature, int type, Lexeme* lexeme) {
    Symbol* symbol = malloc(sizeof(Symbol));

    if(nature == NATUREZA_IDENTIFICADOR || nature == NATUREZA_VETOR)
        symbol->dimension = create_dim(type);
    else
        symbol->dimension = NULL;

    symbol->nature = nature;
    symbol->type = type;
    symbol->args = NULL;
    symbol->registers = NULL;
    symbol->lexeme = malloc(sizeof(Lexeme));
    memmove(symbol->lexeme, lexeme, sizeof(Lexeme));
    if(lexeme->literal_type == LT_NAL || lexeme->literal_type == LT_STRING)
        symbol->lexeme->token_value.string = strdup(lexeme->token_value.string);

    return symbol;
}

Param* create_param(int type, Lexeme* identifier) {
    Param* param = malloc(sizeof(Param));

    param->symbol = create_symbol(NATUREZA_IDENTIFICADOR, type, identifier);
    param->next = NULL;

    return param;
}

Dim* create_dim(int type) {
    Dim *d = malloc(sizeof(Dim));
    d->size = get_type_size(type);
    d->count = 1;
    d->next = NULL;
    return d;
}

RegList* create_reg_list(char* reg) {
    RegList *r = malloc(sizeof(RegList));
    r->reg = reg;
    r->next = NULL;

    return r;
}

Dim* convert_dim(Node *node) {
    Node *current;
    Dim *dimension, *first, *aux;
    int total_count = 1;

    current = node;
    first = (current != NULL) ? malloc(sizeof(Dim)) : NULL;
    
    if(first != NULL) {
        first->size = node->value->token_value.integer;
        total_count *= first->size;
        current = (current->n_children > 0) ? current = current->children[0] : NULL;
        first->next = NULL;
    }

    aux = first;

    while(current != NULL){
        dimension = malloc(sizeof(Dim));
        dimension->size = current->value->token_value.integer;
        total_count *= dimension->size;

        aux->next = dimension;
        aux = aux->next;
        current = (current->n_children > 0) ? current = current->children[0] : NULL;
    }
    aux->next = NULL;

    if(first != NULL)
        first->count = total_count;

    return first;
}

void set_scope(Symbol* symbol, int scope, int type, int dimension){
    if(scope == GLOBAL){
        symbol->address = get_global_offset(type, dimension);
        symbol->base = strdup("rbss"); 
    } else if(scope == LOCAL) {
        symbol->address = get_local_offset(type, dimension);
        symbol->base = strdup("rsp"); 
    } else if(scope == PARAM) {
        symbol->address = get_param_offset(type, dimension);
        symbol->base = strdup("rfp");
    }
}

void add_entry(Table* table, Entry* entry) {
    if(table == NULL) return;

    if((float) table->count / table->size > 0.7)
        resize(table, table->base_size * 2);

    if(get_entry(table, entry->key) != NULL) {
        fprintf(stderr, "ERR_DECLARED: '%s' was already declared.\n", entry->value->lexeme->token_value.string);
        exit(ERR_DECLARED);
    }

    int i = get_hash(entry->key, table->size, 0);
    Entry* current_entry = table->entries[i];

    int attempt = 1;
    while(current_entry != NULL && current_entry != &DELETED_ENTRY) {
        i = get_hash(entry->key, table->size, attempt);
        current_entry = table->entries[i];
        attempt++;
    }

    table->entries[i] = entry;
    table->count++;
}

void add_symbol(Table* table, Symbol* value) {
    Entry* entry = create_entry(value->lexeme->token_value.string, value);
    add_entry(table, entry);
}

Symbol* get_entry(Table* table, const char* key) {
    if(table == NULL) return NULL;

    int i = get_hash(key, table->size, 0);
    Entry* current_entry = table->entries[i];

    int attempt = 1;
    while(current_entry != NULL) {
        if(current_entry != &DELETED_ENTRY && strcmp(current_entry->key, key) == 0)
            return current_entry->value;

        i = get_hash(key, table->size, attempt);
        current_entry = table->entries[i];
        attempt++;
    }

    return NULL;
}

void remove_entry(Table* table, const char* key) {
    if(table == NULL) return;

    int i = get_hash(key, table->size, 0);
    Entry* current_entry = table->entries[i];

    int attempt = 1;
    while(current_entry != NULL) {
        if(current_entry != &DELETED_ENTRY && strcmp(current_entry->key, key) == 0) {
            delete_entry(current_entry);
            table->entries[i] = &DELETED_ENTRY;
            break;
        }

        i = get_hash(key, table->size, attempt);
        current_entry = table->entries[i];
        attempt++;
    }

    table->count--;
}

void add_identifier(Table* table, int type, Lexeme* identifier, int scope){
    Symbol *symbol = create_symbol(NATUREZA_IDENTIFICADOR, type, identifier);
    add_symbol(table, symbol);
    set_scope(symbol, scope, type, NOT_ARRAY);
}

void add_vector(Table* table, int type, Lexeme* identifier, Node* indexer, int scope) {
    Symbol *symbol = create_symbol(NATUREZA_VETOR, type, identifier);
    delete_dim_list(symbol->dimension);
    symbol->dimension = convert_dim(indexer);
    add_symbol(table, symbol);
    set_scope(symbol, scope, type, symbol->dimension->count);
}

void add_function(Table* table, int type, Lexeme* function, Param* params) {
    Symbol* symbol = create_symbol(NATUREZA_FUNCAO, type, function);
    symbol->args = params;
    symbol->base = get_label(); //function's first instruction label

    if(params == NULL) 
        symbol->args_number = 0;
    else
        symbol->args_number = params->count;

    add_symbol(table, symbol);

    // Resets the address relative to rsp
    set_local_offset(0);
    // Resets the address relative to rfp
    set_param_offset(0);

    // Set activation record
    symbol->ar = init_ar(type);
    symbol->ar->arguments_offset = get_arguments_offset(params);
    symbol->ar->dynamic_link_offset = get_dynamic_link_offset(params);
    symbol->ar->static_link_offset = get_static_link_offset(params);
    symbol->ar->machine_state_offset = get_machine_state_offset(params);
    symbol->ar->local_var_offset = get_local_var_offset(params, symbol->registers);
    symbol->ar->pc_addr_offset = get_pc_addr_offset(params);
    symbol->ar->return_addr_offset = get_return_addr_offset(params);
    symbol->ar->return_value_offset = get_return_value_offset(params);
}

void add_param(Table* table, int type, Lexeme* identifier, int scope) {
    Symbol* symbol = create_symbol(NATUREZA_IDENTIFICADOR, type, identifier);
    add_symbol(table, symbol);
    set_scope(symbol, scope, type, NOT_ARRAY);
}

void delete_symbol(Symbol* symbol) {
    delete_param_list(symbol->args);
    delete_dim_list(symbol->dimension);
    
    if(symbol->lexeme != NULL) delete_lexeme(symbol->lexeme);

    free(symbol);
    symbol = NULL;
}

void delete_entry(Entry* entry) {
    free(entry->key);

    delete_symbol(entry->value);
    free(entry);

    entry = NULL;
}

void delete_table(Table* table) {
    if(table == NULL) return;

    for(int i = 0; i < table->size; i++) {
        Entry* entry = table->entries[i];
        if(entry != NULL) delete_entry(entry);
    }

    free(table->entries);
    free(table);

    table = NULL;
}

void delete_param_list(Param* list) {
    Param *aux;

    while(list != NULL) {
        delete_symbol(list->symbol);
        aux = list->next;
        free(list);
        list = aux;
    }
}

void delete_dim_list(Dim* list) {
    Dim *aux;

    while(list != NULL) {
        aux = list->next;
        free(list);
        list = aux;
    }
}

void print_table(Table* table) {
    printf("Key/ID | Line Number | Nature\n");

    if(table == NULL) {
        printf("Table is null.\n");
        return;
    }

    if(table->count == 0) {
        printf("Table is empty.\n");
        return;
    }

    for(int i = 0; i < table->size; i++) {
        if(table->entries[i] != NULL) {
            printf("%s | ", table->entries[i]->key);
            printf("%d | ", table->entries[i]->value->lexeme->line_number);
            printf("%d\n", table->entries[i]->value->nature);
        }
    }
}