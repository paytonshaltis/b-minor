#include <stdio.h>
#include <stdlib.h>
#include "param_list.h"
#include "scope.h"

extern int totalResErrors;

// basic factory function for creating a 'param_list' struct
struct param_list * param_list_create( char *name, struct type *type, struct param_list *next ) {
    struct param_list *p = malloc(sizeof(*p));
    p->name = name;
    p->type = type;                             
    p->next = next;
    return p;
}

// printing function for use by the pretty printer
void param_list_print(struct param_list *a) {
    printf("%s: ", a->name);
    type_print(a->type);
    
    // if there are more parameters to print in the list
    if(a->next != NULL) {
        printf(", ");
        param_list_print(a->next);
    }
}

// conducts name resolution on the paramater list p
void param_list_resolve(struct param_list* p) {

    // base case for recursion
    if(p == NULL) {
        return;
    }

    // we need to begin by resolving the first identifier in the parameter list
    // first we should check and see if it is in the scope (in the function)
    if(scope_lookup_current(p->name) != NULL) {
        printf("resolution error: parameter \"%s\" was already declared\n", p->name);
        totalResErrors++;
    }
    // if the parameter was NOT found in the scope, we need to create a new symbol and enter it into the table
    else {
        p->symbol = symbol_create(SYMBOL_PARAM, p->type, p->name);

        // bind this 'name', 'symbol' pair into the symbol table
        scope_bind(p->name, p->symbol);
        printf("added identifier \"%s\" to the symbol table\n", p->name);
    }

    param_list_resolve(p->next);

    return;
}