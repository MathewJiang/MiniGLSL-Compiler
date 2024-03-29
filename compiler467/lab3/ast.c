#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "common.h"
#include "parser.tab.h"

#define DEBUG_PRINT_TREE 0
#define VB_GLOB 0
#define VB_TRACE 0

void debugP(int verbose_flag, const char* str, ...);

node *ast = NULL;

node *ast_allocate(node_kind kind, ...) {
    va_list args;

    // make the node
    node *ast = (node *) malloc(sizeof (node));
    memset(ast, 0, sizeof *ast);
    ast->kind = kind;

    va_start(args, kind);

    switch (kind) {
        case SCOPE_NODE:
            ast->scope.declarations = va_arg(args, node *);
            ast->scope.statements = va_arg(args, node *);
            ast->line_num = va_arg(args, int);
            debugP(VB_TRACE, "\tSCOPE_NODE\n");
            break;

          case NESTED_SCOPE_NODE:
            ast->nested_scope.scope = va_arg(args, node *);
            ast->line_num = va_arg(args, int);
            debugP(VB_TRACE, "\tNESTED_SCOPE_NODE\n");
            break;
    
        case DECLARATIONS_NODE:
            ast->declarations.declarations = va_arg(args, node *);
            ast->declarations.declaration = va_arg(args, node *);
            ast->line_num = va_arg(args, int);
            debugP(VB_TRACE, "\tDECLARATIONS_NODE\n");
            break;
            
        case STATEMENTS_NODE:
            ast->statements.statements = va_arg(args, node *);
            ast->statements.statement = va_arg(args, node *);
            ast->line_num = va_arg(args, int);
            debugP(VB_TRACE, "\tSTATEMENTS_NODE\n");
            break;

        case DECLARATION_NODE:
            ast->declaration.is_const = va_arg(args, int);
            ast->declaration.type = va_arg(args, node *);

//            temp_str = (char *) malloc(32 * sizeof (char));
//            strcpy(temp_str, va_arg(args, char *));
//            printf("[debug]ast->declaration.id: %s\n", temp_str);
            ast->declaration.id = va_arg(args, char*);
//            printf("[debug]ast->declaration.id: %s\n", ast->declaration.id);
            ast->declaration.expr = va_arg(args, node *);
            ast->line_num = va_arg(args, int);
            debugP(VB_TRACE, "\tDECLARATION_NODE\n");
            break;

        case ASSIGNMENT_STATEMENT_NODE:
            ast->assign_statement.var = va_arg(args, node *);
            ast->assign_statement.expr = va_arg(args, node *);
            ast->line_num = va_arg(args, int);
            break;
            
        case IF_STATEMENT_NODE:
            ast->if_statement_node.if_condition = va_arg(args, node *);
            ast->if_statement_node.statement = va_arg(args, node *);
            ast->if_statement_node.else_statement = va_arg(args, node *);
            ast->line_num = va_arg(args, int);
            break;
            
        case ELSE_STATEMENT_NODE:
            ast->else_statement_node.else_statement = va_arg(args, node *);
            ast->line_num = va_arg(args, int);
            break;

        case TYPE_NODE:
//            temp_str = (char *) malloc(6 * sizeof (char));
//            strcpy(temp_str, va_arg(args, char*));
            ast->type_node.type_name = va_arg(args, char *);
            ast->type_node.is_vec = va_arg(args, int);
            ast->line_num = va_arg(args, int);
            ast->type_node.vec_size = -1;
            if (ast->type_node.is_vec) {
                switch (ast->type_node.type_name[0]) {
                    case 'i':
                        ast->type_node.vec_size = ast->type_node.type_name[4] - '0';
                        break;
                    case 'v':
                        ast->type_node.vec_size = ast->type_node.type_name[3] - '0';
                        break;
                    case 'b':
                        ast->type_node.vec_size = ast->type_node.type_name[4] - '0';
                        break;
                }
            }

            debugP(VB_TRACE, "Allocating type node with type=%s, is_vec=%d, vec_size=%d\n",
                    ast->type_node.type_name, ast->type_node.is_vec, ast->type_node.vec_size);
            break;

        case INT_NODE:
            ast->int_val = va_arg(args, int);
            ast->line_num = va_arg(args, int);
            debugP(VB_TRACE, "Allocating int node with val=%d\n", ast->int_val);
            break;

        case FLOAT_NODE:
            ast->float_val = va_arg(args, double);
            ast->line_num = va_arg(args, int);
            debugP(VB_TRACE, "Allocating float node with val=%.2lf\n", ast->float_val);
            break;

        case BOOL_NODE:
            if (va_arg(args, int) == 0) {
                ast->bool_val = 0;
            } else {
                ast->bool_val = 1;
            }
            ast->line_num = va_arg(args, int);
            debugP(VB_TRACE, "Allocating bool node with val=%d\n", ast->bool_val);
            break;

        case VAR_NODE:
//            temp_str = (char *) malloc(32 * sizeof (char));
//            strcpy(temp_str, va_arg(args, char*));
            ast->var_node.id = va_arg(args, char*);
            ast->var_node.is_vec = va_arg(args, int);
            ast->var_node.vec_idx = (ast->var_node.is_vec) ? (va_arg(args, int)) : (-1);
            ast->line_num = va_arg(args, int);
            debugP(VB_TRACE, "Allocating var node with id=%s, is_vec=%d, vec_idx=%d\n",
                    ast->var_node.id, ast->var_node.is_vec, ast->var_node.vec_idx);
            break;

        case UNARY_EXPRESSION_NODE:
            ast->unary_expr.op = va_arg(args, int);
            ast->unary_expr.right = va_arg(args, node *);
            ast->line_num = va_arg(args, int);
            debugP(VB_TRACE, "Allocating unary exp node, op=%d\n", ast->unary_expr.op);
            break;

        case BINARY_EXPRESSION_NODE:
            ast->binary_expr.op = va_arg(args, int);
            ast->binary_expr.left = va_arg(args, node *);
            ast->binary_expr.right = va_arg(args, node *);
            ast->line_num = va_arg(args, int);
            debugP(VB_TRACE, "Allocating binary exp node, op=%d\n", ast->binary_expr.op);
            break;

            // ...
        case FUNCTION_NODE:
//            temp_str = (char *) malloc(4 * sizeof (char));
//            strcpy(temp_str, va_arg(args, char*));
            ast->function.func_id = va_arg(args, char*);
            ast->function.args = va_arg(args, node*);
            ast->line_num = va_arg(args, int);
            debugP(VB_TRACE, "[ast_allocate]FUNCTION_NODE: func_id: %s\n", ast->function.func_id);
            break;

        case CONSTRUCTOR_NODE:
            ast->constructor.type = va_arg(args, node *);
            ast->constructor.args = va_arg(args, node *);
            ast->line_num = va_arg(args, int);
            debugP(VB_TRACE, "[ast_allocate]CONSTRUCTOR_NODE: type: %s\n", ast->constructor.type);
            break;
            
        case ARGS_NODE:
            ast->args_node.expr = va_arg(args, node *);
            ast->args_node.args = va_arg(args, node *);
            ast->line_num = va_arg(args, int);
            break;

        default: break;
    }

    va_end(args);

    return ast;
}

