#ifndef SCOPE_H
#define SCOPE_H

#include <stdlib.h>
#include <stdio.h>
#include "hash_table.h"
#include "symbol.h"

struct stack {
    struct hash_table* thisTable;       // the hash table pointed to by this node in the stack
    struct hash_table* nextTable;       // the next hash table pointed to by this node inthe stack
};

void scope_enter();
void scope_exit();
int scope_level();
void scope_bind(const char* name, struct symbol* sym);
struct symbol* scope_lookup(const char* name);
struct symbol* scope_lookup_current(const char* name);

#endif