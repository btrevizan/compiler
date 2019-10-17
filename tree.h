#ifndef COMPILER_TREE_H
#define COMPILER_TREE_H

#include "lexical.h"

extern void *arvore;

typedef struct node {
    int index;
    struct node* parent;
    struct lexeme* value;
    struct node** children;
    int n_children;
} Node;

void add_node(Node* parent, Node* child);
void add_lexeme(Node* parent, Lexeme* value);  // alias for add_node(parent, create_node(lexeme));

Node* create_node(Lexeme* value);
Node* unary_node(Lexeme* value, Node* node);
Node* binary_node(Lexeme* value, Node* node, Node* node2);
Node* ternary_node(Lexeme* value, Node* node, Node* node2, Node* node3);
Node* quaternary_node(Lexeme* value, Node* node, Node* node2, Node* node3, Node* node4);
Node* quinary_node(Lexeme* value, Node* node, Node* node2, Node* node3, Node* node4, Node* node5);

void libera(void *arvore);
void exporta(void *arvore);

void print_node_value(Lexeme* value);

#endif //COMPILER_TREE_H
