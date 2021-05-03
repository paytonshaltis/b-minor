#ifndef PARAM_LIST_H
#define PARAM_LIST_H

#include "type.h"
#include <stdio.h>
#include <stdbool.h>

struct expr;

struct param_list {
	char *name;
	struct type *type;
	struct symbol *symbol;
	struct param_list *next;
};

struct param_list * param_list_create( char *name, struct type *type, struct param_list *next );
void param_list_print( struct param_list *a );

void param_list_resolve(struct param_list* p);
bool param_list_compare(struct param_list* p1, struct param_list* p2);
struct param_list* param_list_copy(struct param_list* p);
void param_list_delete(struct param_list* p);
bool param_list_fcall_compare(struct expr* calledArgs, struct param_list* p);

int param_list_count(struct param_list* p, const char* name);

#endif
