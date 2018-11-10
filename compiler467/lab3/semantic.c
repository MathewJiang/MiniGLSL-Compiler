#include "ast.h"
#include "semantic.h"
#include "symbol.h"
#include "common.h"
#include "parser.tab.h"


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
                candidate_sentry->valid = false;
            }
            // Add candidate to scope table
            sentry_push(candidate_sentry, curr_scope);
            snode_print(curr_scope);
            
            if (ast->declaration.expr) {
                ast_semantic_check_help(ast->declaration.expr, curr_scope);
            }
        }
            break;
            
        case STATEMENTS_NODE:
            ast_semantic_check_help(ast->statements.statements, curr_scope);
            ast_semantic_check_help(ast->statements.statement, curr_scope);
            break;
        
        case TYPE_NODE:
            break;

        case VAR_NODE:
        {
            // Check if the variable has been declared before reaching here.
            sentry* var_entry = find_var_reference_by_id(ast->var_node.id, curr_scope);
            if (!var_entry) {
                printf("[error]Scope checking: variable %s has not declared before use\n", ast->var_node.id);
                ast->inferred_type.type_name = ANY;
                errorOccurred = true;
            } else if (!var_entry->valid || var_entry->stype == UNKNOWN_TYPE) {
                printf("[error]Scope checking: reference to INVALID variable %s. possibly undeclared successfully.\n", ast->var_node.id);
                ast->inferred_type.type_name = ANY;
                errorOccurred = true;
            } else {
                // Save inferred data into ast node struct.
                ast->inferred_type.type_name = stype_to_type_id(var_entry->stype);
                ast->inferred_type.is_const = var_entry->is_const;
                ast->inferred_type.is_vec = var_entry->is_vec;
                ast->inferred_type.vec_size = var_entry->vec_size;
            }
        }
            break;

        case INT_NODE:
            ast->inferred_type.type_name = INT; // How can this be done? Wont override int_node struct in union?
            ast->inferred_type.is_vec = 0;
            ast->inferred_type.vec_size = 0;
            ast->inferred_type.is_const = 1; // All literals cannot be re-assigned. i.e. 6 = 5;
            break;

        case FLOAT_NODE:
            ast->inferred_type.type_name = FLOAT;
            ast->inferred_type.is_vec = 0;
            ast->inferred_type.vec_size = 0;
            ast->inferred_type.is_const = 1;
            break;

        case BOOL_NODE:
            ast->inferred_type.type_name = BOOL;
            ast->inferred_type.is_vec = 0;
            ast->inferred_type.vec_size = 0;
            ast->inferred_type.is_const = 1;
            break;

        case UNARY_EXPRESSION_NODE:
        {
            ast_semantic_check_help(ast->unary_expr.right, curr_scope);
            type_id right_type = ast->unary_expr.right->inferred_type.type_name;
            
            if (ast->unary_expr.op == NOTEQUAL) {
                //s, v 
                if (right_type != BOOL) {
                    errorOccurred = true;
                    ast->inferred_type.type_name = ANY;
                    perror("Bad operand type: [UNARY]!: operand must be BOOL type\n");
                } else {
                    if (ast->unary_expr.right->inferred_type.is_vec) {
                        ast->inferred_type.type_name = right_type;
                        ast->inferred_type.is_vec = 1;
                        ast->inferred_type.vec_size = ast->unary_expr.right->inferred_type.vec_size;
                        ast->inferred_type.is_const = ast->unary_expr.right->inferred_type.is_const;
                    } else {
                        ast->inferred_type.type_name = right_type;
                        ast->inferred_type.is_vec = 0;
                        ast->inferred_type.is_const = ast->unary_expr.right->inferred_type.is_const;
                    }
                }
            } else if (ast->unary_expr.op == SUBTRACT) {
                //s, v
                if (right_type == BOOL || right_type == ANY) {
                    errorOccurred = true;
                    ast->inferred_type.type_name = ANY;
                    perror("Bad operand type: [UNARY]-: operand must be arithmetic types\n");
                } else {
                    if (ast->unary_expr.right->inferred_type.is_vec) {
                        ast->inferred_type.type_name = right_type;
                        ast->inferred_type.is_vec = 1;
                        ast->inferred_type.vec_size = ast->unary_expr.right->inferred_type.vec_size;
                        ast->inferred_type.is_const = ast->unary_expr.right->inferred_type.is_const;
                    } else {
                        ast->inferred_type.type_name = right_type;
                        ast->inferred_type.is_vec = 0;
                        ast->inferred_type.is_const = ast->unary_expr.right->inferred_type.is_const;
                    }
                }
            } else {
                errorOccurred = true;
                perror("Unknown unary type\n");
            }
        }
            break;

        case BINARY_EXPRESSION_NODE:
        {
            printf("Enter binary\n");
            //TODO: have not dealt with all the vector case
            //Only scalar case for now
            ast_semantic_check_help(ast->binary_expr.left, curr_scope);
            type_id left_type = ast->binary_expr.left->inferred_type.type_name;
            int left_is_vec = ast->binary_expr.left->inferred_type.is_vec;
            int left_vec_size = ast->binary_expr.left->inferred_type.vec_size;
            int left_is_const = ast->binary_expr.left->inferred_type.is_const;
            ast_semantic_check_help(ast->binary_expr.right, curr_scope);
            type_id right_type = ast->binary_expr.right->inferred_type.type_name;
            int right_is_vec = ast->binary_expr.right->inferred_type.is_vec;
            int right_vec_size = ast->binary_expr.right->inferred_type.vec_size;
            int right_is_const = ast->binary_expr.right->inferred_type.is_const;
            
            if (ast->binary_expr.op == AND     ||
                ast->binary_expr.op == OR) {
                //ss, vv
                if (left_type != BOOL || right_type != BOOL) {
                    errorOccurred = true;
                    ast->inferred_type.type_name = ANY;
                    perror("Bad operand type: [BINARY]&&, ||: both operand must be BOOL\n");
                } else {
                    if (left_is_vec != right_is_vec) {
                        errorOccurred = true;
                        ast->inferred_type.type_name = ANY;
                        perror("Bad operand type: [BINARY]&&, ||: operands type incompatible\n");
                    } else {
                        if (left_is_vec) {
                            //vv
                            if (left_vec_size != right_vec_size) {
                                errorOccurred = true;
                                ast->inferred_type.type_name = ANY;
                                perror("Bad operand type: [BINARY]&&, ||: vector with different size\n");
                            } else {
                                ast->inferred_type.type_name = left_type;
                                ast->inferred_type.is_vec = 1;
                                ast->inferred_type.vec_size = left_vec_size;
                                ast->inferred_type.is_const = left_is_const & right_is_const;
                            }
                        } else {
                            //ss
                            ast->inferred_type.type_name = left_type;
                            ast->inferred_type.is_vec = 0;
                            ast->inferred_type.is_const = left_is_const & right_is_const;
                        }
                    }
                }
            } 
            
            else if (ast->binary_expr.op == GT      ||
                ast->binary_expr.op == GE           ||
                ast->binary_expr.op == LT           ||    
                ast->binary_expr.op == LE) {
                //ss
                if ((left_type != right_type) 
                        || (left_type == ANY || right_type == ANY)) {
                    errorOccurred = true;
                    ast->inferred_type.type_name = ANY;
                    perror("Bad operand type: [BINARY]<, <=, >, >=: cannot compare with different types\n");
                } else if (left_type == BOOL || right_type == BOOL) {
                    errorOccurred = true;
                    ast->inferred_type.type_name = ANY;
                    perror("Bad operand type: [BINARY]<, <=, >, >=: operand must be arithmetic types\n");
                } else {
                    if (left_is_vec || right_is_vec) {
                        errorOccurred = true;
                        ast->inferred_type.type_name = ANY;
                        perror("Bad operand type: [BINARY]<, <=, >, >=: both operand must be scalar\n");
                    } else {
                        ast->inferred_type.type_name = BOOL;
                        ast->inferred_type.is_vec = 0;
                        ast->inferred_type.is_const = left_is_const & right_is_const;
                    }
                }
            }
            
            else if (ast->binary_expr.op == EQUAL   ||
                ast->binary_expr.op == NOTEQUAL) {
                //ss, vv
                if ((left_type != right_type) 
                        || (left_type == ANY || right_type == ANY)) {
                    ast->inferred_type.type_name = ANY;
                    perror("Bad operand type: [BINARY]==, !=: cannot compare with different types\n");
                } else if (left_type == BOOL || right_type == BOOL) {
                    errorOccurred = true;
                    ast->inferred_type.type_name = ANY;
                    perror("Bad operand type: [BINARY]<, <=, >, >=: operand must be arithmetic types\n");
                } else {
                    if (left_is_vec != right_is_vec) {
                        errorOccurred = true;
                        ast->inferred_type.type_name = ANY;
                        perror("Bad operand type: [BINARY]<, <=, >, >=: cannot compare vector with scalar\n");
                    } else if (left_is_vec) {
                        //vv
                        if (left_vec_size != right_vec_size) {
                            errorOccurred = true;
                            ast->inferred_type.type_name = ANY;
                            perror("Bad operand type: [BINARY]<, <=, >, >=: vector with different sizes\n");
                        } else {
                            ast->inferred_type.type_name = BOOL;
                            ast->inferred_type.is_vec = 0;
                            ast->inferred_type.is_const = left_is_const & right_is_const;
                        }
                    } else {
                        //ss
                        ast->inferred_type.type_name = BOOL;
                        ast->inferred_type.is_vec = 0;
                        ast->inferred_type.is_const = left_is_const & right_is_const;
                    }
                }
            }
            
            else if (ast->binary_expr.op == ADD   ||
                    ast->binary_expr.op == SUBTRACT) {
                //ss, vv
                if ((left_type == BOOL || right_type == BOOL) 
                        || (left_type == ANY || right_type == ANY)) {
                    errorOccurred = true;
                    ast->inferred_type.type_name = ANY;
                    perror("Bad operand type: [BINARY]+, -: both operand must not be BOOL\n");
                } else if (left_type == right_type) {
                    if (left_is_vec != right_is_vec) {
                        errorOccurred = true;
                        ast->inferred_type.type_name = ANY;
                        perror("Bad operand type: [BINARY]+, -: operand type incompatible\n");
                    } else {
                        if (left_is_vec) {
                            if (left_vec_size != right_vec_size) {
                                errorOccurred = true;
                                ast->inferred_type.type_name = ANY;
                                perror("Bad operand type: [BINARY]+, -: vector with different size\n");
                            } else {
                                ast->inferred_type.type_name = left_type;
                                ast->inferred_type.is_vec = 1;
                                ast->inferred_type.vec_size = left_vec_size;
                                ast->inferred_type.is_const = (left_is_const & right_is_const); //FIXME: left and right is_const
                            }
                            
                        } else {
                            ast->inferred_type.type_name = left_type;
                            ast->inferred_type.is_vec = 0;
                            ast->inferred_type.is_const = (left_is_const & right_is_const); //FIXME: left and right is_const
                        }
                    }
                } else {
                    errorOccurred = true;
                    ast->inferred_type.type_name = ANY;
                    perror("Bad operand type: [BINARY]+, -: operands not the same type\n");
                }
            } 
            
            else if (ast->binary_expr.op == MULTIPLY) {
                //ss, vv, sv, vs
                if (left_type == BOOL || right_type == BOOL) {
                    errorOccurred = true;
                    ast->inferred_type.type_name = ANY;
                    perror("Bad operand type: [BINARY]*: both operand must not be BOOL\n");
                } else if (left_type == right_type) {
                    if (left_type == ANY || right_type == ANY) {
                        errorOccurred = true;
                        ast->inferred_type.type_name = ANY;
                        perror("Bad operand type: [BINARY]*: operand(s) invalid\n");
                    } else {
                        if (!left_is_vec && !right_is_vec) {
                            //ss
                            ast->inferred_type.type_name = left_type;
                            ast->inferred_type.is_vec = 0;
                            ast->inferred_type.is_const = left_is_const & right_is_const;
                        } else if (!left_is_vec) {
                            //sv
                            ast->inferred_type.type_name = right_type;
                            ast->inferred_type.is_vec = 1;
                            ast->inferred_type.vec_size = right_vec_size;
                            ast->inferred_type.is_const = left_is_const & right_is_const;
                        } else if (!right_is_vec) {
                            //vs
                            ast->inferred_type.type_name = left_type;
                            ast->inferred_type.is_vec = 1;
                            ast->inferred_type.vec_size = left_vec_size;
                            ast->inferred_type.is_const = left_is_const & right_is_const;
                        } else {
                            //vv
                            if (left_vec_size != right_vec_size) {
                                errorOccurred = true;
                                ast->inferred_type.type_name = ANY;
                                perror("Bad operand type: [BINARY]*: multiple vectors with different sizes\n");
                            } else {
                                ast->inferred_type.type_name = left_type;
                                ast->inferred_type.is_vec = 1;
                                ast->inferred_type.vec_size = left_vec_size;
                                ast->inferred_type.is_const = left_is_const & right_is_const;
                            }
                        }
                    }
                } else {
                    errorOccurred = true;
                    ast->inferred_type.type_name = ANY;
                    perror("Bad operand type: [BINARY]*: operands not the same type\n");
                }
            } 
            
            else if (ast->binary_expr.op == DIVIDE    ||
                    ast->binary_expr.op == POWER) {
                //ss
                if (left_type == BOOL || right_type == BOOL) {
                    errorOccurred = true;
                    ast->inferred_type.type_name = ANY;
                    perror("Bad operand type: [BINARY]/, ^: both operand must not be BOOL\n");
                } else if (left_type == right_type) {
                    if (left_is_vec || right_is_vec) {
                        errorOccurred = true;
                        ast->inferred_type.type_name = ANY;
                        perror("Bad operand type: [BINARY]/, ^: both operand must be scalar\n");
                    } else {
                        ast->inferred_type.type_name = left_type;
                        ast->inferred_type.is_vec = 0;
                        ast->inferred_type.is_const = left_is_const & right_is_const;
                    }
                } else {
                    ast->inferred_type.type_name = ANY;
                    perror("Bad operand type: [BINARY]/, ^: operands not the same type\n");
                }
            }
        }
            break;
            
        case IF_STATEMENT_NODE:
            ast_semantic_check_help(ast->if_statement_node.if_condition, curr_scope);
            ast_semantic_check_help(ast->if_statement_node.statement, curr_scope);
            ast_semantic_check_help(ast->if_statement_node.else_statement, curr_scope);
            if (ast->if_statement_node.if_condition->inferred_type.type_name != BOOL) {
                errorOccurred = true;
                ast->inferred_type.type_name = ANY;
                perror("Bad expr type: [IF]: if expr must be evaluated to BOOL type\n");
            } else {
                if (ast->if_statement_node.if_condition->inferred_type.is_vec) {
                    ast->inferred_type.type_name = ANY;
                    perror("Bad expr type: [IF]: if condition cannot be bvec type\n");
                } else {
                    ast->inferred_type.type_name = BOOL;
                }
                    
            }
            break;
            
        case ELSE_STATEMENT_NODE:
            ast_semantic_check_help(ast->else_statement_node.else_statement, curr_scope);
            break;
            
        case ASSIGNMENT_STATEMENT_NODE:
        {
            ast_semantic_check_help(ast->assign_statement.var, curr_scope);
            ast_semantic_check_help(ast->assign_statement.expr, curr_scope);
            
            type_id var_type = ast->assign_statement.var->inferred_type.type_name;
            int var_is_vec = ast->assign_statement.var->inferred_type.is_vec;
            int var_vec_size = ast->assign_statement.var->inferred_type.vec_size;
            int var_is_const = ast->assign_statement.var->inferred_type.is_const;
            type_id expr_type = ast->assign_statement.expr->inferred_type.type_name;
            int expr_is_vec = ast->assign_statement.expr->inferred_type.is_vec;
            int expr_vec_size = ast->assign_statement.expr->inferred_type.vec_size;
            int expr_is_const = ast->assign_statement.expr->inferred_type.is_const;
            
            if (var_type != expr_type) {
                errorOccurred = true;
                ast->inferred_type.type_name = ANY;
                perror("Bad operand type: [Assignment]: operands with different type\n");
            } else {
                if (var_is_vec != expr_is_vec) {
                    errorOccurred = true;
                    ast->inferred_type.type_name = ANY;
                    perror("Bad operand type: [Assignment]: vector and scalar incompatible\n");
                } else if (var_vec_size != expr_vec_size) {
                    errorOccurred = true;
                    ast->inferred_type.type_name = ANY;
                    perror("Bad operand type: [Assignment]: vector with different sizes\n");
                } else if (var_is_const && !expr_is_const) {
                    errorOccurred = true;
                    ast->inferred_type.type_name = ANY;
                    perror("Bad operand type: [Assignment]: var of const type\n");
                } else {
                    ast->inferred_type.type_name = var_type;
                    ast->inferred_type.is_vec = var_is_vec;
                    ast->inferred_type.vec_size = var_vec_size;
                    ast->inferred_type.is_const = var_is_const;
                }
            }
        }
            break;

        case FUNCTION_NODE:
            ast_semantic_check_help(ast->function.args, curr_scope);
            break;

        case CONSTRUCTOR_NODE:
            ast_semantic_check_help(ast->constructor.args, curr_scope);
            ast_semantic_check_help(ast->constructor.type, curr_scope);
            semantic_check_constructor_call(ast);
            break;
        
        case ARGS_NODE:
            ast_semantic_check_help(ast->args_node.args, curr_scope);
            ast_semantic_check_help(ast->args_node.expr, curr_scope);
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