void ast_free(node *ast) {
    //TODO: do a recursive free (post-function)
    ast_destroy_help(ast);
    ast = NULL;
    printf("\nSuccessfully deallocated AST.\n");
}

void ast_destroy_help(node *ast) {
    if (!ast) return;
    
    switch (ast->kind) {
        case SCOPE_NODE:
            ast_destroy_help(ast->scope.statements);
            ast_destroy_help(ast->scope.declarations);
            free(ast);
            break;

          case NESTED_SCOPE_NODE:
            ast_destroy_help(ast->nested_scope.scope->scope.statements);
            ast_destroy_help(ast->nested_scope.scope->scope.declarations);
            free(ast);
            break;
    
        case DECLARATIONS_NODE:
            ast_destroy_help(ast->declarations.declarations);
            ast_destroy_help(ast->declarations.declaration);
            free(ast);
            break;
            
        case STATEMENTS_NODE:
            ast_destroy_help(ast->statements.statements);
            ast_destroy_help(ast->statements.statement);
            free(ast);
            break;

        case DECLARATION_NODE:
            ast_destroy_help(ast->declaration.type);
            ast_destroy_help(ast->declaration.expr);
            free(ast);
            break;

        case ASSIGNMENT_STATEMENT_NODE:
            ast_destroy_help(ast->assign_statement.expr);
            ast_destroy_help(ast->assign_statement.var);
            free(ast);
            break;
            
        case IF_STATEMENT_NODE:
            ast_destroy_help(ast->if_statement_node.if_condition);
            ast_destroy_help(ast->if_statement_node.else_statement);
            ast_destroy_help(ast->if_statement_node.statement);
            free(ast);
            break;
            
        case ELSE_STATEMENT_NODE:
            ast_destroy_help(ast->else_statement_node.else_statement);
            free(ast);
            break;

        case TYPE_NODE:
            free(ast);
            break;

        case INT_NODE:
            free(ast);
            break;

        case FLOAT_NODE:
            free(ast);
            break;

        case BOOL_NODE:
            free(ast);
            break;

        case VAR_NODE:
            free(ast);
            break;

        case UNARY_EXPRESSION_NODE:
            ast_destroy_help(ast->unary_expr.right);
            free(ast);
            break;

        case BINARY_EXPRESSION_NODE:
            ast_destroy_help(ast->binary_expr.left);
            ast_destroy_help(ast->binary_expr.right);
            free(ast);
            break;

        case FUNCTION_NODE:
            ast_destroy_help(ast->function.args);
            free(ast);
            break;

        case CONSTRUCTOR_NODE:
            ast_destroy_help(ast->constructor.type);
            ast_destroy_help(ast->constructor.args);
            free(ast);
            break;
            
        case ARGS_NODE:
            ast_destroy_help(ast->args_node.args);
            ast_destroy_help(ast->args_node.expr);
            free(ast);
            break;

        default: break;
    }
}

