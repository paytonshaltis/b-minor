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
*                                   'type.c'
*                                   --------
*   This is the implementation file for all functions for the 'type' AST nodes. It
*   includes a heavily commented breakdown of each function and how it works, which
*   serve as great debugging elements and descriptions of how the compiler works.
*
*/

#include "type.h"


// basic factory function for creating a 'type' struct
struct type * type_create( type_t kind, struct type *subtype, struct param_list *params, int size ) {
    
    struct type *t = malloc(sizeof(*t));
    
    t->kind = kind;                         // used for all type kinds
    t->subtype = subtype;                   // used for functions and arrays
    t->params = params;                     // only used for functions
    t->size = size;                         // only used for arrays
    
    return t;
}

// printing function for use by the pretty printer
void type_print(struct type *t) {
    
    // for type array
    if(t->kind == TYPE_ARRAY) {
        
        // as long as the array has a subytype
        if(t->subtype != NULL) {
            
            // if the array size is given
            if(t->size != 0) {
                printf("array[%i] ", t->size);
                type_print(t->subtype);
            }
            
            // if the array does not have a size (parameter passing only)
            else {
                printf("array[] ");
                type_print(t->subtype);   
            }
        }
    }

    // for the basic types
    if(t->kind == TYPE_INTEGER) {
        printf("integer");
    }
    if(t->kind == TYPE_STRING) {
        printf("string");
    }
    if(t->kind == TYPE_CHAR) {
        printf("char");
    }
    if(t->kind == TYPE_BOOLEAN) {
        printf("boolean");
    }
    if(t->kind == TYPE_VOID) {
        printf("void");
    }

    // for type function
    if(t->kind == TYPE_FUNCTION || t->kind == TYPE_PROTOTYPE) {
        printf("function ");
        type_print(t->subtype);
        printf(" (");
        
        // only prints parameter list if the function has parameters
        if(t->params != NULL) {
            param_list_print(t->params);
        }
        printf(")");
    }

}

// compares two types to make sure they are the same (returns true if same, false otherwise)
bool type_compare(struct type* t1, struct type* t2) {

    // base case for recursion
    if(t1 == NULL && t2 == NULL) {
        return true;
    }

    // if the kinds are the same, check the subtypes & sizes
    if(t1->kind == t2->kind) {
        if(type_compare(t1->subtype, t2->subtype) && t1->size == t2->size) {
            return true;
        }
        
    }
    // otherwise, return false, types are not equal
    return false;
}

// compares two types to make sure they are the same (does not check array size to allow
// arrays as parameters to pass!) (returns true if same, false otherwise)
bool type_compare_no_size(struct type* t1, struct type* t2) {

    // base case for recursion
    if(t1 == NULL && t2 == NULL) {
        return true;
    }

    // if the kinds are the same, check the subtypes
    if(t1->kind == t2->kind) {
        if(type_compare_no_size(t1->subtype, t2->subtype)) {
            return true;
        }
        
    }
    // otherwise, return false, types are not equal
    return false;
}

// copies and returns a new 'type' struct
struct type* type_copy(struct type* t) {
    
    // if the type was NULL, return NULL
    if(t == NULL) {
        return NULL;
    }

    // creates copy of type t, copying its subtype and parameter list, and returns copy
    struct type* result = type_create(t->kind, type_copy(t->subtype), param_list_copy(t->params), t->size);
    return result;
}

// deletes a type recursively
void type_delete(struct type* t) {

    // base case for recursion
    if(t == NULL) {
        return;
    }

    // deletes all subfields of a type struct
    type_delete(t->subtype);
    param_list_delete(t->params);

    // free this struct
    free(t);
}