/*  all code in this file is original, and was written by:
*  
*   PAYTON JAMES SHALTIS
*   COMPLETED MAY 4TH, 2021
*
*			for
*
*	B-MINOR COMPILER, v1.0
*
*
*   in CSC-425: "Compilers and Interpreters" taught by Professor John DeGood,
*   over the course of the Spring 2021 semester. I understand that keeping this
*   code in a public repository may allow other students to have access. In the
*   event that the course is taught again, with a similar project component, this 
*   code is NOT to be used in place of another student's work.
*
*
*
*                                   'scope.h'
*                                   ---------
*   This file defines the functions that will be used as the interface into the symbol
*   table. Each 'scope' structure is a node in the scope stack, each having a hash
*   table that maps [name, symbol] pairs for use in name resolution.
*
*/

#ifndef SCOPE_H
#define SCOPE_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "hash_table.h"
#include "symbol.h"


// stack node structure, represents a single scope in the program
struct stack_node {
    struct hash_table* table;       // the hash table pointed to by this node in the stack
    struct stack_node* next;        // the next stack node pointed to by this node in the stack
};

// the API for managing scopes in a program
void scope_enter();                                     // enters a new scope by pushing a node onto the stack
void scope_exit();                                      // exits a scope by popping a node from the stack
int scope_level();                                      // returns the current level (number of nodes on stack)
void scope_bind(const char* name, struct symbol* sym);  // binds a [name, symbol] pair into the symbol table
struct symbol* scope_lookup(const char* name);          // searches for the first occurrence of a symbol in all scopes
struct symbol* scope_lookup_current(const char* name);  // searches for a symbol in the current scope only
bool scope_unbind(const char* name);                    // unbinds a [name, symbol] pair from the symbol table (rarely used)

#endif