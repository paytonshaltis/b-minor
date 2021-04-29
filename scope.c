#include "scope.h"
#include "hash_table.h"
#include "symbol.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

extern int counter;

// set DEBUG to true to see debugging messages, false to turn them off
bool DEBUG = false;

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

    if(DEBUG == true) {
        printf("Successfully ENTERED a new scope!\n");
    }
    return;
}

// pops the top node from the stack, deleting the hash table associated with it in the process
void scope_exit() {

    // create a pointer to point to the top node of the stack
    struct stack_node* toBeDeleted = theStackTop;

    // if the stack is already empty
    if(theStackTop == NULL) {
        
        if(DEBUG == true) {
            printf("ERROR: The stack is already EMPTY!\n");
        }
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

    if(DEBUG == true) {
        printf("Successfully EXITED a scope!\n");
    }
    return;
}

// returns the number of hash tables in the stack; used to determine if we are in the global scope or not
int scope_level() {
    
    // temporary node to keep track of position in stack
    struct stack_node* temp = theStackTop;

    // if the top of the stack does not point to a stack node, the stack is empty
    if(temp == NULL) {
        
        if(DEBUG == true) {
            printf("ERROR: Scope at LEVEL 0!\n");
        }
        return 0;
    }

    // otherwise, we must traverse the stack and accumulate a total:
    int total = 1;
    while(temp->next != NULL) {
        total++;
        temp = temp->next;
    }

    if(DEBUG == true) {
        printf("Scope at LEVEL %i\n", total);
    }
    return total;
}

// adds an entry to the top-most hash table, mapping name to the symbol structure sym
void scope_bind(const char* name, struct symbol* sym) {

    // if for some reason there is no scope in the stack
    if(theStackTop == NULL) {
        
        if(DEBUG == true) {
            printf("ERROR: Could not bind because the stack is EMPTY!\n");
        }
        return;
    }

    // otherwise, we insert the (name, symbol) mapping into the hash table of the top-most node of the stack
    // we also must assign the 'which' field based on the current scope's count
    sym->which = counter;
    counter++;
    
    hash_table_insert(theStackTop->table, name, sym);



    if(DEBUG == true) {
        printf("Successfully bound key \"%s\" with symbol named \"%s\"!\n", name, sym->name);
        
        // print the stack address after binding to make sure it works properly
        printf("Stack Address: %s\n", symbol_codegen(sym));
    }
    return;
}

// searches the stack of hash tables from top to bottom, looking for the first entry matching 'name'. Returns NULL if not found
struct symbol* scope_lookup(const char* name) {
    
    // temporary node and symbol to be returned
    struct stack_node* temp = theStackTop;
    struct symbol* result;
    
    // if the stack is somehow empty, return NULL
    if(temp == NULL) {

        if(DEBUG == true) {
            printf("ERROR: The stack is EMPTY, could not find!\n");
        }
        return NULL;
    }
    
    // otherwise, search the hash table in each node, return the symbol struct if found
    while(temp != NULL) {
        
        // looks up the name in the hash table of this node, returns it if found
        result = hash_table_lookup(temp->table, name);
        if(result != NULL) {
            
            if(DEBUG == true) {
                printf("Successfully found symbol named \"%s\" which was bound to key \"%s\" in ANY hash table!\n", result->name, name);
            }
            return result;
        }
        
        // otherwise, move to the next node
        temp = temp->next;
        if(DEBUG == true) {
            printf("    checking next table...\n");
        }
    }

    // this is reached if no node in the stack has a hash table containing the key
    if(DEBUG == true) {
        printf("FAILED to find symbol bound to key \"%s\" in ANY hash table!\n", name);
    }
    return NULL;

}

// searches the top-most node in the stack to see if an identifier has already been defined in this scope
struct symbol* scope_lookup_current(const char* name) {

    // symbol to be returned
    struct symbol* result;

    // if the stack is somehow empty, return NULL
    if(theStackTop == NULL) {

        if(DEBUG == true) {
            printf("ERROR: The stack is EMPTY, could not find!\n");       
        }
        return NULL;
    }

    // otherwise, see if the hash table in the top node has 'name' in its hash table
    result = hash_table_lookup(theStackTop->table, name);
    if(result != NULL) {
        
        if(DEBUG == true) {
            printf("Successfully found symbol named \"%s\" which was bound to key \"%s\" in TOP hash table!\n", result->name, name);
        }
        return result;
    }

    // this is reached if the top node in the stack does not have 'name' in its hash table
    if(DEBUG == true) {
        printf("FAILED to find symbol bound to key \"%s\" in TOP hash table!\n", name);
    }
    return NULL;
} 

// unbinds the key 'key' by removing it from the top hash table (the current scope)
bool scope_unbind(const char* key) {

    // if the stack is empty, return false;
    if(theStackTop == NULL) {
        
        if(DEBUG == true) {
            printf("ERROR: Could not unbind \"%s\", the stack is empty!\n", key);
        }
        return false;
    }
    
    // the associated symbol object returns from an hash_remove()
    struct symbol* garbageScope = hash_table_remove(theStackTop->table, key);

    // if it was successful (symbol struct returned) return true, unbind successful
    if(garbageScope != NULL) {
        
        if(DEBUG == true) {
            printf("Unbounded symbol previously bound to key \"%s\" in TOP hash table!\n", key);
        }
        return true;
    }

    // otherwise, the pair [key, sym] was not found in the top hash table (current scope)
    
    if(DEBUG == true) {
        printf("FAILED to find symbol bound to key \"%s\" in TOP hash table!\n", key);
    }
    return false;
}