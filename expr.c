#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "expr.h"
#include "scope.h"
#include "symbol.h"
#include "scratch.h"
#include "label.h"

extern int totalResErrors;
extern int totalTypeErrors;
extern char localsTP[256][300];
extern int localsTPCounter;
extern int callStackSize;

// basic factory function for creating an 'expr' struct (basic expression)
struct expr * expr_create( expr_t kind, struct expr *left, struct expr *right ) {
    struct expr *e = malloc(sizeof(*e));
    e->kind = kind;
    e->left = left;
    e->right = right;
    return e;
}

// basic factory function for creating an 'expr' struct (identifier)
struct expr * expr_create_name( const char *n ) {
    struct expr *e = expr_create(EXPR_NAME, 0, 0);
    e->name = n;
    return e;
}

// basic factory function for creating an 'expr' struct (integer literal)
struct expr * expr_create_integer_literal( int c ) {
    struct expr *e = expr_create(EXPR_INTLIT, 0, 0);
    e->literal_value = c;
    return e;
}

// basic factory function for creating an 'expr' struct (boolean literal)
struct expr * expr_create_boolean_literal( int c ) {
    struct expr *e = expr_create(EXPR_BOOLLIT, 0, 0);
    e->literal_value = c;
    return e;
}

// basic factory function for creating an 'expr' struct (char literal)
struct expr * expr_create_char_literal( char c ) {
    struct expr *e = expr_create(EXPR_CHARLIT, 0, 0);
    e->literal_value = c;
    return e;
}

// basic factory function for creating an 'expr' struct (string literal)
struct expr * expr_create_string_literal( const char *str ) {
    struct expr *e = expr_create(EXPR_STRINGLIT, 0, 0);
    e->string_literal = str;
    return e;
}

// returns the precedence level of an expression; greater number means a greater precedence level
int precedence(struct expr* e) {

    if(e->kind == EXPR_ASSIGN) {
        return 1;
    }    
    if(e->kind == EXPR_OR) {
        return 2;
    }
    if(e->kind == EXPR_AND) {
        return 3;
    }
    if(e->kind == EXPR_LESS || e->kind == EXPR_LE || e->kind == EXPR_GREATER || e->kind == EXPR_GE || e->kind == EXPR_EQUAL || e->kind == EXPR_NEQUAL) {
        return 4;
    }
    if(e->kind == EXPR_ADD || e->kind == EXPR_SUB) {
        return 5;
    }
    if(e->kind == EXPR_MULT || e->kind == EXPR_DIV || e->kind == EXPR_MOD) {
        return 6;
    }
    if(e->kind == EXPR_EXPON) {
        return 7;
    }
    if(e->kind == EXPR_NOT || e->kind == EXPR_NEG) {
        return 8;
    }
    if(e->kind == EXPR_INC || e->kind == EXPR_DEC) {
        return 9;
    }
    if(e->kind == EXPR_ARRIND || e->kind == EXPR_FCALL || e->kind == EXPR_GROUP) {
        return 10;
    }

    return -1;
}

// returns 1 if the expression t is an atomic type or some unary expression (can be removed from parenthesis if solo, for example: (var++) --> var++)
int unaryExpr(expr_t t) {

    if(t == EXPR_INTLIT || t == EXPR_STRINGLIT || t == EXPR_CHARLIT || t == EXPR_BOOLLIT || t == EXPR_NAME || t == EXPR_INC || t == EXPR_DEC || t == EXPR_NEG || t == EXPR_NOT) {
        return 1;
    }

    return 0;
}

