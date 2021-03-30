#include <stdio.h>
#include <stdlib.h>
#include "type.h"

struct type * type_create( type_t kind, struct type *subtype, struct param_list *params, int size ) {
    struct type *t = malloc(sizeof(*t));
    t->kind = kind;
    t->subtype = subtype;
    t->params = params;
    t->size = size;
    return t;
}

void type_print(struct type *t) {
    //for type array...
    if(t->kind == TYPE_ARRAY) {
        if(t->subtype != NULL) {
            if(t->size != 0) {
                printf("array[%i] ", t->size);
                type_print(t->subtype);
            }
            else {
                printf("array[] ");
                type_print(t->subtype);   
            }
        }
    }

    //for basic types...
    if(t->kind == TYPE_INTEGER) {
        printf("integer");
    }
    if(t->kind == TYPE_STRING) {
        printf("string");
    }
    if(t->kind == TYPE_CHAR) {
        printf("char");
    }
    if(t->kind == TYPE_BOOLEAN) {
        printf("boolean");
    }
    if(t->kind == TYPE_VOID) {
        printf("void");
    }

    if(t->kind == TYPE_FUNCTION) {
        printf("function ");
        type_print(t->subtype);
        printf(" (");
        param_list_print(t->params);
        printf(")");
    }

}