#include "tree.h"
#include "lexical.h"
#include "table.h"
#include "errors.h"
#include "stack.h"
#include "checks.h"
#include "code.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void add_node(Node* parent, Node* child) {
    if(child == NULL) return;

    parent->n_children = parent->n_children + 1;

    if(parent->n_children == 1) parent->children = (Node**) malloc(sizeof(Node*));
    else parent->children = (Node**) realloc(parent->children, sizeof(Node*) * parent->n_children);

    int index = parent->n_children - 1;
    parent->children[index] = child;

    child->index = index;
    child->parent = parent;
}

void add_lexeme(Node* parent, Lexeme* value) {  // alias for add_node(parent, create_node(lexeme));
    Node* child = create_node(value);
    add_node(parent, child);
}

Node* create_node(Lexeme* value) {
    Node* node = (Node*) malloc(sizeof(Node));
    node->index = -1;
    node->type = TYPE_NAN;
    node->coercion = NO_CONVERSION;
    node->codelist = NULL;
    node->temp = NULL;
    node->truelist = NULL;
    node->falselist = NULL;
    node->parent = NULL;
    node->value = value;
    node->n_children = 0;
    node->children = NULL;

    if(value != NULL && value->token_type == TK_ID) {
        check_declaration(scope, node);
        node->type = search(scope, value->token_value.string)->type;
    }

    return node;
}

Node* unary_node(Lexeme* value, Node* node) {
    Node* parent = create_node(value);
    add_node(parent, node);
    return parent;
}

Node* binary_node(Lexeme* value, Node* node, Node* node2) {
    Node* parent = create_node(value);
    add_node(parent, node);
    add_node(parent, node2);
    return parent;
}

Node* ternary_node(Lexeme* value, Node* node, Node* node2, Node* node3) {
    Node* parent = create_node(value);
    add_node(parent, node);
    add_node(parent, node2);
    add_node(parent, node3);
    return parent;
}

Node* quaternary_node(Lexeme* value, Node* node, Node* node2, Node* node3, Node* node4) {
    Node* parent = create_node(value);
    add_node(parent, node);
    add_node(parent, node2);
    add_node(parent, node3);
    add_node(parent, node4);
    return parent;
}

Node* quinary_node(Lexeme* value, Node* node, Node* node2, Node* node3, Node* node4, Node* node5) {
    Node* parent = create_node(value);
    add_node(parent, node);
    add_node(parent, node2);
    add_node(parent, node3);
    add_node(parent, node4);
    add_node(parent, node5);
    return parent;
}

void libera(void *arvore) {
    if(arvore == NULL) return;

    Node* node = arvore;

    for(int i = node->n_children - 1; i >= 0; i--)
        libera(node->children[i]);

    destroy_code_list(node->codelist);
    delete_lexeme(node->value);

    free(node->children);
    free(node->temp);
    free(node->truelist);
    free(node->falselist);
    free(node);
}

void export_node(Node* node, FILE* file) {
    if(node == NULL) return;

    for(int i = 0; i < node->n_children; i++) {
        fprintf(file, "%p, %p\n", node, node->children[i]);
        export_node(node->children[i], file);
    }
}

void exporta(void *arvore) {
    if(arvore == NULL) return;
    Node* node = (Node*) arvore;

    FILE* file = fopen("e3.csv", "w");

    if(node->n_children == 0) {
        fprintf(file, "%p,\n", node);
    } else {
        export_node(node, file);
    }

    fclose(file);
}

//void print_node_value(Lexeme* value) {
//    if(value == NULL) {
//        printf("node is null\n");
//        return;
//    }
//
//    switch (value->literal_type) {
//        case LT_NAL:
//        case LT_STRING:
//            printf("token value: %s\n", value->token_value);
//            break;
//
//        case LT_BOOL:
//            printf("token value (bool): %d\n", value->token_value);
//            break;
//
//        case LT_CHAR:
//            printf("token value: %c\n", value->token_value);
//            break;
//
//        case LT_FLOAT:
//            printf("token value: %f\n", value->token_value);
//            break;
//
//        case LT_INT:
//            printf("token value: %d\n", value->token_value);
//            break;
//    }
//}

int infer_type(Node* node1, Node* node2) {
    // Same
    if(node1->type == TYPE_INT && node2->type == TYPE_INT) return TYPE_INT;
    if(node1->type == TYPE_FLOAT && node2->type == TYPE_FLOAT) return TYPE_FLOAT;
    if(node1->type == TYPE_BOOL && node2->type == TYPE_BOOL) return TYPE_BOOL;

    // Float and int
    if(node1->type == TYPE_FLOAT && node2->type == TYPE_INT) return TYPE_FLOAT;
    if(node1->type == TYPE_INT && node2->type == TYPE_FLOAT) return TYPE_FLOAT;

    // Bool and int
    if(node1->type == TYPE_BOOL && node2->type == TYPE_INT) return TYPE_INT;
    if(node1->type == TYPE_INT && node2->type == TYPE_BOOL) return TYPE_INT;

    // Bool and float
    if(node1->type == TYPE_BOOL && node2->type == TYPE_FLOAT) return TYPE_FLOAT;
    if(node1->type == TYPE_FLOAT && node2->type == TYPE_BOOL) return TYPE_FLOAT;

    implicit_conversion(node1->type, node2);

    return node2->type;
}