void ast_print(node *ast) {

    //debugP(VB_TRACE, "\tEnter ast_print\n");
    ast_print_help(ast, 0);
}

void ast_print_help(node *ast, int indent_num) {
    if (ast == NULL) {
        return;
    }
//    if (ast == NULL) {
//        perror("ast is NULL\n");
//        exit(1);
//    }
    //printf("\n[debug]Enter ast_print\n");

    indent_num++;
    node_kind kind = ast->kind;
    switch (kind) {
        case SCOPE_NODE:
            indent(indent_num);
            fprintf(dumpFile, "(");
            fprintf(dumpFile, "SCOPE ");
            
            if (ast->scope.declarations != NULL) {
                ast_print_help(ast->scope.declarations, indent_num);
            }
            if (ast->scope.statements != NULL) {
                ast_print_help(ast->scope.statements, indent_num);
            }
            fprintf(dumpFile, ")\n");
            break;
            
        case NESTED_SCOPE_NODE:
            indent(indent_num);
            fprintf(dumpFile, "(");
            fprintf(dumpFile, "NESTED_SCOPE ");
            
            if (!ast->nested_scope.scope) {
                perror("\n[error]Nested scope: lacks scope\n");
                exit(1);
            }
            
            if (ast->nested_scope.scope->scope.declarations != NULL) {
                ast_print_help(ast->nested_scope.scope->scope.declarations, indent_num);
            }
            if (ast->nested_scope.scope->scope.statements != NULL) {
                ast_print_help(ast->nested_scope.scope->scope.statements, indent_num);
            }
            fprintf(dumpFile, ")");
            break;
            
        case DECLARATIONS_NODE:
            indent(indent_num);
            fprintf(dumpFile, "(");
            fprintf(dumpFile, "DECLARATIONS ");
            
            if (ast->declarations.declarations != NULL) {
                ast_print_help(ast->declarations.declarations, indent_num);
            }
            if (ast->declarations.declaration != NULL) {
                ast_print_help(ast->declarations.declaration, indent_num);
            }
            fprintf(dumpFile, ")");
            break;

        case DECLARATION_NODE:
            indent(indent_num);
            fprintf(dumpFile, "(");
            fprintf(dumpFile, "DECLARATION ");

            if (ast->declaration.id != NULL) {
                //FIXME: error-prone
                fprintf(dumpFile, "%s ", ast->declaration.id);
            }

            if (ast->declaration.is_const == 1) {
                fprintf(dumpFile, "const ");
            }
            if (ast->declaration.type != NULL) {
                ast_print_help(ast->declaration.type, indent_num);
            }

            if (ast->declaration.expr != NULL) {
                ast_print_help(ast->declaration.expr, indent_num);
            }
            fprintf(dumpFile, ")");
            break;
            
        case STATEMENTS_NODE:
            indent(indent_num);
            fprintf(dumpFile, "(");
            fprintf(dumpFile, "STATEMENTS ");
            
            if (ast->statements.statements != NULL) {
                ast_print_help(ast->statements.statements, indent_num);
            }
            if (ast->statements.statement != NULL) {
                ast_print_help(ast->statements.statement, indent_num);
            }
            fprintf(dumpFile, ")");
            break;
        
        case TYPE_NODE:
            fprintf(dumpFile, "(");
            fprintf(dumpFile, "TYPE ");
            fprintf(dumpFile, "%s", ast->type_node.type_name);
            fprintf(dumpFile, ") ");
            break;

        case VAR_NODE:
            if (ast->var_node.is_vec) {
                //FIXME: not necessarily need (INDEX)... e.g vec4_obj = vec4_obj
                //print out the index if accessing array variable
                fprintf(dumpFile, "(INDEX ");
                print_type_id(ast->inferred_type.type_name, ast->inferred_type.is_vec, 
                    ast->inferred_type.vec_size, ast->inferred_type.is_const);
                fprintf(dumpFile, "%s ", ast->var_node.id);
                fprintf(dumpFile, "%d) ", ast->var_node.vec_idx);
            } else {
                fprintf(dumpFile, "%s ", ast->var_node.id);
            }
            break;

        case INT_NODE:
            fprintf(dumpFile, "%d ", ast->int_val);
            break;

        case FLOAT_NODE:
            fprintf(dumpFile, "%.2lf ", ast->float_val);
            break;

        case BOOL_NODE:
            //TODO: determine types for boolean variable
            //so that "true" or "false" will be printed
            if (ast->bool_val) {
                fprintf(dumpFile, "true ");
            } else {
                fprintf(dumpFile, "false ");
            }
            break;
            //            
            //        case EXPRESSION_NODE: 
            //            break;

        case UNARY_EXPRESSION_NODE:
            fprintf(dumpFile, "(UNARY ");
            //TODO: determine the resulting type afterwards
            //printf("ANY ");
            print_type_id(ast->inferred_type.type_name, ast->inferred_type.is_vec, 
                    ast->inferred_type.vec_size, ast->inferred_type.is_const);
            print_op_unary(ast->unary_expr.op);

            if (ast->unary_expr.right != NULL) {
                ast_print_help(ast->unary_expr.right, indent_num);
            } else {
                perror("\n[error]lack of left operand for BINARY\n");
                exit(1);
            }
            fprintf(dumpFile, ")");
            break;

        case BINARY_EXPRESSION_NODE:
            indent(indent_num);
            fprintf(dumpFile, "(BINARY ");
            //TODO: determine the inferred type afterwards 
            print_type_id(ast->inferred_type.type_name, ast->inferred_type.is_vec, 
                    ast->inferred_type.vec_size, ast->inferred_type.is_const);

            print_op(ast->binary_expr.op);

            if (ast->binary_expr.left != NULL) {
                ast_print_help(ast->binary_expr.left, indent_num);
            } else {
                perror("\n[error]lack of left operand for BINARY\n");
                exit(1);
            }

            if (ast->binary_expr.right != NULL) {
                ast_print_help(ast->binary_expr.right, indent_num);
            } else {
                perror("\n[error]lack of right operand for BINARY\n");
                exit(1);
            }
            fprintf(dumpFile, ")");            
            break;
            
        case IF_STATEMENT_NODE:
            if (!ast->if_statement_node.if_condition) {
                perror("\n[error]if statement node lack condition\n");
                exit(1);
            }
//            if (!ast->if_statement_node.expr) {
//                perror("\n[error]if statement node lack then expression\n");
//                exit(1);
//            }
            indent(indent_num);
            fprintf(dumpFile, "(IF ");
            ast_print_help(ast->if_statement_node.if_condition, indent_num);
            if (ast->if_statement_node.statement) {
                ast_print_help(ast->if_statement_node.statement, indent_num);
            }
            if (ast->if_statement_node.else_statement)
                ast_print_help(ast->if_statement_node.else_statement, indent_num);
            fprintf(dumpFile, ")");
            
            break;
            
        case ELSE_STATEMENT_NODE:
//            if (!ast->else_statement_node.else_statement) {
//                perror("\n[error]else statement node lack statement\n");
//                exit(1);
//            }
            indent(indent_num);
            fprintf(dumpFile, " (ELSE ");
            ast_print_help(ast->else_statement_node.else_statement, indent_num);
            fprintf(dumpFile, ")");
            break;
            
        case ASSIGNMENT_STATEMENT_NODE:
            if (!ast->assign_statement.var) {
                perror("\n[error]Assignment statement lack assignee name\n");
                exit(1);
            }
            if (!ast->assign_statement.expr) {
                perror("\n[error]Assignment statement lack assign value\n");
                exit(1);
            }
            indent(indent_num);
            fprintf(dumpFile, "(ASSIGN ");
            
            //TODO: determine the resulting type afterwards 
            print_type_id(ast->inferred_type.type_name, ast->inferred_type.is_vec, 
                    ast->inferred_type.vec_size, ast->inferred_type.is_const);
            ast_print_help(ast->assign_statement.var, indent_num);
            ast_print_help(ast->assign_statement.expr, indent_num);
            fprintf(dumpFile, ")");
            break;

        case FUNCTION_NODE:
            indent(indent_num);
            fprintf(dumpFile, "(CALL ");
            fprintf(dumpFile, "%s ", ast->function.func_id);
            if (ast->function.args != NULL) {
                ast_print_help(ast->function.args, indent_num);
            }
            fprintf(dumpFile, ")");
            break;
            
        case CONSTRUCTOR_NODE:
            if (!ast->constructor.type) {
                perror("\n[error]: constructor missing type\n");
                exit(1);
            }
            fprintf(dumpFile, "(CALL %s ", ast->constructor.type->type_node.type_name);
            ast_print_help(ast->constructor.args, indent_num);
            fprintf(dumpFile, ")");
            break;
            
        case ARGS_NODE:
            if (ast->args_node.args) {
                ast_print_help(ast->args_node.args, indent_num);
                fprintf(dumpFile, ", ");
            }
            fprintf(dumpFile, "(ARG ");
            if (!ast->args_node.expr) {
                perror("\n[error]: argument missing value\n");
                exit(1);
            }
            ast_print_help(ast->args_node.expr, indent_num);
            fprintf(dumpFile, ")");
            break;
            
        default:
            printf("[debug]AST Print: unknown AST node\n");
            break;
    }

}

