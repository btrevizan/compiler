#ifndef COMPILER_LEXICAL_H
#define COMPILER_LEXICAL_H

// Token type
#define TK_PR       0  // keyword
#define TK_SC       1  // special char
#define TK_OC       2  // compound(?) operator
#define TK_ID       3  // identifier
#define TK_LT       4  // literal

// Literal type
#define LT_NAL      3210  // Not A Literal
#define LT_INT      40
#define LT_FLOAT    41
#define LT_BOOL     42
#define LT_CHAR     43
#define LT_STRING   44

union Value {
    int integer;  // can be a boolean too
    float real;
    char character;
    char* string;
};

struct Lexeme {
    int line_number;
    int token_type;
    union Value token_value;
    int literal_type;
};

void init_lexeme();
void set_as_pr();
void set_as_sc();
void set_as_oc();
void set_as_id();
void set_as_int();
void set_as_float();
void set_as_true();
void set_as_false();
void set_as_char();
void set_as_string();

#endif
