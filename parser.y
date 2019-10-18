%{
	#include <stdio.h>
	#include <stdlib.h>
	#include "lexical.h"
	#include "tree.h"

	extern int yylineno;

	int yylex(void);
	void yyerror (char const *s);
%}

%union {
    struct lexeme* lexical_value;
    struct node* node;
}

%token <lexical_value> TK_PR_INT
%token <lexical_value> TK_PR_FLOAT
%token <lexical_value> TK_PR_BOOL
%token <lexical_value> TK_PR_CHAR
%token <lexical_value> TK_PR_STRING
%token <lexical_value> TK_PR_IF
%token <lexical_value> TK_PR_THEN
%token <lexical_value> TK_PR_ELSE
%token <lexical_value> TK_PR_WHILE
%token <lexical_value> TK_PR_DO
%token <lexical_value> TK_PR_INPUT
%token <lexical_value> TK_PR_OUTPUT
%token <lexical_value> TK_PR_RETURN
%token <lexical_value> TK_PR_CONST
%token <lexical_value> TK_PR_STATIC
%token <lexical_value> TK_PR_FOREACH
%token <lexical_value> TK_PR_FOR
%token <lexical_value> TK_PR_SWITCH
%token <lexical_value> TK_PR_CASE
%token <lexical_value> TK_PR_BREAK
%token <lexical_value> TK_PR_CONTINUE
%token <lexical_value> TK_PR_CLASS
%token <lexical_value> TK_PR_PRIVATE
%token <lexical_value> TK_PR_PUBLIC
%token <lexical_value> TK_PR_PROTECTED
%token <lexical_value> TK_PR_END
%token <lexical_value> TK_PR_DEFAULT
%token <lexical_value> TK_OC_LE
%token <lexical_value> TK_OC_GE
%token <lexical_value> TK_OC_EQ
%token <lexical_value> TK_OC_NE
%token <lexical_value> TK_OC_AND
%token <lexical_value> TK_OC_OR
%token <lexical_value> TK_OC_SL
%token <lexical_value> TK_OC_SR
%token <lexical_value> TK_OC_FORWARD_PIPE
%token <lexical_value> TK_OC_BASH_PIPE
%token <lexical_value> TK_LIT_INT
%token <lexical_value> TK_LIT_FLOAT
%token <lexical_value> TK_LIT_FALSE
%token <lexical_value> TK_LIT_TRUE
%token <lexical_value> TK_LIT_CHAR
%token <lexical_value> TK_LIT_STRING
%token <lexical_value> TK_IDENTIFICADOR
%token <lexical_value> TOKEN_ERRO
%token <lexical_value> ';' ':' ',' '(' ')' '[' ']' '{' '}' '+' '-' '|' '?' '@' '*' '/' '<' '>' '=' '!' '&' '%' '#' '^' '.' '$'

%type <node> prog
%type <node> global_var
%type <node> function
%type <node> params
%type <node> list_of_params
%type <node> simple_command
%type <node> command_list
%type <node> block
%type <node> type
%type <node> initialization
%type <node> local_var_with_init
%type <node> local_var_without_init
%type <node> id
%type <node> assignment
%type <node> args
%type <node> input
%type <node> output
%type <node> call
%type <node> shift_op
%type <node> shift
%type <node> return
%type <node> if
%type <node> else
%type <node> if_else
%type <node> for_list_element
%type <node> for_list
%type <node> for
%type <node> while
%type <node> literal
%type <node> directTerm
%type <node> term
%type <node> expr
%type <node> body

/*
Precedence and associativity defined according to
https://pt.wikipedia.org/wiki/Operadores_em_C_e_C%2B%2B#Precedência_de_operadores
*/
%right '='
%right '?' ':'
%left TK_OC_AND TK_OC_OR
%left TK_OC_LE TK_OC_GE '>' '<'TK_OC_EQ TK_OC_NE
%left '&' '|'
%left TK_OC_SR TK_OC_SL
%right '!'
%left '+' '-'
%left '*' '/' '%'
%left '(' ')'
%left '[' ']'
%right '^'
%right '#'

/* Different associativity based in whether it's a binary or unary op */
%right UMINUS
%right UPLUS
%right UADDRESS
%right UPOINTER

/* Detailed error message */
%define parse.error verbose

%start prog
%%

prog: function prog 		{ $$ = $1; arvore = $$; add_node($$, $2); }
|     global_var prog 		{ $$ = NULL; }
| 				{ $$ = NULL; };

