#ifndef COMPILER_SYMBOL_TABLE_H
#define COMPILER_SYMBOL_TABLE_H

// Based on https://github.com/jamesroutley/write-a-hash-table

#include "lexical.h"
#include "tree.h"
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
#define NATUREZA_FUNCAO             7
#define NATUREZA_VETOR              8

#define TYPE_NAN                    -1
#define TYPE_INT                    10
#define TYPE_FLOAT                  11
#define TYPE_CHAR                   12
#define TYPE_STRING                 13
#define TYPE_BOOL                   14

#define SIZE_INT                    4
#define SIZE_CHAR                   1
#define SIZE_FLOAT                  8
#define SIZE_BOOL                   1

typedef struct symbol {
    // line number is in lexeme->line_number
    int nature;
    int type;
    int size;
    // args_number is in args->count
    struct table* args;
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

typedef struct param {
    Symbol* symbol;
    struct param* next;
} Param;

static Entry DELETED_ENTRY = {NULL, NULL};

Table* create_table();
Table* create_sized_table(int size);
Entry* create_entry(const char* key, Symbol* value);
Symbol* create_symbol(int nature, int type, Lexeme* lexeme);
Param* create_param(int type, Lexeme* identifier);

Symbol* get_entry(Table* table, const char* key);

void remove_entry(Table* table, const char* key);
void add_symbol(Table* table, Symbol* value);
void add_identifier(Table* table, int type, Lexeme* identifier);
void add_function(Table* table, int type, Lexeme* function, Param* params);

void delete_symbol(Symbol* symbol);
void delete_entry(Entry* entry);
void delete_table(Table* table);

void print_table(Table* table);

#endif
