#ifndef EXPR_H
#define EXPR_H

#include "symbol.h"

typedef enum {
	EXPR_NAME,		// sym tab	r: sym tab
	EXPR_INTLIT,	//
	EXPR_BOOLLIT,	//
	EXPR_CHARLIT,	//
	EXPR_STRINGLIT,	//
	EXPR_FCALL,		// st/args	r: sym tab
	EXPR_GROUP,		// expr_typecheck(lt)
	EXPR_ARRIND,	// sym tab  r: sym tab	
	EXPR_BRACKET,	// inside	r: i
	EXPR_CURLS,		// special case
	EXPR_INC,		//
	EXPR_DEC,		//
	EXPR_NOT,		//
	EXPR_NEG,		//
	EXPR_EXPON,		//
	EXPR_MOD,		//
	EXPR_DIV,		//
	EXPR_MULT,		//
	EXPR_SUB,		//
	EXPR_ADD,		//
	EXPR_NEQUAL,	//
	EXPR_EQUAL,		// 
	EXPR_GE,		// 
	EXPR_GREATER,	//
	EXPR_LE,		//
	EXPR_LESS,		//
	EXPR_AND,		//
	EXPR_OR,		//
	EXPR_ASSIGN,	// sym tab	r: sym table
	EXPR_ARGS		// inside	r: type of all
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

#endif
