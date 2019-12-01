#ifndef COMPILER_ERROR_H
#define COMPILER_ERROR_H

/*
 * Um identificador não declarado é encontrado
 */

#define ERR_UNDECLARED  10
/*
 * Um identificador já declarado é encontrado
 */
#define ERR_DECLARED    11

/*
 * O identificador encontrado deve ser utilizado como uma variável, em
 * situações onde este é encontrade sendo usado como função ou como vetor,
 * ou algum outro cenário semelhante.
 */
#define ERR_VARIABLE    20

/*
 * O identificador encontrado deve ser utilizado como um vetor, em
 * situações onde este é encontrado sendo usado como variável ou função,
 * ou algum outro cenário semelhante.
 */
#define ERR_VECTOR      21

/*
 * O identificador encontrado deve ser utilizado como uma função, em
 * situações onde este é encontrado sendo usado como variável ou vetor,
 * ou algum outro cenário semelhante.
 */
#define ERR_FUNCTION    22

/*
 * Os tipos (em atribuições e outras cenários) são incompatíveis.
 */
#define ERR_WRONG_TYPE  30

/*
 * Coerção impossível da variável do tipo string para outro tipo qualquer.
 */
#define ERR_STRING_TO_X 31

/*
 * Coerção impossível da variável do tipo char para outro tipo qualquer.
 */
#define ERR_CHAR_TO_X   32

/*
 * Faltam argumentos em uma chamada de função.
 */
#define ERR_MISSING_ARGS    40

/*
 * Sobram argumentos em uma chamada de função.
 */
#define ERR_EXCESS_ARGS     41

/*
 * Os parâmetros reais são de tipos incompatíveis com os correspondentes formais.
 */
#define ERR_WRONG_TYPE_ARGS 42 //argumentos incompatíveis

/*
 * No caso específico do comando input, o parâmetro não é identificador.
 */
#define ERR_WRONG_PAR_INPUT  50 //parâmetro não é identificador

/*
 * No caso específico do comando output, os parâmetros não são literal string ou expressão
 */
#define ERR_WRONG_PAR_OUTPUT 51

/*
 * No caso específico do comando return, o parâmetro não é compatível com tipo do retorno
 */
#define ERR_WRONG_PAR_RETURN 52

/*
 * No caso especíco da geração de código, se a função main não estiver definida
 */
#define MAIN_NOT_FOUND_ERROR 53

#endif //COMPILER_ERROR_H
