#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "scratch.h"

/*
implements a scratch table; houses the scratch registers and indicates whether or 
not they are being used. Interface of functions allows interaction with these registers.
*/


// parallel arrays will be used to determine which scratch registers are in use
char* name[7] = {"x9", "x10", "x11", "x12", "x13", "x14", "x15"};
bool inUse[7] = {false};

// finds an unused register in the table, marks it as 'in use,' returns that register number
int scratch_alloc() {

    // simply searches each index of the array for free register
    for(int i = 0; i < 7; i++) {
        if(inUse[i] == false) {
            inUse[i] = true;
            return i;
        }
    }

    // in case of no free registers, print error and exit with status 1
    printf("\033[0;31mcodegen usage error\033[0;0m: all scratch registers currently in use\n");
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
        printf("\033[0;31mcodegen usage error\033[0;0m: not a valid index to the register table\n");
    }
}

// returns the name of a register given its number 'r'
const char* scratch_name(int r) {
    
    // make sure the bounds are good
    if(r >= 0 && r <= 6) {
        return name[r];
    }

    // otherwise print an error and return ERROR
    printf("\033[0;31mcodegen usage error\033[0;0m: not a valid index to the register table\n");
    return "ERROR";

}