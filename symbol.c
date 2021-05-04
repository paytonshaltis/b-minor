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
*                                   'symbol.c'
*                                   ----------
*   This file implements the functions that will be used to create and generate code
*   for 'symbol' structures. Symbol creation is used for name resolution, and 
*   symbol code generation is used for the codegen phase of compiling. 
*
*/

#include "symbol.h"


// function that creates a symbol structure
struct symbol * symbol_create(symbol_t kind, struct type *type, char *name) {
    
    // the symbol that will be returned from this function
    struct symbol* s = (struct symbol*)malloc(sizeof(*s));

    // assign fileds of structure
    s->kind = kind;
    s->type = type_copy(type);      // IMPORTANT: duplicates type so that type_delete and param_list_delete work later!
    s->name = strdup(name);         // IMPORTANT: duplicates string so that param_list_delete works later!

    return s;
}

// returns the name that contains the relative stack address for a symbol (codegen)
const char* symbol_codegen(struct symbol *s) {

    // if the current scope is global, just return the name
    if(s->kind == SYMBOL_GLOBAL) {
        return s->name;
    }

    // if the current scope is local, use 'which' to compute stack address
    char* result = malloc(sizeof(char) * 16);
    sprintf(result, "[sp, %i]", (s->which + 2) * 8);
    return result;
}