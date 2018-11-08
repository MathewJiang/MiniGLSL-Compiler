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
    char* temp_str = NULL;

    va_start(args, kind);

    switch (kind) {

        case SCOPE_NODE:
            ast->scope.declarations = va_arg(args, node *);
            ast->scope.statements = va_arg(args, node *);
            debugP(VB_TRACE, "\tSCOPE_NODE\n");
            break;

          case NESTED_SCOPE_NODE:
            ast->nested_scope.scope = va_arg(args, node *);
            debugP(VB_TRACE, "\tNESTED_SCOPE_NODE\n");
            break;
    
        case DECLARATIONS_NODE:
            ast->declarations.declarations = va_arg(args, node *);
            ast->declarations.declaration = va_arg(args, node *);
            debugP(VB_TRACE, "\tDECLARATIONS_NODE\n");
            break;
            
        case STATEMENTS_NODE:
            ast->statements.statements = va_arg(args, node *);
            ast->statements.statement = va_arg(args, node *);
            debugP(VB_TRACE, "\tDECLARATIONS_NODE\n");
            break;

        case DECLARATION_NODE:
            ast->declaration.is_const = va_arg(args, int);
            ast->declaration.type = va_arg(args, node *);

            temp_str = (char *) malloc(32 * sizeof (char));
            strcpy(temp_str, va_arg(args, char *));
            printf("[debug]ast->declaration.id: %s\n", temp_str);
            ast->declaration.id = temp_str;
            ast->declaration.expr = va_arg(args, node *);
            debugP(VB_TRACE, "\tDECLARATION_NODE\n");
            break;

        case ASSIGNMENT_STATEMENT_NODE:
            ast->assign_statement.var = va_arg(args, node *);
            ast->assign_statement.expr = va_arg(args, node *);
            break;
            
        case IF_STATEMENT_NODE:
            ast->if_statement_node.if_statement = va_arg(args, node *);
            ast->if_statement_node.expr = va_arg(args, node *);
            ast->if_statement_node.else_statement = va_arg(args, node *);
            break;

        case TYPE_NODE:
            temp_str = (char *) malloc(6 * sizeof (char));
            strcpy(temp_str, va_arg(args, char*));
            ast->type_node.type_name = temp_str;
            ast->type_node.is_vec = va_arg(args, int);
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
            debugP(VB_TRACE, "Allocating int node with val=%d\n", ast->int_val);
            break;

        case FLOAT_NODE:
            ast->float_val = va_arg(args, double);
            debugP(VB_TRACE, "Allocating float node with val=%.2lf\n", ast->float_val);
            break;

        case BOOL_NODE:
            if (va_arg(args, int) == 0) {
                ast->bool_val = 0;
            } else {
                ast->bool_val = 1;
            }
            debugP(VB_TRACE, "Allocating bool node with val=%d\n", ast->bool_val);
            break;

        case VAR_NODE:
            temp_str = (char *) malloc(32 * sizeof (char));
            strcpy(temp_str, va_arg(args, char*));
            ast->var_node.id = temp_str;
            ast->var_node.is_vec = va_arg(args, int);
            ast->var_node.vec_idx = (ast->var_node.is_vec) ? (va_arg(args, int)) : (-1);
            debugP(VB_TRACE, "Allocating var node with id=%s, is_vec=%d, vec_idx=%d\n",
                    ast->var_node.id, ast->var_node.is_vec, ast->var_node.vec_idx);
            break;

        case UNARY_EXPRESSION_NODE:
            ast->unary_expr.op = va_arg(args, int);
            ast->unary_expr.right = va_arg(args, node *);
            debugP(VB_TRACE, "Allocating unary exp node, op=%d\n", ast->unary_expr.op);
            break;

        case BINARY_EXPRESSION_NODE:
            ast->binary_expr.op = va_arg(args, int);
            ast->binary_expr.left = va_arg(args, node *);
            ast->binary_expr.right = va_arg(args, node *);
            debugP(VB_TRACE, "Allocating binary exp node, op=%d\n", ast->binary_expr.op);
            break;

            // ...
        case FUNCTION_NODE:
            temp_str = (char *) malloc(4 * sizeof (char));
            strcpy(temp_str, va_arg(args, char*));
            ast->function.func_id = temp_str;
            ast->function.args = va_arg(args, node*);
            debugP(VB_TRACE, "[ast_allocate]FUNCTION_NODE: func_id: %s\n", ast->function.func_id);
            break;

            //  case CONSTRUCTOR_NODE:
            //    ast->constructor.type = va_arg(args, char*);
            //    ast->constructor.args = va_arg(args, node*);
            //    debugP(VB_TRACE, "[ast_allocate]CONSTRUCTOR_NODE: type: %s\n", ast->constructor.type);
            //    break;


        default: break;
    }

    va_end(args);

    return ast;
}

