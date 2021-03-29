#include "decl.h"
#include "expr.h"
#include "type.h"
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
    type_print(d->type);
    
    //for declarations of type integer...
    if(d->type->kind == TYPE_INTEGER) {

        if(d->value != NULL){
            printf(" = ");
            //expr_print(d->value);
            printf("expr");
        }
        printf(";\n");
    }

    //for declarations of type string...
    if(d->type->kind == TYPE_STRING) {

        if(d->value != NULL){
            printf(" = ");
            //expr_print(d->value);
            printf("\"%s\"", d->value->string_literal);
        }
        printf(";\n");
    }

    //for declarations of type char...
    if(d->type->kind == TYPE_CHAR) {

        if(d->value != NULL){
            printf(" = ");
            if(d->value->literal_value == 10){
                printf("\'\\n\'");
            }
            else if(d->value->literal_value == 0){
                printf("\'\\0\'");
            }
            else {
                printf("\'%c\'", d->value->literal_value);
            }
            //expr_print(d->value);
            
        }
        printf(";\n");
    }

    //for declarations of type boolean...
    if(d->type->kind == TYPE_BOOLEAN) {

        if(d->value != NULL){
            printf(" = ");
            //expr_print(d->value);
            printf("expr");
        }
        printf(";\n");
    }
    
    //for declarations of type array...
    if(d->type->kind == TYPE_ARRAY) {
        
        if(d->value != NULL) {
            printf(" = ");
            //expr_print(d->value);
            printf("expr");
        }
        printf(";\n");
    }

    if(d->type->kind == TYPE_FUNCTION) {
        printf(" INCOMPLETE");
    }

    //prints out the next declaration with same indent
    if(d->next != NULL) {
        decl_print(d->next, 0);
    }
}