%{
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include "lexical.h"
	#include "tree.h"
	#include "stack.h"
	#include "table.h"
	#include "checks.h"
	#include "code.h"

	extern int yylineno;

	int yylex(void);
	void yyerror (char const *s);

	extern Stack *scope;
%}

%union {
    struct lexeme* lexical_value;
    struct node* node;
    int integer;
    struct param* parameter;
}

%token <integer> TK_PR_INT
%token <integer> TK_PR_FLOAT
%token <integer> TK_PR_BOOL
%token <integer> TK_PR_CHAR
%token <integer> TK_PR_STRING

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

%type <parameter> params
%type <parameter> list_of_params
%type <node> prog
%type <node> end_prog
%type <node> global_var
%type <node> lit_indexer
%type <node> function
%type <node> simple_command
%type <node> command_list
%type <node> block
%type <integer> type
%type <node> initialization
%type <node> local_var_with_init
%type <node> local_var_without_init
%type <node> declared_id
%type <node> indexer
%type <node> id
%type <node> assignment
%type <node> args
%type <node> input
%type <node> output
%type <node> call
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

%start end_prog
%%

end_prog: prog { $$ = $1, arvore = $$; setup_code_start($$, scope); destroy_stack(scope); }

prog: init_env function prog 		{ $$ = $2; arvore = $$; add_node($$, $3); link_code($$, $3); }
|     init_env global_var prog 		{ $$ = NULL; }
| 					{ $$ = NULL; };

/** SYMBOL TABLE STACK INITIALIZATION **/
init_env: /* Empty */ { if(scope == NULL) { scope = init_stack(); push(scope, create_table()); } };

/** ENTER NEW SCOPE **/
enter_scope: /* Empty */ { push(scope, create_table()); };

/** LEAVE SCOPE **/
leave_scope: /* Empty */ { delete_table(pop(scope)); };

/** GLOBAL VAR DECLARATION **/
global_var: TK_PR_STATIC type TK_IDENTIFICADOR ';'	        	{ add_identifier(peek(scope), $2, $3,  GLOBAL); $$ = NULL; delete_lexeme($3); }
|	    TK_PR_STATIC type TK_IDENTIFICADOR lit_indexer ';'	        { add_vector(peek(scope), $2, $3, $4, GLOBAL); libera($4); $$ = NULL; delete_lexeme($3); }
| 	    type TK_IDENTIFICADOR ';'					{ add_identifier(peek(scope), $1, $2, GLOBAL); $$ = NULL; delete_lexeme($2);}
| 	    type TK_IDENTIFICADOR lit_indexer ';'			{ add_vector(peek(scope), $1, $2, $3, GLOBAL); libera($3); $$ = NULL; delete_lexeme($2); };

/* Literal indexer */
lit_indexer: '[' literal ']'	{ implicit_conversion(TYPE_INT, $2); $$ = $2;}
| '[' literal ']' lit_indexer	{ implicit_conversion(TYPE_INT, $2); $$ = $2; add_node($$, $4); };

/** FUNCTION **/
function: TK_PR_STATIC type TK_IDENTIFICADOR '(' { add_function(peek(scope), $2, $3, NULL); } ')' enter_scope body leave_scope {
	$$ = unary_node($3, $8);
}
| 	  type TK_IDENTIFICADOR '(' { add_function(peek(scope), $1, $2, NULL); } ')' enter_scope body leave_scope {
	$$ = unary_node($2, $7);
	setup_function(scope, $$, $7);
}
| 	  TK_PR_STATIC type TK_IDENTIFICADOR '(' enter_scope list_of_params { add_function(scope->top->next->value, $2, $3, $6); } ')' body leave_scope {
	$$ = unary_node($3, $9);
}
| 	  type TK_IDENTIFICADOR '(' enter_scope list_of_params { add_function(scope->top->next->value, $1, $2, $5); } ')' body leave_scope {
	$$ = unary_node($2, $8); 
};

