#include "token.h"
#include <stdio.h>

extern FILE *yyin;
extern int yylex();
extern char *yytext;

int main()
{
    // FIXME: handle command line switches, e.g. "-scan"
    //        hint: man getopt_long_only()

    // FIXME: specify filename on command line
    // FIXME: bminor source code requires filename suffix .bminor
    yyin = fopen("program.c","r");
    if(!yyin) {
        printf("could not open program.c!\n");
        return 1;
    }

    while(1) {
        token_t t = yylex();
        if(t==TOKEN_EOF) break;
        printf("token: %d  text: %s\n",t,yytext);
    }
}