void ast_free(node *ast) {
    //TODO: do a recursive free (post-function)
    free(ast);
}

void ast_print(node *ast) {

    //debugP(VB_TRACE, "\tEnter ast_print\n");
    ast_print_help(ast, 0);
}

void ast_print_help(node *ast, int indent_num) {
    if (ast == NULL) {
        perror("ast is NULL\n");
        exit(1);
    }
    //printf("\n[debug]Enter ast_print\n");

    indent_num++;
    node_kind kind = ast->kind;
    switch (kind) {
        case SCOPE_NODE:
            indent(indent_num);
            printf("(");
            printf("SCOPE ");
            if (ast->scope.declarations != NULL) {
                ast_print_help(ast->scope.declarations, indent_num);
            }
            if (ast->scope.statements != NULL) {
                ast_print_help(ast->scope.statements, indent_num);
            }
            printf(")\n");
            break;
            
        case NESTED_SCOPE_NODE:
            indent(indent_num);
            printf("(");
            printf("NESTED_SCOPE ");
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
            printf(")");
            break;
            
        case DECLARATIONS_NODE:
            indent(indent_num);
            printf("(");
            printf("DECLARATIONS ");
            if (ast->declarations.declarations != NULL) {
                ast_print_help(ast->declarations.declarations, indent_num);
            }
            if (ast->declarations.declaration != NULL) {
                ast_print_help(ast->declarations.declaration, indent_num);
            }
            printf(")");
            break;

        case DECLARATION_NODE:
            indent(indent_num);
            printf("(");
            printf("DECLARATION ");

            if (ast->declaration.id != NULL) {
                //FIXME: error-prone
                printf("%s ", ast->declaration.id);
            }

            if (ast->declaration.is_const == 1) {
                printf("CONST ");
            }
            if (ast->declaration.type != NULL) {
                ast_print_help(ast->declaration.type, indent_num);
            }

            if (ast->declaration.expr != NULL) {
                ast_print_help(ast->declaration.expr, indent_num);
            }
            printf(")");
            break;
            
        case STATEMENTS_NODE:
            indent(indent_num);
            printf("(");
            printf("STATEMENTS ");
            if (ast->statements.statements != NULL) {
                ast_print_help(ast->statements.statements, indent_num);
            }
            if (ast->statements.statement != NULL) {
                ast_print_help(ast->statements.statement, indent_num);
            }
            printf(")");
            break;
        
        case TYPE_NODE:
            printf("(");
            printf("TYPE ");
            printf("%s", ast->type_node.type_name);
            //            if (ast->type_node.type == 1) {
            //                printf("INT ");
            //            } else if (ast->type_node.type == 2) {
            //                printf("FLOAT ");
            //            } else if (ast->type_node.type == 3) {
            //                printf("BOOL ");
            //            } else if (ast->type_node.type == 4) {
            //                printf("(INDEX ");
            //                printf("\nvec1,2,3,4 not implemented\n");
            //                exit(1);
            //            } else {
            //                perror("[error]TYPE_NODE undefined\n");
            //                exit(1);
            //            }
            printf(") ");
            break;

        case VAR_NODE:

            if (ast->var_node.is_vec) {
                //print out the index if accessing array variable
                printf("(INDEX ");
                printf("ANY "); //TODO: fix the type after semantic checking
                printf("%s ", ast->var_node.id);
                printf("%d)", ast->var_node.vec_idx);
            } else {
                printf("%s ", ast->var_node.id);
            }
            break;

        case INT_NODE:
            printf("%d ", ast->int_val);
            break;

        case FLOAT_NODE:
            printf("%f ", ast->float_val);
            break;

            //            
            //        case EXPRESSION_NODE: 
            //            break;

        case UNARY_EXPRESSION_NODE:
            printf("(UNARY ");
            //TODO: determine the resulting type afterwards
            printf("ANY ");
            print_op(ast->unary_expr.op);

            if (ast->unary_expr.right != NULL) {
                ast_print_help(ast->unary_expr.right, indent_num);
            } else {
                perror("\n[error]lack of left operand for BINARY\n");
                exit(1);
            }
            printf(")");
            break;

        case BINARY_EXPRESSION_NODE:
            indent(indent_num);
            printf("(BINARY ");
            //TODO: determine the resulting type afterwards 
            printf("ANY ");

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
            printf(")");            
            break;
            
        case IF_STATEMENT_NODE:
            if (!ast->if_statement_node.if_statement) {
                perror("\n[error]if statement node lack condition\n");
                exit(1);
            }
            if (!ast->if_statement_node.expr) {
                perror("\n[error]if statement node lack then expression\n");
                exit(1);
            }
            indent(indent_num);
            printf("(IF ");
            ast_print_help(ast->if_statement_node.if_statement, indent_num);
            ast_print_help(ast->if_statement_node.expr, indent_num);
            if (ast->if_statement_node.else_statement)
                ast_print_help(ast->if_statement_node.else_statement, indent_num);
            printf(")");
            
            break;
            
        case ELSE_STATEMENT_NODE:
            if (!ast->else_statement_node.else_statement) {
                perror("\n[error]else statement node lack statement\n");
                exit(1);
            }
            printf("ELSE ");
            ast_print_help(ast->else_statement_node.else_statement, indent_num);
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
            printf("(ASSIGN ");
            
            //TODO: determine the resulting type afterwards 
            printf("ANY ");
            ast_print_help(ast->assign_statement.var, indent_num);
            ast_print_help(ast->assign_statement.expr, indent_num);
            printf(")");
            break;


        case BOOL_NODE:
            //TODO: determine types for boolean variable
            //so that "true" or "false" will be printed
            if (ast->bool_val) {
                printf("true ");
            } else {
                printf("false ");
            }
            break;

        case FUNCTION_NODE:
            indent(indent_num);
            printf("(CALL ");
            printf("%s ", ast->function.func_id);
            if (ast->function.args != NULL) {
                ast_print_help(ast->function.args, indent_num);
            }
            printf(")");
            break;

            //        case INT_NODE: 
            //            break;
            //        case FLOAT_NODE: 
            //            break;
            //        case IDENT_NODE: 
            //            break;
            //        case FUNCTION_NODE: 
            //            break;
            //        case CONSTRUCTOR_NODE: 
            //            break;
            //        case STATEMENT_NODE: 
            //            break;
            //        case IF_STATEMENT_NODE: 
            //            break;
            //        case ASSIGNMENT_NODE:     
            //            break;    
            //        case NESTED_SCOPE_NODE: 
            //            break;
            //        case BOOL_NODE:
            //            break;
            //        case DECLARATION_NODE: 
            //            break;
        default:
            printf("[debug]default\n");
            break;
    }

}

void indent(int num) {
    printf("\n");
    for (int i = 0; i < num; i++) {
        printf("\t");
    }
}

void print_op(int op) {
    switch (op) {
        case ADD:
            printf("ADD ");
            break;
        case SUBTRACT:
            printf("SUBTRACT ");
            break;
        case MULTIPLY:
            printf("MULTIPLY ");
            break;
        case DIVIDE:
            printf("DIVIDE ");
            break;
        case ASSIGNMENT:
            //FIXME: error-prone
            printf("ASSIGNMENT ");
            break;
        case POWER:
            printf("POWER ");
            break;

        case AND:
            printf("AND ");
            break;
        case OR:
            printf("OR ");
            break;
        case EQUAL:
            printf("EQUAL ");
            break;
        case NOTEQUAL:
            printf("NOTEQUAL ");
            break;
        case GT:
            printf("GT ");
            break;
        case GE:
            printf("GE ");
            break;
        case LT:
            printf("LT ");
            break;
        case LE:
            printf("LE ");
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
