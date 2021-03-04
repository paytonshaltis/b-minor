#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>

extern FILE *yyin;
extern int yylex();
extern char *yytext;

/* function used to modify yytext for char and string literals */
void modifyText(token_t t) {
    
    /* creates a new pointer for storing characters one by one */
    int stringsize = strlen(yytext) + 1;
    char newyytext[stringsize];
    int textPos = 0;

    /* copies one character at a time, ignoring quotes 
    and taking proper action for escape characters */
    for(int i = 0; i < stringsize; i++) {
        
        /* in case of starting and ending quotes */
        if((yytext[i] == '\"' && t == TOKEN_STRINGLIT) || (yytext[i] == '\'' && t == TOKEN_CHARLIT)) {
            /* nothing should be duplicated */
        }
        
        /* in case of escape sequence */
        else if(yytext[i] == '\\') {
            if(yytext[i+1] == 'n') {
                newyytext[textPos] = 10;
                textPos++;
                i++;
            }
            else if(yytext[i+1] == '0') {
                newyytext[textPos] = 0;
                textPos++;
                i++;
            }
            else {
                newyytext[textPos] = yytext[i + 1];
                textPos++;
                i++;
            }
        }
        
        /* in case of any other character */
        else {
            newyytext[textPos] = yytext[i];
            textPos++;
        }
    }

    /* copies string pointed to by 'newyytext' to 'yytext' */
    for(int i = 0; i < stringsize; i++) {
        yytext[i] = newyytext[i];
    }
}

/* main function */
int main(int argc, char* argv[]) {
    
    /* array that maps token value to name (implicitly through index) */
    char* tokenArray[TOKEN_ERROR + 1] = {   
        "EOF",
        "ARRAY",
        "BOOLEAN",
        "CHAR",
        "ELSE",
        "FALSE",
        "FOR",
        "FUNCTION",
        "IF",
        "INTEGER",
        "PRINT",
        "RETURN",
        "STRING",
        "TRUE",
        "VOID",
        "COLON",
        "SEMICOLON",
        "COMMA",
        "LEFT_BRACKET",
        "RIGHT_BRACKET",
        "LEFT_CURLY",
        "RIGHT_CURLY",
        "LEFT_PAREN",
        "RIGHT_PAREN",
        "ASSIGNMENT",
        "CARET",
        "PLUS",
        "MINUS",
        "INCREMENT",
        "DECREMENT",
        "MULTIPLY",
        "DIVIDE",
        "MOD",
        "EQUAL",
        "GE",
        "LE",
        "LESS",
        "GREATER",
        "NOT_EQUAL",
        "IDENTIFIER",
        "INT_LITERAL",
        "CHAR_LITERAL",
        "STRING_LITERAL",
        "LOGICAL_OR",
        "LOGICAL_AND",
        "LOGICAL_NOT",
        "ERROR"  
    };
      
    /* array of options; currently only contains "scan" and the required "all-0s" option structs */
    int scanFlag = 0;                   
    int index = 0;
    struct option options[] = { 
        {"scan", required_argument, &scanFlag, 1}, 
        {0, 0, 0, 0} 
    };

    /* gets options from the command line */
    int opt = getopt_long_only(argc, argv, "", options, &index);

    /* tries to open the source file */
    char* filename = optarg;
    yyin = fopen(filename,"r");
    if(!yyin) {
        fprintf(stderr, "Error, could not open %s!\n",filename);
        exit(1);
    }
    if(strlen(filename) < 8 || strcmp(filename + strlen(filename) - 7, ".bminor") != 0) {
        fprintf(stderr, "Error, %s is not a .bminor source file!\n", filename);
        exit(1);
    }

    /* when the '-scan' command line option is used */
    if(scanFlag == 1) {

        /* loops until end of file (TOKEN_EOF) or invalid token (TOKEN_ERROR) */
        while(1) {
            token_t t = yylex();
            
            /* reached end of file */
            if(t==TOKEN_EOF) {
                exit(0);
            }
            /* reached unrecognized token */
            else if(t==TOKEN_ERROR) {
                fprintf(stderr, "Scan error: %s is not a valid token.\n", yytext);
                exit(1);
            }
            /* reached identifier or an integer literal token */
            else if(t==TOKEN_IDENT || t==TOKEN_INTLIT) {
                printf("%s %s\n",tokenArray[t], yytext);
            }
            /* reached char literal or string literal token */
            else if(t==TOKEN_CHARLIT || t==TOKEN_STRINGLIT) {
                /* modifyText() removes quotes and deals with escape characters  */
                modifyText(t);
                printf("%s %s\n", tokenArray[t], yytext);
            }
            /* reached any other token */
            else {
                printf("%s\n",tokenArray[t]);
            }
        }
    }
}
