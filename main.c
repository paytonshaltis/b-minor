#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>
#include "decl.h"
#include "scope.h"
#include "symbol.h"
#include "type.h"

extern FILE *yyin;
extern int yylex();
extern int yyparse();
extern void yyrestart();
extern char *yytext;
extern struct decl* parser_result;
extern int yylineno;
extern int totalResErrors;

/* function used to modify 'yytext' for char and string literals */
void modifyText(enum yytokentype t) {

    /* creates a new array for storing characters one by one */
    int stringSize = strlen(yytext) + 1;
    char newyytext[stringSize];
    int textPos = 0;

    /* copies one character at a time, ignoring quotes 
    and taking proper action for escape characters */
    for(int i = 0; i < stringSize; i++) {
        
        /* in case of starting and ending quotes */
        if((yytext[i] == '\"' && t == TOKEN_STRINGLIT) || (yytext[i] == '\'' && t == TOKEN_CHARLIT)) {
            /* nothing should be duplicated into the 'newyytext' array */
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

    /* copies characters from 'newyytext' array to 'yytext' pointer,
    overwriting the originally scanned string from the souce file 
    with the new changes made within this function */
    for(int i = 0; i < stringSize; i++) {
        yytext[i] = newyytext[i];
    }
}

/* main function */
int main(int argc, char* argv[]) {

    /* array that maps token value to name (implicitly through array indices) */
    char* tokenArray[46] = {   
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
      
    /* flags that determine the command line option / index for getopt_long_only() call */
    int scanFlag = 0;
    int parseFlag = 0;
    int printFlag = 0;
    int resolveFlag = 0;
    int index = 0;

    /* array of options; currently only contains "scan" and the required "all-0s" option structs */
    struct option options[] = { 
        {"scan", required_argument, &scanFlag, 1},
        {"parse", required_argument, &parseFlag, 1},
        {"print", required_argument, &printFlag, 1},
        {"resolve", required_argument, &resolveFlag, 1}, 
        {0, 0, 0, 0} 
    };

    /* gets options from the command line */
    int opt = getopt_long_only(argc, argv, "", options, &index);

    /* tries to open the source file */
    char* filename = optarg;
    if(filename != NULL) {
        yyin = fopen(filename,"r");
        if(!yyin) {
            fprintf(stderr, "Error, could not open %s!\n",filename);
            exit(1);
        }
        if(strlen(filename) < 8 || strcmp(filename + strlen(filename) - 7, ".bminor") != 0) {
            fprintf(stderr, "Error, not a .bminor source file!\n");
            exit(1);
        }
    }

    /* tokens should only be output during the scanning phase if '-scan' option is used */
    int tokenOutput = 0;
    if(scanFlag == 1) {
       tokenOutput = 1;
    }

    /* scanning phase: done with all command line options */
    if(scanFlag == 1 || parseFlag == 1 || printFlag == 1 || resolveFlag == 1) {

        /* loops until end of file (TOKEN_EOF) or invalid token (TOKEN_ERROR) */
        while(1) {
            enum yytokentype t = yylex();
            
            /* reached end of file (yylex returns 0 when at EOF) */
            if(t==0) {
                if(tokenOutput == 1)
                    exit(0);
                else
                    break;
            }
            /* reached unrecognized token */
            else if(t==TOKEN_ERROR) {
                fprintf(stderr, "Scan error near line %d: %s is not a valid token.\n", yylineno, yytext);
                exit(1);
            }
            /* reached identifier or an integer literal token */
            else if((t==TOKEN_IDENT || t==TOKEN_INTLIT) && tokenOutput == 1) {
                printf("%s %s\n", tokenArray[t - 258], yytext);
            }
            /* reached char literal or string literal token */
            else if((t==TOKEN_CHARLIT || t==TOKEN_STRINGLIT) && tokenOutput == 1) {
                //cleans up the string or char literal
                modifyText(t);
                printf("%s %s\n", tokenArray[t - 258], yytext);
            }
            /* reached any other token */
            else if(tokenOutput == 1) {
                printf("%s\n",tokenArray[t - 258]);
            }
        }
    }
    
    /* parser status should only be output during the parsing phase if '-parse' option is used */
    int parseOutput = 0;
    if(parseFlag == 1) {
       parseOutput = 1;
    }

    /* parsing phase: done with all command line options other than '-scan' */
    if(parseFlag == 1 || printFlag == 1 || resolveFlag == 1) {
        
        /* reopens and restarts the source file so parsing may
        begin from the beginning of the file after scanning */
        yyin = fopen(filename,"r");
        yyrestart(yyin);
        
        /* resets 'yylineno' for parsing */
        yylineno = 1;

        /* if the source file has valid B-Minor syntax */
        if(yyparse() == 0) {
            if(parseOutput == 1) {
                printf("Parse successful!\n");
                exit(0);
            }
        }

        /* if the source file does not have valid B-Minor syntax */
        else {
            printf("Parse failed!\n");
            exit(1);
        }
    }

    /* printing phase: done with the command line option -print */
    if(printFlag == 1) {
        printf("Pretty Print:\n\n");
        
        // you cannot get here without passing the parsing phase,
        // so a NULL 'parser_result' means we have an empty program
        if(parser_result != NULL) {
            decl_print(parser_result, 0);
        }
        printf("\nEnd of Pretty Print\n");
    }

    /* typechecking phase: done with the command line option -resolve */
    if(resolveFlag == 1) {
        printf("This is the resolution phase.\n\n");

        // we must enter the global scope before doing any resolutions
        scope_enter();
        
        // resolve the first declaration of the program
        decl_resolve(parser_result);

        // print out the total number of resolution errors found
        printf("There were %i resolution errors detected.\n", totalResErrors);

        // use the total number of resolution errors to determine what status to exit with
        if(totalResErrors > 0) {
            exit(1);
        }
        else {
            exit(0);
        }
    }
    
    /* completed each phase of the compiler */
    exit(0);
}
