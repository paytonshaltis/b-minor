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
*                                   'main.c'
*                                   --------
*   This file uses the command-line arguments in order to determine which phase of 
*   compilation the user is requesting, and calls the appropriate functions to perform
*   those phases. Successful compilations exit with code 0; failures with code 1.
*
*/

#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "decl.h"
#include "scope.h"
#include "token.h"

// variables used within the 'main' and 'modify_text' functions
extern FILE *yyin;                      // file descriptor for the source file (SOURCE: 'scanner.c')
extern int yylex();                     // function to perform lexical analysis (scanning) (SOURCE: 'scanner.c')
extern int yyparse();                   // function to perform parsing (parsing) (SOURCE: 'parser.c')
extern void yyrestart();                // function to reset the read head of the input file between phases (SOURCE: 'scanner.c')
extern char *yytext;                    // pointer the the actual text of a literal or variable name (SOURCE: 'scanner.c')
extern struct decl* parser_result;      // the root node of the abstract syntax tree (SOURCE: 'parser.bison')
extern int yylineno;                    // the line number of the current scan/parse position; used to print errors (SOURCE: 'scanner.c')
extern int totalResErrors;              // stores the total number of resolution errors (SOURCE: 'decl.c')
extern int totalTypeErrors;             // stores the total number of typechecking errors (SOURCE: 'decl.c')
FILE* fp;                               // file descriptor for the output file
int resOutput;                          // determines whether resolution messages other than errors should be output


// function used to modify 'yytext' for char and string literals
void modifyText(enum yytokentype t) {

    // creates a new array for storing characters one by one
    int stringSize = strlen(yytext) + 1;
    char newyytext[stringSize];
    int textPos = 0;

    // copies one character at a time, ignoring quotes and taking proper action for escape characters
    for(int i = 0; i < stringSize; i++) {
        
        // in case of starting and ending quotes
        if((yytext[i] == '\"' && t == TOKEN_STRINGLIT) || (yytext[i] == '\'' && t == TOKEN_CHARLIT)) {
            // nothing should be duplicated into the 'newyytext' array
        }
        
        // in case of escape sequence
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
        
        // in case of any other character
        else {
            newyytext[textPos] = yytext[i];
            textPos++;
        }
    }

    // copies characters from 'newyytext' array to 'yytext' pointer,
    // overwriting the originally scanned string from the souce file 
    // with the new changes made within this function
    for(int i = 0; i < stringSize; i++) {
        yytext[i] = newyytext[i];
    }
}

