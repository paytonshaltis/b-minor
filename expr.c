#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "expr.h"
#include "scope.h"
#include "symbol.h"
#include "scratch.h"

extern int totalResErrors;
extern int totalTypeErrors;

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
                    if(type_compare_no_size(expr_typecheck(e->right->left), e->left->symbol->type->params->type) == false || e->left->symbol->type->params->next != NULL) {
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

    // if the expression is NULL, we should return
    if(e == NULL) {
        return;
    }

    // switches all kinds of expressions
    switch(e->kind) {

        // Leaf node: allocate register and load value

        // for variables
        case EXPR_NAME:
            e->reg = scratch_alloc();
            printf("ldr %s, %s\n", scratch_name(e->reg), symbol_codegen(e->symbol));
        break;

        // Interior node: generate children, then add them

        // for addition of two registers
        case EXPR_ADD:
            expr_codegen(e->left);
            expr_codegen(e->right);
            printf("add %s, %s, %s\n", scratch_name(e->right->reg), scratch_name(e->left->reg), scratch_name(e->right->reg));
            e->reg = e->right->reg;
            scratch_free(e->left->reg);
        break;

        // for assigning one register to another
        case EXPR_ASSIGN:
            expr_codegen(e->right);
            printf("str, %s, %s\n", scratch_name(e->right->reg), symbol_codegen(e->left->symbol));
            e->reg = e->right->reg;
        break;
        

    }
}