#ifndef STMT_H
#define STMT_H

#include "decl.h"

typedef enum {
	STMT_DECL,
	STMT_EXPR,		// done
	STMT_IF,		// done
	STMT_IF_ELSE,	// done
	STMT_FOR,		// done
	STMT_PRINT,		// done
	STMT_RETURN,	// done
	STMT_BLOCK		// done
} stmt_t;

struct stmt {
	stmt_t kind;
	struct decl *decl;
	struct expr *init_expr;
	struct expr *expr;
	struct expr *next_expr;
	struct stmt *body;
	struct stmt *else_body;
	struct stmt *next;
};

struct stmt * stmt_create( stmt_t kind, struct decl *decl, struct expr *init_expr, struct expr *expr, struct expr *next_expr, struct stmt *body, struct stmt *else_body, struct stmt *next );
void stmt_print( struct stmt *s, int indent );

void stmt_resolve(struct stmt* s);
void stmt_typecheck(struct stmt* s);


#endif