body: '{' command_list '}' 	{ $$ = $2; }
|     '{' '}'			{ $$ = NULL; };

params: TK_PR_CONST type TK_IDENTIFICADOR 	{ $$ = create_param($2, $3); add_symbol(peek(scope), create_symbol(NATUREZA_IDENTIFICADOR, $2, $3)); delete_lexeme($3); }
| 	type TK_IDENTIFICADOR			{ $$ = create_param($1, $2); add_symbol(peek(scope), create_symbol(NATUREZA_IDENTIFICADOR, $1, $2)); delete_lexeme($2); };

list_of_params: params				{ $$ = $1; $$->count = 1; }
| 		params ',' list_of_params	{ $$ = $1; $$->next = $3; $$->count = $3->count + 1; };

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
| 	      simple_command ';' command_list		{ if($1 == NULL) { $$ = $3; } else { $$ = $1; add_node($$, $3); $$->codelist = concat_code($$->codelist, $3->codelist); } };

block: '{' enter_scope command_list leave_scope '}' 	{ $$ = unary_node(NULL, $3); }
|      '{' '}'						{ $$ = create_node(NULL); };

/** Local variable declaration **/
type: TK_PR_INT		{ $$ = TYPE_INT; }
|     TK_PR_FLOAT	{ $$ = TYPE_FLOAT; }
|     TK_PR_BOOL	{ $$ = TYPE_BOOL; }
|     TK_PR_CHAR	{ $$ = TYPE_CHAR; }
|     TK_PR_STRING	{ $$ = TYPE_STRING; };

initialization: TK_OC_LE directTerm							{ $$ = unary_node($1, $2); };

local_var_with_init: TK_PR_STATIC TK_PR_CONST type TK_IDENTIFICADOR initialization	{ $$ = $5; implicit_conversion($3, $$->children[0]); add_identifier(peek(scope), $3, $4, LOCAL); add_lexeme($$, $4); }
|	    	     TK_PR_STATIC type TK_IDENTIFICADOR initialization			{ $$ = $4; implicit_conversion($2, $$->children[0]); add_identifier(peek(scope), $2, $3, LOCAL); add_lexeme($$, $3); }
|	   	     TK_PR_CONST type TK_IDENTIFICADOR initialization			{ $$ = $4; implicit_conversion($2, $$->children[0]); add_identifier(peek(scope), $2, $3, LOCAL); add_lexeme($$, $3); }
|	   	     type TK_IDENTIFICADOR initialization				{ $$ = $3; implicit_conversion($1, $$->children[0]); add_identifier(peek(scope), $1, $2, LOCAL); add_lexeme($$, $2); store_assign(scope, $2, $$); };

local_var_without_init: TK_PR_STATIC TK_PR_CONST type TK_IDENTIFICADOR			{ add_identifier(peek(scope), $3, $4, LOCAL); delete_lexeme($4); }
| 	   	        TK_PR_STATIC type TK_IDENTIFICADOR				{ add_identifier(peek(scope), $2, $3, LOCAL); delete_lexeme($3); }
|	   	        TK_PR_CONST type TK_IDENTIFICADOR				{ add_identifier(peek(scope), $2, $3, LOCAL); delete_lexeme($3); }
|	   	        type TK_IDENTIFICADOR						{ add_identifier(peek(scope), $1, $2, LOCAL); delete_lexeme($2); };

/** Assignment **/
declared_id: TK_IDENTIFICADOR 		{ $$ = create_node($1); check_declaration(scope, $$); };

indexer: '[' expr ']'			{ $$ = $2; implicit_conversion(TYPE_INT, $2); }
|        '[' expr ']' indexer	        { $$ = $2; implicit_conversion(TYPE_INT, $2);  add_node($$, $4); };

id: declared_id indexer			{ $$ = binary_node(NULL, $1, $2); $1->value->token_type = TK_VC; $$->type = $1->type; check_usage(scope, $1); }
|   declared_id				{ $$ = $1; check_usage(scope, $$); };

