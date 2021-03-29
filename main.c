#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>
#include <string.h>
#include "decl.h"

extern FILE *yyin;
extern int yylex();
extern int yyparse();
extern void yyrestart();
extern char *yytext;
extern struct decl* parser_result;

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
    int index = 0;

    /* array of options; currently only contains "scan" and the required "all-0s" option structs */
    struct option options[] = { 
        {"scan", required_argument, &scanFlag, 1},
        {"parse", required_argument, &parseFlag, 1},
        {"print", required_argument, &printFlag, 1}, 
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
    int tokenOutput = 1;
    if(parseFlag == 1 || printFlag == 1) {
       tokenOutput = 0;
    }

    /* scanning phase: done with all command line options */
    if(scanFlag == 1 || parseFlag == 1 || printFlag == 1) {

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
                fprintf(stderr, "Scan error: %s is not a valid token.\n", yytext);
                exit(1);
            }
            /* reached identifier or an integer literal token */
            else if((t==TOKEN_IDENT || t==TOKEN_INTLIT) && tokenOutput == 1) {
                printf("%s %s\n", tokenArray[t - 258], yytext);
            }
            /* reached char literal or string literal token */
            else if((t==TOKEN_CHARLIT || t==TOKEN_STRINGLIT) && tokenOutput == 1) {
                printf("%s %s\n", tokenArray[t - 258], yytext);
            }
            /* reached any other token */
            else if(tokenOutput == 1) {
                printf("%s\n",tokenArray[t - 258]);
            }
        }
    }
    
    /* parser status should only be output during the parsing phase if '-parse' option is used */
    int parseOutput = 1;
    if(printFlag == 1) {
       parseOutput = 0;
    }

    /* parsing phase: done with all command line options other than '-scan' */
    if(parseFlag == 1 || printFlag == 1) {
        
        /* reopens and restarts the source file so parsing may
        begin from the beginning of the file after scanning */
        yyin = fopen(filename,"r");
        yyrestart(yyin);
        
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

    if(printFlag == 1) {
        printf("Pretty Print:\n\n");
        decl_print(parser_result, 0);
        printf("\nEnd of Pretty Print\n");
    }

    /* completed each phase of the compiler */
    exit(0);
}
