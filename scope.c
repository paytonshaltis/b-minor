#include "scope.h"
#include "hash_table.h"
#include "symbol.h"
#include <stdlib.h>
#include <stdio.h>

// this is the master stack that will be used by all functions below
struct stack* theStackTop;

// pushes a new hash table on top of the stack, indicating entry of a new scope
void scope_enter() {
    
    // allocates memory for the new stack node 
    struct stack* newStackNode = malloc(sizeof(*newStackNode));
    
    // assigns a scope symbol based on the scope
    if(theStackTop == NULL) {
        newStackNode->thisScope = SYMBOL_GLOBAL;
    }
    else {
        newStackNode->thisScope = SYMBOL_LOCAL;
    }

    // creates a new hash_table struct for this node
    newStackNode->table = hash_table_create(0, 0);

    // the next node should point to the current top of the stack
    newStackNode->next = theStackTop;
    
    // the top of the stack should now point to the new node
    theStackTop = newStackNode;

    return;
}

// pops the top node from the stack, deleting the hash table associated with it in the process
void scope_exit() {

    // create a pointer to point to the top node of the stack
    struct stack* toBeDeleted = theStackTop;

    // if there is more than one node in the stack before exiting scope
    if(theStackTop->next != NULL) {
        // the second node becomes the top of the stack
        theStackTop = theStackTop->next;
    }
    // if there is only one node in the stack before exiting scope
    else {
        // the top of the stack should point to NULL
        theStackTop = NULL;
    }

    // deletes the hash table of the node to be deleted
    hash_table_delete(toBeDeleted->table);

    // frees the memory allocated to the node being deleted
    free(toBeDeleted);
}