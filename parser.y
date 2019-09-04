%{
	#include <stdio.h>
	#include <stdlib.h>

	extern int yylineno;

	int yylex(void);
	void yyerror (char const *s);
%}

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

prog: expr

/****** SIMPLE COMMANDS ******/
/** Local variable declaration **/


/****** ARITHMETIC AND LOGICAL EXPRESSIONS ******/
literal: TK_LIT_INT
| 	 TK_LIT_FLOAT
|	 TK_LIT_FALSE
|	 TK_LIT_TRUE
|	 TK_LIT_CHAR
|	 TK_LIT_STRING;

args: expr | expr ',' args | ;
call: TK_IDENTIFICADOR '(' args ')';

indexer: '[' expr ']' | ;
id: TK_IDENTIFICADOR indexer;

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