/** GLOBAL VAR DECLARATION **/
global_var: TK_PR_STATIC type id ';'	{ libera($3); $$ = NULL; }
| 	    type id ';'			{ libera($2); $$ = NULL; };

/** FUNCTION **/
function: TK_PR_STATIC type TK_IDENTIFICADOR '(' ')' body			{ $$ = unary_node($3, $6); }
| 	  type TK_IDENTIFICADOR '(' ')' body					{ $$ = unary_node($2, $5); }
| 	  TK_PR_STATIC type TK_IDENTIFICADOR '(' list_of_params ')' body	{ $$ = unary_node($3, $7); }
| 	  type TK_IDENTIFICADOR '(' list_of_params ')' body			{ $$ = unary_node($2, $6); };

body: '{' command_list '}' 	{ $$ = $2; }
|     '{' '}'			{ $$ = NULL; };

params: TK_PR_CONST type TK_IDENTIFICADOR 	{ libera(create_node($3)); $$ = NULL; }
| 	type TK_IDENTIFICADOR			{ libera(create_node($2)); $$ = NULL; };

list_of_params: params				{ $$ = NULL; }
| 		params ',' list_of_params	{ $$ = NULL; };

/****** SIMPLE COMMANDS ******/
simple_command: local_var_with_init	{ $$ = $1; }
|		local_var_without_init  { $$ = NULL; }
|		assignment		{ $$ = $1; }
|		input			{ $$ = NULL; }
|		output			{ $$ = NULL; }
|		shift			{ $$ = $1; }
|		return			{ $$ = $1; }
|		TK_PR_BREAK		{ $$ = create_node($1); }
|		TK_PR_CONTINUE		{ $$ = create_node($1); }
|		if_else			{ $$ = $1; }
|	        for			{ $$ = $1; }
|	        while			{ $$ = $1; }
|		block			{ $$ = $1; }
|		call			{ $$ = $1; };

command_list: simple_command ';'			{ $$ = $1; }
| 	      simple_command ';' command_list		{ if($1 == NULL) { $$ = $3; } else { $$ = $1; add_node($$, $3); } };

block: '{' command_list '}' 	{ $$ = unary_node(NULL, $2); }
|      '{' '}'			{ $$ = create_node(NULL); };

/** Local variable declaration **/
type: TK_PR_INT		{ $$ = NULL; }
|     TK_PR_FLOAT	{ $$ = NULL; }
|     TK_PR_BOOL	{ $$ = NULL; }
|     TK_PR_CHAR	{ $$ = NULL; }
|     TK_PR_STRING	{ $$ = NULL; };

initialization: TK_OC_LE directTerm							{ $$ = unary_node($1, $2); };

local_var_with_init: TK_PR_STATIC TK_PR_CONST type TK_IDENTIFICADOR initialization	{ $$ = $5; add_lexeme($$, $4); }
|	    	     TK_PR_STATIC type TK_IDENTIFICADOR initialization			{ $$ = $4; add_lexeme($$, $3); }
|	   	     TK_PR_CONST type TK_IDENTIFICADOR initialization			{ $$ = $4; add_lexeme($$, $3); }
|	   	     type TK_IDENTIFICADOR initialization				{ $$ = $3; add_lexeme($$, $2); };

local_var_without_init: TK_PR_STATIC TK_PR_CONST type TK_IDENTIFICADOR			{ libera(create_node($4)); $$ = NULL; }
| 	   	        TK_PR_STATIC type TK_IDENTIFICADOR				{ libera(create_node($3)); $$ = NULL; }
|	   	        TK_PR_CONST type TK_IDENTIFICADOR				{ libera(create_node($3)); $$ = NULL; }
|	   	        type TK_IDENTIFICADOR						{ libera(create_node($2)); $$ = NULL; };

/** Assignment **/
id: TK_IDENTIFICADOR '[' expr ']'	{ $$ = create_node(NULL); add_lexeme($$, $1); add_node($$, $3); }
|   TK_IDENTIFICADOR			{ $$ = create_node($1); };

assignment: id '=' expr			{ $$ = binary_node($2, $1, $3); };

/** Input and output **/
args: expr 				{ $$ = $1; }
|     expr ',' args			{ $$ = $1; add_node($$, $3); };

input: TK_PR_INPUT expr			{ libera($2); $$ = NULL; };
output: TK_PR_OUTPUT args		{ libera($2); $$ = NULL; };

