/*
Função principal para realização da E3.

Este arquivo não pode ser modificado.
*/
#include <stdio.h>
#include "parser.tab.h" //arquivo gerado com bison -d parser.y
extern int yylex_destroy(void);

void *arvore = NULL;
void libera (void *arvore);
void exporta (void *arvore);
void gera_codigo (void *arvore);

int main (int argc, char **argv)
{
    int ret = yyparse();
    exporta(arvore);
    libera(arvore);
    arvore = NULL;
    yylex_destroy();
    return ret;
}