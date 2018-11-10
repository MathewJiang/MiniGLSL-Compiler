
#ifndef AST_H_
#define AST_H_ 1

#include <stdarg.h>
#include <stdbool.h>

// Dummy node just so everything compiles, create your own node/nodes
//
// The code provided below is an example ONLY. You can use/modify it,
// but do not assume that it is correct or complete.
//
// There are many ways of making AST nodes. The approach below is an example
// of a descriminated union.
//

// Forward declarations
struct node_;
typedef struct node_ node;
extern node *ast;

typedef enum {
  UNKNOWN               = 0,

  SCOPE_NODE            = (1 << 0),
  
  EXPRESSION_NODE       = (1 << 2),
  UNARY_EXPRESSION_NODE = (1 << 2) | (1 << 3),
  BINARY_EXPRESSION_NODE= (1 << 2) | (1 << 4),
  INT_NODE              = (1 << 2) | (1 << 5), 
  FLOAT_NODE            = (1 << 2) | (1 << 6),
  IDENT_NODE            = (1 << 2) | (1 << 7),
  VAR_NODE              = (1 << 2) | (1 << 8),
  FUNCTION_NODE         = (1 << 2) | (1 << 9),
  CONSTRUCTOR_NODE      = (1 << 2) | (1 << 10),
  TYPE_NODE             = (1 << 2) | (1 << 11),
  ARGS_NODE              = (1 << 2) | (1 << 12),


  STATEMENTS_NODE       = (1 << 1),
  IF_STATEMENT_NODE     = (1 << 1) | (1 << 11),
  WHILE_STATEMENT_NODE  = (1 << 1) | (1 << 12),	/*ignored*/
  ASSIGNMENT_STATEMENT_NODE = (1 << 1) | (1 << 13),
  NESTED_SCOPE_NODE     = (1 << 1) | (1 << 14),
  BOOL_NODE             = (1 << 1) | (1 << 15),
  ELSE_STATEMENT_NODE   = (1 << 1) | (1 << 16),

  DECLARATIONS_NODE     = (1 << 14),
  DECLARATION_NODE      = (1 << 15)
} node_kind;

typedef enum {
    INT     = 100,
    FLOAT   = 101,
    BOOL    = 102,
    NUMBER  = 103,
//    BVEC2   = 103,
//    BVEC3   = 104,
//    BVEC4   = 105,
//    IVEC2   = 106,        
//    IVEC3   = 107,        
//    IVEC4   = 108,        
//    VEC2    = 109,
//    VEC3    = 110,
//    VEC4    = 111,
    ANY     = 200
}type_id;

typedef enum {
    RSQ          = 50000000,
    DP3          = 60000000,
    LIT          = 70000000,
    UNKNOWN_FUNC = 80000000
}func_id;

struct node_type {
    type_id type_name = ANY;
    int is_vec = 0;
    int vec_size = -1;
    int is_const = 0;
};
typedef struct node_type node_type;


struct node_ {

  // an example of tagging each node with a type
  node_kind kind;
  node_type inferred_type;
  
  union {
    struct {
      node *declarations;
      node *statements;
    } scope;

    struct {
      node* declarations;
      node* declaration;
    } declarations;

    struct {
      node* statements;
      node* statement;
    } statements;

    struct {
      int is_const;   //note: here (char == bool)
      node* type;
      char* id;
      node* expr;
    } declaration;
    
//    struct {
//      node* expression;
//    }expression_node;

    struct {
      node* statement;
      node* if_condition;
      node* else_statement;
    } if_statement_node;
    
    struct {
        node* else_statement;
    } else_statement_node;
    
    struct {
      node* scope;
    } nested_scope;


    struct {
      node* var;
      node* expr;
    } assign_statement;

    struct {
      int op;
      node *left;
      node *right;
    } binary_expr;
    
    struct {
      int op;
      node *right;
    } unary_expr;

    struct {
	  char* id;
	  int is_vec;
	  int vec_idx;
    } var_node;

	int int_val;
	float float_val;
	bool bool_val;

    // TODO: add more type of nodes

    struct {
	node* type;
	node* args;
    } constructor;
    
    struct {
	char* func_id;
	node* args;
    } function;

    struct {
	node* args;
	node* expr;
    } args_node;

    struct {
        char* type_name;
        bool is_vec;
        int vec_size;
    }type_node;
    
    
  };
};

node *ast_allocate(node_kind type, ...);
void ast_free(node *ast);
void ast_print(node * ast);
void ast_print_help(node *ast, int indent_num);
void indent(int num);
void print_op(int op);
void print_op_unary (int op);
void print_type_id(type_id type_name, int is_vec, int vec_index, int is_const);
char* get_type_id_name(type_id type_name);
func_id func_name_to_id(char* func_name);
int semantic_check(node * ast);

#endif /* AST_H_ */
