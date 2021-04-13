#ifndef TYPE_H
#define TYPE_H

#include "param_list.h"
#include <stdbool.h>

typedef enum {
	TYPE_INTEGER,
	TYPE_STRING,
	TYPE_CHAR,
	TYPE_BOOLEAN,
	TYPE_VOID,
	TYPE_ARRAY,
	TYPE_FUNCTION,
	TYPE_PROTOTYPE
} type_t;

struct type {
	type_t kind;
	struct param_list *params;
	struct type *subtype;
	int size;
};

struct type * type_create( type_t kind, struct type *subtype, struct param_list *params, int size );
void type_print( struct type *t );
struct type* type_copy(struct type* t);

bool type_compare(struct type* t1, struct type* t2);

#endif
