#include <stdio.h>
#include <stdlib.h>
#include "codegen.h"


/**********************************************************************
 * Helper functions go here
 *********************************************************************/ 



/**********************************************************************
 * Lab 4: Code Generation
 *********************************************************************/ 

/*
 * genCode
 * For code generation
 * output into frag.txt file
 */
void genCode(node *ast) {
    if (ast == NULL) {
        return;
    }

    node_kind kind = ast->kind;
    switch (kind) {
        case SCOPE_NODE:
            break;
            
        case NESTED_SCOPE_NODE:
            break;
            
        case DECLARATIONS_NODE:
            break;

        case DECLARATION_NODE:
            break;
            
        case STATEMENTS_NODE:
            break;
        
        case TYPE_NODE:
            break;

        case VAR_NODE:
            break;

        case INT_NODE:
            break;

        case FLOAT_NODE:
            break;

        case BOOL_NODE:
            break;
           
//      case EXPRESSION_NODE: 
//          break;

        case UNARY_EXPRESSION_NODE:
            break;

        case BINARY_EXPRESSION_NODE:        
            break;
            
        case IF_STATEMENT_NODE:
            break;
            
        case ELSE_STATEMENT_NODE:
            break;
            
        case ASSIGNMENT_STATEMENT_NODE:
            break;

        case FUNCTION_NODE:
            break;
            
        case CONSTRUCTOR_NODE:
            break;
            
        case ARGS_NODE:
            break;
            
        default:
            printf("[debug]AST Print: unknown AST node\n");
            break;
    }
}



