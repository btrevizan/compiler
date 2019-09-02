/* Definitions */
%{
    #include "parser.tab.h"
%}

%option yylineno

%x comment

SPECIAL_CHAR        [\,\;\:\(\)\[\]\{\}\+\-\|\?\@\*\/\<\>\=\!\&\%\#\^\.\$\~\`]

DIGIT               [0-9]
INT                 (\+|\-)?{DIGIT}+
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

if                  { return TK_PR_IF; }
then                { return TK_PR_THEN; }
else                { return TK_PR_ELSE; }

while               { return TK_PR_WHILE; }
do                  { return TK_PR_DO; }

input               { return TK_PR_INPUT; }
output              { return TK_PR_OUTPUT; }

return              { return TK_PR_RETURN; }

const               { return TK_PR_CONST; }
static              { return TK_PR_STATIC; }

foreach             { return TK_PR_FOREACH; }
for                 { return TK_PR_FOR; }

switch              { return TK_PR_SWITCH; }
case                { return TK_PR_CASE; }
break               { return TK_PR_BREAK; }

continue            { return TK_PR_CONTINUE; }

class               { return TK_PR_CLASS; }
private             { return TK_PR_PRIVATE; }
public              { return TK_PR_PUBLIC; }
protected           { return TK_PR_PROTECTED; }

end                 { return TK_PR_END; }
default             { return TK_PR_DEFAULT; }

true    			{ return TK_LIT_TRUE; }
false   			{ return TK_LIT_FALSE; }

    /* Special characters */
{SPECIAL_CHAR}              { return (int) yytext[0]; }

    /* Identifiers */
[a-zA-Z_][a-zA-Z_0-9]*      { return TK_IDENTIFICADOR; }
({INT}|{FLOAT})+[a-zA-Z_]+  { return TOKEN_ERRO; }          /* ID begins with a number */

    /* Two-or-more-char operatores */
"<="                { return TK_OC_LE;  }
">="                { return TK_OC_GE;  }
"=="                { return TK_OC_EQ;  }
"!="                { return TK_OC_NE;  }
"&&"                { return TK_OC_AND; }
"||"                { return TK_OC_OR;  }
">>"                { return TK_OC_SL;  }
"<<"                { return TK_OC_SR;  }
"%>%"               { return TK_OC_FORWARD_PIPE; }
"%|%"               { return TK_OC_BASH_PIPE; }

    /* Literals */
{INT}   			{ return TK_LIT_INT; }
{FLOAT}				{ return TK_LIT_FLOAT; }
{CHAR}  			{ return TK_LIT_CHAR; }
{STRING}			{ return TK_LIT_STRING; }

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
