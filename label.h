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
*                                   'label.h'
*                                   ---------
*   This file defines the functions that are used to generate different kinds of labels
*   during the code generation of compiling. Includes prototypes for variable, literal,
*   conditional, and statement labels. Implementation can be found in 'label.c'.
*
*/

#ifndef LABEL_H
#define LABEL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// create and get names for variable name labels
int var_label_create();
const char* var_label_name(int label); 

// create and get names for string literal labels
int lit_label_create();
const char* lit_label_name(int label);

// create and get names for conditional labels
int cond_label_create();
const char* cond_label_name(int label);

// create and get names for statement labels
int stmt_label_create();
const char* stmt_label_name(int label);

#endif