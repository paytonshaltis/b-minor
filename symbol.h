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
*                                   'symbol.h'
*                                   ----------
*   This file defines the functions that will be used to create and generate code
*   for 'symbol' structures. Symbol creation is used for name resolution, and 
*   symbol code generation is used for the codegen phase of compiling. 
*
*/

#ifndef SYMBOL_H
#define SYMBOL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "type.h"


// enumeration for the different symbol kinds (essentially scopes)
typedef enum {
	SYMBOL_LOCAL,
	SYMBOL_PARAM,
	SYMBOL_GLOBAL
} symbol_t;

// structure for a symbol with its fields
struct symbol {
	symbol_t kind;			// the kind of symbol, from enumeration above (variable scope)
	struct type *type;		// the type of the symbol (variable type)
	char *name;				// the name of the symbol (variable name)
	int which;				// a unique number assigned to each variable within a funciton; used for relative stack address
};

// function to create a symbol structure (semantic routines in bison)
struct symbol * symbol_create( symbol_t kind, struct type *type, char *name );

// function to generate ARM assembly formatted, relative stack address for local variables (codegen)
const char* symbol_codegen(struct symbol *s);

#endif
