#include <stdio.h>
#include <stdlib.h>
#include "stmt.h"
#include "scope.h"

extern int totalResErrors;
extern int totalTypeErrors;

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

// conducts name resolution on statement s
void stmt_resolve(struct stmt* s) {

    // base case for recursion
    if(s == NULL) {
        return;
    }

    // all of the different statement types that need to call other resolution functions
    if(s->kind == STMT_DECL) {
        decl_resolve(s->decl);
    }
    if(s->kind == STMT_EXPR) {
        expr_resolve(s->expr);
    }
    if(s->kind == STMT_IF) {
        expr_resolve(s->expr);
        stmt_resolve(s->body);
    }
    if(s->kind == STMT_IF_ELSE) {
        expr_resolve(s->expr);
        stmt_resolve(s->body);
        stmt_resolve(s->else_body);
    }
    if(s->kind == STMT_FOR) {
        expr_resolve(s->init_expr);
        expr_resolve(s->expr);
        expr_resolve(s->next_expr);
        stmt_resolve(s->body);
    }
    if(s->kind == STMT_PRINT) {
        expr_resolve(s->expr);
    }
    if(s->kind == STMT_RETURN) {
        expr_resolve(s->expr);
    }
    if(s->kind == STMT_BLOCK) {
        // for block statements, we must be sure to enter a new scope!
        scope_enter();
        stmt_resolve(s->body);
        scope_exit();
    }

    // resolve the next statement in the statement list
    stmt_resolve(s->next);
}

void stmt_typecheck(struct stmt* s, struct type* ft) {
    
    if(s == NULL) {
        return;
    }

    struct type* t;
    switch(s->kind) {
        case STMT_EXPR:
            t = expr_typecheck(s->expr);
            type_delete(t);
            break;
        case STMT_IF:
            t = expr_typecheck(s->expr);
            if(t->kind != TYPE_BOOLEAN) {
                printf("typechecking error: condition of 'if' statement must return boolean type\n");
                totalTypeErrors++;
                break;
            }
            type_delete(t);
            stmt_typecheck(s->body, ft);
            break;
        case STMT_IF_ELSE:
            t = expr_typecheck(s->expr);
            if(t->kind != TYPE_BOOLEAN) {
                printf("typechecking error: condition of 'if' statement must return boolean type\n");
                totalTypeErrors++;
                break;
            }
            type_delete(t);
            stmt_typecheck(s->body, ft);
            stmt_typecheck(s->else_body, ft);
            break;
        case STMT_FOR:
            if(s->init_expr != NULL) {
                t = expr_typecheck(s->init_expr);
                if(t->kind != TYPE_INTEGER) {
                    printf("typechecking error: first expression in 'for-loop' must return integer type\n");
                    totalTypeErrors++;
                    break;
                }
            }
            type_delete(t);
            if(s->expr != NULL) {
                t = expr_typecheck(s->expr);
                if(t->kind != TYPE_BOOLEAN) {
                    printf("typechecking error: second expression in 'for-loop' must return boolean type\n");
                    totalTypeErrors++;
                    break;
                }
            }
            type_delete(t);
            if(s->next_expr != NULL) {
                t = expr_typecheck(s->next_expr);
                if(t->kind != TYPE_INTEGER) {
                    printf("typechecking error: third expression in 'for-loop' must return integer type\n");
                    totalTypeErrors++;
                    break;
                }
            }
            type_delete(t);
            stmt_typecheck(s->body, ft);
            break;
        case STMT_PRINT:
            expr_typecheck(s->expr);
            break;
        case STMT_RETURN:
            // void function returning some value
            if(ft->kind == TYPE_VOID && s->expr != NULL) {
                printf("typechecking error: void function must not return value\n");
                totalTypeErrors++;
                break;
            }
            // non-void function not returning a value
            else if (ft->kind != TYPE_VOID && s->expr == NULL) {
                printf("typechecking error: non-void function must return a value\n");
                totalTypeErrors++;
                break;
            }
            // functions that return other types
            else if(expr_typecheck(s->expr)->kind != ft->kind) {
                printf("typechecking error: return type does not match function subtype\n");
                totalTypeErrors++;
                break;
            }
            break;
        case STMT_BLOCK:
            stmt_typecheck(s->body, ft);
            break;
        case STMT_DECL:
            decl_typecheck(s->decl);
    }
    stmt_typecheck(s->next, ft);
}