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

/* ========================= HIGHEST LEVEL PRODUCTIONS ========================= */

//a program consists of a list of declarations
program			: programlist
				| 
				;

//this list of declarations may contain one or more declaration
programlist		: programlist decl
				| decl
				;

//a declaration can be one of the following
decl			: global												// global variables that may optionally be initialized
				| proto													// function prototypes that contain no body of code
				| function												// function implementations that contain a body of code
				;

/* ========================= GLOBALS, PROTOTYPES, FUNCTION DECLARATIONS ========================= */

//global variables may be declared in one of two ways
global			: stddecl TOKEN_SEMICOLON
				| cstdecl TOKEN_SEMICOLON
				;

//function prototypes, declared with or without parameters
proto			: TOKEN_IDENT TOKEN_COLON TOKEN_FUNCTION type TOKEN_LPAREN TOKEN_RPAREN TOKEN_SEMICOLON
				| TOKEN_IDENT TOKEN_COLON TOKEN_FUNCTION type TOKEN_LPAREN paramslist TOKEN_RPAREN TOKEN_SEMICOLON		
				;

//function implementations, defined with or without parameters and contain a body of statements
function		: TOKEN_IDENT TOKEN_COLON TOKEN_FUNCTION type TOKEN_LPAREN TOKEN_RPAREN TOKEN_ASSIGN TOKEN_LCURLY stmtlist TOKEN_RCURLY
				| TOKEN_IDENT TOKEN_COLON TOKEN_FUNCTION type TOKEN_LPAREN paramslist TOKEN_RPAREN TOKEN_ASSIGN TOKEN_LCURLY stmtlist TOKEN_RCURLY
				;

/* ========================= STANDARD, CONSTANT, AND EXPRESSION DECLARATIONS ========================= */

//standard declarations do not involve variable initialization
stddecl			: TOKEN_IDENT TOKEN_COLON type 							// may declare a basic type
				| TOKEN_IDENT TOKEN_COLON array							// may declare an array
				;

//constant declarations involve variable initialization with a constant value
cstdecl			: TOKEN_IDENT TOKEN_COLON type TOKEN_ASSIGN TOKEN_INTLIT							// positive integers
				| TOKEN_IDENT TOKEN_COLON type TOKEN_ASSIGN TOKEN_MINUS TOKEN_INTLIT				// accounts for negative integers
				| TOKEN_IDENT TOKEN_COLON type TOKEN_ASSIGN TOKEN_STRINGLIT
				| TOKEN_IDENT TOKEN_COLON type TOKEN_ASSIGN TOKEN_CHARLIT
				| TOKEN_IDENT TOKEN_COLON type TOKEN_ASSIGN TOKEN_TRUE
				| TOKEN_IDENT TOKEN_COLON type TOKEN_ASSIGN TOKEN_FALSE
				| TOKEN_IDENT TOKEN_COLON array TOKEN_ASSIGN TOKEN_LCURLY exprlist TOKEN_RCURLY
				;

//expression declarations involve variable initialization with an expression or a constant
expdecl			: TOKEN_IDENT TOKEN_COLON type TOKEN_ASSIGN expr
				| TOKEN_IDENT TOKEN_COLON array TOKEN_ASSIGN TOKEN_LCURLY exprlist TOKEN_RCURLY
				;

/* ========================= STATEMENT PRODUCTION RULES ========================= */

//list of both balanced and unbalanced statements
stmtlist		: balanced stmtlist				
				| unbalanced stmtlist
				| balanced
				| unbalanced	
				;

//all of the possible unbalanced statements (if/else/for)
unbalanced		: TOKEN_FOR TOKEN_LPAREN TOKEN_SEMICOLON TOKEN_SEMICOLON TOKEN_RPAREN unbalanced					// for(;;) with an unbalanced statement following it 
				| TOKEN_FOR TOKEN_LPAREN expr TOKEN_SEMICOLON expr TOKEN_SEMICOLON expr TOKEN_RPAREN unbalanced		// for(expr) with an unbalanced statement following it 
				| TOKEN_IF TOKEN_LPAREN expr TOKEN_RPAREN balanced													// lone if statement is automatically unbalanced
				| TOKEN_IF TOKEN_LPAREN expr TOKEN_RPAREN unbalanced												// lone if statement is automatically unbalanced
				| TOKEN_IF TOKEN_LPAREN expr TOKEN_RPAREN balanced TOKEN_ELSE unbalanced							// if-else statement with only one balanced statement
				;
				
//all of the possible balanced statements (if/else/for)
balanced		: TOKEN_FOR TOKEN_LPAREN TOKEN_SEMICOLON TOKEN_SEMICOLON TOKEN_RPAREN balanced						// for(;;) with a balanced statement following it
				| TOKEN_FOR TOKEN_LPAREN expr TOKEN_SEMICOLON expr TOKEN_SEMICOLON expr TOKEN_RPAREN balanced		// for(expr) with a balanced statement following it
				| TOKEN_IF TOKEN_LPAREN expr TOKEN_RPAREN balanced TOKEN_ELSE balanced								// if statement that is balanced (both statements)
				| other_stmt																						// any one of the balanced statements below
				;

//the non-if/else/for statements that are by definition balanced
other_stmt		: TOKEN_RETURN TOKEN_SEMICOLON																		// return nothing statement 
				| TOKEN_RETURN expr TOKEN_SEMICOLON																	// return expression statement 
				| TOKEN_PRINT TOKEN_SEMICOLON																		// print nothing statement
				| TOKEN_PRINT exprlist TOKEN_SEMICOLON																// print expressions statement 
				| stddecl TOKEN_SEMICOLON																			// standard declaration statement 
				| expdecl TOKEN_SEMICOLON																			// expression declaration statement
				| expr TOKEN_SEMICOLON																				// expression statement
				| TOKEN_LCURLY stmtlist TOKEN_RCURLY																// block statement of 'stmtlists'
				;

