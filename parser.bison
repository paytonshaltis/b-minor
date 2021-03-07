%{
#include <stdio.h>
#include <stdlib.h>    

extern char* yytext;
extern int yylex();
extern int yyerror(char* str);

%}

%token TOKEN_ARRAY
%token TOKEN_BOOLEAN
%token TOKEN_CHAR
%token TOKEN_ELSE
%token TOKEN_FALSE
%token TOKEN_FOR
%token TOKEN_FUNCTION
%token TOKEN_IF
%token TOKEN_INTEGER
%token TOKEN_PRINT
%token TOKEN_RETURN
%token TOKEN_STRING
%token TOKEN_TRUE
%token TOKEN_VOID
%token TOKEN_COLON
%token TOKEN_SEMICOLON
%token TOKEN_COMMA
%token TOKEN_LBRACKET
%token TOKEN_RBRACKET
%token TOKEN_LCURLY
%token TOKEN_RCURLY
%token TOKEN_LPAREN
%token TOKEN_RPAREN
%token TOKEN_ASSIGN
%token TOKEN_CARET
%token TOKEN_PLUS
%token TOKEN_MINUS
%token TOKEN_INCREMENT
%token TOKEN_DECREMENT
%token TOKEN_MULTIPLY
%token TOKEN_DIVIDE
%token TOKEN_MOD
%token TOKEN_EQUAL
%token TOKEN_GE
%token TOKEN_LE
%token TOKEN_LESS
%token TOKEN_GREATER
%token TOKEN_NEQUAL
%token TOKEN_IDENT
%token TOKEN_INTLIT
%token TOKEN_CHARLIT
%token TOKEN_STRINGLIT
%token TOKEN_OR
%token TOKEN_AND
%token TOKEN_NOT
%token TOKEN_ERROR

%%

program			: decllist											// a program is a series of declarations
				|
				;

decllist		: decllist decl										// a series of 1 or more declarations
				| decl
				;

decl			: assign TOKEN_SEMICOLON							// an assignment 
				| onlydecl TOKEN_SEMICOLON							// just a declarations
				| expr TOKEN_SEMICOLON								// assignment of existing variable
				| TOKEN_IDENT TOKEN_COLON TOKEN_FUNCTION type TOKEN_LPAREN TOKEN_RPAREN TOKEN_SEMICOLON
				| TOKEN_IDENT TOKEN_COLON TOKEN_FUNCTION TOKEN_VOID TOKEN_LPAREN TOKEN_RPAREN TOKEN_SEMICOLON
				| TOKEN_IDENT TOKEN_COLON TOKEN_FUNCTION type TOKEN_LPAREN fnctparams TOKEN_RPAREN TOKEN_SEMICOLON
				| TOKEN_IDENT TOKEN_COLON TOKEN_FUNCTION TOKEN_VOID TOKEN_LPAREN fnctparams TOKEN_RPAREN TOKEN_SEMICOLON
				| TOKEN_IDENT TOKEN_COLON TOKEN_FUNCTION type TOKEN_LPAREN TOKEN_RPAREN TOKEN_ASSIGN TOKEN_LCURLY stmtlist TOKEN_RCURLY
				| TOKEN_IDENT TOKEN_COLON TOKEN_FUNCTION TOKEN_VOID TOKEN_LPAREN TOKEN_RPAREN TOKEN_ASSIGN TOKEN_LCURLY stmtlist TOKEN_RCURLY
				| TOKEN_IDENT TOKEN_COLON TOKEN_FUNCTION type TOKEN_LPAREN fnctparams TOKEN_RPAREN TOKEN_ASSIGN TOKEN_LCURLY stmtlist TOKEN_RCURLY
				| TOKEN_IDENT TOKEN_COLON TOKEN_FUNCTION TOKEN_VOID TOKEN_LPAREN fnctparams TOKEN_RPAREN TOKEN_ASSIGN TOKEN_LCURLY stmtlist TOKEN_RCURLY
				;		

