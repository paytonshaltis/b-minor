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

int main(int argc, char* argv[])
{

    int scan = 0;                           /* flag that indicates the "-scan" option was used */
    int opt = 0;                            /* return value for getopt_long_only() */
    int index = 0;                          /* index of the option stored here by getopt_long_only() */
    int stringsize;                         /* stores the size of 'yytext', used when printing string and char literals */
    char* filename = "";                    /* name of the source file used in command line */
    char* tokenArray[TOKEN_ERROR + 1] = {   /* array that maps token value to name (implicitly through index) */
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
    struct option options[] = { 
        {"scan", required_argument, &scan, 1}, 
        {0, 0, 0, 0} 
    };

    /* gets options from the command line */
    opt = getopt_long_only(argc, argv, "", options, &index);

    if(scan == 1) {
        /* the argument for the "scan" option is the filename */
        filename = optarg;
        
        /* tries to open the source file */
        yyin = fopen(filename,"r");
        if(!yyin) {
            fprintf(stderr, "Error, could not open %s!\n",filename);
            exit(1);
        }

        /* loops until end of file (TOKEN_EOF) or invalid token (TOKEN_ERROR) */
        while(1) {
            token_t t = yylex();
            if(t==TOKEN_EOF) {
                exit(0);
            }
            else if(t==TOKEN_ERROR) {
                fprintf(stderr, "Scan error: %s is not a valid token.\n", yytext);
                exit(1);
            }
            else if(t==TOKEN_IDENT || t==TOKEN_INTLIT) {
                printf("%s %s\n",tokenArray[t], yytext);
            }
            else if(t==TOKEN_CHARLIT || t==TOKEN_STRINGLIT) {
                /* stores the size of the yytext c-string in stringsize */
                stringsize = strlen(yytext) + 1;

                /* prints one character at a time, ignoring quotes 
                and taking proper action for escape characters */
                printf("%s ", tokenArray[t]);
                for(int i = 0; i < stringsize; i++) {
                    /* in case of quotes */
                    if((yytext[i] == '\"' && t == TOKEN_STRINGLIT) || (yytext[i] == '\'' && t == TOKEN_CHARLIT)) {
                        /* nothing should be printed */
                    }
                    /* in case of escape sequence */
                    else if(yytext[i] == '\\') {
                        if(yytext[i+1] == 'n') {
                            printf("\n");
                            i++;
                        }
                        else if(yytext[i+1] == '0') {
                            break;
                        }
                        else {
                            printf("%c", yytext[i+1]);
                            i++;
                        }
                    }
                    /* in case of any other character */
                    else {
                        printf("%c", yytext[i]);
                    }
                }
                printf("\n");
            }
            /* any token other than EOF, ERROR, IDENT, INTLIT, CHARLIT, or STRINGLIT */
            else {
                printf("%s\n",tokenArray[t]);
            }
        }
    }
}
