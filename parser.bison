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

program			: stmtlist
				|
				;

stmtlist		: stmtlist stmt
				| stmt
				;

stmt			: decl TOKEN_SEMICOLON
				| assign TOKEN_SEMICOLON
				;

decl			: TOKEN_IDENT TOKEN_COLON TOKEN_INTEGER
				| TOKEN_IDENT TOKEN_COLON TOKEN_STRING
				| TOKEN_IDENT TOKEN_COLON TOKEN_CHAR
				| TOKEN_IDENT TOKEN_COLON TOKEN_BOOLEAN
				;

assign			: decl TOKEN_ASSIGN expr
				| decl TOKEN_ASSIGN TOKEN_STRINGLIT
				| decl TOKEN_ASSIGN TOKEN_CHARLIT
				| TOKEN_IDENT TOKEN_ASSIGN expr
				| TOKEN_IDENT TOKEN_ASSIGN TOKEN_STRINGLIT
				| TOKEN_IDENT TOKEN_ASSIGN TOKEN_CHARLIT
				;

expr			: logor
				;

logor			: logand TOKEN_OR logor
				| logand
				;

logand			: comparison TOKEN_AND logand
				| comparison
				;

comparison		: addsub TOKEN_LESS comparison
				| addsub TOKEN_LE comparison
				| addsub TOKEN_GREATER comparison
				| addsub TOKEN_GE comparison
				| addsub TOKEN_EQUAL comparison
				| addsub TOKEN_NEQUAL comparison
				| addsub
				;

addsub			: multdiv TOKEN_PLUS addsub
				| multdiv TOKEN_MINUS addsub
				| multdiv
				;

multdiv			: expon TOKEN_MULTIPLY multdiv
				| expon TOKEN_DIVIDE multdiv
				| expon TOKEN_MOD multdiv
				| expon
				;

expon			: unary TOKEN_CARET expon
				| unary
				;

unary			: TOKEN_MINUS incdec
				| TOKEN_NOT incdec
				| incdec
				;

incdec			: atomic TOKEN_INCREMENT
				| atomic TOKEN_DECREMENT
				| atomic
				;

atomic			: TOKEN_INTLIT
				| TOKEN_IDENT
				| TOKEN_TRUE
				| TOKEN_FALSE
				| TOKEN_LPAREN expr TOKEN_RPAREN
				;

%%

int yyerror(char* str) {
	printf("Parse error: %s\n",str);
	return 1;
}