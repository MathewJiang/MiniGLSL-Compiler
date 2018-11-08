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
  char* temp_str = NULL;

  va_start(args, kind); 

  switch(kind) {

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
     
     temp_str = (char *)malloc(32 * sizeof(char));
     strcpy(temp_str, va_arg(args, char *));
     
     ast->declaration.id = temp_str;
     ast->declaration.expr = va_arg(args, node *);
     debugP(VB_TRACE, "\tDECLARATION_NODE\n");
     break;
 
 case ASSIGNMENT_NODE:
     ast->assign_statement.var = va_arg(args, node *);
     ast->assign_statement.expr = va_arg(args, node *);
     break;
 
 case TYPE_NODE:
      temp_str = (char *) malloc(sizeof(char) * 6);
      strcpy(temp_str, va_arg(args, char*));
      ast->type_node.type_name = temp_str;
      ast->type_node.is_vec = va_arg(args, int);
      ast->type_node.vec_size = -1;
      if (ast->type_node.is_vec) {
          switch(ast->type_node.type_name[0]) {
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
    switch(kind){
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
            indent(indent_num);
            printf("(BINARY ");
            
            //TODO: determine the resulting type afterwards 
            printf("temp_type ");
            
            if (ast->binary_expr.op == ADD) {
                //TODO: how to determine the type of return operation
                printf("ADD ");
            }
            
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


void indent(int num) {
    printf("\n");
    for (int i = 0; i < num; i++) {
        printf("\t");
    }
}

void debugP(int verbose_flag, const char* str, ...) {
    if (!VB_GLOB || !verbose_flag) return;
    va_list args;
    va_start(args, str);
    vprintf(str, args);
    va_end(args);
}
