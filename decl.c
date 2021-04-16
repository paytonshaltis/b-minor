#include "decl.h"
#include "expr.h"
#include "type.h"
#include "scope.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int totalResErrors = 0;
int totalTypeErrors = 0;

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
    
    // for declarations of type other than function and prototype
    if(d->type->kind != TYPE_FUNCTION && d->type->kind != TYPE_PROTOTYPE) {

        // if there is an initial value
        if(d->value != NULL){
            printf(" = ");
            expr_print(d->value);
        }
        printf(";\n");
    }

    // for declarations of type function and prototype
    if(d->type->kind == TYPE_FUNCTION || d->type->kind == TYPE_PROTOTYPE) {
        
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

    
    // first we must distinguish function implementation from others since they are handled differently

    // if the declaration is NOT of type 'function'
    if(d->type->kind != TYPE_FUNCTION) {
        
        // if the identifier is found in the symbol table, emit an error (redeclaration of non function)
        if(scope_lookup_current(d->name) != NULL) {
            printf("resolution error: identifier \"%s\" already in current scope in the symbol table\n", d->name);
            totalResErrors++;
        }
        
        // if the identifier is NOT found in the symbol table, add it to the table
        else {
            scope_bind(d->name, d->symbol);
            printf("added identifier \"%s\" to the symbol table\n", d->name);
        }
    }

    // flag used later to determine if parameters and code should be resolved
    bool resolveParamCode = false;

    // if the declaration is of type 'function'
    if(d->type->kind == TYPE_FUNCTION) {
        
        struct symbol* symCheck = scope_lookup_current(d->name);

        /* CASE (1): if it is in the symbol table and if the symbol in the table is of type TYPE_PROTOTYPE, we can update it with implementation */
        if(symCheck != NULL && symCheck->type->kind == TYPE_PROTOTYPE) {

            // if the parameters match between prototype and implementation
            if(param_list_compare(d->type->params, symCheck->type->params)) {
                
                // set the flag to true so we know to resolve params and code
                resolveParamCode = true;

                // unbind the prototype from the symbol table
                scope_unbind(d->name);
                
                // change the kind of the declaration to TYPE_FUNCTION
                d->symbol->type->kind = TYPE_FUNCTION;

                // rebind the key and symbol structure to the symbol table
                scope_bind(d->name, d->symbol);

                // print message to identify function update
                printf("function prototype \"%s\" updated in symbol table with implementation\n", d->name);
            }
            
            // if the parameters do not match
            else {
                printf("resolution error: function implementation for \"%s\" does not match prototype's parameters\n", d->name);
                totalResErrors++;
            }
        }

        /* CASE (2): if it is in the symbol table and if the symbol in the table is of any type other than TYPE_PROTOTYPE, emit error */
        else if(symCheck != NULL && symCheck->type->kind != TYPE_PROTOTYPE) {
            
            // will reach here if the same name is declared as anything other than a function prototype
            printf("resolution error: The function \"%s\" cannot be implemented, as identifier \"%s\" already exists\n", d->name, d->name);
            totalResErrors++;
        }
    
        /* CASE (3): if it is not in the symbol table, we add it as a type TYPE_FUNCTION */
        else {
            
            // set the flag to true so we know to resolve params and code
            resolveParamCode = true;

            // bind the name and symbol to the symbol table
            scope_bind(d->name, d->symbol);
            printf("added identifier \"%s\" to the symbol table\n", d->name);
        }
    }

    // if the declaration is a function prototype or implementation (with matching prototypes), resolve parameters and code (if applicapble)
    if((d->type->kind == TYPE_FUNCTION && resolveParamCode) || d->type->kind == TYPE_PROTOTYPE) {
        scope_enter();
        param_list_resolve(d->type->params);
        stmt_resolve(d->code);
        scope_exit();
    }

    // resolve the next declaration in the code
    decl_resolve(d->next);

    return;

}