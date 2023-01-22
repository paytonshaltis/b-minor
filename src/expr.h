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
*                                   'expr.h'
*                                   --------
*   This is the header file that defines the structure for the 'expr' AST nodes. It
*   includes prototypes for the different functions that are used throughout the project
*   to manage and manipulate these structures. Implementation may be found in 'expr.c'
*
*/

#ifndef EXPR_H
#define EXPR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "label.h"
#include "scope.h"
#include "scratch.h"
#include "symbol.h"


// enumeration for the different expression kinds
typedef enum {
	EXPR_NAME,		
	EXPR_INTLIT,	
	EXPR_BOOLLIT,	
	EXPR_CHARLIT,	
	EXPR_STRINGLIT,	
	EXPR_FCALL,		
	EXPR_GROUP,	
	EXPR_ARRIND,	
	EXPR_BRACKET,
	EXPR_CURLS,		
	EXPR_INC,		
	EXPR_DEC,		
	EXPR_NOT,		
	EXPR_NEG,		
	EXPR_EXPON,		
	EXPR_MOD,		
	EXPR_DIV,		
	EXPR_MULT,		
	EXPR_SUB,		
	EXPR_ADD,		
	EXPR_NEQUAL,	
	EXPR_EQUAL,		
	EXPR_GE,		
	EXPR_GREATER,	
	EXPR_LE,	
	EXPR_LESS,		
	EXPR_AND,		
	EXPR_OR,		
	EXPR_ASSIGN,	
	EXPR_ARGS		
} expr_t;

// structure for expressions
struct expr {
	expr_t kind;					// the expression kind from the enum list above
	struct expr *left;				// the left expression of a binary expression
	struct expr *right;				// the right expression of a binary expression
	const char *name;				// the name of a variable expression (vars are called EXPR_NAME)
	int literal_value;				// the literal value for integer, char, and boolean literals
	const char * string_literal;	// the string value for string literals
	struct symbol *symbol;			// the symbol mapped with type for this expression
	int reg;						// stores the register assigned to an expression during codegen
};

// functions for creating general and leaf expressions (semantic routines in bison)
// general expression creation
struct expr * expr_create( expr_t kind, struct expr *left, struct expr *right );

// leaf expression creation
struct expr * expr_create_name( const char *n );
struct expr * expr_create_integer_literal( int c );
struct expr * expr_create_boolean_literal( int c );
struct expr * expr_create_char_literal( char c );
struct expr * expr_create_string_literal( const char *str );

// functions for printing expressions (pretty printing) and helper functions
void expr_print( struct expr *e );
int precedence(struct expr* e);
int unaryExpr(expr_t t);

// functions used during name resolution and typechecking for expressions
void expr_resolve(struct expr* e);
struct type* expr_typecheck(struct expr* e);

// functions used to generate ARM assembly code (codegen) and a variant of 'expr_print()' 
// that prints to the file stream of the output file instead of stdout
void expr_codegen(struct expr* e);
void expr_print_file( struct expr *e );

#endif
