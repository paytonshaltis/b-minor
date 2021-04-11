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
    
    // assigns a symbol type based on the scope
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
