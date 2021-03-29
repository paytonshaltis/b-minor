#include <stdio.h>
#include <stdlib.h>
#include "type.h"

struct type * type_create( type_t kind, struct type *subtype, struct param_list *params, int size ) {
    struct type *t = malloc(sizeof(*t));
    t->kind = kind;
    t->subtype = subtype;
    t->params = params;
    t->size = size;
    return t;
}