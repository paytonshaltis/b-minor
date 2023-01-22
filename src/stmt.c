/*  all code in this file is original, and was written by:
*  
*   PAYTON JAMES SHALTIS
*   COMPLETED MAY 4TH, 2021
*
*			for
*
*	B-MINOR COMPILER, v1.0
*
*
*   in CSC-425: "Compilers and Interpreters" taught by Professor John DeGood,
*   over the course of the Spring 2021 semester. I understand that keeping this
*   code in a public repository may allow other students to have access. In the
*   event that the course is taught again, with a similar project component, this 
*   code is NOT to be used in place of another student's work.
*
*
*
*                                   'stmt.c'
*                                   --------
*   This is the implementation file for all functions for the 'stmt' AST nodes. It
*   includes a heavily commented breakdown of each function and how it works, which
*   serve as great debugging elements and descriptions of how the compiler works.
*
*/

#include "stmt.h"


// variables used by the different statement functions below
extern int totalResErrors;          // keeps track of the total number of resolution errors (SOURCE: 'decl.c')
extern int totalTypeErrors;         // keeps track of the total number of typechecking errors (SOURCE: 'decl.c')
extern int callStackSize;           // the size in bytes of a function's call stack (SOURCE: 'decl.c')
extern FILE* fp;                    // file pointer to the output file for assembly code (SOURCE: 'main.c')

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

// performs typechecking on the elements of a statement
void stmt_typecheck(struct stmt* s, struct type* ft) {
    
    // base case for recursion
    if(s == NULL) {
        return;
    }

    // temporary structures used for statement traversal in the switch below
    struct type* t;             // temporary type; used to store results of typechecks
    struct expr* temp;          // temporary expression; used to traverse complex statements
    struct type* printType;     // temporary type; used to store the type of an expression being printed
    
    // switch for all of the different statement kinds
    switch(s->kind) {
        
        // for expression statements
        case STMT_EXPR:
            t = expr_typecheck(s->expr);
            type_delete(t);
            break;
        
        // for if statements
        case STMT_IF:
            t = expr_typecheck(s->expr);
            
            // the condition should always be a boolean type
            if(t->kind != TYPE_BOOLEAN) {
                printf("\033[0;31mtypechecking error\033[0;0m: condition of 'if' statement must return boolean type\n");
                totalTypeErrors++;
                break;
            }
            type_delete(t);
            stmt_typecheck(s->body, ft);
            break;
        
        // for if-else statements
        case STMT_IF_ELSE:
            t = expr_typecheck(s->expr);
            
            // the condition should always be a boolean type
            if(t->kind != TYPE_BOOLEAN) {
                printf("\033[0;31mtypechecking error\033[0;0m: condition of 'if' statement must return boolean type\n");
                totalTypeErrors++;
                break;
            }
            type_delete(t);
            stmt_typecheck(s->body, ft);
            stmt_typecheck(s->else_body, ft);
            break;
        
        // for 'for-loops'
        case STMT_FOR:
            if(s->init_expr != NULL) {
                t = expr_typecheck(s->init_expr);
                
                // the initial expression should be an integer type
                if(t->kind != TYPE_INTEGER) {
                    printf("\033[0;31mtypechecking error\033[0;0m: first expression in 'for-loop' must return integer type\n");
                    totalTypeErrors++;
                    break;
                }
                type_delete(t);
            }
            if(s->expr != NULL) {
                t = expr_typecheck(s->expr);
                
                // the conditional expression must be a boolea ntype
                if(t->kind != TYPE_BOOLEAN) {
                    printf("\033[0;31mtypechecking error\033[0;0m: second expression in 'for-loop' must return boolean type\n");
                    totalTypeErrors++;
                    break;
                }
                type_delete(t);
            }
            if(s->next_expr != NULL) {
                t = expr_typecheck(s->next_expr);
                
                // the next expression should be an integer type
                if(t->kind != TYPE_INTEGER) {
                    printf("\033[0;31mtypechecking error\033[0;0m: third expression in 'for-loop' must return integer type\n");
                    totalTypeErrors++;
                    break;
                }
                type_delete(t);
            }
            stmt_typecheck(s->body, ft);
            break;
        
        // for print statements
        case STMT_PRINT:
            // need to check each expression being printed to make sure they are a type
            
            // if only one expression is being printed:
            if(s->expr->kind != EXPR_ARGS) {
                
                // set temp to that expression
                temp = s->expr;
                
                // typecheck the expression and print if it isn't the right type
                printType = expr_typecheck(temp);
                if(printType->kind == TYPE_FUNCTION || printType->kind == TYPE_ARRAY || printType->kind == TYPE_VOID || printType->kind == TYPE_PROTOTYPE) {
                    printf("\033[0;31mtypechecking error\033[0;0m: cannot print type (");
                    type_print(printType);
                    printf(")\n");
                    totalTypeErrors++;
                    break;
                }
            }
            // if more than one expression is being printed:
            else {
                temp = s->expr;

                // loop while there are more than two expressions left to check
                while(temp->right->kind == EXPR_ARGS) {

                    // make sure the expression type is not function, array, or void
                    printType = expr_typecheck(temp->left);
                    if(printType->kind == TYPE_FUNCTION || printType->kind == TYPE_ARRAY || printType->kind == TYPE_VOID || printType->kind == TYPE_PROTOTYPE) {
                        printf("\033[0;31mtypechecking error\033[0;0m: cannot print type (");
                        type_print(printType);
                        printf(")\n");
                        totalTypeErrors++;
                    }
                    temp = temp->right;
                }

                // when there are two expression left to check, check left and right individually
                printType = expr_typecheck(temp->left);
                if(printType->kind == TYPE_FUNCTION || printType->kind == TYPE_ARRAY || printType->kind == TYPE_VOID || printType->kind == TYPE_PROTOTYPE) {
                    printf("\033[0;31mtypechecking error\033[0;0m: cannot print type (");
                    type_print(printType);
                    printf(")\n");
                    totalTypeErrors++;
                }
                printType = expr_typecheck(temp->right);
                if(printType->kind == TYPE_FUNCTION || printType->kind == TYPE_ARRAY || printType->kind == TYPE_VOID || printType->kind == TYPE_PROTOTYPE) {
                    printf("\033[0;31mtypechecking error\033[0;0m: cannot print type (");
                    type_print(printType);
                    printf(")\n");
                    totalTypeErrors++;
                }
            }
            break;

        // for return statements
        case STMT_RETURN:
            
            // void function returning some value
            if(ft->kind == TYPE_VOID && s->expr != NULL) {
                printf("\033[0;31mtypechecking error\033[0;0m: void function must not return value\n");
                totalTypeErrors++;
                break;
            }
            
            // non-void function not returning a value
            else if (ft->kind != TYPE_VOID && s->expr == NULL) {
                printf("\033[0;31mtypechecking error\033[0;0m: non-void function type (");
                type_print(ft);
                printf(") must return a value\n");
                totalTypeErrors++;
                break;
            }

            // functions that return other types
            else if(s->expr != NULL && expr_typecheck(s->expr)->kind != ft->kind) {
                printf("\033[0;31mtypechecking error\033[0;0m: returned type does not match function type (");
                type_print(ft);
                printf(")\n");
                totalTypeErrors++;
                break;
            }
            break;

        // for block statements
        case STMT_BLOCK:
            stmt_typecheck(s->body, ft);
            break;

        // for declaration statements
        case STMT_DECL:
            decl_typecheck(s->decl);
    }

    // typecheck the next statement in the statement list
    stmt_typecheck(s->next, ft);
}