stmtlist		: stmtlist stmt
				| stmt
				;

stmt			: TOKEN_RETURN TOKEN_SEMICOLON
				| TOKEN_RETURN expr TOKEN_SEMICOLON
				| TOKEN_PRINT TOKEN_SEMICOLON
				| TOKEN_PRINT printlist TOKEN_SEMICOLON
				| TOKEN_FOR TOKEN_LPAREN expr TOKEN_SEMICOLON expr TOKEN_SEMICOLON expr TOKEN_RPAREN TOKEN_LCURLY stmtlist TOKEN_RCURLY
				| other
				;

printlist		: expr TOKEN_COMMA printlist
				| expr
				;

other			: decl
				;

fnctparams		: onlydecl TOKEN_COMMA fnctparams
				| onlydecl
				;

assign			: TOKEN_IDENT TOKEN_COLON type TOKEN_ASSIGN expr	// expression to the right side of assignment
				;

onlydecl		: TOKEN_IDENT TOKEN_COLON type						// declaration only
				| TOKEN_IDENT TOKEN_COLON TOKEN_ARRAY bracket
				| TOKEN_IDENT TOKEN_COLON TOKEN_ARRAY bracket TOKEN_ASSIGN TOKEN_LCURLY arrlist TOKEN_RCURLY
				;

arrlist			: expr TOKEN_COMMA arrlist
				| expr
				;

type			: TOKEN_INTEGER										// basic types
				| TOKEN_STRING
				| TOKEN_CHAR
				| TOKEN_BOOLEAN
				;

expr			: expr TOKEN_ASSIGN logor							// array subscript is a left side expression	
				| logor
				;

logor			: logor TOKEN_OR logand
				| logand
				;

logand			: logand TOKEN_AND comparison
				| comparison
				;

comparison		: comparison TOKEN_LESS addsub
				| comparison TOKEN_LE addsub
				| comparison TOKEN_GREATER addsub
				| comparison TOKEN_GE addsub
				| comparison TOKEN_EQUAL addsub
				| comparison TOKEN_NEQUAL addsub
				| addsub
				;

addsub			: addsub TOKEN_PLUS multdiv
				| addsub TOKEN_MINUS multdiv
				| multdiv
				;

multdiv			: multdiv TOKEN_MULTIPLY expon
				| multdiv TOKEN_DIVIDE expon
				| multdiv TOKEN_MOD expon
				| expon
				;

expon			: expon TOKEN_CARET unary
				| unary
				;

unary			: TOKEN_MINUS unary
				| TOKEN_NOT unary
				| incdec
				;

incdec			: incdec TOKEN_INCREMENT
				| incdec TOKEN_DECREMENT
				| groups
				;

groups			: TOKEN_LPAREN expr TOKEN_RPAREN					// grouping in parens
				| TOKEN_IDENT bracket								// array subscript (multiple dimensions)
				| TOKEN_IDENT TOKEN_LPAREN fnctcalllist TOKEN_RPAREN// function call
				| TOKEN_IDENT TOKEN_LPAREN TOKEN_RPAREN				// function call (blank)
				| atomic											// any atomic type
				;

fnctcalllist	: expr TOKEN_COMMA fnctcalllist
				| expr
				;

bracket 		: bracket TOKEN_LBRACKET expr TOKEN_RBRACKET		// series of array subscripts in brackets
				| TOKEN_LBRACKET expr TOKEN_RBRACKET				// single expression in brackets
				;

atomic			: TOKEN_IDENT										// atomic types for expressions
				| TOKEN_INTLIT
				| TOKEN_STRINGLIT
				| TOKEN_CHARLIT
				| TOKEN_TRUE
				| TOKEN_FALSE
				;




%%

int yyerror(char* str) {
	printf("Parse error: %s\n",str);
	return 1;
}