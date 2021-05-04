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
*                                   'label.c'
*                                   ---------
*   This file contains the implementation for the function prototypes in 'label.h'.
*   It implements functions for creating and returning names of different kinds of
*   labels used during the code generation phase of compiling.
*
*/

#include "label.h"


// variables used in the label functions below
int varLabelCount = 0;      // counter for variable labels
int litLabelCount = 0;      // counter for literal labels
int condLabelCount = 0;     // counter for conditional labels
int stmtLabelCount = 0;     // counter for statement labels

// increments the label counter, returns its current value
int var_label_create() {

    varLabelCount++;
    return varLabelCount;
}

// returns a label in string form
const char* var_label_name(int label) {

    // creates modifies labelName with label (number)
    char* labelName = malloc(sizeof(char) * 10);
    sprintf(labelName, ".LSVAR%i", label);

    // returns modified label
    return labelName;
}

// increments the literal label counter, returns its current value
int lit_label_create() {

    litLabelCount++;
    return litLabelCount;
}

// returns a literal label in string form
const char* lit_label_name(int label) {

    // creates modifies labelName with label (number)
    char* labelName = malloc(sizeof(char) * 10);
    sprintf(labelName, ".LSLIT%i", label);

    // returns modified label
    return labelName;
}

// increments the conditional label counter, returns its current value
int cond_label_create() {

    condLabelCount++;
    return condLabelCount;
}

// returns a conditional label in string form
const char* cond_label_name(int label) {

    // creates modifies labelName with label (number)
    char* labelName = malloc(sizeof(char) * 10);
    sprintf(labelName, ".LCOND%i", label);

    // returns modified label
    return labelName;
}

// increments the statement label counter, returns its current value
int stmt_label_create() {

    stmtLabelCount++;
    return stmtLabelCount;
}

// returns a statement label in string form
const char* stmt_label_name(int label) {

    // creates modifies labelName with label (number)
    char* labelName = malloc(sizeof(char) * 10);
    sprintf(labelName, ".LSTMT%i", label);

    // returns modified label
    return labelName;
}