#ifndef COMPILER_SYMBOL_TABLE_H
#define COMPILER_SYMBOL_TABLE_H

// Based on https://github.com/jamesroutley/write-a-hash-table

#include "lexical.h"
#include <stdlib.h>

#define TABLE_LENGTH 20
#define PRIME_1 131
#define PRIME_2 137

#define NATUREZA_LITERAL_INT        1
#define NATUREZA_LITERAL_FLOAT      2
#define NATUREZA_LITERAL_CHAR       3
#define NATUREZA_LITERAL_STRING     4
#define NATUREZA_LITERAL_BOOL       5
#define NATUREZA_IDENTIFICADOR      6

typedef struct symbol {
    int line_number;
    int nature;
    int size;
    int args_number;
    struct symbol** args;
    Lexeme* lexeme;
} Symbol;

typedef struct entry {
    char* key;
    Symbol* value;
} Entry;

typedef struct table {
    int size;
    int count;
    int base_size;
    Entry** entries;
} Table;

static Entry DELETED_ENTRY = {NULL, NULL};

Table* create_table();
Table* create_sized_table(int size);
Entry* create_entry(const char* key, Symbol* value);
Symbol* create_symbol(int line_number, int nature, int args_number, Symbol** args, Lexeme* lexeme);

void add_entry(Table* table, Entry* entry);
void add_symbol(Table* table, const char* key, Symbol* value);
Symbol* get_entry(Table* table, const char* key);
void remove_entry(Table* table, const char* key);

void delete_symbol(Symbol* symbol);
void delete_entry(Entry* entry);
void delete_table(Table* table);

#endif
