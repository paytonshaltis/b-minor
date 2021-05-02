#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "label.h"

/*
implements the functions that allow for creating and accessing
labels that will be used when printing to the output file
*/

int varLabelCount = 0;
int litLabelCount = 0;
int condLabelCount = 0;

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