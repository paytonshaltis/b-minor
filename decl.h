/*  all code in this file is original, and was written by:
*  
*   PAYTON JAMES SHALTIS
*	COMPLETED MAY 4TH, 2021
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
*                                   'decl.h'
*                                   --------
*   This is the header file that defines the structure for the 'decl' AST nodes. It
*   includes prototypes for the different functions that are used throughout the project
*   to manage and manipulate these structures. Implementation may be found in 'decl.c'
*
*/

#ifndef DECL_H
#define DECL_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "expr.h"
#include "label.h"
#include "scope.h"
#include "scratch.h"
#include "stmt.h"
#include "type.h"


// structure for declarations
struct decl {
	char *name;					// name of the variable declared
	struct type *type;			// its type (integer, string, function, etc.)
	struct expr *value;			// initial value if declared with one
	struct stmt *code;			// for function types, the list of statements
	struct symbol *symbol;		// the symbol mapped with type for this declaration
	struct decl *next;			// the next declaration in the program
};

// functions for creating the declaration (semantic routines in bison) and printing declarations (pretty printing)
struct decl * decl_create( char *name, struct type *type, struct expr *value, struct stmt *code, struct decl *next );
void decl_print( struct decl *d, int indent );

// functions for name resolution and typechecking
void decl_resolve(struct decl* d);
void decl_typecheck(struct decl* d);

// helper functions for the two above, used to verify array initializer lists
void decl_check_initList(struct type* t, struct expr* initList);
int count_list_elements(struct expr* e, struct type* t);

// functions for generating assembly code (codegen) and helper function to get local variable count for a funciton's statement list
void decl_codegen(struct decl* d);
int decl_local_count(struct stmt* s);

#endif
