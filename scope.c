#include "scope.h"
#include "hash_table.h"
#include "symbol.h"
#include <stdlib.h>
#include <stdio.h>

// this is the master stack that will be used by all functions below
struct stack_node* theStackTop;

// pushes a new hash table on top of the stack, indicating entry of a new scope
void scope_enter() {
    
    // allocates memory for the new stack node 
    struct stack_node* newStackNode = malloc(sizeof(*newStackNode));
    
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
    struct stack_node* toBeDeleted = theStackTop;

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

// returns the number of hash tables in the stack; used to determine if we are in the global scope or not
int scope_level() {
    
    // temporary node to keep track of position in stack
    struct stack_node* temp = theStackTop;

    // if the top of the stack does not point to a stack node, the stack is empty
    if(temp == NULL) {
        return 0;
    }

    // otherwise, we must traverse the stack and accumulate a total:
    int total = 1;
    while(temp->next != NULL) {
        total++;
        temp = temp->next;
    }

    return total;
}

// adds an entry to the top-most hash table, mapping name to the symbol structure sym
void scope_bind(const char* name, struct symbol* sym) {

    // if for some reason there is no scope in the stack
    if(theStackTop == NULL) {
        return;
    }

    // otherwise, we insert the name, symbol mapping into the hash table of the top-most node of the stack
    hash_table_insert(theStackTop->table, name, sym);
}