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
*                                   'scratch.h'
*                                   -----------
*   This file defines the functions that will be used to allocate, free, and return
*   names of registers that may be used during code generation. This file assumes
*   ARMv8-a architecture's general register layout.
*
*/

#ifndef SCRATCH_H
#define SCRATCH_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


// allocates a 'free' register, marking it as 'in use' and returning its number
int scratch_alloc();

// freeing an 'in use' regisger, marking it as 'free'
void scratch_free(int r);

// returns the ARM assembly register name of register number 'r'
const char* scratch_name(int r);

#endif