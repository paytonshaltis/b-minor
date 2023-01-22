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
*                                   'decl.c'
*                                   --------
*   This is the implementation file for all functions for the 'decl' AST nodes. It
*   includes a heavily commented breakdown of each function and how it works, which
*   serve as great debugging elements and descriptions of how the compiler works.
*
*/

#include "decl.h"

// variables used in declaration functions
int totalResErrors = 0;         // keeps track of the total number of resolution errors
int totalTypeErrors = 0;        // keeps track of the total number of typechecking errors
extern int resOutput;           // determines whether resolution messages other than errors should be output (SOURCE: 'main.c')
int counter;                    // counter used to generate relative stack addresses of local variables during name resolution

char localsTP[256][300];        // array of strings; stores local symbols for codegen to be printed at the bottom of the output file
int localsTPCounter = 0;        // counter that keeps track of the next position to write into localsTP
int callStackSize;              // the size in bytes of a function's call stack
extern FILE* fp;                // file pointer to the output file for assembly code (SOURCE: 'main.c')


// basic factory function for creating a 'decl' struct
struct decl * decl_create( char *name, struct type *type, struct expr *value, struct stmt *code, struct decl *next ) {
    struct decl *d = malloc(sizeof(*d));
    d->name = name;
    d->type = type;
    d->value = value;
    d->code = code;
    d->next = next;
    return d;
}

// printing function for use by pretty printer
void decl_print(struct decl *d, int indent) {
    
    // for all declarations
    printf("%s: ", d->name);
    type_print(d->type);
    
    // for declarations of type other than function and prototype
    if(d->type->kind != TYPE_FUNCTION && d->type->kind != TYPE_PROTOTYPE) {

        // if there is an initial value
        if(d->value != NULL){
            printf(" = ");
            expr_print(d->value);
        }
        printf(";\n");
    }

    // for declarations of type function and prototype
    if(d->type->kind == TYPE_FUNCTION || d->type->kind == TYPE_PROTOTYPE) {
        
        // if it is an actual function implementation rather than prototype
        if(d->code != NULL) {
            printf(" = ");
            printf("\n{\n");
            stmt_print(d->code, indent + 1);
            printf("}");
        }
        
        // just the prototype
        else{
            printf(";");
        }
        
        printf("\n");
    }

    // prints out the next declaration with same indent
    if(d->next != NULL) {
        decl_print(d->next, 0);
    }
}

