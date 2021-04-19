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