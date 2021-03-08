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

program			: programlist
				| 
				;

programlist		: programlist decl																															// a program is a series of declarations
				| decl
				;

decl			: global																																// declarations may be global, function prototypes, or function declarations
				| proto
				| function
				;

global			: stddecl TOKEN_SEMICOLON
				| expdecl TOKEN_SEMICOLON
				;

stddecl			: TOKEN_IDENT TOKEN_COLON type 																											// standard declaration
				;


expdecl			: TOKEN_IDENT TOKEN_COLON type TOKEN_ASSIGN expr
				| TOKEN_IDENT TOKEN_COLON type TOKEN_ASSIGN TOKEN_LCURLY exprlist TOKEN_RCURLY
				;

proto			: TOKEN_IDENT TOKEN_COLON TOKEN_FUNCTION type TOKEN_LPAREN TOKEN_RPAREN TOKEN_SEMICOLON													// global function prototypes with and w/o parameters
				| TOKEN_IDENT TOKEN_COLON TOKEN_FUNCTION type TOKEN_LPAREN paramslist TOKEN_RPAREN TOKEN_SEMICOLON		

function		: TOKEN_IDENT TOKEN_COLON TOKEN_FUNCTION type TOKEN_LPAREN TOKEN_RPAREN TOKEN_ASSIGN TOKEN_LCURLY stmtlist TOKEN_RCURLY
				| TOKEN_IDENT TOKEN_COLON TOKEN_FUNCTION type TOKEN_LPAREN paramslist TOKEN_RPAREN TOKEN_ASSIGN TOKEN_LCURLY stmtlist TOKEN_RCURLY

stmtlist		: stmt stmtlist
				| stmt	
				;

stmt			: TOKEN_RETURN TOKEN_SEMICOLON																										// FIXME return
				| TOKEN_RETURN expr TOKEN_SEMICOLON
				| TOKEN_PRINT TOKEN_SEMICOLON
				| TOKEN_PRINT exprlist TOKEN_SEMICOLON
				| stddecl TOKEN_SEMICOLON
				| expdecl TOKEN_SEMICOLON
				| expr TOKEN_SEMICOLON
				;

type			: TOKEN_INTEGER
				| TOKEN_STRING
				| TOKEN_CHAR
				| TOKEN_BOOLEAN
				| TOKEN_VOID
				| TOKEN_ARRAY TOKEN_LBRACKET TOKEN_INTLIT TOKEN_RBRACKET type																					// FIXME expr
				;

expr			: expr TOKEN_ASSIGN logor								
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
				;																														// FIXME intlit 



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
				| group
				;

group			: TOKEN_LPAREN expr TOKEN_RPAREN																										// grouping with priority just below atomics
				| TOKEN_IDENT bracket
				| TOKEN_IDENT TOKEN_LPAREN call TOKEN_RPAREN
				| TOKEN_IDENT TOKEN_LPAREN TOKEN_RPAREN
				| atomic
				;																																		

atomic			: TOKEN_IDENT																															// basic expression atomic types
				| TOKEN_INTLIT
				| TOKEN_STRINGLIT
				| TOKEN_CHARLIT
				| TOKEN_TRUE
				| TOKEN_FALSE
				;

bracket			: bracket TOKEN_LBRACKET expr TOKEN_RBRACKET
				| TOKEN_LBRACKET expr TOKEN_RBRACKET
				;

call 			: expr TOKEN_COMMA call																													// goes into a function call
				| expr
				;

exprlist		: expr TOKEN_COMMA exprlist																												// FIXME expr
				| expr
				;

paramslist		: stddecl TOKEN_COMMA paramslist
				| stddecl
				;

%%

int yyerror(char* str) {
	printf("Parse error: %s\n",str);
	return 1;
}