// conducts name resolution on the declaration d
void decl_resolve(struct decl* d) {

    // base case for recursion
    if(d == NULL) {
        return;
    }

    // if there exists more than 1 scope, the kind is local; otherwise, the kind is global
    symbol_t kind = scope_level() > 1 ? SYMBOL_LOCAL : SYMBOL_GLOBAL;

    // creates and links the symbol for d into the d struct; symbol is now linked into the AST
    // this uses the 'kind' determined above, and the 'type' and 'name' from d
    d->symbol = symbol_create(kind, type_copy(d->type), strdup(d->name));

    // next we need to resolve the expression associated with this declaration
    expr_resolve(d->value);

    // flag used later to determine if parameters and code should be resolved
    bool resolveParamCode = false;

    // first we must distinguish function implementation from others since they are handled differently

    // if the declaration is NOT of type 'function'
    if(d->type->kind != TYPE_FUNCTION) {
        
        // if the identifier is found in the symbol table, emit an error (redeclaration of non function)
        if(scope_lookup_current(d->name) != NULL) {
            printf("\033[0;31mresolution error\033[0;0m: identifier \"%s\" already declared in current scope\n", d->name);
            totalResErrors++;
        }
        
        // if the identifier is NOT found in the symbol table, add it to the table
        else {
            scope_bind(d->name, d->symbol);
            
            // print proper message depending on scope IF resOutput is 1
            if(resOutput == 1) {
                if(scope_lookup_current(d->name)->type->kind == TYPE_PROTOTYPE) {
                    printf("\033[38;5;46madded\033[0;0m prototype \"%s\" to symbol table\n", d->name);
                }
                else if(scope_lookup_current(d->name)->kind == SYMBOL_GLOBAL) {
                    printf("\033[38;5;46madded\033[0;0m global \"%s\" to symbol table\n", d->name);
                }
                if(scope_lookup_current(d->name)->kind == SYMBOL_LOCAL) {
                    printf("\033[38;5;46madded\033[0;0m local \"%s\" to symbol table (which = %i)\n", d->name, d->symbol->which);
                }
                if(scope_lookup_current(d->name)->kind == SYMBOL_PARAM) {
                    printf("\033[38;5;46madded\033[0;0m parameter \"%s\" to symbol table (which = %i)\n", d->name, d->symbol->which);
                }
            }
            resolveParamCode = true;
        }
    }

    // if the declaration is of type 'function'
    if(d->type->kind == TYPE_FUNCTION) {
        
        // create a symbol that will be used to check the function in question
        struct symbol* symCheck = scope_lookup_current(d->name);

        // CASE (1): if it is in the symbol table and if the symbol in the table is of type TYPE_PROTOTYPE, we can update it with implementation
        if(symCheck != NULL && symCheck->type->kind == TYPE_PROTOTYPE) {

            // if the parameters AND return types match between prototype and implementation
            if(param_list_compare(d->type->params, symCheck->type->params) && (d->type->subtype->kind == symCheck->type->subtype->kind)) {
                
                // set the flag to true so we know to resolve params and code
                resolveParamCode = true;

                // unbind the prototype from the symbol table
                scope_unbind(d->name);
                
                // change the kind of the declaration to TYPE_FUNCTION
                d->symbol->type->kind = TYPE_FUNCTION;

                // rebind the key and symbol structure to the symbol table
                scope_bind(d->name, d->symbol);

                // print message to identify function update IF resOutput is 1
                if(resOutput == 1)
                    printf("\033[38;5;46madded\033[0;0m implementation for \"%s\" to symbol table\n", d->name);
            }
            
            // if the parameters do not match
            else {
                printf("\033[0;31mresolution error\033[0;0m: implementation for \"%s\" does not match prototype\n", d->name);
                totalResErrors++;
            }
        }

        // CASE (2): if it is in the symbol table and if the symbol in the table is of any type other than TYPE_PROTOTYPE, emit error
        else if(symCheck != NULL && symCheck->type->kind != TYPE_PROTOTYPE) {
            
            // will reach here if the same name is declared as anything other than a function prototype
            printf("\033[0;31mresolution error\033[0;0m: function \"%s\" cannot be implemented, \"%s\" already declared in current scope\n", d->name, d->name);
            totalResErrors++;
        }
    
        // CASE (3): if it is not in the symbol table, we add it as a type TYPE_FUNCTION
        else {
            
            // set the flag to true so we know to resolve params and code
            resolveParamCode = true;

            // bind the name and symbol to the symbol table
            scope_bind(d->name, d->symbol);
            if(resOutput == 1)
                printf("\033[38;5;46madded\033[0;0m function \"%s\" to symbol table\n", d->name);
        }
    }

    // if the declaration is a function prototype, resolve parameters and code (if applicapble) **NOTE - To properly typecheck, even if implementation does not match
    // a prototype, we should still resolve everything within so that typechecking may work appropriately**
    if((d->type->kind == TYPE_FUNCTION) || (d->type->kind == TYPE_PROTOTYPE)) {
        
        scope_enter();
        
        //reset counter after entering a funtion's scope for determining stack locations for locals
        counter = 0;

        // perform name resolution on the function's contents
        param_list_resolve(d->type->params);
        stmt_resolve(d->code);
        scope_exit();
        
        // reset counter after leaving a function's scope (for safety) for determining stack locations for locals
        counter = 0;
    }

    // resolve the next declaration in the code
    decl_resolve(d->next);

    return;

}

