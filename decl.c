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

        if(d->value != NULL){
            //expr_print(d->value);
            printf(" expr");
        }
        printf(";\n");
    }

    //for declarations of type string...
    if(d->type->kind == TYPE_STRING) {
        
        printf("string");

        if(d->value != NULL){
            //expr_print(d->value);
            printf(" \"%s\"", d->value->string_literal);
        }
        printf(";\n");
    }

    //for declarations of type char...
    if(d->type->kind == TYPE_CHAR) {
        
        printf("char");

        if(d->value != NULL){
            //expr_print(d->value);
            printf(" \'%c\'", d->value->literal_value);
        }
        printf(";\n");
    }

    //for declarations of type boolean...
    if(d->type->kind == TYPE_BOOLEAN) {

        printf("boolean");

        if(d->value != NULL){
            //expr_print(d->value);
            printf(" expr");
        }
        printf(";\n");
    }
    
    //prints out the next declaration with same indent
    if(d->next != NULL) {
        decl_print(d->next, 0);
    }
}