// printing function for use by the pretty printer
void expr_print(struct expr *e) {

    /* for each expression, first consider the left and right sides. NOTE: single groups are not affected here, 
    they are dealt with in the expr_print for 'group' section below, as well as nested groups! */
    
    /* if the left side of an expression is a group, do the following: */
    if(e->left != NULL && e->left->kind == EXPR_GROUP) {
        
        /* if the precedence of the expression within the group is greater than OR EQUAL TO the precedence of the outer expression 
        (we include equal to because order of ops prioritizes expressions left to right, so no need for parens if left is equal priority!)*/
        if(precedence(e->left->left) >= precedence(e)) {

            /* we can extract the expression from the group, replacing e->left */
            e->left = e->left->left;

        }
    }

    /* if the right side of an expression is a group, do the following: */
    if(e->right != NULL && e->right->kind == EXPR_GROUP) {
        
        /* if the precedence of the expression within the group is greater than the precedence of the outer expression */
        if(precedence(e->right->left) > precedence(e)) {

            /* we can extract the expression from the group, replacing e->right */
            e->right = e->right->left;
            
        }
    }

    /* after expressions have been extracted from groups if necessary, we can print normally */

    if(e->kind == EXPR_NAME) {
        printf("%s", e->name);
    }
    if(e->kind == EXPR_INTLIT) {
        printf("%i", e->literal_value);
    }
    if(e->kind == EXPR_BOOLLIT) {
        if(e->literal_value == 0) {
            printf("false");
        }
        else
            printf("true");
    }
    if(e->kind == EXPR_STRINGLIT) {

        // go character by character, and print escape sequences literally if found
        for(int i = 0; i < strlen(e->string_literal); i++) {
            if(e->string_literal[i] == 10) {
                printf("\\n");
            }
            else if(e->string_literal[i] == 0) {
                printf("\\0");
            }
            else{
                printf("%c", e->string_literal[i]);
            }
        }
    }
    if(e->kind == EXPR_CHARLIT) {
        printf("\'");
        
        // print escape sequences literally if found
        if(e->literal_value == 10) {
            printf("\\n");
        }
        else if(e->literal_value == 0) {
            printf("\\0");
        }
        else{
            printf("%c", e->literal_value);
        }
        printf("\'");
    }
    if(e->kind == EXPR_FCALL) {
        expr_print(e->left);
        printf("(");
        
        // in case the function call requires no parameters
        if(e->right != NULL) {
            expr_print(e->right);
        }
        printf(")");
    }
    if(e->kind == EXPR_GROUP) {
        
        /* no need for more than one set of parens (((((like this))))) */
        if(e->left->kind == EXPR_GROUP) {
            expr_print(e->left);
        }
        
        /* no need for parens around a single term / unary expression */
        else if(unaryExpr(e->left->kind) == 1) {
            expr_print(e->left);
        }

        /* if parens are absolutely required */
        else {
            printf("(");
            expr_print(e->left);
            printf(")");
        }
    }
    if(e->kind == EXPR_ARRIND) {
        expr_print(e->left);
        printf("[");
        expr_print(e->right);
        printf("]");
    }
    if(e->kind == EXPR_BRACKET) {
        expr_print(e->left);
        printf("]");
        if(e->right != NULL) {
            printf("[");
            expr_print(e->right);
        }
    }
    if(e->kind == EXPR_CURLS) {
        printf("{");
        if(e->left != NULL) {
            expr_print(e->left);
        }
        printf("}");
    }
    if(e->kind == EXPR_ARGS) {
        expr_print(e->left);
        if(e->right != NULL) {
            printf(", ");
            expr_print(e->right);
        }
    }
    if(e->kind == EXPR_INC) {
        expr_print(e->left);
        printf("++");
    }
    if(e->kind == EXPR_DEC) {
        expr_print(e->left);
        printf("--");
    }
    if(e->kind == EXPR_NOT) {
        printf("!");
        expr_print(e->left);
    }
    if(e->kind == EXPR_NEG) {
        printf("-");
        expr_print(e->left);
    }
    if(e->kind == EXPR_EXPON) {
        expr_print(e->left);
        printf("^");
        expr_print(e->right);
    }
    if(e->kind == EXPR_MOD) {
        expr_print(e->left);
        printf("%%");
        expr_print(e->right);
    }
    if(e->kind == EXPR_DIV) {
        expr_print(e->left);
        printf("/");
        expr_print(e->right);
    }
    if(e->kind == EXPR_MULT) {
        expr_print(e->left);
        printf("*");
        expr_print(e->right);
    }
    if(e->kind == EXPR_SUB) {
        expr_print(e->left);
        printf("-");
        expr_print(e->right);
    }
    if(e->kind == EXPR_ADD) {
        expr_print(e->left);
        printf("+");
        expr_print(e->right);
    }
    if(e->kind == EXPR_NEQUAL) {
        expr_print(e->left);
        printf("!=");
        expr_print(e->right);
    }
    if(e->kind == EXPR_EQUAL) {
        expr_print(e->left);
        printf("==");
        expr_print(e->right);
    }
    if(e->kind == EXPR_GE) {
        expr_print(e->left);
        printf(">=");
        expr_print(e->right);
    }
    if(e->kind == EXPR_GREATER) {
        expr_print(e->left);
        printf(">");
        expr_print(e->right);
    }
    if(e->kind == EXPR_LE) {
        expr_print(e->left);
        printf("<=");
        expr_print(e->right);
    }
    if(e->kind == EXPR_LESS) {
        expr_print(e->left);
        printf("<");
        expr_print(e->right);
    }
    if(e->kind == EXPR_AND) {
        expr_print(e->left);
        printf("&&");
        expr_print(e->right);
    }
    if(e->kind == EXPR_OR) {
        expr_print(e->left);
        printf("||");
        expr_print(e->right);
    }
    if(e->kind == EXPR_ASSIGN) {
        expr_print(e->left);
        printf("=");
        expr_print(e->right);
    }
}

// conducts name resolution on the expression e
void expr_resolve(struct expr* e) {

    // base case for recursion
    if(e == NULL) {
        return;
    }

    // different cases for the expression types
    if(e->kind == EXPR_NAME) {
        e->symbol = scope_lookup(e->name);
        if(e->symbol == NULL) {
            printf("\033[0;31mresolution error\033[0;0m: \"%s\" not found in scope\n", e->name);
            totalResErrors++;
        }
        else {
            
            // print proper message depending on scope
            if(scope_lookup(e->name)->type->kind == TYPE_PROTOTYPE) {
                printf("\033[38;5;45mreferenced\033[0;0m prototype \"%s\" from symbol table\n", e->name);
            }
            else if(scope_lookup(e->name)->type->kind == TYPE_FUNCTION) {
                printf("\033[38;5;45mreferenced\033[0;0m function \"%s\" from symbol table\n", e->name);
            }
            else if(scope_lookup(e->name)->kind == SYMBOL_GLOBAL) {
                printf("\033[38;5;45mreferenced\033[0;0m global \"%s\" from symbol table\n", e->name);
            }
            if(scope_lookup(e->name)->kind == SYMBOL_LOCAL) {
                printf("\033[38;5;45mreferenced\033[0;0m local \"%s\" from symbol table\n", e->name);
            }
            if(scope_lookup(e->name)->kind == SYMBOL_PARAM) {
                printf("\033[38;5;45mreferenced\033[0;0m parameter \"%s\" from symbol table\n", e->name);
            }
               
        }
    }
    else {
        expr_resolve(e->left);
        expr_resolve(e->right);
    }
}

// function used to typecheck expressions; returns the type of expression, even in case of error
struct type* expr_typecheck(struct expr* e) {

    // base case for recursion
    if(e == NULL) {
        return NULL;
    }

    // typecheck the left and right expressions, storing their addresses
    struct type* lt = expr_typecheck(e->left);
    struct type* rt = expr_typecheck(e->right);

    // the result to be returned (even if typechecking fails!)
    struct type* result;
    struct expr* temp;
    int totalDerefs = 0;