// main function that initiates the compiler phases
int main(int argc, char* argv[]) {

    // array that maps token value to name (implicitly through array indices, ORDER IS CRITICAL HERE, DO NOT CHANGE!)
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
      
    // flags that determine the command line option / index for getopt_long_only() call
    int scanFlag = 0;
    int parseFlag = 0;
    int printFlag = 0;
    int resolveFlag = 0;
    int typecheckFlag = 0;
    int codegenFlag = 0;
    int index = 0;

    // array of options; all possible command-line options and the required "all-0s" option structs
    struct option options[] = { 
        {"scan", required_argument, &scanFlag, 1},
        {"parse", required_argument, &parseFlag, 1},
        {"print", required_argument, &printFlag, 1},
        {"resolve", required_argument, &resolveFlag, 1},
        {"typecheck", required_argument, &typecheckFlag, 1},
        {"codegen", required_argument, &codegenFlag, 1},
        {0, 0, 0, 0} 
    };

    // gets options from the command line
    int opt = getopt_long_only(argc, argv, "", options, &index);

    //tries to open the source file
    char* filename = optarg;
    if(filename != NULL) {
        yyin = fopen(filename,"r");
        if(!yyin) {
            fprintf(stderr, "\033[0;31mERROR\033[0;0m: could not open %s\n",filename);
            exit(1);
        }
        if(strlen(filename) < 8 || strcmp(filename + strlen(filename) - 7, ".bminor") != 0) {
            fprintf(stderr, "\033[0;31mERROR\033[0;0m: not a '.bminor' source file\n");
            exit(1);
        }
    }

    // tokens should only be output during the scanning phase if '-scan' option is used
    int tokenOutput = 0;
    if(scanFlag == 1) {
       tokenOutput = 1;
    }

    // scanning phase: done with all command line options
    if(scanFlag == 1 || parseFlag == 1 || printFlag == 1 || resolveFlag == 1 || typecheckFlag == 1 || codegenFlag == 1) {

        // loops until end of file (TOKEN_EOF) or invalid token (TOKEN_ERROR)
        while(1) {
            enum yytokentype t = yylex();
            
            // reached end of file (yylex returns 0 when at EOF)
            if(t==0) {
                if(tokenOutput == 1) {
                    printf("\033[38;5;46mSUCCESS\033[0;0m: scanning phase passed, all tokens valid\n");
                    exit(0);
                }
                else
                    break;
            }
            
            // reached unrecognized token
            else if(t==TOKEN_ERROR) {
                fprintf(stderr, "\033[0;31mERROR\033[0;0m: while scanning; near line %d: %s is not a valid token\n", yylineno, yytext);
                exit(1);
            }
            
            // reached identifier or an integer literal token
            else if((t==TOKEN_IDENT || t==TOKEN_INTLIT) && tokenOutput == 1) {
                printf("%s %s\n", tokenArray[t - 258], yytext);
            }
            
            // reached char literal or string literal token
            else if((t==TOKEN_CHARLIT || t==TOKEN_STRINGLIT) && tokenOutput == 1) {
                
                //cleans up the string or char literal
                modifyText(t);
                printf("%s %s\n", tokenArray[t - 258], yytext);
            }
            // reached any other token
            else if(tokenOutput == 1) {
                printf("%s\n",tokenArray[t - 258]);
            }
        }
    }
    
    // parser status should only be output during the parsing phase if '-parse' option is used
    int parseOutput = 0;
    if(parseFlag == 1) {
       parseOutput = 1;
    }

    // parsing phase: done with all command line options other than '-scan'
    if(parseFlag == 1 || printFlag == 1 || resolveFlag == 1 || typecheckFlag == 1 || codegenFlag == 1) {
        
        // reopens and restarts the source file so parsing may
        // begin from the beginning of the file after scanning
        yyin = fopen(filename,"r");
        yyrestart(yyin);
        
        // resets 'yylineno' for parsing
        yylineno = 1;

        // if the source file has valid B-Minor syntax
        if(yyparse() == 0) {
            if(parseOutput == 1) {
                printf("\033[38;5;46mSUCCESS\033[0;0m: parsing phase passed, all grammar is valid\n");
                exit(0);
            }
        }

        // if the source file does not have valid B-Minor syntax
        else {
            exit(1);
        }
    }

    // printing phase: done with the command line option -print
    if(printFlag == 1) {
        
        // you cannot get here without passing the parsing phase,
        // so a NULL 'parser_result' means we have an empty program
        if(parser_result != NULL) {
            decl_print(parser_result, 0);
        }
        printf("\033[38;5;46mSUCCESS\033[0;0m: printing phase passed, program preview above\n");
    }

    // resolution 'adds' and 'references' do not need to be displayed during typechecking and codegen
    resOutput = 0;
    if(resolveFlag == 1) {
       resOutput = 1;
    }

    // resolution phase: done with the command line option -resolve, -typecheck, and -codegen
    if(resolveFlag == 1 || typecheckFlag == 1 || codegenFlag == 1) {
        
        // we must enter the global scope before doing any resolutions
        scope_enter();
        
        // resolve the first declaration of the program
        decl_resolve(parser_result);

        // print out the total number of resolution errors found
        if(totalResErrors == 1) {
            printf("\n\033[0;31mERROR\033[0;0m: there was %d total resolution error detected\n", totalResErrors);

            // new line for typechecking
            if(typecheckFlag == 1) {
                printf("\n");
            }
        }
        else if(totalResErrors > 1){
            printf("\n\033[0;31mERROR\033[0;0m: there were %d total resolution errors detected\n", totalResErrors);
            
            // new line for typechecking
            if(typecheckFlag == 1) {
                printf("\n");
            }
        }

        // print out success message if resOutput is 1
        if(totalResErrors == 0 && resOutput == 1) {
            printf("\n\033[38;5;46mSUCCESS\033[0;0m: name resolution phase passed, see above for references\n");
        }
    }
    
    // typechecking phase: done with the command line options -typecheck and -codegen
    if(typecheckFlag == 1 || codegenFlag == 1) {

        // just need to call the decl_typecheck() function on the first declaration of the AST
        decl_typecheck(parser_result);

        // print out the totla number of typechecking errors found
        if(totalTypeErrors == 1) {
            printf("\n\033[0;31mERROR\033[0;0m: there was %d total typechecking error detected\n", totalTypeErrors);       
        }
        else if(totalTypeErrors > 0){
            printf("\n\033[0;31mERROR\033[0;0m: there were %d total typechecking errors detected\n", totalTypeErrors);
        }

        // print out success message if on typechecking phase specifically
        if(totalTypeErrors == 0 && typecheckFlag == 1) {
            printf("\033[38;5;46mSUCCESS\033[0;0m: typechecking phase passed, all types match\n");
        }
    }

    // use the total number of resolution and typechecking errors to determine exit code
    if(totalResErrors > 0 || totalTypeErrors > 0) {
        exit(1);
    }

    // codegen phase: done with the command line option -codegen
    if(codegenFlag == 1) {

        // print an error and exit with code 1 if no output name is given
        if(argc < 4) {
            printf("\033[0;31mERROR\033[0;0m: codegen usage: ./bminor -codegen <source.bminor> <outputname.s>\n");
            exit(1);
        }

        // otherwise, we need to create a file that uses the same name as the fourth command-line argument
        fp = fopen(argv[3], "w+");

        // assembly header
        fprintf(fp, "#\tARMv8-a assembly code generated by \n#\t'B-Minor Compiler' v1.0,\n#\twritten by:\n#\n#\tPAYTON JAMES SHALTIS\n#\tCOMPLETED MAY 4TH, 2021 for\n#\n#\tCSC-425: \"Compilers and Interpreters\", \n#\tProfessor John DeGood, Spring 2021 at\n#\tThe College of New Jersey\n\n");
        fprintf(fp, ".arch armv8-a\n.file \"%s\"\n\n", argv[3]);

        // call decl_codegen(), which recursively generates code for the entire program
        decl_codegen(parser_result);

        // close the file after writing has concluded
        fclose(fp);

        // print a success message to stdout
        printf("\033[38;5;46mSUCCESS\033[0;0m: file \033[38;5;45m\"%s\"\033[0;0m compiled to \033[38;5;45m\"%s\"\033[0;0m!\n", filename, argv[3]);
    }

    // completed each phase of the compiler
    exit(0);
}