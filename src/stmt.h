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
*                                   'stmt.h'
*                                   --------
*   This is the header file that defines the structure for the 'stmt' AST nodes. It
*   includes prototypes for the different functions that are used throughout the project
*   to manage and manipulate these structures. Implementation may be found in 'stmt.c'
*
*/

#ifndef STMT_H
#define STMT_H

#include <stdio.h>
#include <stdlib.h>
#include "decl.h"
#include "label.h"
#include "scope.h"
#include "scratch.h"


// enumeration of the different statement kinds
typedef enum {
	STMT_DECL,
	STMT_EXPR,		
	STMT_IF,		
	STMT_IF_ELSE,	
	STMT_FOR,		
	STMT_PRINT,		
	STMT_RETURN,	
	STMT_BLOCK		
} stmt_t;

// statement structure with several statement-related fields
struct stmt {
	stmt_t kind;				// the kind of statement, from the enumeration above
	struct decl *decl;			// if the statement is a declaration, this field is used
	struct expr *init_expr;		// initial expression field used in 'for-loops'
	struct expr *expr;			// expression field used in many statement kinds (EXPR, IF, IF_ELSE, FOR, etc.)
	struct expr *next_expr;		// the next expression filed used in 'for-loops'
	struct stmt *body;			// the body of a statement (IF, IF_ELSE, FOR, etc.)
	struct stmt *else_body;		// the else body of an 'if-else-statement'
	struct stmt *next;			// the next statement in the statement list
};

// function to create a statement; parameters are ALL fields of the structure (semantic routines in bison)
struct stmt * stmt_create( stmt_t kind, struct decl *decl, struct expr *init_expr, struct expr *expr, struct expr *next_expr, struct stmt *body, struct stmt *else_body, struct stmt *next );

// function to print a statement (pretty printing)
void stmt_print( struct stmt *s, int indent );

// functions to perform name resolution and typechecking
void stmt_resolve(struct stmt* s);
void stmt_typecheck(struct stmt* s, struct type* ft);

// function that generates ARM assembly code for a statement
void stmt_codegen(struct stmt* s);


#endif
