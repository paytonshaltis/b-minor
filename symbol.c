#include "symbol.h"
#include "type.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct symbol * symbol_create(symbol_t kind, struct type *type, char *name) {
    
    // the symbol that will be returned from this function
    struct symbol* s = (struct symbol*)malloc(sizeof(*s));

    s->kind = kind;
    s->type = type_copy(type);
    s->name = strdup(name);

    return s;
}

// returns the name that contains the stack address for a symbol
const char* symbol_codegen(struct symbol *s) {

    // if the current scope is global, just return the name
    if(s->kind == SYMBOL_GLOBAL) {
        return s->name;
    }

    // if the current scope is local, use 'which' to compute stack address
    char* result = malloc(sizeof(char) * 16);
    sprintf(result, "[sp, %i]", (s->which + 1) * 4);
    return result;
}