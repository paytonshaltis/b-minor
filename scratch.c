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
*                                   'scratch.c'
*                                   -----------
*   This file implements the functions that will be used to allocate, free, and return
*   names of registers that may be used during code generation. This file assumes
*   ARMv8-a architecture's general register layout.
*
*/

#include "scratch.h"


// parallel arrays will be used to determine which scratch registers are in use
char* name[7] = {"x9", "x10", "x11", "x12", "x13", "x14", "x15"};
bool inUse[7] = {false};

// finds an unused register in the table, marks it as 'in use' and returns that register number
int scratch_alloc() {

    // simply searches each index of the array for free register
    for(int i = 0; i < 7; i++) {
        if(inUse[i] == false) {
            inUse[i] = true;
            return i;
        }
    }

    // in case of no free registers, print error and exit with status 1
    printf("\033[0;31mERROR\033[0;0m: while generating code; all scratch registers currently in use\n");
    exit(1);
    return -1;
}

// marks the indicated register 'r' as available to be used
void scratch_free(int r) {
    
    // make sure the bounds are good
    if(r >= 0 && r <= 6) {
        inUse[r] = false;
    }

    // otherwise print an error
    else {
        printf("\033[0;31mERROR\033[0;0m: codegen usage; not a valid index to the register table\n");
    }
}

// returns the name of a register given its number 'r'
const char* scratch_name(int r) {
    
    // make sure the bounds are good
    if(r >= 0 && r <= 6) {
        return name[r];
    }

    // otherwise print an error and return ERROR
    printf("\033[0;31mERROR\033[0;0m: codegen usage; not a valid index to the register table\n");
    return "ERROR";

}