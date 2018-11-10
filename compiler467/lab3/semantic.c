#include "ast.h"
#include "semantic.h"


snode* root_scope = NULL;

void ast_semantic_check(node* ast) {
    ast_semantic_check_help(ast, root_scope);
}


void ast_semantic_check_help(node* ast, snode* curr_scope) {
    if (ast == NULL) {
        return;
    }
    
    node_kind kind = ast->kind;
    switch (kind) {
        case SCOPE_NODE:
            // Scope checking, push new scope onto symbol table, pop before return.
            if (!curr_scope) {
                // Allocate the root scope.
                root_scope = snode_alloc(NULL);
                curr_scope = root_scope;
            } else {
                printf("[error]Scope checking: root scope already exists;");
            }
            // Type checking and proceed to other node checkings...
            ast_semantic_check_help(ast->scope.declarations, curr_scope);
            ast_semantic_check_help(ast->scope.statements, curr_scope);
            // Destroy the outermost scope.
            snode_destroy(root_scope);
            root_scope = NULL;
            break;
            
        case NESTED_SCOPE_NODE:
            // Scope checking, push new scope onto symbol table, pop before return.
            curr_scope = snode_alloc(curr_scope);
            // Type checking and proceed to other node checkings...
            ast_semantic_check_help(ast->nested_scope.scope->scope.declarations, curr_scope);
            ast_semantic_check_help(ast->nested_scope.scope->scope.statements, curr_scope);
            // Destroy current scope.
            snode_destroy(curr_scope);
            break;
            
        case DECLARATIONS_NODE:
            ast_semantic_check_help(ast->declarations.declarations, curr_scope);
            ast_semantic_check_help(ast->declarations.declaration, curr_scope);
            break;

        case DECLARATION_NODE:
        {
            // Scope checking, report error and set type to unknown if invalid.
            sentry* candidate_sentry = ast_node_to_sentry(ast);
            if (!scope_check_var_declaration_valid(ast, curr_scope)) {
                printf("[error]Scope checking: var declaration for %s not valid, changing stype to UNKNOWN\n", ast->declaration.id);
                candidate_sentry->stype = UNKNOWN_TYPE;
            }
            // Add candidate to scope table
            sentry_push(candidate_sentry, curr_scope);
            snode_print(curr_scope);
            
        }
            break;
            
        case STATEMENTS_NODE:
            ast_semantic_check_help(ast->statements.statements, curr_scope);
            ast_semantic_check_help(ast->statements.statement, curr_scope);
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

sentry* find_var_reference_by_id(char* id, snode* curr_scope) {
    if (!id || !curr_scope) return NULL;
    sentry* result = NULL;
    snode* scope_finder = curr_scope;
    while(scope_finder != NULL) {
        result = find_sentry_in_snode_by_id(id, scope_finder);
        if (result != NULL) {
            break;
        }
        scope_finder = scope_finder->parent_scope;
    }
    return result;
}

bool scope_check_var_declaration_valid(node* ast, snode* curr_scope) {
    if (!curr_scope || !ast) return 0;
    if (ast->kind != DECLARATION_NODE) {
        printf("[Error]Scope checking: ast node checked is not a declaration.\n");
    }
    return find_sentry_in_snode_by_id(ast->declaration.id, curr_scope) ? 0 : 1; // NOT valid if found!!!
}