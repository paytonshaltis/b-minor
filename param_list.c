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
*                                  'param_list.c'
*                                  --------------
*   This is the implementation file for all functions for the 'param_list' AST nodes. It
*   includes a heavily commented breakdown of each function and how it works, which
*   serve as great debugging elements and descriptions of how the compiler works.
*
*/

#include "expr.h"
#include "param_list.h"
#include "scope.h"


// variables used by parameter list functions
extern int totalResErrors;      // keeps track of the total number of resolution errors (SOURCE: 'decl.c')
extern int resOutput;           // determines whether resolution messages other than errors should be output (SOURCE: 'main.c')

// basic factory function for creating a 'param_list' struct
struct param_list* param_list_create(char* name, struct type *type, struct param_list *next) {
    struct param_list *p = malloc(sizeof(*p));
    p->name = name;
    p->type = type;                             
    p->next = next;
    return p;
}

// printing function for use by the pretty printer
void param_list_print(struct param_list *a) {
    printf("%s: ", a->name);
    type_print(a->type);
    
    // if there are more parameters to print in the list
    if(a->next != NULL) {
        printf(", ");
        param_list_print(a->next);
    }
}

// conducts name resolution on the paramater list p
void param_list_resolve(struct param_list* p) {

    // base case for recursion
    if(p == NULL) {
        return;
    }

    // we need to begin by resolving the first identifier in the parameter list
    // first we should check and see if it is in the scope (in the function)
    if(scope_lookup_current(p->name) != NULL) {
        printf("\033[0;31mresolution error\033[0;0m: parameter (%s) already declared in current scope\n", p->name);
        totalResErrors++;
    }
    // if the parameter was NOT found in the scope, we need to create a new symbol and enter it into the table
    else {
        p->symbol = symbol_create(SYMBOL_PARAM, type_copy(p->type), strdup(p->name));

        // bind this 'name', 'symbol' pair into the symbol table
        scope_bind(p->name, p->symbol);
        if(resOutput == 1)
            printf("\033[38;5;46madded\033[0;0m parameter \"%s\" to symbol table (which = %i)\n", p->name, p->symbol->which);
    }

    // resolve the next parameter in the list
    param_list_resolve(p->next);

    return;
}

// used to compare parameter lists for determining whether function implementation matches prototype
bool param_list_compare(struct param_list* p1, struct param_list* p2) {

    // create copies of the param_list head pointers
    struct param_list* p1copy = p1;
    struct param_list* p2copy = p2;

    // traverse the parameter lists, making sure that parameters match up
    while(p1copy != NULL && p2copy != NULL) {

        // if the name and type matches
        if(strcmp(p1copy->name, p2copy->name) == 0 && type_compare(p1copy->type, p2copy->type)) {

            // move to the next parameter
            p1copy = p1copy->next;
            p2copy = p2copy->next;
        }
        
        // if the name and type does not match
        else {
            return false;
        }
    }

    // when one parameter list concludes, make sure the other has too
    if(p1copy == NULL && p2copy == NULL) {
        return true;
    }
    // otherwise, the two lists differ in size, return false;
    return false;

}

// returns a copy of the param list structure
struct param_list* param_list_copy(struct param_list* p) {
    
    // if the parameter list is NULL, return NULL
    if(p == NULL) {
        return NULL;
    }
    
    // creates a new param_list using parameters from before, copying the next one
    struct param_list* result = param_list_create(strdup(p->name), type_copy(p->type), param_list_copy(p->next));
    return result;

}

// recursively deletes a param_list struct
void param_list_delete(struct param_list* p) {

    // base case for recursion
    if(p == NULL) {
        return;
    }

    // frees the subfields of the param_list struct
    param_list_delete(p->next);
    type_delete(p->type);
    
    if(p->name != NULL) {
        free(p->name);
    }
    
    // free this param_list struct itself
    if(p != NULL)
        free(p);
}

// returns true if a function call matches parameters
bool param_list_fcall_compare(struct expr* calledArgs, struct param_list* p) {

    // base case / case for no args function call
    if(calledArgs == NULL && p == NULL) {
        return true;
    }

    // if one or the other becomes NULL first, return false
    if(calledArgs == NULL) {

        return false;
    }
    if(p == NULL) {

        return false;
    }

    // check the next item in the parameter list and expression list, ensure same type
    
    // this block means that there are more than 2 expressions left, since the right side is an EXPR_ARGS
    if(calledArgs->left != NULL && calledArgs->right->kind == EXPR_ARGS) {

        // check and see if the next type in each list matches
        if(type_compare_no_size(expr_typecheck(calledArgs->left), p->type)) {

            // if the types match, move on to the next in each list
            return param_list_fcall_compare(calledArgs->right, p->next);
        }

        // if the next types don't match; return false
        return false;
    }

    // this block means that there are only 2 expressions left, since the right side is NOT an EXPR_ARGS
    if(calledArgs->left != NULL && calledArgs->right->kind != EXPR_ARGS) {
        
        // check and see if the second to last type in each list match
        if(type_compare_no_size(expr_typecheck(calledArgs->left), p->type)) {
            
            // make sure that there is still another parameter left to check to see if the last types match
            if(p->next != NULL && type_compare_no_size(expr_typecheck(calledArgs->right), p->next->type)) {

                // should both be NULL, this will be the last call to this function to hit the base case
                // NOTE - NULL is send since we KNOW that there are no more called arguments!
                return param_list_fcall_compare(NULL, p->next->next);
            }
            
            // if there isn't another param or the last types don't match; return false
            return false;
        }

        // if the second to last types don't match; return false
        return false;
    }
    return false;
}

// returns the number of parameters in a function
int param_list_count(struct param_list* p, const char* name) {

    // in the case where there are no parameters
    if(p == NULL) {
        return 0;
    }

    // in the case where there are more than 0 parameters
    struct param_list* ptemp = p;
    int count = 0;
    while(ptemp != NULL) {
        count++;
        
        // arrays as parameters not supported as per assignment description; exit with code 1 
        // (main allowed to pass fib test case good19.bminor, and for command-line args (not implemented!))
        if(ptemp->type->kind == TYPE_ARRAY && strcmp(name, "main") != 0 ) {
            printf("\033[0;31mERROR\033[0;0m: while generating code; arrays as function parameters not implemented\n");
            exit(1);
        }
        ptemp = ptemp->next;
    }

    return count;
}