    // switch statement for all kinds of expressions
    switch(e->kind) {
        
        /* NORMAL CASES: understandably similar to one another */
        // the most basic cases; basic expressions: returns that type
        case EXPR_INTLIT:
            result = type_create(TYPE_INTEGER, 0, 0, 0);
            break;
        case EXPR_STRINGLIT:
            result = type_create(TYPE_STRING, 0, 0, 0);
            break;
        case EXPR_CHARLIT:
            result = type_create(TYPE_CHAR, 0, 0, 0);
            break;
        case EXPR_BOOLLIT:
            result = type_create(TYPE_BOOLEAN, 0, 0, 0);
            break;
        
        // the unary arithmetic: the left expression should be an integer: returns integer
        case EXPR_INC:
            if(lt->kind != TYPE_INTEGER) {
                printf("\033[0;31mtypechecking error\033[0;0m: cannot increment ");
                type_print(lt);
                if(e->left->name != NULL) {
                    printf(" (%s)\n", e->left->name);
                }
                else {
                    printf(" (");
                    expr_print(e->left);
                    printf(") \n");
                }
                totalTypeErrors++;
            }
            result = type_create(TYPE_INTEGER, 0, 0, 0);
            break; 
        case EXPR_DEC:
            if(lt->kind != TYPE_INTEGER) {
                printf("\033[0;31mtypechecking error\033[0;0m: cannot decrement ");
                type_print(lt);
                if(e->left->name != NULL) {
                    printf(" (%s)\n", e->left->name);
                }
                else {
                    printf(" (");
                    expr_print(e->left);
                    printf(") \n");
                }
                totalTypeErrors++;
            }
            result = type_create(TYPE_INTEGER, 0, 0, 0);
            break;
        case EXPR_NEG:
            if(lt->kind != TYPE_INTEGER) {
                printf("\033[0;31mtypechecking error\033[0;0m: cannot negate ");
                type_print(lt);
                if(e->left->name != NULL) {
                    printf(" (%s)\n", e->left->name);
                }
                else {
                    printf(" (");
                    expr_print(e->left);
                    printf(") \n");
                }
                totalTypeErrors++;
            }
            result = type_create(TYPE_INTEGER, 0, 0, 0);
            break;
        
        // the binary arithmetic: both left and right expressions should be integers, returns integer
        case EXPR_EXPON:
            if(lt->kind != TYPE_INTEGER || rt->kind != TYPE_INTEGER) {
                printf("\033[0;31mtypechecking error\033[0;0m: cannot exponentiate ");
                type_print(lt);
                if(e->left->name != NULL) {
                    printf(" (%s) with ", e->left->name);
                }
                else {
                    printf(" (");
                    expr_print(e->left);
                    printf(") with ");
                }
                type_print(rt);
                if(e->right->name != NULL) {
                    printf(" (%s)\n", e->right->name);
                }
                else {
                    printf(" (");
                    expr_print(e->right);
                    printf(") \n");
                }
                totalTypeErrors++;
            }
            result = type_create(TYPE_INTEGER, 0, 0, 0);
            break;
        case EXPR_MOD:
            if(lt->kind != TYPE_INTEGER || rt->kind != TYPE_INTEGER) {
                printf("\033[0;31mtypechecking error\033[0;0m: cannot compute modulo on ");
                type_print(lt);
                if(e->left->name != NULL) {
                    printf(" (%s) and ", e->left->name);
                }
                else {
                    printf(" (");
                    expr_print(e->left);
                    printf(") and ");
                }
                type_print(rt);
                if(e->right->name != NULL) {
                    printf(" (%s)\n", e->right->name);
                }
                else {
                    printf(" (");
                    expr_print(e->right);
                    printf(") \n");
                }
                totalTypeErrors++;
            }
            result = type_create(TYPE_INTEGER, 0, 0, 0);
            break;
        case EXPR_DIV:
            if(lt->kind != TYPE_INTEGER || rt->kind != TYPE_INTEGER) {
                printf("\033[0;31mtypechecking error\033[0;0m: cannot divide ");
                type_print(lt);
                if(e->left->name != NULL) {
                    printf(" (%s) by ", e->left->name);
                }
                else {
                    printf(" (");
                    expr_print(e->left);
                    printf(") by ");
                }
                type_print(rt);
                if(e->right->name != NULL) {
                    printf(" (%s)\n", e->right->name);
                }
                else {
                    printf(" (");
                    expr_print(e->right);
                    printf(") \n");
                }
                totalTypeErrors++;
            }
            result = type_create(TYPE_INTEGER, 0, 0, 0);
            break;
        case EXPR_MULT:
            if(lt->kind != TYPE_INTEGER || rt->kind != TYPE_INTEGER) {
                printf("\033[0;31mtypechecking error\033[0;0m: cannot multiply ");
                type_print(lt);
                if(e->left->name != NULL) {
                    printf(" (%s) and ", e->left->name);
                }
                else {
                    printf(" (");
                    expr_print(e->left);
                    printf(") and ");
                }
                type_print(rt);
                if(e->right->name != NULL) {
                    printf(" (%s)\n", e->right->name);
                }
                else {
                    printf(" (");
                    expr_print(e->right);
                    printf(") \n");
                }
                totalTypeErrors++;
            }
            result = type_create(TYPE_INTEGER, 0, 0, 0);
            break;
        case EXPR_SUB:
            if(lt->kind != TYPE_INTEGER || rt->kind != TYPE_INTEGER) {
                printf("\033[0;31mtypechecking error\033[0;0m: cannot subtract ");
                type_print(rt);
                if(e->right->name != NULL) {
                    printf(" (%s) from ", e->right->name);
                }
                else {
                    printf(" (");
                    expr_print(e->right);
                    printf(") from ");
                }
                type_print(lt);
                if(e->left->name != NULL) {
                    printf(" (%s)\n", e->left->name);
                }
                else {
                    printf(" (");
                    expr_print(e->left);
                    printf(") \n");
                }
                totalTypeErrors++;
            }
            result = type_create(TYPE_INTEGER, 0, 0, 0);
            break;
        case EXPR_ADD:
            if(lt->kind != TYPE_INTEGER || rt->kind != TYPE_INTEGER) {
                printf("\033[0;31mtypechecking error\033[0;0m: cannot add ");
                type_print(lt);
                if(e->left->name != NULL) {
                    printf(" (%s) to ", e->left->name);
                }
                else {
                    printf(" (");
                    expr_print(e->left);
                    printf(") to ");
                }
                type_print(rt);
                if(e->right->name != NULL) {
                    printf(" (%s)\n", e->right->name);
                }
                else {
                    printf(" (");
                    expr_print(e->right);
                    printf(") \n");
                }
                totalTypeErrors++;
            }
            result = type_create(TYPE_INTEGER, 0, 0, 0);
            break;

        // the binary integer logical arithmetic: left / right expressions should be integers, returns boolean
        case EXPR_GE:
        case EXPR_GREATER:
        case EXPR_LE:
        case EXPR_LESS:
            if(lt->kind != TYPE_INTEGER || rt->kind != TYPE_INTEGER) {
                printf("\033[0;31mtypechecking error\033[0;0m: cannot perform integer comparison between ");
                type_print(lt);
                if(e->left->name != NULL) {
                    printf(" (%s) and ", e->left->name);
                }
                else {
                    printf(" (");
                    expr_print(e->left);
                    printf(") and ");
                }
                type_print(rt);
                if(e->right->name != NULL) {
                    printf(" (%s)\n", e->right->name);
                }
                else {
                    printf(" (");
                    expr_print(e->right);
                    printf(") \n");
                }
                totalTypeErrors++;
            }
            result = type_create(TYPE_BOOLEAN, 0, 0, 0);
            break;
        
        // the unary boolean logical arithmetic: left expression should be boolean, returns boolean
        case EXPR_NOT:
            if(lt->kind != TYPE_BOOLEAN) {
                printf("\033[0;31mtypechecking error\033[0;0m: cannot perform boolean logic on ");
                type_print(lt);
                if(e->left->name != NULL) {
                    printf(" (%s)\n", e->left->name);
                }
                else {
                    printf(" (");
                    expr_print(e->left);
                    printf(") \n");
                }
                totalTypeErrors++;
            }
            result = type_create(TYPE_BOOLEAN, 0, 0, 0);
            break;

        // the birary boolean logical arithmetic: left / right expressions should be booleans, returns boolean
        case EXPR_AND:
        case EXPR_OR:
            if(lt->kind != TYPE_BOOLEAN || rt->kind != TYPE_BOOLEAN) {
                printf("\033[0;31mtypechecking error\033[0;0m: cannot perform boolean logic on ");
                type_print(lt);
                if(e->left->name != NULL) {
                    printf(" (%s) and ", e->left->name);
                }
                else {
                    printf(" (");
                    expr_print(e->left);
                    printf(") and ");
                }
                type_print(rt);
                if(e->right->name != NULL) {
                    printf(" (%s)\n", e->right->name);
                }
                else {
                    printf(" (");
                    expr_print(e->right);
                    printf(") \n");
                }
                totalTypeErrors++;
            }
            result = type_create(TYPE_BOOLEAN, 0, 0, 0);
            break;
        
        // the integer, string, and char equivalence expressions: left and right must match, returns boolean
        case EXPR_NEQUAL:
        case EXPR_EQUAL:
            if((lt->kind != TYPE_INTEGER || rt->kind != TYPE_INTEGER) && 
               (lt->kind != TYPE_STRING || rt->kind != TYPE_STRING) && 
               (lt->kind != TYPE_CHAR || rt->kind != TYPE_CHAR) &&
               (lt->kind != TYPE_BOOLEAN || rt->kind != TYPE_BOOLEAN)) {
                printf("\033[0;31mtypechecking error\033[0;0m: cannot compute equivalence on ");
                type_print(lt);
                if(e->left->name != NULL) {
                    printf(" (%s) and ", e->left->name);
                }
                else {
                    printf(" (");
                    expr_print(e->left);
                    printf(") and ");
                }
                type_print(rt);
                if(e->right->name != NULL) {
                    printf(" (%s)\n", e->right->name);
                }
                else {
                    printf(" (");
                    expr_print(e->right);
                    printf(") \n");
                }
                totalTypeErrors++;
            }
            result = type_create(TYPE_BOOLEAN, 0, 0, 0);
            break;

        /* SPECIAL CASES: not as uniform as the ones above */
        // a group expression: simply typecheck the expressions stored within the parens
        case EXPR_GROUP:
            result = type_copy(lt);
            break;
        
        // an identifier expression: should find and return the type from the symbol table
        case EXPR_NAME:
            
            // see if the identifier has a symbol struct binded to it
            if(e->symbol == NULL) {
                printf("\033[0;31mtypechecking error\033[0;0m: identifier (%s) may not have been declared (defaults to integer for remainder of typechecking)\n", e->name);
                totalTypeErrors++;
                result = type_create(TYPE_INTEGER, 0, 0, 0);
                break;
            }
            
            // copies the type from the 'symbol' struct for the identifier
            else {
                result = type_copy(e->symbol->type);
                break;
            }
        
        // an assignment expression: left and right must be of the same type
        case EXPR_ASSIGN:               
            if(!type_compare(lt, rt)) {
                printf("\033[0;31mtypechecking error\033[0;0m: cannot assign different types: ");
                type_print(lt);
                if(e->left->name != NULL) {
                    printf(" (%s) and ", e->left->name);
                }
                else {
                    printf(" (");
                    expr_print(e->left);
                    printf(") and ");
                }
                type_print(rt);
                if(e->right->name != NULL) {
                    printf(" (%s)\n", e->right->name);
                }
                else {
                    printf(" (");
                    expr_print(e->right);
                    printf(") \n");
                }
                totalTypeErrors++;
               }
            result = type_copy(lt);
            break;
        
        // a function call: need to return the subtype of the function named e->name, and make sure params match
        case EXPR_FCALL:

            // should only do if the identifier of the call is a function or prototype
            if(lt->kind == TYPE_FUNCTION || lt->kind == TYPE_PROTOTYPE) {
                
                // for multiple arguments (2+), send the EXPR_ARGS expression
                if(e->right != NULL && e->right->left->kind == EXPR_ARGS) {
                    
                    // if the arguments and function parameters do not match, emit an error
                    if(!param_list_fcall_compare(e->right->left, e->left->symbol->type->params)) {
                        printf("\033[0;31mtypechecking error\033[0;0m: function call arguments do not match function (%s) parameter(s)\n", e->left->name);
                        totalTypeErrors++;
                    }
                    
                    // either way, result becomes the function's subtype
                    result = type_copy(lt->subtype);
                    break;
                }

                // for a single arg (1), send only the expression in question
                else if(e->right != NULL && e->right->left->kind != EXPR_ARGS) {

                    // if the first param matches the type of the only argument, and there are no more params, we are good
                    if(e->left->symbol->type->params == NULL || (expr_typecheck(e->right->left), e->left->symbol->type->params->type) == false || e->left->symbol->type->params->next != NULL) {
                        printf("\033[0;31mtypechecking error\033[0;0m: function call argument does not match function (%s) parameter(s)\n", e->left->name);
                        totalTypeErrors++;
                    }

                    // either way, result becomes the function's subtype
                    result = type_copy(lt->subtype);
                    break;
                }

                // for a no args argument (0), send NULL
                else {
                    
                    // as long as the parameter in the function is NULL, we are good
                    if(!param_list_fcall_compare(NULL, e->left->symbol->type->params)) {
                        printf("\033[0;31mtypechecking error\033[0;0m: function (%s) requires at least one argument, none passed\n", e->left->name);
                        totalTypeErrors++;
                    }

                    // either way, result becomes the function's subtype
                    result = type_copy(lt->subtype);
                    break;
                }
            }
            
            // if the identifier being called is not a function or prototype
            else {
                printf("\033[0;31mtypechecking error\033[0;0m: identifier (%s) is not a function\n", e->left->name);
                totalTypeErrors++;
                
                // result becomes the type of the identifier to allow for continued typecheckin
                result = type_copy(lt);
                break;
            }
        
        // arguments for a function call; always returns type integer so that freeing works (checked in different function)
        case EXPR_ARGS:
            result = type_create(TYPE_INTEGER, 0, 0, 0);
            break;
        
        // dereferencing an array at some index: left should be type array, right should be bracket type
        case EXPR_ARRIND:
            
            // if the identifier being indexed is of type array
            if(lt->kind == TYPE_ARRAY) {
                
                // singly-indexed arrays (arr[1]) must be treated differently
                if(e->right->kind != EXPR_BRACKET) {
                    
                    // if the single index is of type integer
                    if(expr_typecheck(e->right)->kind == TYPE_INTEGER) {

                        // the result becomes the subtype of the identifier, break here
                        result = type_copy(lt->subtype);
                        break;
                    }
                    
                    // if the index is NOT of type integer
                    else {
                        printf("\033[0;31mtypechecking error\033[0;0m: array index for (%s) must be type integer\n", e->left->name);
                        totalTypeErrors++;
                    }         
                }

                // multi-indexed arrays (arr[1][2]) must be treated differently
                else if(e->right->kind == EXPR_BRACKET) {
                    
                    // begin counting the number of dereferences to get type later
                    totalDerefs++;
                    
                    // navigate through all EXPR_BRACKETs
                    temp = e->right;
                    while(temp->right->kind == EXPR_BRACKET) {

                        // make sure that each index is of type integer
                        if(expr_typecheck(temp->left)->kind != TYPE_INTEGER) {
                            printf("\033[0;31mtypechecking error\033[0;0m: array indices for (%s) must be type integer\n", e->left->name);
                            totalTypeErrors++;
                        }
                        
                        // move on to the next dereference
                        totalDerefs++;
                        temp = temp->right;
                    }
                    
                    // the last two indices, should both be of type integer
                    if(expr_typecheck(temp->left)->kind != TYPE_INTEGER) {
                        printf("\033[0;31mtypechecking error\033[0;0m: array indices for (%s) must be type integer\n", e->left->name);
                        totalTypeErrors++;
                    }
                    if(expr_typecheck(temp->right)->kind != TYPE_INTEGER) {
                        printf("\033[0;31mtypechecking error\033[0;0m: array indices for (%s) must be type integer\n", e->left->name);
                        totalTypeErrors++;
                    }
                    totalDerefs++;

                    // using the totalDerefs, get the type by traversing the identifier's subtypes
                    result = type_copy(lt->subtype);
                    for(int i = 0; i < totalDerefs - 1; i++) {
                        
                        // grab the next subtype
                        if(result->subtype != NULL) {
                            result = type_copy(result->subtype);
                        }

                        // once the subtypes are used up (indexed into a non-existent higher dimension)
                        else {
                            printf("\033[0;31mtypechecking error\033[0;0m: cannot index array (%s) outside of its dimensions\n", e->left->name);
                        }
                    }
                    
                    // important: break here with the result we have; either the desired type or the furthest subtype of the array
                    break;
                }

                // valid type for all TYPE_ARRAY that did not typecheck correctly
                result = type_copy(lt->subtype);
                break;
            }
            
            // if the identifier being indexed is NOT of type array
            else {
                
                printf("\033[0;31mtypechecking error\033[0;0m: identifier (%s) is not of type array\n", e->left->name);
                totalTypeErrors++;

                // return the type of the identifier to allow for continued typechecking
                result = type_copy(lt);
                break;
            }

        // array dereference brackets; doesn't matter what it returns, just done so it can be freed later
        case EXPR_BRACKET:
            result = type_create(TYPE_BOOLEAN, 0, 0, 0);
            break;

        // array initializer lists are dealt with in 'decl_typecheck()'. This is here for the switch case
        case EXPR_CURLS:
            result = type_create(TYPE_BOOLEAN, 0, 0, 0);
            break;

    
    }

