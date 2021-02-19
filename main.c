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

    char* filename = "";                    /* name of the source file used in command line */
    int scan = 0;                           /* flag that indicates the "-scan" option was used */
    int opt = 0;                            /* return value for getopt_long_only() */
    int index = 0;                          /* index of the option stored here by getopt_long_only() */
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

                /* converts the char* into a char array, then I can remove the first and last element (quotes) */
                
                /* create a char array from the char* yytext */
                int stringsize = strlen(yytext) + 1;
                char newstring[stringsize];
                if(stringsize != 3) {
                    char* curpos = yytext;
                    int count = 0;
                    while(*curpos != '\0') {
                        newstring[count] = *curpos;
                        count++;
                        curpos += sizeof(char);
                    }
                    newstring[count] = '\0';

                    /* remove the quotes from the begining and end of the string */
                    for(int i = 1; i < stringsize; i++) {
                        newstring[i - 1] = newstring[i];
                    }
                    newstring[stringsize - 3] = '\0';

                    printf("%s ", tokenArray[t]);

                    /* scans each character in newstring for escape characters */
                    for(int i = 0; i < stringsize; i++) {
                        if(newstring[i] == '\\') {
                            if(newstring[i + 1] == 'n') {
                                printf("\n");
                                i++;
                            }
                            else if(newstring[i + 1] == '0') {
                                break;
                            }
                            else {
                                printf("%c", newstring[i + 1]);
                                i++;
                            }
                        }
                        else {
                            printf("%c", newstring[i]);
                        }
                    }
                    printf("\n");
                }
                /* strings of size 3 will be empty strings: "\"\"\0" (two quotes and a null terminator.)
                a char will never end up here since scanner will not recognize '' as an empty char.) */
                else {
                    newstring[0] = '\0';
                    newstring[1] = '\0';
                    newstring[2] = '\0';
                    
                    printf("%s %s\n", tokenArray[t], newstring);
                }
                
            }
            else {
                printf("%s\n",tokenArray[t]);
            }
        }
    }
}
