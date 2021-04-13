#ifndef SYMBOL_H
#define SYMBOL_H

#include "type.h"

typedef enum {
	SYMBOL_LOCAL,
	SYMBOL_PARAM,
	SYMBOL_GLOBAL
} symbol_t;

struct symbol {
	symbol_t kind;
	struct type *type;
	char *name;
	int which;

	// used only for functions. This can be accessed from the hash table. When a function is looked up,
	// if it is found, it may ONLY be allowed if it is an implementation of the same function name that
	// has NOT already had an implementation (funcImp == 0). All second instances of prototypes 
	// should result in error.
	int funcImp;
};

struct symbol * symbol_create( symbol_t kind, struct type *type, char *name );

#endif