    // types of left and right expressions no longer needed, delete these
    type_delete(lt);
    type_delete(rt);



    // returns the result, regardless of typechecking error
    return result;
}

void expr_codegen(struct expr* e) {

    // temp to hold literal string label and info
    int tempLitLabel;
    char strBuffer[300];
    struct expr* tempe;
    struct expr* tempe2;
    int paramRegCount;
    int final;

    // if the expression is NULL, we should return
    if(e == NULL) {
        return;
    }

    // switches all kinds of expressions
    switch(e->kind) {

        // Leaf node: allocate register and load value

        // for variables
        case EXPR_NAME:
            
            // for global string variables (using labels)
            if(e->symbol->type->kind == TYPE_STRING && e->symbol->kind == SYMBOL_GLOBAL) {
                e->reg = scratch_alloc();
                printf("\t\tadrp\t%s, %s\n", scratch_name(e->reg), e->name);
                printf("\t\tadd\t%s, %s, :lo12:%s\n", scratch_name(e->reg), scratch_name(e->reg), e->name);
                break;
            }

            // for local string variables (using labels)
            if(e->symbol->type->kind == TYPE_STRING && e->symbol->kind == SYMBOL_LOCAL) {
                e->reg = scratch_alloc();
                printf("\t\tadrp\t%s, %s\n", scratch_name(e->reg), label_name(e->symbol->which));
                printf("\t\tadd\t%s, %s, :lo12:%s\n", scratch_name(e->reg), scratch_name(e->reg), label_name(e->symbol->which));
                break;
            }

            // for all other variables
            e->reg = scratch_alloc();
            printf("\t\tldr\t%s, %s\n", scratch_name(e->reg), symbol_codegen(e->symbol));
        break;

        case EXPR_INTLIT:
            e->reg = scratch_alloc();
            printf("\t\tmov\t%s, #%i\n", scratch_name(e->reg), e->literal_value);
        break;

        case EXPR_BOOLLIT:
            e->reg = scratch_alloc();
            printf("\t\tmov\t%s, #%i\n", scratch_name(e->reg), e->literal_value);
        break;

        case EXPR_CHARLIT:
            e->reg = scratch_alloc();
            printf("\t\tmov\t%s, #%i\n", scratch_name(e->reg), e->literal_value);
        break;

        case EXPR_STRINGLIT:
            e->reg = scratch_alloc();

            // need to create a new label for the string literal
            tempLitLabel = lit_label_create();
            
            // unique label number will come from tempLitLabel
            memset(strBuffer, 0, 300);
            sprintf(strBuffer, "\t.section\t.rodata\n\t.align 3\n%s:\n\t.string %s\n", lit_label_name(tempLitLabel), e->string_literal);

            // store this into the array of strings that will be printed at the end of the function
            for(int i = 0; i < 300; i++) {
                localsTP[localsTPCounter][i] = strBuffer[i];
            }
            localsTPCounter++;

            // generate the code to store the address of this string literal into a free register
            printf("\t\tadrp\t%s, %s\n", scratch_name(e->reg), lit_label_name(tempLitLabel));
            printf("\t\tadd\t%s, %s, :lo12:%s\n", scratch_name(e->reg), scratch_name(e->reg), lit_label_name(tempLitLabel));

        break;

        // Interior node: generate children, then add them

        case EXPR_ADD:
            expr_codegen(e->left);
            expr_codegen(e->right);
            printf("\t\tadd\t%s, %s, %s\n", scratch_name(e->right->reg), scratch_name(e->left->reg), scratch_name(e->right->reg));
            e->reg = e->right->reg;
            scratch_free(e->left->reg);
            
        break;

        case EXPR_SUB:
            expr_codegen(e->left);
            expr_codegen(e->right);
            printf("\t\tsub\t%s, %s, %s\n", scratch_name(e->right->reg), scratch_name(e->left->reg), scratch_name(e->right->reg));
            e->reg = e->right->reg;
            scratch_free(e->left->reg);
        break;

        case EXPR_MULT:
            expr_codegen(e->left);
            expr_codegen(e->right);
            printf("\t\tmul\t%s, %s, %s\n", scratch_name(e->right->reg), scratch_name(e->left->reg), scratch_name(e->right->reg));
            e->reg = e->right->reg;
            scratch_free(e->left->reg);
        break;

        case EXPR_DIV:
            expr_codegen(e->left);
            expr_codegen(e->right);
            printf("\t\tsdiv\t%s, %s, %s\n", scratch_name(e->right->reg), scratch_name(e->left->reg), scratch_name(e->right->reg));
            e->reg = e->right->reg;
            scratch_free(e->left->reg);
        break;

        case EXPR_MOD:
            expr_codegen(e->left);
            expr_codegen(e->right);
            
            // register will hold the result of each intermediate / final
            e->reg = scratch_alloc();

            // will need more than one machine instruction for mod
            printf("\t\tudiv\t%s, %s, %s\n", scratch_name(e->reg), scratch_name(e->left->reg), scratch_name(e->right->reg));
            printf("\t\tmul\t%s, %s, %s\n", scratch_name(e->reg), scratch_name(e->right->reg), scratch_name(e->reg));
            printf("\t\tsub\t%s, %s, %s\n", scratch_name(e->reg), scratch_name(e->left->reg), scratch_name(e->reg));
            
            // free the two operand scratch registers
            scratch_free(e->left->reg);
            scratch_free(e->right->reg);
        break;

        case EXPR_AND:
            expr_codegen(e->left);
            expr_codegen(e->right);
            printf("\t\tand\t%s, %s, %s\n", scratch_name(e->right->reg), scratch_name(e->left->reg), scratch_name(e->right->reg));
            e->reg = e->right->reg;
            scratch_free(e->left->reg);
        break;

        case EXPR_OR:
            expr_codegen(e->left);
            expr_codegen(e->right);
            printf("\t\torr\t%s, %s, %s\n", scratch_name(e->right->reg), scratch_name(e->left->reg), scratch_name(e->right->reg));
            e->reg = e->right->reg;
            scratch_free(e->left->reg);
        break;

        case EXPR_INC:
            expr_codegen(e->left);
            printf("\t\tadd\t%s, %s, #1\n", scratch_name(e->left->reg), scratch_name(e->left->reg));
            e->reg = e->left->reg;
        break;

        case EXPR_DEC:
            expr_codegen(e->left);
            printf("\t\tsub\t%s, %s, #1\n", scratch_name(e->left->reg), scratch_name(e->left->reg));
            e->reg = e->left->reg;
        break;

        case EXPR_NEG:
            expr_codegen(e->left);
            printf("\t\tneg\t%s, %s\n", scratch_name(e->left->reg), scratch_name(e->left->reg));
            e->reg = e->left->reg;
        break;

        case EXPR_NOT:
            expr_codegen(e->left);
            printf("\t\tmvn\t%s, %s\n", scratch_name(e->left->reg), scratch_name(e->left->reg));
            e->reg = e->left->reg;
        break;

        case EXPR_GROUP:
            expr_codegen(e->left);
            e->reg = e->left->reg;
        break;

        case EXPR_EXPON:
            
            // generate code to get left and right operands into registers
            expr_codegen(e->left);
            expr_codegen(e->right);

            // move operands from their registers to x0 and x1 and call function
            printf("\t\tmov\tx0, %s\n", scratch_name(e->left->reg));
            printf("\t\tmov\tx1, %s\n", scratch_name(e->right->reg));
            printf("\t\tbl\tinteger_power\n");

            // this expression's register will be the right register, which needs x0's contents
            e->reg = e->right->reg;
            printf("\t\tmov\t%s, x0\n", scratch_name(e->reg));
            
            // free the register used to fetch the left operand
            scratch_free(e->right->reg);

        break;

        case EXPR_FCALL:

            // before the function call, we should save the contents of each register x9-x15 (obviously not optimized!)
            printf("\t\tstr\tx9, [sp, %i]\n",  callStackSize - 8*6);
            printf("\t\tstr\tx10, [sp, %i]\n", callStackSize - 8*5);
            printf("\t\tstr\tx11, [sp, %i]\n", callStackSize - 8*4);
            printf("\t\tstr\tx12, [sp, %i]\n", callStackSize - 8*3);
            printf("\t\tstr\tx13, [sp, %i]\n", callStackSize - 8*2);
            printf("\t\tstr\tx14, [sp, %i]\n", callStackSize - 8*1);
            printf("\t\tstr\tx15, [sp, %i]\n", callStackSize - 8*0);

            // if the function call does not pass parameters
            if(e->right == NULL) {
                
                // simply branch to the function, no params needed
                printf("\t\tbl\t");
                expr_print(e->left);
                printf("\n");

            }

            // if the function call requires a single parameter
            if(e->right != NULL && e->right->left->kind != EXPR_ARGS) {
                
                // generate code for the one parameter
                expr_codegen(e->right->left);

                // move the register with param into x0
                printf("\t\tmov\tx0, %s\n", scratch_name(e->right->left->reg));
                
                // branch to the function's label
                printf("\t\tbl\t");
                expr_print(e->left);
                printf("\n");

                // free the register used
                scratch_free(e->right->left->reg);

            }

            // if the function call requires two or more parameters
            if(e->right != NULL && e->right->left->kind == EXPR_ARGS) {

                // use a temporary expression to store the next arg or args list
                tempe = e->right->left;
                paramRegCount = 0;

                // do this loop until there are only 2 expressions left
                while(tempe->right->kind == EXPR_ARGS) {

                    // load the argument into the next parameter register
                    expr_codegen(tempe->left);
                    printf("\t\tmov\tx%i, %s\n", paramRegCount, scratch_name(tempe->left->reg));
                    scratch_free(tempe->left->reg);

                    // increment register count and update tempe
                    paramRegCount++;
                    tempe = tempe->right;

                    // if more than 6 parameters are called
                    if(paramRegCount == 5) {
                        printf("\033[0;31mcodegen error\033[0;0m: cannot exceed 6 function parameters, registers filled\n");
                        exit(1);
                    }
                }
                
                // load the second to last parameter
                expr_codegen(tempe->left);
                printf("\t\tmov\tx%i, %s\n", paramRegCount, scratch_name(tempe->left->reg));
                scratch_free(tempe->left->reg);

                // load the last parameter
                expr_codegen(tempe->right);
                printf("\t\tmov\tx%i, %s\n", paramRegCount + 1, scratch_name(tempe->right->reg));
                scratch_free(tempe->right->reg);

                // branch to the function's label
                printf("\t\tbl\t");
                expr_print(e->left);
                printf("\n");
                
            }

            // after the function call, we should restore the contents of each register x9-x15 (obviously not optimized!)
            printf("\t\tldr\tx9, [sp, %i]\n",  callStackSize - 8*6);
            printf("\t\tldr\tx10, [sp, %i]\n", callStackSize - 8*5);
            printf("\t\tldr\tx11, [sp, %i]\n", callStackSize - 8*4);
            printf("\t\tldr\tx12, [sp, %i]\n", callStackSize - 8*3);
            printf("\t\tldr\tx13, [sp, %i]\n", callStackSize - 8*2);
            printf("\t\tldr\tx14, [sp, %i]\n", callStackSize - 8*1);
            printf("\t\tldr\tx15, [sp, %i]\n", callStackSize - 8*0);

            // function call result should be saved to an alternate register (NOT x0) upon return
            e->reg = scratch_alloc();
            printf("\t\tmov\t%s, x0\n", scratch_name(e->reg));
        break;

        // for assigning expressions
        case EXPR_ASSIGN:

            // for assigning one (1) variable a value
            // identifiers may either be string, non-string local, or non-string global
            if(e->left->kind != EXPR_ASSIGN) {

                // if it is a string
                if(e->left->symbol->type != NULL && e->left->symbol->type->kind == TYPE_STRING) {
                    
                    // generate code to get the righmost string into a register
                    expr_codegen(e->right);

                    // generate code to get the string in a register
                    expr_codegen(e->left);

                    // need to move every character from right to left string
                    for(int i = 0; i < 29; i++) {
                        printf("\t\tldrb\tw0, [%s, %i]\n", scratch_name(e->right->reg), i);
                        printf("\t\tstrb\tw0, [%s, %i]\n", scratch_name(e->left->reg), i);
                        final = i + 1;
                    }
                    printf("\t\tmov\tw0, 0\n");
                    printf("\t\tstrb\tw0, [%s, %i]\n", scratch_name(e->left->reg), final);

                    // free up the two registers used
                    scratch_free(e->right->reg);
                    scratch_free(e->left->reg);

                    break;
                }

                // if it is a local, non-string
                if(e->left->symbol->kind == SYMBOL_LOCAL) {
                    
                    // generate code to compute the right expression
                    expr_codegen(e->right);

                    // store the result into the memory location
                    printf("\t\tstr\t%s, %s\n", scratch_name(e->right->reg), symbol_codegen(e->left->symbol));

                    // free the register since no more assigns are needed
                    scratch_free(e->right->reg);

                    break;
                }

                // if it is a global, non-string
                else if(e->left->symbol->kind == SYMBOL_GLOBAL) {
                    
                    // generate code to compute the right expression
                    expr_codegen(e->right);

                    // store the address of the global variable in a free register
                    e->reg = scratch_alloc();
                    printf("\t\tadrp\t%s, %s\n", scratch_name(e->reg), e->left->name);
                    printf("\t\tadd\t%s, %s, :lo12:%s\n", scratch_name(e->reg), scratch_name(e->reg), e->left->name);
                    
                    // store the result of the right expression into the global variable
                    printf("\t\tstr\t%s, [%s]\n", scratch_name(e->right->reg), scratch_name(e->reg));

                    // free the register that held the address of the global
                    scratch_free(e->reg);

                    // free the right register too since no more assigns needed
                    scratch_free(e->right->reg);

                    break;
                }
            }

            // for assigning more than one (2+) variable a value
            if(e->left->kind == EXPR_ASSIGN) {

                // for all but the last assignment
                tempe = e->left;
                tempe2 = e->right;
                
                while(tempe->kind == EXPR_ASSIGN) {

                    // if it is a string
                    if(tempe->right->symbol->type != NULL && tempe->right->symbol->type->kind == TYPE_STRING) {
                        
                        // generate code to get the righmost string into a register
                        expr_codegen(tempe2);

                        // generate code to get the string in a register
                        expr_codegen(tempe->right);

                        // need to move every character from right to left string
                        for(int i = 0; i < 29; i++) {
                            printf("\t\tldrb\tw0, [%s, %i]\n", scratch_name(tempe2->reg), i);
                            printf("\t\tstrb\tw0, [%s, %i]\n", scratch_name(tempe->right->reg), i);
                            final = i + 1;
                        }
                        printf("\t\tmov\tw0, 0\n");
                        printf("\t\tstrb\tw0, [%s, %i]\n", scratch_name(tempe->right->reg), final);

                        // free up the two registers used
                        scratch_free(tempe->right->reg);
                        scratch_free(tempe2->reg);

                    }

                    // if it is a local, non-string
                    else if(tempe->right->symbol->kind == SYMBOL_LOCAL) {
                    
                        // generate code to compute the right expression
                        expr_codegen(tempe2);

                        // store the result into the memory location
                        printf("\t\tstr\t%s, %s\n", scratch_name(tempe2->reg), symbol_codegen(tempe->right->symbol));

                        // free the used register, value stored in tempe now
                        scratch_free(tempe2->reg);
                        
                    }

                    // if it is a global, non-string
                    else if(tempe->right->symbol->kind == SYMBOL_GLOBAL) {
                        
                        // generate code to compute the right expression
                        expr_codegen(tempe2);

                        // store the address of the global variable in a free register
                        e->reg = scratch_alloc();
                        printf("\t\tadrp\t%s, %s\n", scratch_name(e->reg), tempe->right->name);
                        printf("\t\tadd\t%s, %s, :lo12:%s\n", scratch_name(e->reg), scratch_name(e->reg), tempe->right->name);
                        
                        // store the result of the right expression into the global variable
                        printf("\t\tstr\t%s, [%s]\n", scratch_name(tempe2->reg), scratch_name(e->reg));

                        // free the register that held the address of the global
                        scratch_free(e->reg);

                        // free the used register, value stored in tempe now
                        scratch_free(tempe2->reg);
                    }

                    // move to the next assignment
                    tempe2 = tempe->right;
                    tempe = tempe->left;
                    
                }

                // for the last assignment
                
                // if it is a string
                if(tempe->symbol->type != NULL && tempe->symbol->type->kind == TYPE_STRING) {
                    
                    // generate code to get the righmost string into a register
                    expr_codegen(tempe2);

                    // generate code to get the string in a register
                    expr_codegen(tempe);

                    // need to move every character from right to left string
                    for(int i = 0; i < 29; i++) {
                        printf("\t\tldrb\tw0, [%s, %i]\n", scratch_name(tempe2->reg), i);
                        printf("\t\tstrb\tw0, [%s, %i]\n", scratch_name(tempe->reg), i);
                        final = i + 1;
                    }
                    printf("\t\tmov\tw0, 0\n");
                    printf("\t\tstrb\tw0, [%s, %i]\n", scratch_name(tempe->reg), final);

                    // free up the two registers used
                    scratch_free(tempe2->reg);
                    scratch_free(tempe->reg);

                    break;
                }

                // for local, non-string
                else if(tempe->symbol->kind == SYMBOL_LOCAL) {
                
                    // generate code to compute the right expression
                    expr_codegen(tempe2);

                    // store the result into the memory location
                    printf("\t\tstr\t%s, %s\n", scratch_name(tempe2->reg), symbol_codegen(tempe->symbol));

                    // free the used register, no more assignments needed
                    scratch_free(tempe2->reg);
                    
                    break;
                }

                // for global, non-string
                else if(tempe->symbol->kind == SYMBOL_GLOBAL) {
                    
                    // generate code to compute the right expression
                    expr_codegen(tempe2);

                    // store the address of the global variable in a free register
                    e->reg = scratch_alloc();
                    printf("\t\tadrp\t%s, %s\n", scratch_name(e->reg), tempe->name);
                    printf("\t\tadd\t%s, %s, :lo12:%s\n", scratch_name(e->reg), scratch_name(e->reg), tempe->name);
                    
                    // store the result of the right expression into the global variable
                    printf("\t\tstr\t%s, [%s]\n", scratch_name(tempe2->reg), scratch_name(e->reg));

                    // free the register that held the address of the global
                    scratch_free(e->reg);

                    // free the used register, value stored in tempe now
                    scratch_free(tempe2->reg);

                    break;
                }

            }

        break;

        case EXPR_ARGS:
        case EXPR_CURLS:
        case EXPR_BRACKET:
        default:
        break;
    }
}