// generate ARM assembly code for a statement structure
void stmt_codegen(struct stmt* s) {
    
    // variables used in the statement kind switch below
    struct type* t;             // temporary type; stores the type of an expression to print
    struct expr* exprtemp;      // temporary expression; used to traverse expressions to print
    int elseLabel;              // label for if-else statements; the else label
    int doneLabel;              // label for if-else, if, for-loop statements; the done label
    int loopLabel;              // label for for-loop statements; the loop label

    //final statement in a statement list should return
    if(s == NULL) {
        return;
    }

    // switches for all kinds of statements
    switch(s->kind) {

        // for expression statements
        case STMT_EXPR:
            expr_codegen(s->expr);
            scratch_free(s->expr->reg);
        break;

        // for declaration statements
        case STMT_DECL:
            decl_codegen(s->decl);
        break;

        // for print statements
        case STMT_PRINT:
            
            // see if print has an expression attatched
            if(s->expr == NULL) {
                break;
            }

            // for only a single expression to print
            if(s->expr->kind != EXPR_ARGS) {
                t = expr_typecheck(s->expr);

                // check the type and branch to the appropriate function after loading the value from
                // some register (from expr_codegen()) into register x0
                if(t->kind == TYPE_INTEGER) {
                    expr_codegen(s->expr);
                    fprintf(fp, "\t\tmov\tx0, %s\n", scratch_name(s->expr->reg));
                    fprintf(fp, "\t\tbl\tprint_integer\n");
                    scratch_free(s->expr->reg);
                }
                if(t->kind == TYPE_STRING) {                                                         
                    expr_codegen(s->expr);
                    fprintf(fp, "\t\tmov\tx0, %s\n", scratch_name(s->expr->reg));
                    fprintf(fp, "\t\tbl\tprint_string\n");
                    scratch_free(s->expr->reg);
                }
                if(t->kind == TYPE_CHAR) {
                    expr_codegen(s->expr);
                    fprintf(fp, "\t\tmov\tx0, %s\n", scratch_name(s->expr->reg));
                    fprintf(fp, "\t\tbl\tprint_character\n");
                    scratch_free(s->expr->reg);
                }
                if(t->kind == TYPE_BOOLEAN) {
                    expr_codegen(s->expr);
                    fprintf(fp, "\t\tmov\tx0, %s\n", scratch_name(s->expr->reg));
                    fprintf(fp, "\t\tbl\tprint_boolean\n");
                    scratch_free(s->expr->reg);
                }

            }

            // for more than one expressions to print
            else {
                
                // navigate through all of the expressions to print
                exprtemp = s->expr;
                while(exprtemp->right->kind == EXPR_ARGS) {
                    
                    // branch to the correct function for each argument visited
                    t = expr_typecheck(exprtemp->left);

                    if(t->kind == TYPE_INTEGER) {
                        expr_codegen(exprtemp->left);
                        fprintf(fp, "\t\tmov\tx0, %s\n", scratch_name(exprtemp->left->reg));
                        fprintf(fp, "\t\tbl\tprint_integer\n");
                        scratch_free(exprtemp->left->reg);
                    }
                    if(t->kind == TYPE_STRING) {                                                         
                        expr_codegen(exprtemp->left);
                        fprintf(fp, "\t\tmov\tx0, %s\n", scratch_name(exprtemp->left->reg));
                        fprintf(fp, "\t\tbl\tprint_string\n");
                        scratch_free(exprtemp->left->reg);
                    }
                    if(t->kind == TYPE_CHAR) {
                        expr_codegen(exprtemp->left);
                        fprintf(fp, "\t\tmov\tx0, %s\n", scratch_name(exprtemp->left->reg));
                        fprintf(fp, "\t\tbl\tprint_character\n");
                        scratch_free(exprtemp->left->reg);
                    }
                    if(t->kind == TYPE_BOOLEAN) {
                        expr_codegen(exprtemp->left);
                        fprintf(fp, "\t\tmov\tx0, %s\n", scratch_name(exprtemp->left->reg));
                        fprintf(fp, "\t\tbl\tprint_boolean\n");
                        scratch_free(exprtemp->left->reg);
                    }

                    exprtemp = exprtemp->right;
                }

                // we have to do the last two manually here:
                t = expr_typecheck(exprtemp->left);
                if(t->kind == TYPE_INTEGER) {
                    expr_codegen(exprtemp->left);
                    fprintf(fp, "\t\tmov\tx0, %s\n", scratch_name(exprtemp->left->reg));
                    fprintf(fp, "\t\tbl\tprint_integer\n");
                    scratch_free(exprtemp->left->reg);
                }
                if(t->kind == TYPE_STRING) {                                                        
                    expr_codegen(exprtemp->left);
                    fprintf(fp, "\t\tmov\tx0, %s\n", scratch_name(exprtemp->left->reg));
                    fprintf(fp, "\t\tbl\tprint_string\n");
                    scratch_free(exprtemp->left->reg);
                }
                if(t->kind == TYPE_CHAR) {
                    expr_codegen(exprtemp->left);
                    fprintf(fp, "\t\tmov\tx0, %s\n", scratch_name(exprtemp->left->reg));
                    fprintf(fp, "\t\tbl\tprint_character\n");
                    scratch_free(exprtemp->left->reg);
                }
                if(t->kind == TYPE_BOOLEAN) {
                    expr_codegen(exprtemp->left);
                    fprintf(fp, "\t\tmov\tx0, %s\n", scratch_name(exprtemp->left->reg));
                    fprintf(fp, "\t\tbl\tprint_boolean\n");
                    scratch_free(exprtemp->left->reg);
                }

                t = expr_typecheck(exprtemp->right);
                if(t->kind == TYPE_INTEGER) {
                    expr_codegen(exprtemp->right);
                    fprintf(fp, "\t\tmov\tx0, %s\n", scratch_name(exprtemp->right->reg));
                    fprintf(fp, "\t\tbl\tprint_integer\n");
                    scratch_free(exprtemp->right->reg);
                }
                if(t->kind == TYPE_STRING) {                                                        
                    expr_codegen(exprtemp->right);
                    fprintf(fp, "\t\tmov\tx0, %s\n", scratch_name(exprtemp->right->reg));
                    fprintf(fp, "\t\tbl\tprint_string\n");
                    scratch_free(exprtemp->right->reg);
                }
                if(t->kind == TYPE_CHAR) {
                    expr_codegen(exprtemp->right);
                    fprintf(fp, "\t\tmov\tx0, %s\n", scratch_name(exprtemp->right->reg));
                    fprintf(fp, "\t\tbl\tprint_character\n");
                    scratch_free(exprtemp->right->reg);
                }
                if(t->kind == TYPE_BOOLEAN) {
                    expr_codegen(exprtemp->right);
                    fprintf(fp, "\t\tmov\tx0, %s\n", scratch_name(exprtemp->right->reg));
                    fprintf(fp, "\t\tbl\tprint_boolean\n");
                    scratch_free(exprtemp->right->reg);
                }
            }

        break;

        // for block statements
        case STMT_BLOCK:
            
            // just generate code for what is in the body
            stmt_codegen(s->body);
            
        break;

        // for return statements
        case STMT_RETURN:

            // if there is an expression to be returned
            if(s->expr != NULL) {
                
                // generate code to get the return value into a register
                expr_codegen(s->expr);
                
                // print the code to move register contents above into x0
                fprintf(fp, "\t\tmov\tx0, %s\n", scratch_name(s->expr->reg));

                // free the register used to move the return value
                scratch_free(s->expr->reg);
            }

            // whether returning a value or not, we need to shrink stack and return
            fprintf(fp, "\t\tldp\tx29, x30, [sp], #%i\n\t\tret\n", callStackSize);

        break;

        // for 'if' statements
        case STMT_IF:

            // generate the code to get the expression result into a register
            expr_codegen(s->expr);

            // create a done label
            doneLabel = stmt_label_create();

            // compare the result of the expression
            fprintf(fp, "\t\tcmp\t%s, 0\n", scratch_name(s->expr->reg));
            
            // if the expression is false, jump to done
            fprintf(fp, "\t\tb.eq\t%s\n", stmt_label_name(doneLabel));

            // if the expression is true, generate code for the expression
            stmt_codegen(s->body);

            // print the done label after the body of the statement
            fprintf(fp, "\t%s:\n", stmt_label_name(doneLabel));

            // free the scratch register used
            scratch_free(s->expr->reg);

        break;

        // for 'if' statements
        case STMT_IF_ELSE:

            // generate the code to get the expression result into a register
            expr_codegen(s->expr);

            // create an else and done label
            elseLabel = stmt_label_create();
            doneLabel = stmt_label_create();

            // compare the result of the expression
            fprintf(fp, "\t\tcmp\t%s, 0\n", scratch_name(s->expr->reg));
            
            // if the expression is false, jump to else
            fprintf(fp, "\t\tb.eq\t%s\n", stmt_label_name(elseLabel));

            // if the expression is true, generate code for the expression
            stmt_codegen(s->body);

            // jump to the done label once done with 'if' body
            fprintf(fp, "\t\tb\t%s\n", stmt_label_name(doneLabel));

            // print the else label after the body of the statement
            fprintf(fp, "\t%s:\n", stmt_label_name(elseLabel));

            // print the else body for if the expression is false
            stmt_codegen(s->else_body);

            // print the done label so that 'if' body can skip 'else' body
            fprintf(fp, "\t%s:\n", stmt_label_name(doneLabel));

            // free the scratch register used
            scratch_free(s->expr->reg);

        break;

        // for for-loop statements
        case STMT_FOR:

            // create a label for the top of the loop and for escaping when done
            loopLabel = stmt_label_create();
            doneLabel = stmt_label_create();

            // generate code for the initial expression if it exists
            if(s->init_expr) {
                expr_codegen(s->init_expr);
                scratch_free(s->init_expr->reg);
            }

            // print the label for the top of the loop
            fprintf(fp, "\t%s:\n", stmt_label_name(loopLabel));

            // should only be done if the loop has a middle expression
            if(s->expr != NULL) {
            
                // generate code for the expression to be checked
                expr_codegen(s->expr);

                // check to see if the expression is false
                fprintf(fp, "\t\tcmp\t%s, 0\n", scratch_name(s->expr->reg));

                // if so, jump to the done label outside of the loop
                fprintf(fp, "\t\tb.eq\t%s\n", stmt_label_name(doneLabel));

                // free the scratch register used
                scratch_free(s->expr->reg);
            }

            // print the body of the loop
            stmt_codegen(s->body);

            // generate code for the 'next' expression if it exists
            if(s->next_expr != NULL) {
                expr_codegen(s->next_expr);
                scratch_free(s->next_expr->reg);
            }

            // jump back to the top of the loop
            fprintf(fp, "\t\tb\t%s\n", stmt_label_name(loopLabel));

            // print the done label for escaping the loop
            fprintf(fp, "\t%s:\n", stmt_label_name(doneLabel));

        break;

    }

    // generate code for the next statement
    stmt_codegen(s->next);

}