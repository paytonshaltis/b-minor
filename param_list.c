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

// used to compare parameter lists for determining whether function implementation matches prototype
bool param_list_compare(struct param_list* p1, struct param_list* p2) {

    // create copies of the param_list head pointers
    struct param_list* p1copy = p1;
    struct param_list* p2copy = p2;

    // traverse the parameter lists, making sure that parameters match up
    while(p1copy != NULL && p2copy != NULL) {

        // if the name and type matches
        if((p1copy->name == p2copy->name) && type_compare(p1copy->type, p2copy->type)) {

            // move to the next parameter
            p1copy = p1copy->next;
            p2copy = p2copy->next;
        }
        
        // if the name and type does not match
        else {
            return false;
        }
    }

    // when one parameter list concludes, make sure the other has too
    if(p1copy == NULL && p2copy == NULL) {
        return true;
    }
    // otherwise, the two lists differ in size, return false;
    return false;

}
