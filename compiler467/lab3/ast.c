#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "common.h"
#include "parser.tab.h"

#define DEBUG_PRINT_TREE 0
#define VB_GLOB 1
#define VB_TRACE 1

void debugP(int verbose_flag, const char* str, ...);

node *ast = NULL;

node *ast_allocate(node_kind kind, ...) {
  va_list args;

  // make the node
  node *ast = (node *) malloc(sizeof(node));
  memset(ast, 0, sizeof *ast);
  ast->kind = kind;

  va_start(args, kind); 

  switch(kind) {
  
  // ...

 case SCOPE_NODE:
    ast->scope.declarations = va_arg(args, node *);
    ast->scope.statements = va_arg(args, node *);
    debugP(VB_TRACE, "\tSCOPE_NODE\n");
    break;
 
 case DECLARATIONS_NODE:
     ast->declarations.declarations = va_arg(args, node *);
     ast->declarations.declaration = va_arg(args, node *);
     debugP(VB_TRACE, "\tDECLARATIONS_NODE\n");
     break;
     
 case DECLARATION_NODE:
     ast->declaration.is_const = va_arg(args, int);
     ast->declaration.type = va_arg(args, node *);
     ast->declaration.id = va_arg(args, char *);
     ast->declaration.expr = va_arg(args, node *);
     debugP(VB_TRACE, "\tDECLARATION_NODE\n");
     break;
 
 case ASSIGNMENT_NODE:
     ast->assign_statement.var = va_arg(args, node *);
     ast->assign_statement.expr = va_arg(args, node *);
     break;
 
 case TYPE_NODE:
      ast->type_node.type = va_arg(args, int);    
      break;
 
 case VAR_NODE:
     //TODO: do we need a type for the variable node?
     ast->var_node.id = va_arg(args, char*);
     ast->var_node.is_vec = va_arg(args, int);
     ast->var_node.vec_idx = (ast->var_node.is_vec) ? (va_arg(args, int)) : (-1);
     debugP(VB_TRACE, "Allocating var node with id=%s, is_vec=%d, vec_idx=%d\n",
     ast->var_node.id, ast->var_node.is_vec, ast->var_node.vec_idx);
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
    ast->bool_val = va_arg(args, int);
    debugP(VB_TRACE, "Allocating bool node with val=%d\n", ast->bool_val);
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
    ast->function.func_id = va_arg(args, char*);
    ast->function.args = va_arg(args, node*);
    debugP(VB_TRACE, "[ast_allocate]FUNCTION_NODE: func_id: %s\n", ast->function.func_id);
    break;
    
  case CONSTRUCTOR_NODE:
    ast->constructor.type = va_arg(args, char*);
    ast->constructor.args = va_arg(args, node*);
    debugP(VB_TRACE, "[ast_allocate]CONSTRUCTOR_NODE: type: %s\n", ast->constructor.type);
    break;

  
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
    if (ast == NULL) {
        perror("ast is NULL\n");
        exit(1);
    }
    //printf("\n[debug]Enter ast_print\n");
    
    node_kind kind = ast->kind;
    switch(kind){
        case SCOPE_NODE:
            printf("(");
            printf("SCOPE ");
            if (ast->scope.declarations != NULL) {
                ast_print(ast->scope.declarations);
            }
            if (ast->scope.statements != NULL) {
                ast_print(ast->scope.statements);
            }
            printf(")\n");
            break;
            
        case DECLARATIONS_NODE:
            printf("(");
            printf("DECLARATIONS ");
            if (ast->declarations.declarations != NULL) {
                ast_print(ast->declarations.declarations);
            }
            if (ast->declarations.declaration != NULL) {
                ast_print(ast->declarations.declaration);
            }
            printf(")");
            break;
            
        case DECLARATION_NODE:
            printf("(");
            printf("DECLARATION ");
            
            if (ast->declaration.id != NULL) {
                //FIXME: error-prone
                printf("%s ", ast->declaration.id);
            }
            
            if (ast->declaration.is_const == (char)1) {
                printf("CONST ");
            }
            if (ast->declaration.type != NULL) {
                ast_print(ast->declaration.type);
            }
            
            if (ast->declaration.expr != NULL) {
                ast_print(ast->declaration.expr);
            }
            printf(")");
            break;
        
        case TYPE_NODE:
//            printf("(");
//            printf("TYPE ");
            if (ast->type_node.type == 1) {
                printf("INT ");
            } else if (ast->type_node.type == 2) {
                printf("FLOAT ");
            } else if (ast->type_node.type == 3) {
                printf("BOOL ");
            } else if (ast->type_node.type == 4) {
                printf("(INDEX ");
                printf("\nvec1,2,3,4 not implemented\n");
                exit(1);
            } else {
                perror("[error]TYPE_NODE undefined\n");
                exit(1);
            }
//            printf(") ");
            break;
            
        case VAR_NODE:
            printf("%s ", ast->var_node.id);
            if (ast->var_node.is_vec) {
                //printf("%s ", ast->var_node.id);
                printf("(is_vec, vec_index: %d) ", ast->var_node.vec_idx);
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
//        case UNARY_EXPRESSION_NODE: 
//            break;
        case BINARY_EXPRESSION_NODE: 
            printf("(BINARY ");
            
            //TODO: determine the resulting type afterwards 
            printf("temp_type ");
            
            if (ast->binary_expr.op == ADD) {
                //TODO: how to determine the type of return operation
                printf("ADD ");
            }
            
            if (ast->binary_expr.left != NULL) {
                ast_print(ast->binary_expr.left);
            } else {
                perror("\n[error]lack of left operand for BINARY\n");
                exit(1);
            }
            
            if (ast->binary_expr.right != NULL) {
                ast_print(ast->binary_expr.right);
            } else {
                perror("\n[error]lack of right operand for BINARY\n");
                exit(1);
            }
            printf(")");            
            break;
            
//        case INT_NODE: 
//            break;
//        case FLOAT_NODE: 
//            break;
//        case IDENT_NODE: 
//            break;
//        case VAR_NODE: 
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


void debugP(int verbose_flag, const char* str, ...) {
    if (!VB_GLOB || !verbose_flag) return;
    va_list args;
    va_start(args, str);
    vprintf(str, args);
    va_end(args);
}
