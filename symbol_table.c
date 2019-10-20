#include "symbol_table.h"
#include "lexical.h"
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

Symbol* create_symbol(int line_number, int nature, int type, int args_number, Symbol** args, Lexeme* lexeme) {
    Symbol* symbol = malloc(sizeof(Symbol));

    symbol->line_number = line_number;
    symbol->nature = nature;
    symbol->type = type;
    symbol->size = get_type_size(symbol->type);
    symbol->args_number = args_number;
    symbol->args = args;
    symbol->lexeme = lexeme;

    return symbol;
}

void add_entry(Table* table, Entry* entry) {
    if((float) table->count / table->size > 0.7)
        resize(table, table->base_size * 2);

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

void add_symbol(Table* table, const char* key, Symbol* value) {
    Entry* entry = create_entry(key, value);
    add_entry(table, entry);
}

Symbol* get_entry(Table* table, const char* key) {
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

void delete_symbol(Symbol* symbol) {
    for(int i = 0; i < symbol->args_number; i++) {
        Symbol* symbol = symbol->args[i];
        if(symbol != NULL) delete_symbol(symbol);
    }

    delete_lexeme(symbol->lexeme);

    free(symbol->args);
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
    for(int i = 0; i < table->size; i++) {
        Entry* entry = table->entries[i];
        if(entry != NULL) delete_entry(entry);
    }

    free(table->entries);
    free(table);

    table = NULL;
}