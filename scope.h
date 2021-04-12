#ifndef SCOPE_H
#define SCOPE_H

#include <stdlib.h>
#include <stdio.h>
#include "hash_table.h"
#include "symbol.h"

struct stack_node {
    symbol_t thisScope;             // stores the scope of this node in the stack, either local, global, or param
    struct hash_table* table;       // the hash table pointed to by this node in the stack
    struct stack_node* next;             // the next stack node pointed to by this node in the stack
};

void scope_enter();
void scope_exit();
int scope_level();
void scope_bind(const char* name, struct symbol* sym);
struct symbol* scope_lookup(const char* name);
struct symbol* scope_lookup_current(const char* name);

#endif