#ifndef DECL_H
#define DECL_H

#include "type.h"
#include "stmt.h"
#include "expr.h"
#include <stdio.h>

struct decl {
	char *name;
	struct type *type;
	struct expr *value;
	struct stmt *code;
	struct symbol *symbol;
	struct decl *next;
};

struct decl * decl_create( char *name, struct type *type, struct expr *value, struct stmt *code, struct decl *next );
void decl_print( struct decl *d, int indent );

void decl_resolve(struct decl* d);
void decl_typecheck(struct decl* d);
void decl_check_initList(struct type* t, struct expr* initList);
int count_list_elements(struct expr* e, struct type* t);

void decl_codegen(struct decl* d);
int decl_local_count(struct stmt* s);

#endif
