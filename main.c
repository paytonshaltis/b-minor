#include "token.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <stdbool.h>

extern FILE *yyin;
extern int yylex();
extern char *yytext;

int main(int argc, char* argv[])
{

    char* filename = "";                    //name of the source file used in command line  
    int scan = 0;                           //flag that indicates the "-scan" option was used
    int opt = 0;                            //return value for getopt_long_only()
    int index = 0;                          //index of the option stored here by getopt_long_only()
    char* tokenArray[TOKEN_ERROR + 1] = {  //array that maps token value to name (implicitly through index)
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
        "CARROT",
        "ADD",
        "SUBTRACT",
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
        "LOGICAL_AND"
        "LOGICAL_NOT"
        "ERROR"  
    };    
    //array of options; currently only contains "scan" and the required "all-0s" option structs
    struct option options[] = { 
        {"scan", required_argument, &scan, 1}, 
        {0, 0, 0, 0} 
    };

    //while there are more options to consider
    opt = getopt_long_only(argc, argv, "", options, &index);

    if(scan == 1) {
        //the argument for the "scan" option is the filename
        filename = optarg;
        
        //tries to open the source file
        yyin = fopen(filename,"r");
        if(!yyin) {
            printf("could not open program.c!\n");
            return 1;
        }

        //loops until end of file (TOKEN_EOF) or invalid token (TOKEN_ERROR)
        while(1) {
            token_t t = yylex();
            if(t==TOKEN_EOF) {
                exit(0);
            }
            else if(t==TOKEN_ERROR) {
                fprintf(stderr, "scan error: %s is not a valid token.\n", yytext);
                exit(1);
            }
            else if(t==TOKEN_IDENT || t==TOKEN_INTLIT || t==TOKEN_CHARLIT || t==TOKEN_STRINGLIT) {
                printf("%s %s\n",tokenArray[t], yytext);
            }
            else {
                printf("%s\n",tokenArray[t]);
            }
        }
    }
}
