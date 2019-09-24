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

Node* create_node(Lexeme* value);
void add_node(Node* parent, Node* child);
void add_lexeme(Node* parent, Lexeme* value);  // alias for add_node(parent, create_node(lexeme));

void destroy_node(Node* node);
void update_node(Node* node, Node* new_node);

void libera(void *arvore);
void exporta(void *arvore);

#endif //COMPILER_TREE_H
