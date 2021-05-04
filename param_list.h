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
*                                  'param_list.h'
*                                  --------------
*   This is the header file that defines the structure for the 'param_list' AST nodes. It
*   includes prototypes for the different functions that are used throughout the project
*   to manage and manipulate these structures. Implementation may be found in 'param_list.c'
*
*/

#ifndef PARAM_LIST_H
#define PARAM_LIST_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "type.h"

// believe this helps to avoid circular dependencies
struct expr;

// parameter list structure
struct param_list {
	char *name;					// name of the variable
	struct type *type;			// type of the variable
	struct symbol *symbol;		// symbol from symbol table for variable
	struct param_list *next;	// the next parameter in the list
};

// function for creating a parameter list (semantic routines in bison)
struct param_list* param_list_create(char* name, struct type *type, struct param_list *next);

// function for printing a parameter list structure (pretty printing)
void param_list_print( struct param_list *a );

// functions for name resolution in parameter lists
void param_list_resolve(struct param_list* p);

// helper funcitons for name resolution and typechecking parameter lists
bool param_list_compare(struct param_list* p1, struct param_list* p2);
struct param_list* param_list_copy(struct param_list* p);
void param_list_delete(struct param_list* p);
bool param_list_fcall_compare(struct expr* calledArgs, struct param_list* p);

// function used to count the number of parameters in a list (codegen)
int param_list_count(struct param_list* p, const char* name);

#endif
