#include <stdio.h>
#include <stdlib.h>
#include "stmt.h"

// basic factory function for creating a 'stmt' struct
struct stmt * stmt_create( stmt_t kind, struct decl *decl, struct expr *init_expr, struct expr *expr, struct expr *next_expr, struct stmt *body, struct stmt *else_body, struct stmt *next ) {
    struct stmt *s = malloc(sizeof(*s));
    s->kind = kind;
    s->decl = decl;
    s->init_expr = init_expr;
    s->expr = expr;
    s->next_expr = next_expr;
    s->body = body;
    s->else_body = else_body;
    s->next = next;
    return s;
}

// printing function for use by pretty printer
void stmt_print(struct stmt *s, int indent) {
   
    // indents to proper tabstop
    for(int i = 0; i < indent; i++) {
        printf("    ");
    }
    
    // print statements
    if(s->kind == STMT_PRINT) {
        printf("print");
        
        // only prints if something follows "print"
        if(s->expr != NULL) {
            printf(" ");
            expr_print(s->expr);
        }
        printf(";\n");
    }

    // return statements
    if(s->kind == STMT_RETURN) {
        printf("return");
        
        // only prints if something follows "return"
        if(s->expr != NULL) {
            printf(" ");
            expr_print(s->expr);
        }
        printf(";\n");
    }

    // declaration statements
    if(s->kind == STMT_DECL) {
        decl_print(s->decl, indent);
    }

    // expression statements
    if(s->kind == STMT_EXPR) {
        expr_print(s->expr);
        printf(";\n");
    }

    // if statements
    if(s->kind == STMT_IF) {
        printf("if (");
        expr_print(s->expr);
        printf(")\n");
        
        // ensures proper tabbing for block vs. single statements
        if(s->body->kind != STMT_BLOCK) {
            stmt_print(s->body, indent + 1);
        }
        else {
            stmt_print(s->body, indent);
        }
    }

    // if-else statements
    if(s->kind == STMT_IF_ELSE) {
        printf("if (");
        expr_print(s->expr);
        printf(")\n");
        
        //ensures proper tabbing for block vs. single statements
        if(s->body->kind != STMT_BLOCK) {
            stmt_print(s->body, indent + 1);
        }
        else {
            stmt_print(s->body, indent);
        }
        
        // ensures proper tabbing for else 
        for(int i = 0; i < indent; i++) {
            printf("    ");
        }   
        
        printf("else\n");
        
        // ensures proper tabbing for block vs. single statements
        if(s->else_body->kind != STMT_BLOCK) {
            stmt_print(s->else_body, indent + 1);
        }
        else {
            stmt_print(s->else_body, indent);
        }
    }

    // for statements
    if(s->kind == STMT_FOR) {
        printf("for (");
        
        // print each for-loop expression if it exists
        if(s->init_expr != NULL) {
            expr_print(s->init_expr);
        }
        printf(";");
        if(s->expr != NULL) {
            expr_print(s->expr);
        }
        printf(";");
        if(s->next_expr != NULL) {
            expr_print(s->next_expr);
        }
        printf(")\n");

        //print the body w/ proper indenting
        if(s->body->kind != STMT_BLOCK) {
            stmt_print(s->body, indent + 1);
        }
        else {
            stmt_print(s->body, indent);
        }
    }

    // for block statements
    if(s->kind == STMT_BLOCK) {
        printf("{\n");
        stmt_print(s->body, indent + 1);
        
        //ensures proper tabbing for closing curly brace
        for(int i = 0; i < indent; i++) {
            printf("    ");
        }

        printf("}\n");
    }

    // prints the next statement in the list
    if(s->next != NULL) {
        stmt_print(s->next, indent);
    }
    
}