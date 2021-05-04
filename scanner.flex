%{
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
*                                  'scanner.flex'
*                                  --------------
*   This file defines the rules for scanning in the B-Minor language. Flex will use
*   this file to create 'scanner.c', the source-code for a lexical analyzer. Rules
*   use regular expressions to define how tokens are scanner during compilation.
*
*/

#include "token.h"
%}
%option yylineno
DIGIT  [0-9]
LETTER [a-zA-Z]
%%
(" "|\t|\n|\r)  // skip whitespace
 /* ~~~~~~~~~~~~~  KEYWORDS  ~~~~~~~~~~~~~ */
array           { return TOKEN_ARRAY; }
boolean         { return TOKEN_BOOLEAN; }
char            { return TOKEN_CHAR; }
else            { return TOKEN_ELSE; }
false           { return TOKEN_FALSE; }
for             { return TOKEN_FOR; }
function        { return TOKEN_FUNCTION; }
if              { return TOKEN_IF; }
integer         { return TOKEN_INTEGER; }
print           { return TOKEN_PRINT; }
return          { return TOKEN_RETURN; }
string          { return TOKEN_STRING; }
true            { return TOKEN_TRUE; }
void            { return TOKEN_VOID; }
 /* ~~~~~~~~~~~~~  SYMBOLS  ~~~~~~~~~~~~~ */
:               { return TOKEN_COLON; }
;               { return TOKEN_SEMICOLON; }
,               { return TOKEN_COMMA; }
\[              { return TOKEN_LBRACKET; }
\]              { return TOKEN_RBRACKET; }
\{              { return TOKEN_LCURLY; }
\}              { return TOKEN_RCURLY; }
\(              { return TOKEN_LPAREN; }
\)              { return TOKEN_RPAREN; }
=               { return TOKEN_ASSIGN; }
\^              { return TOKEN_CARET; }
\+              { return TOKEN_PLUS; }
-               { return TOKEN_MINUS; }
\+\+            { return TOKEN_INCREMENT; }
--              { return TOKEN_DECREMENT; }
\*              { return TOKEN_MULTIPLY; }
\/              { return TOKEN_DIVIDE; }
%               { return TOKEN_MOD; }
==              { return TOKEN_EQUAL; }
\>=             { return TOKEN_GE; }
\<=             { return TOKEN_LE; }
\<              { return TOKEN_LESS; }
\>              { return TOKEN_GREATER; }
!=              { return TOKEN_NEQUAL; }
\|\|            { return TOKEN_OR; }
&&              { return TOKEN_AND; }
!               { return TOKEN_NOT; }
 /* ~~~~~~~~~~~~~   LITERALS / IDENTIFIER   ~~~~~~~~~~~~~ */
({LETTER}|"_")({LETTER}|"_"|{DIGIT}){0,255}     { return TOKEN_IDENT; }
{DIGIT}+                                        { return TOKEN_INTLIT; }
'((\\?[^\\\'\n])|(\\\\)|(\\\'))'                { return TOKEN_CHARLIT; }
\"((\\.|[^\\"\n]){0,255})\"                     { return TOKEN_STRINGLIT; }
 /* ~~~~~~~~~~~~~   COMMENTS   ~~~~~~~~~~~~~ */
"//".*          // skips single line C++ style comments, multi-lined C style comments below
"/*"            {               
                int lineNumber = 0;
                int c;
                while((c = input()) != 0) {
                    if(c == '\n')
                        lineNumber++;
                    else if(c == '*') {
                        if((c = input()) == '/')
                            goto escape;
                        else
                            unput(c);
                        }
                    }
                    return TOKEN_ERROR;
                    escape: 
                    break;
                }
 /* ~~~~~~~~~~~~~   ERROR   ~~~~~~~~~~~~~ */
.               { return TOKEN_ERROR; }
%%
int yywrap() { return 1; }
