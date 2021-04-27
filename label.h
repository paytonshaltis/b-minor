#ifndef LABEL_H
#define LABEL_H

/*
this file contains the prototypes for the 'label' family of functions. This interface
will allow for label creation as well as label name retrieval; will be used when printing 
*/

int label_create();
const char* label_name(int label); 

#endif