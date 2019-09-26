#include "tree.h"
#include "lexical.h"
#include <stdlib.h>

Node* create_node(Lexeme* value) {
    Node* node = (Node*) malloc(sizeof(Node));
    node->index = -1;
    node->parent = NULL;
    node->value = value;
    node->n_children = 0;
    node->children = NULL;

    return node;
}

void add_node(Node* parent, Node* child) {
    parent->n_children = parent->n_children + 1;

    if(parent->n_children == 1) parent->children = (Node**) malloc(sizeof(Node*));
    else parent->children = (Node**) realloc(parent->children, sizeof(Node*) * parent->n_children);

    int index = parent->n_children - 1;
    parent->children[index] = child;

    child->index = index;
    child->parent = parent;

    arvore = parent;
}

void add_lexeme(Node* parent, Lexeme* value) {  // alias for add_node(parent, create_node(lexeme));
    Node* child = create_node(value);
    add_node(parent, child);
}

void unlink_child(Node* parent, const int index) {
    for(int i = index; i < parent->n_children - 1; i++)
        parent->children[index] = parent->children[index + 1];

    parent->n_children = parent->n_children - 1;
    parent->children = (Node**) realloc(parent->children, sizeof(Node*) * parent->n_children);
}

void unlink_node(Node* node) {
    unlink_child(node->parent, node->index);
}

void destroy_value(Lexeme* lexeme) {
    switch (lexeme->token_type) {
        case TK_PR:
        case TK_OC:
        case TK_ID:
        case TK_LT:
            if(lexeme->literal_type == LT_STRING || lexeme->literal_type == LT_NAL) {
                free(lexeme->token_value.string);
                lexeme->token_value.string = NULL;
            }
    }

    free(lexeme);
    lexeme = NULL;
}

void destroy_node(Node* node) {
    unlink_node(node);

    for(int i = 0; i < node->n_children; i++)
        destroy_node(node->children[i]);

    free(node->children);
    node->children = NULL;

    destroy_value(node->value);
    node->value = NULL;

    free(node);
    node = NULL;
}

void update_node(Node* node, Node* new_node) {
    if(new_node->parent != NULL) return;

    add_node(node->parent, new_node);
    destroy_node(node);
}

void libera(void *arvore) {
    destroy_node((Node*) arvore);
}

//void exporta(void *arvore);