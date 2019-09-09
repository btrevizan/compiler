%{
	#include <stdio.h>
	#include <stdlib.h>

	extern int yylineno;

	int yylex(void);
	void yyerror (char const *s);
%}

%define parse.error verbose

%token TK_PR_INT
%token TK_PR_FLOAT
%token TK_PR_BOOL
%token TK_PR_CHAR
%token TK_PR_STRING
%token TK_PR_IF
%token TK_PR_THEN
%token TK_PR_ELSE
%token TK_PR_WHILE
%token TK_PR_DO
%token TK_PR_INPUT
%token TK_PR_OUTPUT
%token TK_PR_RETURN
%token TK_PR_CONST
%token TK_PR_STATIC
%token TK_PR_FOREACH
%token TK_PR_FOR
%token TK_PR_SWITCH
%token TK_PR_CASE
%token TK_PR_BREAK
%token TK_PR_CONTINUE
%token TK_PR_CLASS
%token TK_PR_PRIVATE
%token TK_PR_PUBLIC
%token TK_PR_PROTECTED
%token TK_PR_END
%token TK_PR_DEFAULT
%token TK_OC_LE
%token TK_OC_GE
%token TK_OC_EQ
%token TK_OC_NE
%token TK_OC_AND
%token TK_OC_OR
%token TK_OC_SL
%token TK_OC_SR
%token TK_OC_FORWARD_PIPE
%token TK_OC_BASH_PIPE
%token TK_LIT_INT
%token TK_LIT_FLOAT
%token TK_LIT_FALSE
%token TK_LIT_TRUE
%token TK_LIT_CHAR
%token TK_LIT_STRING
%token TK_IDENTIFICADOR
%token TOKEN_ERRO

%start prog

%left '+' '-' '*' '/' '%' '|' '&' '^' '>' '<' '?' '!' '#' ':' TK_OC_LE TK_OC_GE TK_OC_EQ TK_OC_NE TK_OC_AND TK_OC_OR

%%

prog: function prog | global_var prog | ;

/** GLOBAL VAR DECLARATION **/
global_var: TK_PR_STATIC type id ';' | type id ';' ;

/** FUNCTION **/
function: TK_PR_STATIC type TK_IDENTIFICADOR '(' ')' block
| 	  type TK_IDENTIFICADOR '(' ')' block
| 	  TK_PR_STATIC type TK_IDENTIFICADOR '(' list_of_params ')' block
| 	  type TK_IDENTIFICADOR '(' list_of_params ')' block;

params: TK_PR_CONST type TK_IDENTIFICADOR | type TK_IDENTIFICADOR;
list_of_params: params | params ',' list_of_params; 

/****** SIMPLE COMMANDS ******/
simple_command: local_var
|		assignment
|		input
|		output
|		shift
|		return
|		if_else
|		for
|		while
|		TK_PR_BREAK
|		TK_PR_CONTINUE;

command_list: simple_command ';' | simple_command ';' command_list;
block: '{' command_list '}' | '{' '}';

/** Local variable declaration **/
type: TK_PR_INT
|     TK_PR_FLOAT
|     TK_PR_BOOL
|     TK_PR_CHAR
|     TK_PR_STRING;

initialization: TK_OC_LE directTerm;

local_var: TK_PR_STATIC TK_PR_CONST type TK_IDENTIFICADOR initialization
| 	   TK_PR_STATIC TK_PR_CONST type TK_IDENTIFICADOR
| 	   TK_PR_STATIC type TK_IDENTIFICADOR initialization
| 	   TK_PR_STATIC type TK_IDENTIFICADOR
|	   TK_PR_CONST type TK_IDENTIFICADOR initialization
|	   TK_PR_CONST type TK_IDENTIFICADOR
|	   type TK_IDENTIFICADOR initialization
|	   type TK_IDENTIFICADOR;

/** Assignment **/
indexer: '[' expr ']' ;
id: TK_IDENTIFICADOR indexer | TK_IDENTIFICADOR;

assignment: id '=' expr;

/** Input and output **/
args: expr | expr ',' args;

input: TK_PR_INPUT expr;
output: TK_PR_OUTPUT args;

/** Function call **/
call: TK_IDENTIFICADOR '(' args ')' | TK_IDENTIFICADOR '(' ')';

/** Shift command **/
shift_op: TK_OC_SL | TK_OC_SR;
shift: id shift_op expr;

/** Flow change commands **/
return: TK_PR_RETURN expr;

/** If-then-else statement **/
if: TK_PR_IF '(' expr ')' block;
else: TK_PR_ELSE block;
if_else: if else | if;

/** Iterative commands **/
for_list_element: local_var | assignment;
for_list: for_list_element | for_list_element ',' for_list;
for: TK_PR_FOR '(' for_list ':' expr ':' for_list ')' block;

while: TK_PR_WHILE '(' expr ')' TK_PR_DO block;

/****** ARITHMETIC AND LOGICAL EXPRESSIONS ******/
literal: TK_LIT_INT
| 	 TK_LIT_FLOAT
|	 TK_LIT_FALSE
|	 TK_LIT_TRUE
|	 TK_LIT_CHAR
|	 TK_LIT_STRING;

directTerm: id | literal;
term: directTerm | call;

expr: term
|     '+' expr
|     '-' expr
|     '!' expr
|     '&' expr
|     '*' expr
|     '?' expr
|     '#' expr
|     expr '+' expr
|     expr '-' expr
|     expr '*' expr
|     expr '/' expr
|     expr '%' expr
|     expr '|' expr
|     expr '&' expr
|     expr '^' expr
|     expr '>' expr
|     expr '<' expr
|     expr TK_OC_LE expr
|     expr TK_OC_GE expr
|     expr TK_OC_EQ expr
|     expr TK_OC_NE expr
|     expr TK_OC_AND expr
|     expr TK_OC_OR expr
|     expr '?' expr ':' expr
|     '(' expr ')';

%%

void yyerror (char const *s) {
	printf("Error on line %d. %s\n", yylineno, s);
}