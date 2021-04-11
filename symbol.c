#include "symbol.h"
#include <stdlib.h>
#include <stdio.h>

struct symbol * symbol_create(symbol_t kind, struct type *type, char *name) {
    
    // the symbol that will be returned from this function
    struct symbol* s = (struct symbol*)malloc(sizeof(*s));

    s->kind = kind;
    s->type = type;
    s->name = name;

    return s;
}