%{
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include "decl.h" 
#include "type.h"
#include "stmt.h"
#include "expr.h"
#include "param_list.h"

extern char* yytext;
extern int yylex();
extern int yyerror(char* str);

struct decl* parser_result = 0;

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

%union {
	struct decl* decl;
	struct type* type;
	struct stmt* stmt;
	struct expr* expr;
	struct param_list* param_list;
	char* word;
	int number;
	char letter;

}

%type <decl> program programlist decl global proto function stddecl cstdecl expdecl
%type <type> type array sizearr nosizearr
%type <stmt> stmtlist unbalanced balanced otherstmt
%type <expr> atomic group incdec unary expon multdiv addsub comparison logand logor expr exprfor exprlist bracket
%type <param_list> paramslist  
%type <word> ident string
%type <number> value true false
%type <letter> char

%%

/* ========================= HIGHEST LEVEL PRODUCTIONS ========================= */

//a program consists of a list of declarations
program			: programlist		{parser_result = $1;}
				| 					{parser_result = NULL;}
				;

//this list of declarations may contain one or more declaration
programlist		: decl programlist	{$$ = $1, $1->next = $2;}
				| decl				{$$ = $1;}
				;

//a declaration can be one of the following
decl			: global			{$$ = $1;}								// global variables that may optionally be initialized
				| proto				{$$ = $1;}								// function prototypes that contain no body of code
				| function			{$$ = $1;}								// function implementations that contain a body of code
				;

/* ========================= GLOBALS, PROTOTYPES, FUNCTION DECLARATIONS ========================= */

//global variables may be declared in one of two ways
global			: stddecl TOKEN_SEMICOLON	{$$ = $1;}
				| cstdecl TOKEN_SEMICOLON	{$$ = $1;}
				;

//function prototypes, declared with or without parameters
proto			: ident TOKEN_COLON TOKEN_FUNCTION type TOKEN_LPAREN TOKEN_RPAREN TOKEN_SEMICOLON					{$$ = decl_create($1, type_create(TYPE_FUNCTION, $4, 0, 0), 0, 0, 0);}
				| ident TOKEN_COLON TOKEN_FUNCTION type TOKEN_LPAREN paramslist TOKEN_RPAREN TOKEN_SEMICOLON		{$$ = decl_create($1, type_create(TYPE_FUNCTION, $4, $6, 0), 0, 0, 0);}
				;

//function implementations, defined with or without parameters and contain a body of statements
function		: ident TOKEN_COLON TOKEN_FUNCTION type TOKEN_LPAREN TOKEN_RPAREN TOKEN_ASSIGN TOKEN_LCURLY stmtlist TOKEN_RCURLY					{$$ = decl_create($1, type_create(TYPE_FUNCTION, $4, 0, 0), 0, $9, 0);}
				| ident TOKEN_COLON TOKEN_FUNCTION type TOKEN_LPAREN paramslist TOKEN_RPAREN TOKEN_ASSIGN TOKEN_LCURLY stmtlist TOKEN_RCURLY		{$$ = decl_create($1, type_create(TYPE_FUNCTION, $4, $6, 0), 0, $10, 0);}
				;

/* ========================= STANDARD, CONSTANT, AND EXPRESSION DECLARATIONS ========================= */

//standard declarations do not involve variable initialization
stddecl			: ident TOKEN_COLON type 											{$$ = decl_create($1, $3, 0, 0, 0);}									// may declare a basic type
				| ident TOKEN_COLON sizearr											{$$ = decl_create($1, $3, 0, 0, 0);}									// may declare an array
				;

//constant declarations involve variable initialization with a constant value
cstdecl			: ident TOKEN_COLON type TOKEN_ASSIGN value							{$$ = decl_create($1, $3, expr_create_integer_literal($5), 0, 0);}										// positive integers
				| ident TOKEN_COLON type TOKEN_ASSIGN TOKEN_MINUS value				{$$ = decl_create($1, $3, expr_create(EXPR_NEG, expr_create_integer_literal($6), 0), 0, 0);}			// accounts for negative integers
				| ident TOKEN_COLON type TOKEN_ASSIGN string						{$$ = decl_create($1, $3, expr_create_string_literal($5), 0, 0);}
				| ident TOKEN_COLON type TOKEN_ASSIGN char							{$$ = decl_create($1, $3, expr_create_char_literal($5), 0, 0);}
				| ident TOKEN_COLON type TOKEN_ASSIGN true							{$$ = decl_create($1, $3, expr_create_boolean_literal($5), 0, 0);}
				| ident TOKEN_COLON type TOKEN_ASSIGN false							{$$ = decl_create($1, $3, expr_create_boolean_literal($5), 0, 0);}
				| ident TOKEN_COLON sizearr TOKEN_ASSIGN expr						{$$ = decl_create($1, $3, $5, 0, 0);}
				;

//expression declarations involve variable initialization with an expression or a constant
expdecl			: ident TOKEN_COLON type TOKEN_ASSIGN expr							{$$ = decl_create($1, $3, $5, 0, 0);}
				| ident TOKEN_COLON array TOKEN_ASSIGN expr							{$$ = decl_create($1, $3, $5, 0, 0);}
				;

/* ========================= STATEMENT PRODUCTION RULES ========================= */

//list of both balanced and unbalanced statements
stmtlist		: balanced stmtlist			{$$ = $1, $1->next = $2;}	
				| unbalanced stmtlist		{$$ = $1, $1->next = $2;}
				| balanced					{$$ = $1;}
				| unbalanced				{$$ = $1;}
				;

//all of the possible unbalanced statements (if/else/for)
unbalanced		: TOKEN_FOR TOKEN_LPAREN exprfor TOKEN_SEMICOLON exprfor TOKEN_SEMICOLON exprfor TOKEN_RPAREN unbalanced	{$$ = stmt_create(STMT_FOR, 0, $3, $5, $7, $9, 0, 0);}		// for() with an unbalanced statement following it 
				| TOKEN_IF TOKEN_LPAREN expr TOKEN_RPAREN balanced															{$$ = stmt_create(STMT_IF, 0, 0, $3, 0, $5, 0, 0);}			// lone if statement is automatically unbalanced
				| TOKEN_IF TOKEN_LPAREN expr TOKEN_RPAREN unbalanced														{$$ = stmt_create(STMT_IF, 0, 0, $3, 0, $5, 0, 0);}			// lone if statement is automatically unbalanced
				| TOKEN_IF TOKEN_LPAREN expr TOKEN_RPAREN balanced TOKEN_ELSE unbalanced									{$$ = stmt_create(STMT_IF_ELSE, 0, 0, $3, 0, $5, $7, 0);}	// if-else statement with only one balanced statement
				;
				
//all of the possible balanced statements (if/else/for)
balanced		: TOKEN_FOR TOKEN_LPAREN exprfor TOKEN_SEMICOLON exprfor TOKEN_SEMICOLON exprfor TOKEN_RPAREN balanced		{$$ = stmt_create(STMT_FOR, 0, $3, $5, $7, $9, 0, 0);}		// for(expr) with a balanced statement following it
				| TOKEN_IF TOKEN_LPAREN expr TOKEN_RPAREN balanced TOKEN_ELSE balanced										{$$ = stmt_create(STMT_IF_ELSE, 0, 0, $3, 0, $5, $7, 0);}	// if statement that is balanced (both statements)
				| otherstmt																									{$$ = $1;}													// any one of the balanced statements below
				;

//the non-if/else/for statements that are by definition balanced
otherstmt		: TOKEN_RETURN TOKEN_SEMICOLON				{$$ = stmt_create(STMT_RETURN, 0, 0, 0, 0, 0, 0, 0);}				// return nothing statement 
				| TOKEN_RETURN expr TOKEN_SEMICOLON			{$$ = stmt_create(STMT_RETURN, 0, 0, $2, 0, 0, 0, 0);}				// return expression statement 
				| TOKEN_PRINT TOKEN_SEMICOLON				{$$ = stmt_create(STMT_PRINT, 0, 0, 0, 0, 0, 0, 0);}				// print nothing statement
				| TOKEN_PRINT exprlist TOKEN_SEMICOLON		{$$ = stmt_create(STMT_PRINT, 0, 0, $2, 0, 0, 0, 0);}				// print expressions statement 
				| stddecl TOKEN_SEMICOLON					{$$ = stmt_create(STMT_DECL, $1, 0, 0, 0, 0, 0, 0);}				// standard declaration statement 
				| expdecl TOKEN_SEMICOLON					{$$ = stmt_create(STMT_DECL, $1, 0, 0, 0, 0, 0, 0);}				// expression declaration statement
				| expr TOKEN_SEMICOLON						{$$ = stmt_create(STMT_EXPR, 0, 0, $1, 0, 0, 0, 0);}				// expression statement
				| TOKEN_LCURLY stmtlist TOKEN_RCURLY		{$$ = stmt_create(STMT_BLOCK, 0, 0, 0, 0, $2, 0, 0);}				// block statement of 'stmtlists'
				;

/* ========================= TYPE PRODUCTION RULES ========================= */

//basic types used for declaring variables and functions
type			: TOKEN_INTEGER		{$$ = type_create(TYPE_INTEGER, 0, 0, 0);}
				| TOKEN_STRING		{$$ = type_create(TYPE_STRING, 0, 0, 0);}
				| TOKEN_CHAR		{$$ = type_create(TYPE_CHAR, 0, 0, 0);}
				| TOKEN_BOOLEAN		{$$ = type_create(TYPE_BOOLEAN, 0, 0, 0);}
				| TOKEN_VOID		{$$ = type_create(TYPE_VOID, 0, 0, 0);}			// must later typecheck to make sure only functions are of type void
				;

//the array type; split from the 'type' production since it is more specialized
array			: sizearr			{$$ = $1;}
				| nosizearr			{$$ = $1;}
				;

//array with a given size
sizearr 		: TOKEN_ARRAY TOKEN_LBRACKET value TOKEN_RBRACKET type				{$$ = type_create(TYPE_ARRAY, $5, 0, $3);}		// this production describes a one-dimensional array of type 'type'
				| TOKEN_ARRAY TOKEN_LBRACKET value TOKEN_RBRACKET sizearr			{$$ = type_create(TYPE_ARRAY, $5, 0, $3);}		// this production describes n-dimensional arrays, must eventually take a 'type'
				;

//array without a given size
nosizearr		: TOKEN_ARRAY TOKEN_LBRACKET TOKEN_RBRACKET type					{$$ = type_create(TYPE_ARRAY, $4, 0, 0);}		// this production describes a one-dimensional array of type 'type'
				| TOKEN_ARRAY TOKEN_LBRACKET TOKEN_RBRACKET nosizearr				{$$ = type_create(TYPE_ARRAY, $4, 0, 0);}		// this production describes n-dimensional arrays, must eventually take a 'type'
				;

/* ========================= EXPRESSION PRODUCTION RULES ========================= */

//highest priority, an expression itself
expr			: expr TOKEN_ASSIGN logor							{$$ = expr_create(EXPR_ASSIGN, $1, $3);}							// multiple assignments on 'logors'							
				| logor												{$$ = $1;}															// an expression can be anything below; they bubble up
				;

//next highest priority after logical and
logor			: logor TOKEN_OR logand								{$$ = expr_create(EXPR_OR, $1, $3);}								// multiple or operations on 'logands'
				| logand											{$$ = $1;}															// can just be a 'logand'
				;

//next highest priority after comparison operators
logand			: logand TOKEN_AND comparison						{$$ = expr_create(EXPR_AND, $1, $3);}								// multiple and operations on 'comparisons'
				| comparison										{$$ = $1;}															// can just be a 'comparison'
				;

//next highest priority after add and subtract
comparison		: comparison TOKEN_LESS addsub						{$$ = expr_create(EXPR_LESS, $1, $3);}								// multiple less comparisons on 'addsubs'
				| comparison TOKEN_LE addsub						{$$ = expr_create(EXPR_LE, $1, $3);}								// multiple lequal comparisons on 'addsubs'
				| comparison TOKEN_GREATER addsub					{$$ = expr_create(EXPR_GREATER, $1, $3);}							// multiple greater comparisons on 'addsubs'
				| comparison TOKEN_GE addsub						{$$ = expr_create(EXPR_GE, $1, $3);}								// multiple gequal comparisons on 'addsubs'
				| comparison TOKEN_EQUAL addsub						{$$ = expr_create(EXPR_EQUAL, $1, $3);}								// multiple equal comparisons on 'addsubs'
				| comparison TOKEN_NEQUAL addsub					{$$ = expr_create(EXPR_NEQUAL, $1, $3);}							// multiple nequals comparisons on 'addsubs'
				| addsub											{$$ = $1;}															// can just be an 'addsub'
				;

//next highest priority after mult, div, and mod
addsub			: addsub TOKEN_PLUS multdiv							{$$ = expr_create(EXPR_ADD, $1, $3);}								// adds multiple 'multdivs'
				| addsub TOKEN_MINUS multdiv						{$$ = expr_create(EXPR_SUB, $1, $3);}								// subtracts multiple 'multdivs'
				| multdiv 											{$$ = $1;}															// can just be a 'multdiv'
				;

//next highest priority after exponentiation
multdiv			: multdiv TOKEN_MULTIPLY expon						{$$ = expr_create(EXPR_MULT, $1, $3);}								// multiplies multiple 'expons'
				| multdiv TOKEN_DIVIDE expon						{$$ = expr_create(EXPR_DIV, $1, $3);}								// divides multiple 'expons'
				| multdiv TOKEN_MOD expon							{$$ = expr_create(EXPR_MOD, $1, $3);}								// modulos multiple 'expons'
				| expon												{$$ = $1;}															// can just be an 'expon'
				;

//next highest priority after unary operations
expon			: expon TOKEN_CARET unary							{$$ = expr_create(EXPR_EXPON, $1, $3);}								// exponentiates multiple 'unaries'
				| unary												{$$ = $1;}															// can just be a 'unary'
				;

//next highest priority after increment and decrement
unary			: TOKEN_MINUS unary									{$$ = expr_create(EXPR_NEG, $2, 0);}								// unary minus
				| TOKEN_NOT unary									{$$ = expr_create(EXPR_NOT, $2, 0);}								// unary not 
				| incdec											{$$ = $1;}															// can just be an 'incdec'
				;

//next highest priority after groups
incdec			: incdec TOKEN_INCREMENT							{$$ = expr_create(EXPR_INC, $1, 0);}								// incrementing a 'group' 
				| incdec TOKEN_DECREMENT							{$$ = expr_create(EXPR_DEC, $1, 0);}								// decrementing a 'group' 
				| group												{$$ = $1;}															// can just be a 'group'
				;

//next highest priority after atomics
group			: TOKEN_LPAREN expr TOKEN_RPAREN 					{$$ = expr_create(EXPR_GROUP, $2, 0);}															// an expresison within parentheses
				| ident bracket										{$$ = expr_create(EXPR_ARRIND, expr_create_name($1), $2);}										// indexing an element of an array 
				| ident TOKEN_LPAREN exprlist TOKEN_RPAREN			{$$ = expr_create(EXPR_FCALL, expr_create_name($1), expr_create(EXPR_ARGS, $3, 0));}			// result of a function call (with parameters)
				| ident TOKEN_LPAREN TOKEN_RPAREN					{$$ = expr_create(EXPR_FCALL, expr_create_name($1), 0);}										// result of a function call (without parameters)
				| TOKEN_LCURLY exprlist TOKEN_RCURLY				{$$ = expr_create(EXPR_CURLS, $2, 0);}															// used in array initializer lists
				| atomic											{$$ = $1;}																						// can just be an 'atomic'
				;																																		

//identifier that is declared with a type
ident 			: TOKEN_IDENT										{$$ = strdup(yytext);}
				;

//integer literal that is declared with a type
value			: TOKEN_INTLIT										{$$ = atoi(yytext);}
				;

//string literal that is declared with a type
string			: TOKEN_STRINGLIT									{$$ = strdup(yytext);}
				;

//char literal that is declared with a type
char			: TOKEN_CHARLIT										{if(yytext[1] == '\\') { if(yytext[2] == 'n') {$$ = 10;} else if(yytext[2] == '0') {$$ = 0;} else {$$ = yytext[2];} } else {$$ = yytext[1];} }
				;

//true literal that is declared with a type
true			: TOKEN_TRUE										{$$ = 1;}
				;

//false literal that is declared with a type
false			: TOKEN_FALSE										{$$ = 0;}
				;

//the atomic types used in an expression
atomic			: ident												{$$ = expr_create_name($1);}																											
				| value												{$$ = expr_create_integer_literal($1);}
				| string											{$$ = expr_create_string_literal($1);}
				| char												{$$ = expr_create_char_literal($1);}
				| true												{$$ = expr_create_boolean_literal($1);}
				| false												{$$ = expr_create_boolean_literal($1);}
				;

/* ========================= MISCELLANEOUS PRODUCTION RULES ========================= */

//expressions in for-loop fields may be expressions or omitted
exprfor			: expr													{$$ = $1;}
				|														{$$ = 0;}
				;

//possibly multiple brackets used for indexing an array
bracket			: TOKEN_LBRACKET expr TOKEN_RBRACKET bracket			{$$ = expr_create(EXPR_BRACKET, $2, $4)}
				| TOKEN_LBRACKET expr TOKEN_RBRACKET					{$$ = $2;}
				;



//list of expressions for print statement and function call
exprlist		: expr TOKEN_COMMA exprlist								{$$ = expr_create(EXPR_ARGS, $1, $3);}
				| expr													{$$ = $1;}
				;

//list of parameters that can be used to declare a function
paramslist		: ident TOKEN_COLON type TOKEN_COMMA paramslist			{$$ = param_list_create($1, $3, $5);}	
				| ident TOKEN_COLON type								{$$ = param_list_create($1, $3, 0);}
				| ident TOKEN_COLON nosizearr TOKEN_COMMA paramslist	{$$ = param_list_create($1, $3, $5);}
				| ident TOKEN_COLON nosizearr							{$$ = param_list_create($1, $3, 0);}
				;

%%

int yyerror(char* str) {
	printf("Parse error: %s\n",str);
	return 1;
}