void indent(int num) {
    fprintf(dumpFile, "\n");
    for (int i = 0; i < num; i++) {
        fprintf(dumpFile, "  ");
    }
}

void print_op(int op) {
    switch (op) {
        case ADD:
            fprintf(dumpFile, "ADD ");
            break;
        case SUBTRACT:
            fprintf(dumpFile, "SUBTRACT ");
            break;
        case MULTIPLY:
            fprintf(dumpFile, "MULTIPLY ");
            break;
        case DIVIDE:
            fprintf(dumpFile, "DIVIDE ");
            break;
        case ASSIGNMENT:
            //FIXME: error-prone
            fprintf(dumpFile, "ASSIGNMENT ");
            break;
        case POWER:
            fprintf(dumpFile, "POWER ");
            break;
        case AND:
            fprintf(dumpFile, "AND ");
            break;
        case OR:
            fprintf(dumpFile, "OR ");
            break;
        case EQUAL:
            fprintf(dumpFile, "EQUAL ");
            break;
        case NOTEQUAL:
            fprintf(dumpFile, "NOTEQUAL ");
            break;
        case GT:
            fprintf(dumpFile, "GT ");
            break;
        case GE:
            fprintf(dumpFile, "GE ");
            break;
        case LT:
            fprintf(dumpFile, "LT ");
            break;
        case LE:
            fprintf(dumpFile, "LE ");
            break;
        default: break;
    }
}

