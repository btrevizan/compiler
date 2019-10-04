/* Definitions */
%{
    #include "parser.tab.h"
    #include "lexical.h"
    #include <string.h>
%}

%option yylineno

%x comment

SPECIAL_CHAR        [\,\;\:\(\)\[\]\{\}\+\-\|\?\@\*\/\<\>\=\!\&\%\#\^\.\$\~\`]

DIGIT               [0-9]
INT                 {DIGIT}+
FLOAT               {INT}\.{DIGIT}+((E|e){INT})?
CHAR                \'([^']|\\\')\'
STRING              \"([^"]|\\\")*\"

%%

    /* Rules */

    /* Reserved words */
int                 { return TK_PR_INT; }
float               { return TK_PR_FLOAT; }
bool                { return TK_PR_BOOL; }
char                { return TK_PR_CHAR; }
string              { return TK_PR_STRING; }

if                  { set_as_pr(); return TK_PR_IF; }
then                { return TK_PR_THEN; }
else                { return TK_PR_ELSE; }

while               { set_as_pr(); return TK_PR_WHILE; }
do                  { return TK_PR_DO; }

input               { return TK_PR_INPUT; }
output              { return TK_PR_OUTPUT; }

return              { set_as_pr(); return TK_PR_RETURN; }

const               { return TK_PR_CONST; }
static              { return TK_PR_STATIC; }

foreach             { return TK_PR_FOREACH; }
for                 { set_as_pr(); return TK_PR_FOR; }

switch              { return TK_PR_SWITCH; }
case                { return TK_PR_CASE; }
break               { set_as_pr(); return TK_PR_BREAK; }

continue            { set_as_pr(); return TK_PR_CONTINUE; }

class               { return TK_PR_CLASS; }
private             { return TK_PR_PRIVATE; }
public              { return TK_PR_PUBLIC; }
protected           { return TK_PR_PROTECTED; }

end                 { return TK_PR_END; }
default             { return TK_PR_DEFAULT; }

true    			{ set_as_true(); return TK_LIT_TRUE; }
false   			{ set_as_false(); return TK_LIT_FALSE; }

    /* Special characters */
[\,\;\(\)\[\]\{\}\@\.\$\~\`]  { return (int) yytext[0]; }
[\:\+\-\|\?\*\/\<\>\=\!\&\%\#\^]  { set_as_sc(); return (int) yytext[0]; }

    /* Identifiers */
[a-zA-Z_][a-zA-Z_0-9]*      { set_as_id(); return TK_IDENTIFICADOR; }
({INT}|{FLOAT})+[a-zA-Z_]+  { return TOKEN_ERRO; }          /* ID begins with a number */

    /* Two-or-more-char operatores */
"<="                { set_as_oc(); return TK_OC_LE;  }
">="                { set_as_oc(); return TK_OC_GE;  }
"=="                { set_as_oc(); return TK_OC_EQ;  }
"!="                { set_as_oc(); return TK_OC_NE;  }
"&&"                { set_as_oc(); return TK_OC_AND; }
"||"                { set_as_oc(); return TK_OC_OR;  }
">>"                { set_as_oc(); return TK_OC_SR;  }
"<<"                { set_as_oc(); return TK_OC_SL;  }
"%>%"               { set_as_oc(); return TK_OC_FORWARD_PIPE; }
"%|%"               { set_as_oc(); return TK_OC_BASH_PIPE; }

    /* Literals */
{INT}   			{ set_as_int(); return TK_LIT_INT; }
{FLOAT}				{ set_as_float(); return TK_LIT_FLOAT; }
{CHAR}  			{ set_as_char(); return TK_LIT_CHAR; }
{STRING}			{ set_as_string(); return TK_LIT_STRING; }

    /* Comments */
"//".*                  { ; }

    /* code from http://dinosaur.compilertools.net/flex/manpage.html */
"/*"		            { BEGIN(comment); }
<comment>[^*\n]*	    { ; }                       /* eat anything that's not a '*' */
<comment>"*"+[^*/\n]*   { ; }                       /* eat up '*'s not followed by '/'s */
<comment>\n		        { ; }                       /* eat new lines */
<comment>"*"+"/"	    { BEGIN(INITIAL); }

    /* To ignore */
[\n]                { ; }  /* new line */
[ ]                 { ; }  /* whitespaces */
[\t]+               { ; }  /* tabs */

.                   { return TOKEN_ERRO; }

%%

/* User code */
int get_line_number (void) {
    /*
    yylineno
          directs flex to generate a scanner  that	maintains
          the  number of the current line read from its input
          in the global variable yylineno.	 This  option  is
          implied by %option lex-compat.

    From http://dinosaur.compilertools.net/flex/manpage.html
    */
    return yylineno;
}

void init_lexeme() {
    yylval.lexical_value = (Lexeme*) malloc(sizeof(Lexeme));
    yylval.lexical_value->line_number = get_line_number();
}

void set_as_pr() {
    init_lexeme();
    yylval.lexical_value->token_type = TK_PR;
    yylval.lexical_value->literal_type = LT_NAL;
    yylval.lexical_value->token_value.string = strdup(yytext);
}

void set_as_sc() {
    init_lexeme();
    yylval.lexical_value->token_type = TK_SC;
    yylval.lexical_value->literal_type = LT_NAL;
    yylval.lexical_value->token_value.character = yytext[0];
}

void set_as_oc() {
    init_lexeme();
    yylval.lexical_value->token_type = TK_OC;
    yylval.lexical_value->literal_type = LT_NAL;
    yylval.lexical_value->token_value.string = strdup(yytext);
}

void set_as_id() {
    init_lexeme();
    yylval.lexical_value->token_type = TK_ID;
    yylval.lexical_value->literal_type = LT_NAL;
    yylval.lexical_value->token_value.string = strdup(yytext);
}

void set_as_int() {
    init_lexeme();
    yylval.lexical_value->token_type = TK_LT;
    yylval.lexical_value->literal_type = LT_INT;
    yylval.lexical_value->token_value.integer = atoi(yytext);
}

void set_as_float() {
    init_lexeme();
    yylval.lexical_value->token_type = TK_LT;
    yylval.lexical_value->literal_type = LT_FLOAT;
    yylval.lexical_value->token_value.real = atof(yytext);
}

void set_as_true() {
    init_lexeme();
    yylval.lexical_value->token_type = TK_LT;
    yylval.lexical_value->literal_type = LT_BOOL;
    yylval.lexical_value->token_value.integer = 1;
}

void set_as_false() {
    init_lexeme();
    yylval.lexical_value->token_type = TK_LT;
    yylval.lexical_value->literal_type = LT_BOOL;
    yylval.lexical_value->token_value.integer = 0;
}

void set_as_char() {
    init_lexeme();
    yylval.lexical_value->token_type = TK_LT;
    yylval.lexical_value->literal_type = LT_CHAR;
    yylval.lexical_value->token_value.character = yytext[1];
}

void set_as_string() {
    init_lexeme();
    yylval.lexical_value->token_type = TK_LT;
    yylval.lexical_value->literal_type = LT_STRING;

    yytext[strlen(yytext) - 1] = '\0';
    yylval.lexical_value->token_value.string = strdup(yytext + 1);
}
