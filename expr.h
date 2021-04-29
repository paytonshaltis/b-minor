#ifndef EXPR_H
#define EXPR_H

#include "symbol.h"

typedef enum {
	EXPR_NAME,		//
	EXPR_INTLIT,	//
	EXPR_BOOLLIT,	//
	EXPR_CHARLIT,	//
	EXPR_STRINGLIT,	//	x
	EXPR_FCALL,		//	x
	EXPR_GROUP,		//
	EXPR_ARRIND,	//	x	
	EXPR_BRACKET,	//
	EXPR_CURLS,		//
	EXPR_INC,		//
	EXPR_DEC,		//
	EXPR_NOT,		//
	EXPR_NEG,		//
	EXPR_EXPON,		//	x
	EXPR_MOD,		//
	EXPR_DIV,		//
	EXPR_MULT,		//
	EXPR_SUB,		//
	EXPR_ADD,		//
	EXPR_NEQUAL,	//	x
	EXPR_EQUAL,		//	x 
	EXPR_GE,		//	x 
	EXPR_GREATER,	//	x
	EXPR_LE,		//	x
	EXPR_LESS,		//	x
	EXPR_AND,		//
	EXPR_OR,		//
	EXPR_ASSIGN,	//
	EXPR_ARGS		//	x
} expr_t;

struct expr {
	/* used by all kinds of exprs */
	expr_t kind;
	struct expr *left;
	struct expr *right;

	/* used by various leaf exprs */
	const char *name;
	int literal_value;
	const char * string_literal;
	struct symbol *symbol;

	/* used int the codegen (register) */
	int reg;	
};

struct expr * expr_create( expr_t kind, struct expr *left, struct expr *right );

struct expr * expr_create_name( const char *n );
struct expr * expr_create_integer_literal( int c );
struct expr * expr_create_boolean_literal( int c );
struct expr * expr_create_char_literal( char c );
struct expr * expr_create_string_literal( const char *str );

void expr_print( struct expr *e );
int precedence(struct expr* e);
int unaryExpr(expr_t t);

void expr_resolve(struct expr* e);
struct type* expr_typecheck(struct expr* e);

void expr_codegen(struct expr* e);

#endif