bool semantic_check_constructor_call(node* cstr_stmt) {
    if (cstr_stmt == NULL) {
        printf("[Error]Semantic-check: Constructor node is NULL\n");
        return 0;
    }
    bool err = false;
    
    type_id cstr_type_id = stype_to_type_id(name_to_stype(cstr_stmt->constructor.type->type_node.type_name));
    bool is_vec = cstr_stmt->constructor.type->type_node.is_vec;
    int vec_size = cstr_stmt->constructor.type->type_node.vec_size;
    
    int args_count = 0;
    node* curr_args = cstr_stmt->constructor.args;
    if (!curr_args) {
        printf("[Error]Semantic-check: Constructor has no args.\n");
        return 0;
    }

    while(curr_args) {
        args_count++;
        node* curr_arg_expr = curr_args->args_node.expr;
        // Check if each arg has same type as constructor, then check arg count matches
        struct node_type curr_arg_type = curr_arg_expr->inferred_type;
        if (curr_arg_type.is_vec) {
            printf("[Error]Semantic-check: Constructor argument %d should not be a vector.\n", args_count);
            err = true;
        }
        if (curr_arg_type.type_name != cstr_type_id) {
            printf("[Error]Semantic-check: Constructor argument %d has type %d, expecting %d.\n", args_count, curr_arg_type.type_name, cstr_type_id);
            err = true;
        }
        curr_args = curr_args->args_node.args;
    }
    if (!is_vec && args_count != 1) {
        printf("[Error]Semantic-check: Constructor expects exactly 1 argument, actually passing in %d.\n", args_count);
        err = true;
    } else if (is_vec && args_count != vec_size) {
        printf("[Error]Semantic-check: Constructor expects exactly %d arguments, actually passing in %d.\n", vec_size, args_count);
        err = true;
    }
    
    return !err;
}