// conducts typechecking on declarations
void decl_typecheck(struct decl* d) {
    
    // base case for recursion (the declaration after the last)
    if(d == NULL) {
        return;
    }

    // if a variable is declared with an expression, we need to typecheck
    if(d->value) {
        
        // for all types other than array
        if(d->type->kind != TYPE_ARRAY) {
            struct type* t;
            t = expr_typecheck(d->value);
            
            // makes sure identifiers have symbol structs; may not if implementation does not match
            // prototype, and the implementation contains identifiers!
            if(d->symbol != NULL && !type_compare(t, d->symbol->type)) {
                printf("\033[0;31mtypechecking error\033[0;0m: cannot declare ");
                type_print(d->type);
                printf(" (%s) with ", d->name);
                expr_print(d->value);
                printf("\n");
                totalTypeErrors++;
            }
        }

        // for initializations of type arrays
        if(d->type->kind == TYPE_ARRAY) {
        
            // we need to make sure that the initializer list matches the function...
            decl_check_initList(d->type, d->value);
        }
    }

    // if a declaration is a function implementation, we need to make sure it 
    // returns the correct type, then typecheck its statements
    if(d->type->kind == TYPE_FUNCTION) {
        
        // additional param of the function type is sent; used for return statements
        if(d->code) {
            stmt_typecheck(d->code, d->type->subtype);
        }
    }

    // typecheck the next declaration
    decl_typecheck(d->next);
}

// function being used to verify an array's initializer list
void decl_check_initList(struct type* t, struct expr* initList) {

    // temporary type structures used to traverse the initializer list
    struct type* temp = t;
    struct type* trav = t;
    struct expr* list = initList;

    // need to make sure that there is one element per outer array time
    int numElements = count_list_elements(list->left, t->subtype);

    // ensure sizes match up
    if(trav->size != numElements) {
        printf("\033[0;31mtypechecking error\033[0;0m: initializer list of size %i does not match array of size %i\n", numElements, trav->size);
        totalTypeErrors++;
    }

    trav = trav->subtype;

}

// counts the number of elements in an initializer list
int count_list_elements(struct expr* e, struct type* t) {

    // traverses through, counting the number of elements in list e
    int total = 1;
    while(e->right != NULL) {
        
        // if any element is another initializer list
        if(e->left->kind == EXPR_CURLS) {
            if(count_list_elements(e->left->left, t->subtype) == t->size) {
                // used for debugging
                //printf("Inner list worked!\n");
            }
            else {
                printf("\033[0;31mtypechecking error\033[0;0m: array size does not match initializer list size\n");
                totalTypeErrors++;
            }
        }

        // make sure each element is the correct type
        else if(!type_compare(expr_typecheck(e->left), t)) {
            printf("\033[0;31mtypechecking error\033[0;0m: array of type (");
            type_print(t);
            printf(") cannot be initialized with type (");
            type_print(expr_typecheck(e->left));
            printf(")\n");
            totalTypeErrors++;
        }
        
        total++;
        e = e->right;
    }

    // if any element is another initializer list
    if(e->kind == EXPR_CURLS) {
        if(count_list_elements(e->left, t->subtype) == t->size) {
            // used for debugging
            //printf("Inner list worked!\n");
        }
        else {
            printf("\033[0;31mtypechecking error\033[0;0m: array size does not match initializer list size\n");
            totalTypeErrors++;
        }
    }

    // check the final element, which will be e now
    else if(!type_compare(expr_typecheck(e), t)) {
        printf("\033[0;31mtypechecking error\033[0;0m: array of type (");
        type_print(t);
        printf(") cannot be initialized with type (");
        type_print(expr_typecheck(e));
        printf(")\n");
        totalTypeErrors++;
    }

    // returns the total number of elements in the initializer list
    return total;
}

