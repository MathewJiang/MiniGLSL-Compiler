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
                fprintf(errorFile, "[Error]Scope checking: root scope already exists;\n");
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
                fprintf(errorFile, "[Error]Scope checking: var declaration for %s not valid, changing stype to UNKNOWN\n", ast->declaration.id);
                candidate_sentry->stype = UNKNOWN_TYPE;
                candidate_sentry->valid = false;
                errorOccurred = true;
            }
            
            // Check for predef var redeclarations
            predef_var* pvar = get_predef_var_by_id(ast->declaration.id);
            if (pvar) {
                fprintf(errorFile, "[Error]Scope checking: var declaration of %s overlaps with a pre-defined variable.\n", ast->declaration.id);
                free(pvar);
                pvar = NULL;
                errorOccurred = true;
                break;
            }
            
            // Add candidate to scope table
            sentry_push(candidate_sentry, curr_scope);
            snode_print(curr_scope);
            
            if (ast->declaration.expr) {
                ast_semantic_check_help(ast->declaration.expr, curr_scope);
                
                //TODO: add for the uniform type as well
                int var_is_const = ast->declaration.is_const;
                int expr_is_const = ast->declaration.expr->inferred_type.is_const;
                if (var_is_const && !expr_is_const) {
                    errorOccurred = true;
                    ast->inferred_type.type_name = ANY;
                    fprintf(errorFile, "[Error]Semantic-check: Bad operand: [DECLARATION]: assign const variable non_const values\n");
                }
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
            predef_var* pvar = get_predef_var_by_id(ast->var_node.id);
            if (!var_entry && !pvar) {
                fprintf(errorFile, "[Error]Scope checking: variable %s was not declared before use\n", ast->var_node.id);
                ast->inferred_type.type_name = ANY;
                errorOccurred = true;
            } else if (pvar) {
                if (ast->var_node.vec_idx != -1) {
                    //e.g. gl_Material_Shininess[0]
                    if (ast->var_node.vec_idx < 0 || ast->var_node.vec_idx >= pvar->vec_size) {
                        fprintf(errorFile, "[Error]Semantic-check: Bad operand: [VAR]: index out of bound\n");
                        ast->inferred_type.type_name = ANY;
                        errorOccurred = true;
                    } else {
                        ast->inferred_type.type_name = pvar->type_name;
                        ast->inferred_type.is_const = 1;
                        ast->inferred_type.is_vec = 0;
                        ast->inferred_type.vec_size = 0;
                    }
                } else {
                    ast->inferred_type.predef_info = pvar;
                    ast->inferred_type.type_name = pvar->type_name;
                    ast->inferred_type.is_const = pvar->is_const;
                    ast->inferred_type.is_vec = pvar->is_vec;
                    ast->inferred_type.vec_size = pvar->vec_size;
                }
            } else if (!var_entry->valid || var_entry->stype == UNKNOWN_TYPE) {
                fprintf(errorFile, "[Error]Scope checking: reference to INVALID variable %s. possibly not successfully declared.\n", ast->var_node.id);
                ast->inferred_type.type_name = ANY;
                errorOccurred = true;
            } else {
                if (ast->var_node.vec_idx != -1) {
                    //e.g.f[0]
                    if (ast->var_node.vec_idx < 0 || ast->var_node.vec_idx >= var_entry->vec_size) {
                        fprintf(errorFile, "[Error]Semantic-check: Bad operand: [VAR]: index out of bound\n");
                        ast->inferred_type.type_name = ANY;
                        errorOccurred = true;
                    } else {
                        ast->inferred_type.type_name = stype_to_type_id(var_entry->stype);
                        ast->inferred_type.is_const = var_entry->is_const;
                        ast->inferred_type.is_vec = 0;
                        ast->inferred_type.vec_size = 0;
                    }
                } else {
                    // Save inferred data into ast node struct.
                    ast->inferred_type.type_name = stype_to_type_id(var_entry->stype);
                    ast->inferred_type.is_const = var_entry->is_const;
                    ast->inferred_type.is_vec = var_entry->is_vec;
                    ast->inferred_type.vec_size = var_entry->vec_size;
                }
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
                    fprintf(errorFile, "[Error]Semantic-check: Bad operand type: [UNARY]!: operand must be BOOL type\n");
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
                    fprintf(errorFile, "[Error]Semantic-check: Bad operand type: [UNARY]-: operand must be arithmetic types\n");
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
                fprintf(errorFile, "[Error]Semantic-check: Unknown unary type\n");
            }
        }
            break;

        case BINARY_EXPRESSION_NODE:
        {
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
                    fprintf(errorFile, "[Error]Semantic-check: Bad operand type: [BINARY]&&, ||: both operand must be BOOL\n");
                } else {
                    if (left_is_vec != right_is_vec) {
                        errorOccurred = true;
                        ast->inferred_type.type_name = ANY;
                        fprintf(errorFile, "[Error]Semantic-check: Bad operand type: [BINARY]&&, ||: operands type incompatible\n");
                    } else {
                        if (left_is_vec) {
                            //vv
                            if (left_vec_size != right_vec_size) {
                                errorOccurred = true;
                                ast->inferred_type.type_name = ANY;
                                fprintf(errorFile, "[Error]Semantic-check: Bad operand type: [BINARY]&&, ||: vector with different size\n");
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
                    fprintf(errorFile, "[Error]Semantic-check: Bad operand type: [BINARY]<, <=, >, >=: cannot compare with different types\n");
                } else if (left_type == BOOL || right_type == BOOL) {
                    errorOccurred = true;
                    ast->inferred_type.type_name = ANY;
                    fprintf(errorFile, "[Error]Semantic-check: Bad operand type: [BINARY]<, <=, >, >=: operand must be arithmetic types\n");
                } else {
                    if (left_is_vec || right_is_vec) {
                        errorOccurred = true;
                        ast->inferred_type.type_name = ANY;
                        fprintf(errorFile, "[Error]Semantic-check: Bad operand type: [BINARY]<, <=, >, >=: both operand must be scalar\n");
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
                    fprintf(errorFile, "[Error]Semantic-check: Bad operand type: [BINARY]==, !=: cannot compare with different types\n");
                } else if (left_type == BOOL || right_type == BOOL) {
                    errorOccurred = true;
                    ast->inferred_type.type_name = ANY;
                    fprintf(errorFile, "[Error]Semantic-check: Bad operand type: [BINARY]<, <=, >, >=: operand must be arithmetic types\n");
                } else {
                    if (left_is_vec != right_is_vec) {
                        errorOccurred = true;
                        ast->inferred_type.type_name = ANY;
                        fprintf(errorFile, "[Error]Semantic-check: Bad operand type: [BINARY]<, <=, >, >=: cannot compare vector with scalar\n");
                    } else if (left_is_vec) {
                        //vv
                        if (left_vec_size != right_vec_size) {
                            errorOccurred = true;
                            ast->inferred_type.type_name = ANY;
                            fprintf(errorFile, "[Error]Semantic-check: Bad operand type: [BINARY]<, <=, >, >=: vector with different sizes\n");
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
                    fprintf(errorFile, "[Error]Semantic-check: Bad operand type: [BINARY]+, -: both operand must not be BOOL\n");
                } else if (left_type == right_type) {
                    if (left_is_vec != right_is_vec) {
                        errorOccurred = true;
                        ast->inferred_type.type_name = ANY;
                        fprintf(errorFile, "[Error]Semantic-check: Bad operand type: [BINARY]+, -: operand type incompatible\n");
                    } else {
                        if (left_is_vec) {
                            if (left_vec_size != right_vec_size) {
                                errorOccurred = true;
                                ast->inferred_type.type_name = ANY;
                                fprintf(errorFile, "[Error]Semantic-check: Bad operand type: [BINARY]+, -: vector with different size\n");
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
                    fprintf(errorFile, "[Error]Semantic-check: Bad operand type: [BINARY]+, -: operands not the same type\n");
                }
            } 
            
            else if (ast->binary_expr.op == MULTIPLY) {
                //ss, vv, sv, vs
                if (left_type == BOOL || right_type == BOOL) {
                    errorOccurred = true;
                    ast->inferred_type.type_name = ANY;
                    fprintf(errorFile, "[Error]Semantic-check: Bad operand type: [BINARY]*: both operand must not be BOOL\n");
                } else if (left_type == right_type) {
                    if (left_type == ANY || right_type == ANY) {
                        errorOccurred = true;
                        ast->inferred_type.type_name = ANY;
                        fprintf(errorFile, "[Error]Semantic-check: Bad operand type: [BINARY]*: operand(s) invalid\n");
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
                                fprintf(errorFile, "[Error]Semantic-check: Bad operand type: [BINARY]*: multiple vectors with different sizes\n");
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
                    fprintf(errorFile, "[Error]Semantic-check: Bad operand type: [BINARY]*: operands not the same type\n");
                }
            } 
            
            else if (ast->binary_expr.op == DIVIDE    ||
                    ast->binary_expr.op == POWER) {
                //ss
                if (left_type == BOOL || right_type == BOOL) {
                    errorOccurred = true;
                    ast->inferred_type.type_name = ANY;
                    fprintf(errorFile, "[Error]Semantic-check: Bad operand type: [BINARY]/, ^: both operand must not be BOOL\n");
                } else if (left_type == right_type) {
                    if (left_is_vec || right_is_vec) {
                        errorOccurred = true;
                        ast->inferred_type.type_name = ANY;
                        fprintf(errorFile, "[Error]Semantic-check: Bad operand type: [BINARY]/, ^: both operand must be scalar\n");
                    } else {
                        ast->inferred_type.type_name = left_type;
                        ast->inferred_type.is_vec = 0;
                        ast->inferred_type.is_const = left_is_const & right_is_const;
                    }
                } else {
                    ast->inferred_type.type_name = ANY;
                    fprintf(errorFile, "[Error]Semantic-check: Bad operand type: [BINARY]/, ^: operands not the same type\n");
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
                fprintf(errorFile, "[Error]Semantic-check: Bad expr type: [IF]: if expr must be evaluated to BOOL type\n");
            } else {
                if (ast->if_statement_node.if_condition->inferred_type.is_vec) {
                    ast->inferred_type.type_name = ANY;
                    fprintf(errorFile, "[Error]Semantic-check: Bad expr type: [IF]: if condition cannot be bvec type\n");
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
                fprintf(errorFile, "[Error]Semantic-check: Bad operand type: [Assignment]: operands with different type\n");
            } else {
                if (var_is_vec != expr_is_vec) {
                    errorOccurred = true;
                    ast->inferred_type.type_name = ANY;
                    fprintf(errorFile, "[Error]Semantic-check: Bad operand type: [Assignment]: vector and scalar incompatible\n");
                } else if (var_vec_size != expr_vec_size && (var_vec_size != -1 && expr_vec_size == 0)) {
                    errorOccurred = true;
                    ast->inferred_type.type_name = ANY;
                    fprintf(errorFile, "[Error]Semantic-check: Bad operand type: [Assignment]: vector with different sizes\n");
                } else if (var_is_const && !expr_is_const) {
                    errorOccurred = true;
                    ast->inferred_type.type_name = ANY;
                    fprintf(errorFile, "[Error]Semantic-check: Bad operand type: [Assignment]: var of const type\n");
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
            errorOccurred |= semantic_check_function_call(ast);
//            printf("[debug]Function call to %s's inferred type: type_name=%s, is_const=%d, is_vec=%d, vec_size=%d\n\n",
//                    ast->function.func_id, get_type_id_name(ast->inferred_type.type_name),
//                    ast->inferred_type.is_const, ast->inferred_type.is_vec, ast->inferred_type.vec_size);
            break;

        case CONSTRUCTOR_NODE:
            ast_semantic_check_help(ast->constructor.args, curr_scope);
            ast_semantic_check_help(ast->constructor.type, curr_scope);
            errorOccurred |= semantic_check_constructor_call(ast);
            break;
        
        case ARGS_NODE:
            ast_semantic_check_help(ast->args_node.args, curr_scope);
            ast_semantic_check_help(ast->args_node.expr, curr_scope);
            break;
    
        default:
            fprintf(errorFile, "[debug]AST Semantic check: unknown AST node\n");
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
        fprintf(errorFile, "[Error]Scope checking: ast node checked is not a declaration.\n");
    }
    return find_sentry_in_snode_by_id(ast->declaration.id, curr_scope) ? 0 : 1; // NOT valid if found!!!
}

// return 1 if error occurs, 0 if succeed.
bool semantic_check_constructor_call(node* cstr_stmt) {
    if (cstr_stmt == NULL) {
        fprintf(errorFile, "[Error]Semantic-check: Constructor node is NULL\n");
        return 1;
    }
    bool err = false;
    
    type_id cstr_type_id = stype_to_type_id(name_to_stype(cstr_stmt->constructor.type->type_node.type_name));
    bool is_vec = cstr_stmt->constructor.type->type_node.is_vec;
    int vec_size = cstr_stmt->constructor.type->type_node.vec_size;
    
    int args_count = 0;
    node* curr_args = cstr_stmt->constructor.args;
    if (!curr_args) {
        fprintf(errorFile, "[Error]Semantic-check: Constructor has no args.\n");
        // Record inferred return type to constructor node.
        cstr_stmt->inferred_type.type_name = ANY;
        cstr_stmt->inferred_type.is_vec = is_vec;
        cstr_stmt->inferred_type.vec_size = vec_size;
        cstr_stmt->inferred_type.is_const = is_vec ? 0 : 1;
        return 1;
    }

    while(curr_args) {
        args_count++;
        node* curr_arg_expr = curr_args->args_node.expr;
        // Check if each arg has same type as constructor, then check arg count matches
        struct node_type curr_arg_type = curr_arg_expr->inferred_type;
        if (curr_arg_type.is_vec) {
            fprintf(errorFile, "[Error]Semantic-check: Constructor argument %d should not be a vector.\n", args_count);
            err = true;
        }
        if (curr_arg_type.type_name != cstr_type_id) {
            fprintf(errorFile, "[Error]Semantic-check: Constructor argument %d has type %s, expecting %s.\n", args_count,
                    get_type_id_name(curr_arg_type.type_name), get_type_id_name(cstr_type_id));
            err = true;
        }
        curr_args = curr_args->args_node.args;
    }
    if (!is_vec && args_count != 1) {
        fprintf(errorFile, "[Error]Semantic-check: Constructor expects exactly 1 argument, actually passing in %d.\n", args_count);
        err = true;
    } else if (is_vec && args_count != vec_size) {
        fprintf(errorFile, "[Error]Semantic-check: Constructor expects exactly %d arguments, actually passing in %d.\n", vec_size, args_count);
        err = true;
    }
    
    // Record inferred return type to constructor node.
    cstr_stmt->inferred_type.type_name = err ? ANY : cstr_type_id;
    cstr_stmt->inferred_type.is_vec = is_vec;
    cstr_stmt->inferred_type.vec_size = vec_size;
    cstr_stmt->inferred_type.is_const = 1;
    return err;
}

// return 1 if error occurs, 0 if succeed.
bool semantic_check_function_call(node* func_call) {
    if (func_call == NULL) {
        fprintf(errorFile, "[Error]Semantic-check: Function node is NULL\n");
        return 1;
    }
    bool err = false;
    
    int expected_args_count = 0;
    type_id expected_arg_type = ANY;
    int expected_arg_is_vec = false;
    int expected_arg_vec_size_1 = 0;
    int expected_arg_vec_size_2 = 0;
    type_id return_type = ANY;
    int return_val_is_vec = 0;
    int return_val_is_const = 0;
    int return_val_vec_size = 0;
    
    switch (func_name_to_id(func_call->function.func_id)) {
        case LIT:
            expected_args_count = 1;
            expected_arg_type = FLOAT;
            expected_arg_is_vec = true;
            expected_arg_vec_size_1 = 4;
            expected_arg_vec_size_2 = 4;
            return_type = FLOAT;
            return_val_is_vec = true;
            return_val_vec_size = 4;
            return_val_is_const = 0;
            break;
        case RSQ:
            expected_args_count = 1;
            expected_arg_type = NUMBER;
            expected_arg_is_vec = false;
            expected_arg_vec_size_1 = 0;
            expected_arg_vec_size_2 = 0;
            return_type = FLOAT;
            return_val_is_vec = false;
            return_val_vec_size = 0;
            return_val_is_const = 1;
            break;
        case DP3:
            expected_args_count = 2;
            expected_arg_type = NUMBER;
            expected_arg_is_vec = true;
            expected_arg_vec_size_1 = 3;
            expected_arg_vec_size_2 = 4;
            return_type = FLOAT;
            return_val_is_vec = false;
            return_val_vec_size = 0;
            return_val_is_const = 1;
            break;
        default:
        {
            fprintf(errorFile, "[error]Semantic-check: Unsupported func call to %s\n", func_call->function.func_id);
            // Record inferred type into func_call node.
            func_call->inferred_type.type_name = ANY;
            func_call->inferred_type.is_vec = return_val_is_vec;
            func_call->inferred_type.vec_size = return_val_vec_size;
            func_call->inferred_type.is_const = return_val_is_const;
            return 1;
        }
            break;
    }
    
    int args_count = 0;
    int first_arg_vec_size = -1;
    type_id first_arg_vec_type = ANY;
    node* curr_args = func_call->function.args;
    if (!curr_args) {
        fprintf(errorFile, "[Error]Semantic-check: Function has no args.\n");
        // Record inferred return type to Func node.
        func_call->inferred_type.type_name = ANY;
        func_call->inferred_type.is_vec = return_val_is_vec;
        func_call->inferred_type.vec_size = return_val_vec_size;
        func_call->inferred_type.is_const = return_val_is_const;
        return 1;
    }

    while(curr_args) {
        args_count++;
        node* curr_arg_expr = curr_args->args_node.expr;
        // Check if each arg has same type as constructor, then check arg count matches
        struct node_type curr_arg_type = curr_arg_expr->inferred_type;
        if (curr_arg_type.is_vec != expected_arg_is_vec) {
            // Expecting a vector/scalar argument but passed in arg is not.
            fprintf(errorFile, "[Error]Semantic-check: Function call to %s expects %svectors, but argument %d is %sa vector.\n", func_call->function.func_id,
                    expected_arg_is_vec ? "" : "no ", args_count, curr_arg_type.is_vec ? "" : "not ");
            err = true;
        }
        if (curr_arg_type.is_vec && expected_arg_is_vec // This is printed only if expecting a vector(but passed in wrong size).
                && curr_arg_type.vec_size != expected_arg_vec_size_1
                && curr_arg_type.vec_size != expected_arg_vec_size_2) {
            fprintf(errorFile, "[Error]Semantic-check: Function call to %s has argument vector %d with size %d, expecting size %d",
                    func_call->function.func_id, args_count, curr_arg_type.vec_size, expected_arg_vec_size_1);
            if (expected_arg_vec_size_1 != expected_arg_vec_size_2) {
                fprintf(errorFile, " or %d", expected_arg_vec_size_2);
            }
            fprintf(errorFile, ".\n");
            err = true;
        }

        if (curr_arg_type.type_name != expected_arg_type &&
                (expected_arg_type != NUMBER || 
                        (expected_arg_type == NUMBER
                           && curr_arg_type.type_name != INT
                           && curr_arg_type.type_name != FLOAT))) {
            fprintf(errorFile, "[Error]Semantic-check: Function call to %s has argument %d as a %s%s, expecting %s%s.\n",
                    func_call->function.func_id, args_count, get_type_id_name(curr_arg_type.type_name),
                    curr_arg_type.is_vec ? " vector" : "",
                    expected_arg_type == NUMBER ? "int or float" : get_type_id_name(expected_arg_type),
                    expected_arg_is_vec ? (expected_args_count > 1 ? " vectors" : " vector") : "");
            err = true;
        }
        
        // Use first arg's vec_size and type to check second arg (IF VALID).
        if (first_arg_vec_size == -1 && curr_arg_type.is_vec && !err) {
            first_arg_vec_size = curr_arg_type.vec_size;
            first_arg_vec_type = curr_arg_type.type_name;
        } 
        if (first_arg_vec_size != -1 && first_arg_vec_type != ANY && curr_arg_type.is_vec && !err) {
            if (curr_arg_type.vec_size != first_arg_vec_size || curr_arg_type.type_name != first_arg_vec_type) {
                fprintf(errorFile, "[Error]Semantic-check: Function call seems valid so far, but argument %d is a vector with inconsistent",
                    args_count);
                if (curr_arg_type.vec_size != first_arg_vec_size) fprintf(errorFile, " size");
                if (curr_arg_type.vec_size != first_arg_vec_size && curr_arg_type.type_name != first_arg_vec_type) fprintf(errorFile, " and");
                if (curr_arg_type.type_name != first_arg_vec_type) fprintf(errorFile, " type");
            
                fprintf(errorFile, " with previous vector arguments.\n");
                err = true;
            }
        }
        
        curr_args = curr_args->args_node.args;
    }
    if (expected_args_count != args_count) {
        fprintf(errorFile, "[Error]Semantic-check: Function call to %s expects exactly %d arguments, actually passing in %d.\n",
                func_call->function.func_id, expected_args_count, args_count);
        err = true;
    }
    
    // Record inferred return type to constructor node.
    func_call->inferred_type.type_name = err ? ANY : return_type;
    func_call->inferred_type.is_vec = return_val_is_vec;
    func_call->inferred_type.vec_size = return_val_vec_size;
    func_call->inferred_type.is_const = return_val_is_const;
    return err;
}

predef_var* alloc_predef_var(char* id, type_class clazz, type_id type_name, bool is_vec, int vec_size) {
    predef_var* pvar = (predef_var*)malloc(sizeof(predef_var));
    
    pvar->id = id;
    pvar->clazz = clazz;
    pvar->type_name = type_name;
    pvar->is_vec = is_vec;
    pvar->vec_size = vec_size;
    
    switch (clazz) {
        case ATTRIBUTE: // Read only, non-const
            pvar->is_const = 0;
            pvar->is_readable = 1;
            pvar->is_writable = 0;
            break;
        case UNIFORM:
            pvar->is_const = 1;
            pvar->is_readable = 1;
            pvar->is_writable = 0;
            break;
        case RESULT:
            pvar->is_const = 0;
            pvar->is_readable = 0;
            pvar->is_writable = 1;
            break;
        default:
            fprintf(errorFile, "[error]Semantic-check: Declaration of predef var %s failed with unknown class %d\n", id, clazz);
            free(pvar);
            return NULL;
    }
    return pvar;
}

predef_var* get_predef_var_by_id(char* id) {
    if (!id) return NULL;
    if (!strcmp(id, "gl_FragColor")) {
        return alloc_predef_var(id, RESULT, FLOAT, true, 4);
    } else if (!strcmp(id, "gl_FragDepth")) {
        return alloc_predef_var(id, RESULT, BOOL, false, -1);
    } else if (!strcmp(id, "gl_FragCoord")) {
        return alloc_predef_var(id, RESULT, FLOAT, true, 4);
    } else if (!strcmp(id, "gl_TexCoord")) {
        return alloc_predef_var(id, ATTRIBUTE, FLOAT, true, 4);
    } else if (!strcmp(id, "gl_Color")) {
        return alloc_predef_var(id, ATTRIBUTE, FLOAT, true, 4);
    } else if (!strcmp(id, "gl_Secondary")) {
        return alloc_predef_var(id, ATTRIBUTE, FLOAT, true, 4);
    } else if (!strcmp(id, "gl_FogFragCoord")) {
        return alloc_predef_var(id, ATTRIBUTE, FLOAT, true, 4);
    } else if (!strcmp(id, "gl_Light_Half")) {
        return alloc_predef_var(id, UNIFORM, FLOAT, true, 4);
    } else if (!strcmp(id, "gl_Light_Ambient")) {
        return alloc_predef_var(id, UNIFORM, FLOAT, true, 4);
    } else if (!strcmp(id, "gl_Material_Shininess")) {
        return alloc_predef_var(id, UNIFORM, FLOAT, true, 4);
    } else if (!strcmp(id, "env1")) {
        return alloc_predef_var(id, UNIFORM, FLOAT, true, 4);
    } else if (!strcmp(id, "env2")) {
        return alloc_predef_var(id, UNIFORM, FLOAT, true, 4);
    } else if (!strcmp(id, "env3")) {
        return alloc_predef_var(id, UNIFORM, FLOAT, true, 4);
    }
    return NULL;
}