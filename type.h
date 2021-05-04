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
*                                   'type.h'
*                                   --------
*   This is the header file that defines the structure for the 'type' AST nodes. It
*   includes prototypes for the different functions that are used throughout the project
*   to manage and manipulate these structures. Implementation may be found in 'type.c'
*
*/

#ifndef TYPE_H
#define TYPE_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "param_list.h"


// enumeration list for the different type kinds
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

// structure for a type and its various fields
struct type {
	type_t kind;					// the kind of type from the enumeration list above
	struct param_list *params;		// for functions, a list of its parameters
	struct type *subtype;			// for functions and arrays, its subtype
	int size;						// for arrays, the fixed, integer-defined size identified at declaration time
};

// function to create a type structure (semantic routines in bison)
struct type * type_create( type_t kind, struct type *subtype, struct param_list *params, int size );

// function to print out a type structure (pretty printing)
void type_print( struct type *t );

// helper functions used during name resolution and typechecking
struct type* type_copy(struct type* t);
bool type_compare(struct type* t1, struct type* t2);
bool type_compare_no_size(struct type* t1, struct type* t2);
void type_delete(struct type* t);

#endif