assignment: id '=' expr			{ implicit_conversion($1->type, $3); $$ = binary_node($2, $1, $3); $$->type = $1->type; store(scope, $1, $3, $$); };

/** Input and output **/
args: expr 				{ $$ = $1; }
|     expr ',' args			{ $$ = $1; add_node($$, $3); };

input: TK_PR_INPUT expr			{ check_type(INPUT_OP, $2); libera($2); $$ = NULL; };
output: TK_PR_OUTPUT args		{ check_type(OUTPUT_OP, $2); libera($2); $$ = NULL; };

/** Function call **/
call: declared_id '(' args ')'		{ $$ = $1; $$->value->token_type = TK_FN; add_node($$, $3); check_usage(scope, $1); check_args(scope, $1, $3); }
|     declared_id '(' ')'		{ $$ = $1; $$->value->token_type = TK_FN; check_usage(scope, $1); check_args(scope, $1, NULL); };

/** Shift command **/
shift: id TK_OC_SL expr		{ implicit_conversion(TYPE_INT, $3); $$ = binary_node($2, $1, $3); }
|      id TK_OC_SR expr		{ implicit_conversion(TYPE_INT, $3); $$ = binary_node($2, $1, $3); };

/** Flow change commands **/
return: TK_PR_RETURN expr		{ $$ = unary_node($1, $2); check_return_type(scope, $2); };

/** If-then-else statement **/
if: TK_PR_IF '(' expr ')' block		{ implicit_conversion(TYPE_BOOL, $3); $$ = binary_node($1, $3, $5); };
else: TK_PR_ELSE block			{ $$ = $2; };
if_else: if else 			{ $$ = $1; add_node($$, $2); if_then_else($$, $2); };
| 	 if				{ $$ = $1; if_then_else($$, NULL); };

/** Iterative commands **/
for_list_element: local_var_with_init				{ $$ = $1; }
|		  local_var_without_init			{ $$ = NULL; }
| 		  assignment					{ $$ = $1; };

for_list: for_list_element 					{ $$= $1; }
| 	  for_list_element ',' for_list				{ if($1 == NULL) { $$ = $3; } else { $$ = $1; add_node($$, $3); } };

for: TK_PR_FOR '(' for_list ':' expr ':' for_list ')' block	{ implicit_conversion(TYPE_BOOL, $5); $$ = quaternary_node($1, $3, $5, $7, $9); libera(create_node($4)); libera(create_node($6)); };
while: TK_PR_WHILE '(' expr ')' TK_PR_DO block			{ implicit_conversion(TYPE_BOOL, $3); $$ = binary_node($1, $3, $6); while_do($3, $6, $$); };

/****** ARITHMETIC AND LOGICAL EXPRESSIONS ******/
literal: TK_LIT_INT		{ $$ = create_node($1); $$->type = TYPE_INT; }
| 	 TK_LIT_FLOAT		{ $$ = create_node($1); $$->type = TYPE_FLOAT; }
|	 TK_LIT_FALSE		{ $$ = create_node($1); $$->type = TYPE_BOOL; }
|	 TK_LIT_TRUE		{ $$ = create_node($1); $$->type = TYPE_BOOL; }
|	 TK_LIT_CHAR		{ $$ = create_node($1); $$->type = TYPE_CHAR; }
|	 TK_LIT_STRING		{ $$ = create_node($1); $$->type = TYPE_STRING; };

directTerm: id 			{ $$ = $1; load(scope, $$); }
| 	    literal		{ $$ = $1; load(scope, $$); };

term: directTerm		{ $$ = $1; }
|     call			{ $$ = $1; };