/** Function call **/
call: TK_IDENTIFICADOR '(' args ')'	{ $$ = unary_node($1, $3); }
|     TK_IDENTIFICADOR '(' ')'		{ $$ = create_node($1); };

/** Shift command **/
shift_op: TK_OC_SL		{ $$ = create_node($1); }
| 	  TK_OC_SR		{ $$ = create_node($1); };

shift: id shift_op expr		{ $$ = $2; add_node($$, $1); add_node($$, $3); };

/** Flow change commands **/
return: TK_PR_RETURN expr		{ $$ = unary_node($1, $2); };

/** If-then-else statement **/
if: TK_PR_IF '(' expr ')' block		{ $$ = binary_node($1, $3, $5); };
else: TK_PR_ELSE block			{ $$ = $2; };
if_else: if else 			{ $$ = $1; add_node($$, $2); }
| 	 if				{ $$ = $1; };

/** Iterative commands **/
for_list_element: local_var_with_init				{ $$ = $1; }
|		  local_var_without_init			{ $$ = NULL; }
| 		  assignment					{ $$ = $1; };

for_list: for_list_element 					{ $$= $1; }
| 	  for_list_element ',' for_list				{ if($1 == NULL) { $$ = $3; } else { $$ = $1; add_node($$, $3); } };

for: TK_PR_FOR '(' for_list ':' expr ':' for_list ')' block	{ $$ = quaternary_node($1, $3, $5, $7, $9); libera(create_node($4)); libera(create_node($6)); };
while: TK_PR_WHILE '(' expr ')' TK_PR_DO block			{ $$ = binary_node($1, $3, $6); };

/****** ARITHMETIC AND LOGICAL EXPRESSIONS ******/
literal: TK_LIT_INT		{ $$ = create_node($1); }
| 	 TK_LIT_FLOAT		{ $$ = create_node($1); }
|	 TK_LIT_FALSE		{ $$ = create_node($1); }
|	 TK_LIT_TRUE		{ $$ = create_node($1); }
|	 TK_LIT_CHAR		{ $$ = create_node($1); }
|	 TK_LIT_STRING		{ $$ = create_node($1); };

directTerm: id 			{ $$ = $1; }
| 	    literal		{ $$ = $1; };

term: directTerm		{ $$ = $1; }
|     call			{ $$ = $1; };

expr: term			{ $$ = $1; }
|     '+' expr			%prec UPLUS  { $$ = unary_node($1, $2); }
|     '-' expr			%prec UMINUS { $$ = unary_node($1, $2); }
|     '!' expr			{ $$ = unary_node($1, $2); }
|     '&' expr			%prec UADDRESS { $$ = unary_node($1, $2); }
|     '*' expr			%prec UPOINTER { $$ = unary_node($1, $2); }
|     '?' expr			{ $$ = unary_node($1, $2); }
|     '#' expr			{ $$ = unary_node($1, $2); }
|     expr '+' expr		{ $$ = binary_node($2, $1, $3); }
|     expr '-' expr		{ $$ = binary_node($2, $1, $3); }
|     expr '*' expr		{ $$ = binary_node($2, $1, $3); }
|     expr '/' expr		{ $$ = binary_node($2, $1, $3); }
|     expr '%' expr		{ $$ = binary_node($2, $1, $3); }
|     expr '|' expr		{ $$ = binary_node($2, $1, $3); }
|     expr '&' expr		{ $$ = binary_node($2, $1, $3); }
|     expr '^' expr		{ $$ = binary_node($2, $1, $3); }
|     expr '>' expr		{ $$ = binary_node($2, $1, $3); }
|     expr '<' expr		{ $$ = binary_node($2, $1, $3); }
|     expr TK_OC_LE expr	{ $$ = binary_node($2, $1, $3); }
|     expr TK_OC_GE expr	{ $$ = binary_node($2, $1, $3); }
|     expr TK_OC_EQ expr	{ $$ = binary_node($2, $1, $3); }
|     expr TK_OC_NE expr	{ $$ = binary_node($2, $1, $3); }
|     expr TK_OC_AND expr	{ $$ = binary_node($2, $1, $3); }
|     expr TK_OC_OR expr	{ $$ = binary_node($2, $1, $3); }
|     expr '?' expr ':' expr	{ $$ = ternary_node($4, $1, $3, $5); libera(create_node($2)); }
|     '(' expr ')'		{ $$ = $2; };

%%

void yyerror (char const *str){
    fprintf(stderr, "%s on line %d\n", str, yylineno);
}