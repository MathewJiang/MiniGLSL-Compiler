#include "ast.h"
#include "semantic.h"



void ast_semantic_check(node* ast) {
    ast_semantic_check_help(ast);
}


void ast_semantic_check_help(node* ast) {
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
            
        default:
            printf("[debug]AST Print: unknown AST node\n");
            break;
    }
    
}