expr: term				       { $$ = $1; }
|     '+' expr			%prec UPLUS    { $$ = unary_node($1, $2); $$->type = $2->type; check_type(ARITH_OP, $$); }
|     '-' expr			%prec UMINUS   { $$ = unary_node($1, $2); $$->type = $2->type; check_type(ARITH_OP, $$); }
|     '!' expr				       { $$ = unary_node($1, $2); $$->type = $2->type; check_type(BOOL_OP, $$); not($2, $$); }
|     '&' expr			%prec UADDRESS { $$ = unary_node($1, $2); /* TODO: pointer type? */ }
|     '*' expr			%prec UPOINTER { $$ = unary_node($1, $2); /* TODO: type of the pointer? */ }
|     '?' expr				       { $$ = unary_node($1, $2); /* TODO: what type? */ }
|     '#' expr				       { $$ = unary_node($1, $2); /* TODO: what type? */ }
|     expr '+' expr		{ $$ = binary_node($2, $1, $3); $$->type = infer_type($1, $3); check_type(ARITH_OP, $$); numeric("add", $1, $3, $$); }
|     expr '-' expr		{ $$ = binary_node($2, $1, $3); $$->type = infer_type($1, $3); check_type(ARITH_OP, $$); numeric("sub", $1, $3, $$); }
|     expr '*' expr		{ $$ = binary_node($2, $1, $3); $$->type = infer_type($1, $3); check_type(ARITH_OP, $$); numeric("mult", $1, $3, $$); }
|     expr '/' expr		{ $$ = binary_node($2, $1, $3); $$->type = infer_type($1, $3); check_type(ARITH_OP, $$); numeric("div", $1, $3, $$); }
|     expr '%' expr		{ $$ = binary_node($2, $1, $3); $$->type = infer_type($1, $3); check_type(ARITH_OP, $$); }
|     expr '|' expr		{ $$ = binary_node($2, $1, $3); $$->type = infer_type($1, $3); check_type(ARITH_OP, $$); }
|     expr '&' expr		{ $$ = binary_node($2, $1, $3); $$->type = infer_type($1, $3); check_type(ARITH_OP, $$); }
|     expr '^' expr		{ $$ = binary_node($2, $1, $3); $$->type = infer_type($1, $3); check_type(ARITH_OP, $$); }
|     expr '>' expr		{ $$ = binary_node($2, $1, $3); $$->type = infer_type($1, $3); check_type(BOOL_OP, $$); cmp("cmp_GT", $1, $3, $$); }
|     expr '<' expr		{ $$ = binary_node($2, $1, $3); $$->type = infer_type($1, $3); check_type(BOOL_OP, $$); cmp("cmp_LT", $1, $3, $$); }
|     expr TK_OC_LE expr	{ $$ = binary_node($2, $1, $3); $$->type = infer_type($1, $3); check_type(BOOL_OP, $$); cmp("cmp_LE", $1, $3, $$); }
|     expr TK_OC_GE expr	{ $$ = binary_node($2, $1, $3); $$->type = infer_type($1, $3); check_type(BOOL_OP, $$); cmp("cmp_GE", $1, $3, $$); }
|     expr TK_OC_EQ expr	{ $$ = binary_node($2, $1, $3); $$->type = infer_type($1, $3); check_type(BOOL_OP, $$); cmp("cmp_EQ", $1, $3, $$); }
|     expr TK_OC_NE expr	{ $$ = binary_node($2, $1, $3); $$->type = infer_type($1, $3); check_type(BOOL_OP, $$); cmp("cmp_NE", $1, $3, $$); }
|     expr TK_OC_AND expr	{ $$ = binary_node($2, $1, $3); $$->type = infer_type($1, $3); check_type(BOOL_OP, $$); and($1, $3, $$); }
|     expr TK_OC_OR expr	{ $$ = binary_node($2, $1, $3); $$->type = infer_type($1, $3); check_type(BOOL_OP, $$); or($1, $3, $$); }
|     expr '?' expr ':' expr	{ $$ = ternary_node($4, $1, $3, $5); libera(create_node($2));  check_type(BOOL_OP, $1); }
|     '(' expr ')'		{ $$ = $2; };

%%

void yyerror (char const *str){
    fprintf(stderr, "%s on line %d\n", str, yylineno);
}