/* ========================= TYPE PRODUCTION RULES ========================= */

//basic types used for declaring variables and functions
type			: TOKEN_INTEGER
				| TOKEN_STRING
				| TOKEN_CHAR
				| TOKEN_BOOLEAN
				| TOKEN_VOID															// must later typecheck to make sure only functions are of type void
				;

//the array type; split from the 'type' production since it is more specialized
array 			: TOKEN_ARRAY TOKEN_LBRACKET TOKEN_INTLIT TOKEN_RBRACKET type			// this production describes a one-dimensional array of type 'type'
				| TOKEN_ARRAY TOKEN_LBRACKET TOKEN_INTLIT TOKEN_RBRACKET array			// this production describes n-dimensional arrays, must eventually take a 'type'
				;

/* ========================= EXPRESSION PRODUCTION RULES ========================= */

//highest priority, an expression itself
expr			: expr TOKEN_ASSIGN logor							// multiple assignments on 'logors'							
				| logor												// an expression can be anything below; they bubble up
				;

//next highest priority after logical and
logor			: logor TOKEN_OR logand								// multiple or operations on 'logands'
				| logand											// can just be a 'logand'
				;

//next highest priority after comparison operators
logand			: logand TOKEN_AND comparison						// multiple and operations on 'comparisons'
				| comparison										// can just be a 'comparison'
				;

//next highest priority after add and subtract
comparison		: comparison TOKEN_LESS addsub						// multiple less comparisons on 'addsubs'
				| comparison TOKEN_LE addsub						// multiple lequal comparisons on 'addsubs'
				| comparison TOKEN_GREATER addsub					// multiple greater comparisons on 'addsubs'
				| comparison TOKEN_GE addsub						// multiple gequal comparisons on 'addsubs'
				| comparison TOKEN_EQUAL addsub						// multiple equal comparisons on 'addsubs'
				| comparison TOKEN_NEQUAL addsub					// multiple nequals comparisons on 'addsubs'
				| addsub											// can just be an 'addsub'
				;

//next highest priority after mult, div, and mod
addsub			: addsub TOKEN_PLUS multdiv							// adds multiple 'multdivs'
				| addsub TOKEN_MINUS multdiv						// subtracts multiple 'multdivs'
				| multdiv 											// can just be a 'multdiv'
				;

//next highest priority after exponentiation
multdiv			: multdiv TOKEN_MULTIPLY expon						// multiplies multiple 'expons'
				| multdiv TOKEN_DIVIDE expon						// divides multiple 'expons'
				| multdiv TOKEN_MOD expon							// modulos multiple 'expons'
				| expon												// can just be an 'expon'
				;

//next highest priority after unary operations
expon			: expon TOKEN_CARET unary							// exponentiates multiple 'unaries'
				| unary												// can just be a 'unary'
				;

//next highest priority after increment and decrement
unary			: TOKEN_MINUS unary									// unary minus
				| TOKEN_NOT unary									// unary not 
				| incdec											// can just be an 'incdec'
				;

//next highest priority after groups
incdec			: incdec TOKEN_INCREMENT							// incrementing a 'group' 
				| incdec TOKEN_DECREMENT							// decrementing a 'group' 
				| group												// can just be a 'group'
				;

//next highest priority after atomics
group			: TOKEN_LPAREN expr TOKEN_RPAREN 					// an expresison within parentheses
				| TOKEN_IDENT bracket								// indexing an element of an array 
				| TOKEN_IDENT TOKEN_LPAREN exprlist TOKEN_RPAREN	// result of a function call (with parameters)
				| TOKEN_IDENT TOKEN_LPAREN TOKEN_RPAREN				// result of a function call (without parameters)
				| atomic											// can just be an 'atomic'
				;																																		

//the atomic types used in an expression
atomic			: TOKEN_IDENT																															
				| TOKEN_INTLIT
				| TOKEN_STRINGLIT
				| TOKEN_CHARLIT
				| TOKEN_TRUE
				| TOKEN_FALSE
				;

/* ========================= MISCELLANEOUS PRODUCTION RULES ========================= */

//possibly multiple brackets used for indexing an array
bracket			: bracket TOKEN_LBRACKET expr TOKEN_RBRACKET
				| TOKEN_LBRACKET expr TOKEN_RBRACKET
				;

//list of expressions for array initialization, print statement, and function call
exprlist		: expr TOKEN_COMMA exprlist
				| expr
				;

//list of parameters that can be used to declare a function
paramslist		: TOKEN_IDENT TOKEN_COLON type TOKEN_COMMA paramslist			
				| TOKEN_IDENT TOKEN_COLON type
				| paramarr TOKEN_COMMA paramslist
				| paramarr
				;

//an empty array that can be used as a parameter in function declaration
paramarr		: TOKEN_IDENT TOKEN_COLON emptyarrs type
				;

//format for empty arrays in function parameters
emptyarrs		: TOKEN_ARRAY TOKEN_LBRACKET TOKEN_RBRACKET emptyarrs
				| TOKEN_ARRAY TOKEN_LBRACKET TOKEN_RBRACKET
				;

%%

int yyerror(char* str) {
	printf("Parse error: %s\n",str);
	return 1;
}