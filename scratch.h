#ifndef SCRATCH_H
#define SCRATCH_H

/* 
this file contains the prototypes for the 'scratch' family of functions. This interface
will allow register management for scratch registers while computing expressions in code 
*/

int scratch_alloc();
void scratch_free(int r);
const char* scratch_name(int r);

#endif