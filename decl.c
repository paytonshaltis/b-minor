#include "decl.h"
#include "expr.h"
#include "type.h"
#include "scope.h"
#include <stdio.h>
#include <stdlib.h>

int totalResErrors = 0;

// basic factory function for creating a 'decl' struct
struct decl * decl_create( char *name, struct type *type, struct expr *value, struct stmt *code, struct decl *next ) {
    struct decl *d = malloc(sizeof(*d));
    d->name = name;
    d->type = type;
    d->value = value;
    d->code = code;
    d->next = next;
    return d;
}

// printing function for use by pretty printer
void decl_print(struct decl *d, int indent) {
    
    // for all declarations
    printf("%s: ", d->name);
    type_print(d->type);
    
    // for declarations of type other than function
    if(d->type->kind != TYPE_FUNCTION) {

        // if there is an initial value
        if(d->value != NULL){
            printf(" = ");
            expr_print(d->value);
        }
        printf(";\n");
    }

    // for declarations of type function
    if(d->type->kind == TYPE_FUNCTION) {
        
        // if it is an actual function implementation rather than prototype
        if(d->code != NULL) {
            printf(" = ");
            printf("\n{\n");
            stmt_print(d->code, indent + 1);
            printf("}");
        }
        
        // just the prototype
        else{
            printf(";");
        }
        
        printf("\n");
    }

    // prints out the next declaration with same indent
    if(d->next != NULL) {
        decl_print(d->next, 0);
    }
}

// conducts name resolution on the declaration d
void decl_resolve(struct decl* d) {

    // base case for recursion
    if(d == NULL) {
        return;
    }

    // if there exists more than 1 scope, the kind is local; otherwise, the kind is global
    symbol_t kind = scope_level() > 1 ? SYMBOL_LOCAL : SYMBOL_GLOBAL;

    // creates and links the symbol for d into the d struct; symbol is now linked into the AST
    // this uses the 'kind' determined above, and the 'type' and 'name' from d
    d->symbol = symbol_create(kind, d->type, d->name);

    // next we need to resolve the expression associated with this declaration
    expr_resolve(d->value);

    // then we bind the 'name', 'symbol' pair into the symbol table
    // NOTE that we must first check to see if that symbol is in the current scope...
    // if the identifier is found in the table, and it is not a function, resolution error
    if(scope_lookup_current(d->name) != NULL /*&& d->type->kind != TYPE_FUNCTION*/) {
        printf("RESOLUTION ERROR: Identifier \"%s\" already in current scope in the symbol table!\n", d->name);
        totalResErrors++;
    }
    /*
    // otherwise, if we find a function declaration in this scope, we cannot
    // allow a second prototype to be declared:
    else if(scope_lookup_current(d->name) != NULL && d->type->kind == TYPE_FUNCTION && d->code == NULL) {
        printf("RESOLUTION ERROR: Function \"%s\" already has either prototype or implementation!\n", d->name);
        totalResErrors++;
    }
    */
    else {
        scope_bind(d->name, d->symbol);
    }

    // if the declaration has code (meaning it is a function), resolve params and statements
    if(d->code != NULL) {
        scope_enter();
        param_list_resolve(d->type->params);
        stmt_resolve(d->code);
        scope_exit();
    }

    // resolve the next declaration in the code
    decl_resolve(d->next);

    return;

}