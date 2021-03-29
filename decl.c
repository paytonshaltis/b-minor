#include "decl.h"
#include "expr.h"
#include <stdio.h>
#include <stdlib.h>

struct decl * decl_create( char *name, struct type *type, struct expr *value, struct stmt *code, struct decl *next ) {
    struct decl *d = malloc(sizeof(*d));
    d->name = name;
    d->type = type;
    d->value = value;
    d->code = code;
    d->next = next;
    return d;
}

void decl_print( struct decl *d, int indent ) {
    //for all declarations...
    printf("%s : ", d->name);
    
    //for declarations of type integer...
    if(d->type->kind == TYPE_INTEGER) {
        
        printf("integer");

        if(d->value){
            //expr_print(d->value);
        }
        printf(";\n");
    }
}