void print_op_unary (int op) {
    switch (op) {
        case SUBTRACT:
            fprintf(dumpFile, "NEG ");
            break;
        case NOTEQUAL:
            fprintf(dumpFile, "NOT ");
            break;
        default: break;
    }
}

void debugP(int verbose_flag, const char* str, ...) {
    if (!VB_GLOB || !verbose_flag) return;
    va_list args;
    va_start(args, str);
    vprintf(str, args);
    va_end(args);
}


void print_type_id(type_id type_name, int is_vec, int vec_index, int is_const) {
    if (is_const) {
        fprintf(dumpFile, "const ");
    }
    switch (type_name) {
        case INT:
            if (is_vec) {
                fprintf(dumpFile, "ivec%d ", vec_index);
            } else {
                fprintf(dumpFile, "int ");
            }
            break;
        case FLOAT:
            if (is_vec) {
                fprintf(dumpFile, "vec%d ", vec_index);
            } else {
                fprintf(dumpFile, "float ");
            }
            break;
        case BOOL:
            if (is_vec) {
                fprintf(dumpFile, "bvec%d ", vec_index);
            } else {
                fprintf(dumpFile, "bool ");
            }
            break;
//        case VEC: 
//            printf("[error]vec case unimplemented\n");
//            exit(1);
//            break;
        default: 
            fprintf(dumpFile, "ANY ");
            break;
    }
}

char* get_type_id_name(type_id type_name) {
    char* result_buf = (char*)malloc(sizeof(char) * 6);
    switch (type_name) {
        case INT:
            strcpy(result_buf, "int");
            break;
        case FLOAT:
            strcpy(result_buf, "float");
            break;
        case BOOL:
            strcpy(result_buf, "bool");
            break;
        case ANY:
            strcpy(result_buf, "ANY");
            break;
        default: 
            strcpy(result_buf, "[UNEXPECTED]");
            break;
    }
    return result_buf;
}

func_id func_name_to_id(char* func_name) {
    if (!strcmp(func_name, "lit")) return LIT;
    if (!strcmp(func_name, "rsq")) return RSQ;
    if (!strcmp(func_name, "dp3")) return DP3;
    return UNKNOWN_FUNC;
}
