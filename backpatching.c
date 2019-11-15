#include "backpatching.h"
#include <string.h>
#include <stdlib.h>

Patch* makelist(char* label) {
    Patch* patch = malloc(sizeof(Patch));

    patch->arg_ptr = label;
    patch->next = NULL;

    return patch;
}

Patch* merge(Patch* p1, Patch* p2) {
    if(p1 == NULL) {
        (*p1) = (*p2);
    } else {
        Patch* aux = p1;
        while(aux->next != NULL) aux = aux->next;
        aux->next = p2;
    }

    return p1;
}

void backpatch(Patch* p, char* label) {
    if(p == NULL) return;

    Patch* aux = p;

    do {
        strcpy(aux->arg_ptr, label);
        aux = aux->next;
    } while(aux != NULL);
}
