#include "table.h"
#include "lexical.h"
#include "errors.h"
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

    symbol->nature = nature;
    symbol->type = type;
    symbol->size = get_type_size(symbol->type);
    symbol->args = NULL;
    symbol->lexeme = malloc(sizeof(Lexeme));
    memmove(symbol->lexeme, lexeme, sizeof(Lexeme));
    symbol->lexeme->token_value.string = strdup(lexeme->token_value.string);

    return symbol;
}

Param* create_param(int type, Lexeme* identifier) {
    Param* param = malloc(sizeof(Param));

    param->symbol = create_symbol(NATUREZA_IDENTIFICADOR, type, identifier);
    param->next = NULL;

    return param;
}

void add_entry(Table* table, Entry* entry) {
    if(table == NULL) return;

    if((float) table->count / table->size > 0.7)
        resize(table, table->base_size * 2);

    if(get_entry(table, entry->key) != NULL) {
        printf("ERR_DECLARED: '%s' was already declared.\n", entry->value->lexeme->token_value.string);
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

void add_identifier(Table* table, int type, Lexeme* identifier){
    Symbol *symbol = create_symbol(NATUREZA_IDENTIFICADOR, type, identifier);
    add_symbol(table, symbol);
}

void add_vector(Table* table, int type, Lexeme* identifier, Node* indexer) {
    Symbol *symbol = create_symbol(NATUREZA_VETOR, type, identifier);
    symbol->size = symbol->size * indexer->value->token_value.integer;
    add_symbol(table, symbol);
}

void add_function(Table* table, int type, Lexeme* function, Param* params){
    Symbol* symbol = create_symbol(NATUREZA_FUNCAO, type, function);
    symbol->args = params;

    if(params == NULL) 
        symbol->args_number = 0;
    else
        symbol->args_number = params->count;

    add_symbol(table, symbol);
}

void delete_symbol(Symbol* symbol) {
    delete_param_list(symbol->args);
    
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