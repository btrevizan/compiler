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

#define NO_CONVERSION               100
#define INT_TO_FLOAT                101
#define INT_TO_BOOL                 102
#define BOOL_TO_INT                 103
#define BOOL_TO_FLOAT               104
#define FLOAT_TO_INT                105
#define FLOAT_TO_BOOL               106

#define SIZE_INT                    4
#define SIZE_CHAR                   1
#define SIZE_FLOAT                  8
#define SIZE_BOOL                   1

#define GLOBAL                      0
#define LOCAL                       1
#define PARAM                       2

typedef struct symbol {
    // line number is in lexeme->line_number
    Lexeme* lexeme;
    int type;
    int nature;
    char* base; // base register for addressing (either rbss or rfp)
    long long int address;
    struct dimension* dimension;  // dimensions of array type
    struct activation_record* ar;
    struct reg_list* registers;   // registers used in function body
    int args_number;
    struct param* args;
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
    int count;          // list size, from this element forward
    struct param* next;
} Param;

typedef struct dimension {
    int size;
    int count;
    struct dimension* next;
} Dim;

typedef struct reg_list {
    char* reg;
    struct reg_list* last;
    struct reg_list* next;
} RegList;

static Entry DELETED_ENTRY = {NULL, NULL};

Table* create_table();
Table* create_sized_table(int size);
Entry* create_entry(const char* key, Symbol* value);
Symbol* create_symbol(int nature, int type, Lexeme* lexeme);
Param* create_param(int type, Lexeme* identifier);
Dim* create_dim(int type);
RegList* create_reg_list(char* reg);

int get_type_size(int type);
void set_scope(Symbol* symbol, int scope, int type, int dimension);

Symbol* get_entry(Table* table, const char* key);

void remove_entry(Table* table, const char* key);
void add_symbol(Table* table, Symbol* value);
void add_identifier(Table* table, int type, Lexeme* identifier, int scope);
void add_vector(Table* table, int type, Lexeme* identifier, Node* indexer, int scope);
void add_function(Table* table, int type, Lexeme* function, Param* params);
void add_param(Table* table, int type, Lexeme* identifier, int scope);

Dim* convert_dim(Node *node);

void delete_symbol(Symbol* symbol);
void delete_entry(Entry* entry);
void delete_table(Table* table);
void delete_param_list(Param* list);
void delete_dim_list(Dim* list);

void print_table(Table* table);

#endif
