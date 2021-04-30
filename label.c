#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "label.h"

/*
implements the functions that allow for creating and accessing
labels that will be used when printing to the output file
*/

int labelCount = 0;
int litLabelCount = 0;

// increments the label counter, returns its current value
int label_create() {

    labelCount++;
    return labelCount;
}

// returns a label in string form
const char* label_name(int label) {

    // creates modifies labelName with label (number)
    char* labelName = malloc(sizeof(char) * 10);
    sprintf(labelName, ".L%i", label);

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
    sprintf(labelName, ".LT%i", label);

    // returns modified label
    return labelName;
}