// counts and returns the number of local variables within a statement list
int decl_local_count(struct stmt* s) {

    // if the stmt_list is empty, return 0
    if(s == NULL) {
        return 0;
    }

    // otherwise, count the number of declaraion statements
    struct stmt* stemp = s;
    int count = 0;
    while(stemp != NULL) {

        // increment count if a new local variable is declared
        if(stemp->decl != NULL && (stemp->decl->type->kind == TYPE_INTEGER || stemp->decl->type->kind == TYPE_STRING || stemp->decl->type->kind == TYPE_CHAR || stemp->decl->type->kind == TYPE_BOOLEAN)) {
            count++;
        }

        // if the statement introduces a new scope, recursively call this function
        if(stemp->kind == STMT_IF || stemp->kind == STMT_FOR || stemp->kind == STMT_BLOCK) {
            count += decl_local_count(stemp->body);
        }

        // if the statement introduces TWO new scopes, recursively call this function twice
        if(stemp->kind == STMT_IF_ELSE) {
            count += decl_local_count(stemp->body) + decl_local_count(stemp->else_body);
        }

        // check the next statement
        stemp = stemp->next;

    }

    return count;
}

// function that generates ARM assembly code for a given abstract syntax tree (d is the root of the tree)
void decl_codegen(struct decl* d) {

    // variables used within the switch
    char strBuffer[300];    // buffer to hold a local string that needs to be printed after a function
    int numParams;          // stores the number of parameters in a function
    int numLocals;          // stores the number of locals in a function
    int tempReg;            // temporary register used in a number of places
    int tempReg2;           // second temporary register used in a number of places
    int loopLabel;          // loop label used for an assembly loop
    int doneLabel;          // done label used for an assembly loop
    struct expr* tempe;     // temporary expression structure used for traversing lists of expressions

    // if there are no more declarations
    if(d == NULL) {
        return;
    }

    // switches for all kinds of expressions
    switch(d->type->kind) {

        // declaring global and local integers
        case TYPE_INTEGER:
            
            // in case of a global integer
            if(d->symbol->kind == SYMBOL_GLOBAL) {
                fprintf(fp, ".data\n\t.global %s\n\t.align 8\n%s:\t.word ", d->name, d->name);
                if(d->value) expr_print_file(d->value);
                else fprintf(fp, "0");
                fprintf(fp, "\n");
            }

            // in case of a local integer with initial expression
            if(d->symbol->kind == SYMBOL_LOCAL && d->value != NULL) {
                expr_codegen(d->value);
                fprintf(fp, "\t\tstr\t%s, %s\n", scratch_name(d->value->reg), symbol_codegen(d->symbol));
                scratch_free(d->value->reg);
            }

        break;

        // declaring global and local chars
        case TYPE_CHAR:

            // in case of a global char
            if(d->symbol->kind == SYMBOL_GLOBAL) {
                fprintf(fp, ".data\n\t.global %s\n\t.align 8\n%s:\t.word ", d->name, d->name);
                if(d->value) fprintf(fp, "%i", d->value->literal_value);
                else fprintf(fp, "0");
                fprintf(fp, "\n");
            }

            // in case of a local char with initial expression
            if(d->symbol->kind == SYMBOL_LOCAL && d->value != NULL) {
                expr_codegen(d->value);
                fprintf(fp, "\t\tstr\t%s, %s\n", scratch_name(d->value->reg), symbol_codegen(d->symbol));
                scratch_free(d->value->reg);
            }

        break;

        // declaring global and local booleans
        case TYPE_BOOLEAN:

            // in case of a global boolean
            if(d->symbol->kind == SYMBOL_GLOBAL) {
                fprintf(fp, ".data\n\t.global %s\n\t.align 8\n%s:\t.word ", d->name, d->name);
                if(d->value) fprintf(fp, "%i", d->value->literal_value);
                else fprintf(fp, "0");
                fprintf(fp, "\n");
            }

            // in case of a local boolean with initial expression
            if(d->symbol->kind == SYMBOL_LOCAL && d->value != NULL) {
                expr_codegen(d->value);
                fprintf(fp, "\t\tstr\t%s, %s\n", scratch_name(d->value->reg), symbol_codegen(d->symbol));
                scratch_free(d->value->reg);
            }

        break;

        // declaring global and local strings
        case TYPE_STRING:

            // in case of a global string
            if(d->symbol->kind == SYMBOL_GLOBAL) {

                fprintf(fp, ".data\n\t.global %s\n\t.align 8\n%s:\t.string ", d->name, d->name);
                if(d->value)
                    expr_print_file(d->value);
                else
                    fprintf(fp, "\"\\0\"");
                fprintf(fp, "\n");
            }

            // in case of a local string, we should create a new label for the string, store 
            // it in the 'which', then print it with all other labels at the end of the code
            if(d->symbol->kind == SYMBOL_LOCAL) {
                
                // unique label number will be stored in 'which'
                memset(strBuffer, 0, 300);
                d->symbol->which = var_label_create();
                if(d->value != NULL) {

                    // if that value is a string literal, print that
                    if(d->value->string_literal != NULL) {
                        sprintf(strBuffer, "\t.section\t.data\n\t.align 8\n%s:\n\t.string %s\n", var_label_name(d->symbol->which), d->value->string_literal);
                    }

                    // if that value is a string variable name, we have to print an empty field and assign the var's value to this new string
                    if(d->value->kind == EXPR_NAME) {
                        
                        // still store its label, this time empty
                        sprintf(strBuffer, "\t.section\t.data\n\t.align 8\n%s:\n\t.string \"\\0\"\n", var_label_name(d->symbol->which));
                        
                        // now we need to do like a normal assign for this string using the label above.
                        // this is my algorithm for string assignments; one character at a time until '\0' is reached
                        // generate code to get the righmost string into a register
                        expr_codegen(d->value);

                        // generate code to get the left string into a register (manually using its label)
                        tempReg = scratch_alloc();
                        fprintf(fp, "\t\tadrp\t%s, %s\n", scratch_name(tempReg), var_label_name(d->symbol->which));
                        fprintf(fp, "\t\tadd\t%s, %s, :lo12:%s\n", scratch_name(tempReg), scratch_name(tempReg), var_label_name(d->symbol->which));

                        // use this expression's register to hold the current position
                        tempReg2 = scratch_alloc();

                        // need to make a loop label and a done label
                        loopLabel = stmt_label_create();
                        doneLabel = stmt_label_create();

                        // start by loading the position register with a 0
                        fprintf(fp, "\t\tmov\t%s, 0\n", scratch_name(tempReg2));

                        // print the loop label
                        fprintf(fp, "\t%s:\n", stmt_label_name(loopLabel));

                        // move the source character to the destination
                        fprintf(fp, "\t\tldrb\tw0, [%s, %s]\n", scratch_name(d->value->reg), scratch_name(tempReg2));
                        fprintf(fp, "\t\tstrb\tw0, [%s, %s]\n", scratch_name(tempReg), scratch_name(tempReg2));

                        // compare the source character to null terminator to see if we are done
                        fprintf(fp, "\t\tcmp\tw0, 0\n");

                        // if they are equal jump to done
                        fprintf(fp, "\t\tb.eq\t%s\n", stmt_label_name(doneLabel));

                        // otherwise, increment the position register and jump to the top of the loop
                        fprintf(fp, "\t\tadd\t%s, %s, 1\n", scratch_name(tempReg2), scratch_name(tempReg2));
                        fprintf(fp, "\t\tb\t%s\n", stmt_label_name(loopLabel));

                        // print the done label for exiting the loop
                        fprintf(fp, "\t%s:\n", stmt_label_name(doneLabel));

                        // free up the three registers used
                        scratch_free(d->value->reg);
                        scratch_free(tempReg);
                        scratch_free(tempReg2);

                    }
                }
                
                // if no initial value is given, set it as the empty string
                else
                    sprintf(strBuffer, "\t.section\t.data\n\t.align 8\n%s:\n\t.string \"\\0\"\n", var_label_name(d->symbol->which));

                // store this into the array of strings that will be printed at the end of the function
                for(int i = 0; i < 300; i++) {
                    localsTP[localsTPCounter][i] = strBuffer[i];
                }
                
                // move the write head of this buffer ahead one position
                localsTPCounter++;

            }

        break;

        // declaring function implementations
        case TYPE_FUNCTION:
            
            // print the name of the function
            fprintf(fp, ".text\n\t.global %s\n\t%s:\n", d->name, d->name);
            
            // to grow the stack, we need to know how many local declarations we have, parameters, as well as account 
            // for 6 extra spots for saving registers during a 'context switch' (plus 4 extra bytes for safety)
            
            // count the number of parameters and locals (name sent to allow main array parameter)
            numParams = param_list_count(d->type->params, d->name);
            numLocals = decl_local_count(d->code);

            // update the external callStackSize variable
            callStackSize = (numLocals + numParams + 8) * 8;

            // grow the stack accordingly
            fprintf(fp, "\t\tstp\tx29, x30, [sp, #-%i]!\n", callStackSize);
            
            // need to store parameters on the stack
            // if there are more than 6 parameters, codegen error
            if(numParams > 6) {
                printf("\033[0;31mERROR\033[0;0m: while generating gode; cannot exceed 6 function parameters, registers filled\n");
                exit(1);
            }

            // otherwise, store registers x0 - x5 into first 6 stack locations (local variables)
            for(int i = 0; i < numParams; i++) {
                fprintf(fp, "\t\tstr\tx%i, [sp, %i]\n", i, 16 + (i*8));
            }
            
            // generate code for the contents of the function
            stmt_codegen(d->code);

            // shrink the stack and return (in case of void function!)
            fprintf(fp, "\t\tldp\tx29, x30, [sp], #%i\n\t\tret\n", callStackSize);

            // need to print out local string labels
            for(int i = 0; i < 256; i++) {
                fprintf(fp, "%s", localsTP[i]);
                for(int j = 0; j < 300; j++) {
                    localsTP[i][j] = 0;
                }
            }
            
            // reset the write head for this buffer for the next function implementation
            localsTPCounter = 0;
            
        break;

        // declaring arrays
        case TYPE_ARRAY:
        
            // print a code generation error if the array is NOT of type integer, or is local
            // (assignment specifies that ONLY 1D, global integer arrays need be implemented)
            if( d->type->subtype->kind != TYPE_INTEGER || d->symbol->kind != SYMBOL_GLOBAL) {
                printf("\033[0;31mERROR\033[0;0m: while generating code; arrays of non-integer types and local arrays not implemented\n");
                exit(1);
            }

            // print the special header for arrays
            fprintf(fp, ".data\n");
            fprintf(fp, "\t.global %s\n", d->name);
            fprintf(fp, "\t.align 3\n");
            fprintf(fp, "%s:\n", d->name);

            // if the array was not initialized
            if(d->value == NULL) {
                
                // use the size to print zeros for each element
                for(int i = 0; i < d->type->size; i++) {

                    fprintf(fp, "\t.word\t0\n");
                }   
            }

            // if the array was initialized
            if(d->value != NULL) {
                
                // use a temp expression to travers the args list
                tempe = d->value->left;
                
                // print out each of the elements on a seperate line (all but last 2)
                while(tempe->right != NULL && tempe->right->kind == EXPR_ARGS) {
                    
                    // prints the next arg in the EXPR_ARGS
                    fprintf(fp, "\t.word\t");
                    expr_print_file(tempe->left);
                    fprintf(fp, "\n");

                    // moves tempe along
                    tempe = tempe->right;
                }
                
                // print the last two elements, if applicable
                if(d->value->left->right != NULL) {
                    
                    fprintf(fp, "\t.word\t");
                    expr_print_file(tempe->left);
                    fprintf(fp, "\n");

                    
                    fprintf(fp, "\t.word\t");
                    expr_print_file(tempe->right);
                    fprintf(fp, "\n");
                }

                else {
                    fprintf(fp, "\t.word\t");
                    expr_print_file(tempe);
                    fprintf(fp, "\n");
                }
                
            }

        break;
        
        // prototypes need not print anything; they are useful only for name resolution
        case TYPE_PROTOTYPE:
        break;
        
        // no 'void' type is ever declared explicitly (only ever as a 'function' subtype)
        case TYPE_VOID:
        break;

    }

    // generate code for the next expression
    decl_codegen(d->next);
}