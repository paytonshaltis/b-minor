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

program			: decllist
				|
				;

decllist		: decllist decl
				| decl
				;

decl			: TOKEN_IDENT TOKEN_COLON type TOKEN_SEMICOLON
				| TOKEN_IDENT TOKEN_COLON type TOKEN_ASSIGN value TOKEN_SEMICOLON
				| TOKEN_IDENT TOKEN_COLON TOKEN_FUNCTION type TOKEN_LPAREN params TOKEN_RPAREN TOKEN_ASSIGN body
				;

params			: TOKEN_IDENT TOKEN_COLON type
				| 
				;

body			: TOKEN_LCURLY decllist TOKEN_RCURLY
				|
				;

type			: TOKEN_INTEGER
				| TOKEN_STRING
				| TOKEN_CHAR
				| TOKEN_BOOLEAN
				| TOKEN_VOID
				;

value			: TOKEN_INTLIT
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