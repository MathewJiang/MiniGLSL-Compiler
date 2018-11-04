#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
	debugP(VB_TRACE, "Allocating scope node\n");
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

  case VAR_NODE:
    ast->var_node.id = va_arg(args, char*);
    ast->var_node.is_vec = va_arg(args, int);
    ast->var_node.vec_idx = (ast->var_node.is_vec) ? (va_arg(args, int)) : (-1);
    debugP(VB_TRACE, "Allocating var node with id=%s, is_vec=%d, vec_idx=%d\n",
            ast->var_node.id, ast->var_node.is_vec, ast->var_node.vec_idx);
    break;

  // ...

  default: break;
  }

  va_end(args);

  return ast;
}

void ast_free(node *ast) {

}

void ast_print(node * ast) {

}

void debugP(int verbose_flag, const char* str, ...) {
    if (!VB_GLOB || !verbose_flag) return;
    va_list args;
    va_start(args, str);
    vprintf(str, args);
    va_end(args);
}
