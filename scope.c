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

    // creates a new hash_table struct for this node
    newStackNode->table = hash_table_create(0, 0);

    // the next node should point to the current top of the stack
    newStackNode->next = theStackTop;
    
    // the top of the stack should now point to the new node
    theStackTop = newStackNode;

    // for debugging:
    printf("Successfully ENTERED a new scope!\n");
    // end debugging
    return;
}

// pops the top node from the stack, deleting the hash table associated with it in the process
void scope_exit() {

    // create a pointer to point to the top node of the stack
    struct stack_node* toBeDeleted = theStackTop;

    // if the stack is already empty
    if(theStackTop == NULL) {
        
        // for debugging:
        printf("ERROR: The stack is already EMPTY!\n");
        // end debugging
        return;
    }

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

    // for debugging:
    printf("Successfully EXITED a scope!\n");
    // end debugging
    return;
}

// returns the number of hash tables in the stack; used to determine if we are in the global scope or not
int scope_level() {
    
    // temporary node to keep track of position in stack
    struct stack_node* temp = theStackTop;

    // if the top of the stack does not point to a stack node, the stack is empty
    if(temp == NULL) {
        
        // for debugging:
        printf("ERROR: Scope at LEVEL 0!\n");
        // end debugging   
        return 0;
    }

    // otherwise, we must traverse the stack and accumulate a total:
    int total = 1;
    while(temp->next != NULL) {
        total++;
        temp = temp->next;
    }

    // for debugging:
    printf("Scope at LEVEL %i\n", total);
    // end debugging    
    return total;
}

// adds an entry to the top-most hash table, mapping name to the symbol structure sym
void scope_bind(const char* name, struct symbol* sym) {

    // if for some reason there is no scope in the stack
    if(theStackTop == NULL) {
        
        // for debugging:
        printf("ERROR: Could not bind because the stack is EMPTY!\n");
        // end debugging
        return;
    }

    // otherwise, we insert the name, symbol mapping into the hash table of the top-most node of the stack
    hash_table_insert(theStackTop->table, name, sym);

    // for debugging:
    printf("Successfully bound key \"%s\" with symbol named \"%s\"!\n", name, sym->name);
    // end debugging
    return;
}

// searches the stack of hash tables from top to bottom, looking for the first entry matching 'name'. Returns NULL if not found
struct symbol* scope_lookup(const char* name) {

    // temporary node and symbol to be returned
    struct stack_node* temp = theStackTop;
    struct symbol* result;

    // if the stack is somehow empty, return NULL
    if(temp == NULL) {
        
        // for debugging:
        printf("ERROR: The stack is EMPTY, could not find!\n");
        // end debugging
        return NULL;
    }

    // otherwise, search the hash table in each node, return the symbol struct if found
    while(temp != NULL) {
        
        // looks up the name in the hash table of this node, returns it if found
        result = hash_table_lookup(temp->table, name);
        if(result != NULL) {
            
            // for debugging:
            printf("Successfully found symbol named \"%s\" which was bound to key \"%s\" in ANY hash table!\n", result->name, name);
            // end debugging
            return result;
        }

        // otherwise, move to the next node
        temp = temp->next;
        printf("    checking next table...\n");
    }

    // this is reached if no node in the stack has a hash table containing the key
    
    // for debugging:
    printf("FAILED to find symbol bound to key \"%s\" in ANY hash table!\n", name);
    // end debugging
    return NULL;

}

// searches the top-most node in the stack to see if an identifier has already been defined in this scope
struct symbol* scope_lookup_current(const char* name) {

    // symbol to be returned
    struct symbol* result;

    // if the stack is somehow empty, return NULL
    if(theStackTop == NULL) {
        
        // for debugging:
        printf("ERROR: The stack is EMPTY, could not find!\n");
        // end debugging
        return NULL;
    }

    // otherwise, see if the hash table in the top node has 'name' in its hash table
    result = hash_table_lookup(theStackTop->table, name);
    if(result != NULL) {
        
        // for debugging:
        printf("Successfully found symbol named \"%s\" which was bound to key \"%s\" in TOP hash table!\n", result->name, name);
        // end debugging
        return result;
    }

    // this is reached if the top node in the stack does not have 'name' in its hash table
    
    // for debugging:
    printf("FAILED to find symbol bound to key \"%s\" in TOP hash table!\n", name);
    // end debugging
    return NULL;
} 