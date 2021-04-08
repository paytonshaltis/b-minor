#include <stdio.h>
#include <stdlib.h>
#include "param_list.h"

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