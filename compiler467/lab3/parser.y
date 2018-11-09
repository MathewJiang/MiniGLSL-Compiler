%{
/***********************************************************************
 *   Interface to the parser module for CSC467 course project.
 * 
 *   Phase 2: Implement context free grammar for source language, and
 *            parse tracing functionality.
 *   Phase 3: Construct the AST for the source language program.
 ***********************************************************************/

/***********************************************************************
 *  C Definitions and external declarations for this module.
 *
 *  Phase 3: Include ast.h if needed, and declarations for other global or
 *           external vars, functions etc. as needed.
 ***********************************************************************/

#include <string.h>
#include "common.h"
#include "ast.h"
//#include "symbol.h"
//#include "semantic.h"
#define YYERROR_VERBOSE
#define yTRACE(x)    { if (traceParser) fprintf(traceFile, "%s\n", x); }

void yyerror(const char* s);    /* what to do in case of error            */
int yylex();              /* procedure for calling lexical analyzer */
extern int yyline;        /* variable holding current line number   */

%}

/***********************************************************************
 *  Yacc/Bison declarations.
 *  Phase 2:
 *    1. Add precedence declarations for operators (after %start declaration)
 *    2. If necessary, add %type declarations for some nonterminals
 *  Phase 3:
 *    1. Add fields to the union below to facilitate the construction of the
 *       AST (the two existing fields allow the lexical analyzer to pass back
 *       semantic info, so they shouldn't be touched).
 *    2. Add <type> modifiers to appropriate %token declarations (using the
 *       fields of the union) so that semantic information can by passed back
 *       by the scanner.
 *    3. Make the %type declarations for the language non-terminals, utilizing
 *       the fields of the union as well.
 ***********************************************************************/

%{
#define YYDEBUG 1
%}


// TODO:Modify me to add more data types
// Can access me from flex useing yyval

%union {
  int as_int;
  float as_float;
  bool as_bool;
  
  char* as_id;
  char* as_keyword;
  struct node_* as_node;
}
// TODO:Replace myToken with your tokens, you can use these tokens in flex
%token			myToken1 myToken2
%token			WS
%token			IF ELSE
%token          	WHILE

%token          	ASSIGNMENT
%token          	ADD
%token			SUBTRACT
%token			MULTIPLY
%token			DIVIDE
%token			POWER

%token			NOT
%token			EQUAL
%token			NOTEQUAL
%token			AND
%token			OR
%token			GT
%token			GE
%token			LT
%token			LE

%token			LPARENTHESES
%token			RPARENTHESES
%token			LBRACE
%token			RBRACE
%token			LBRACKET
%token			RBRACKET

%token			DOT
%token			SEMICOLON
%token			COMMA

%token<as_id>		ID

%token			CONST
%token          	SIGN
%token<as_keyword>      VEC_T
%token<as_keyword>      FUNC_ID
%token          	VOID_T
%token          	INT_T
%token<as_int>		INT_C
%token			FLOAT_T
%token<as_float>	FLOAT_C
%token			BOOL_T
%token<as_bool>		BOOL_C

%left			OR
%left			AND
%nonassoc		EQUAL NOTEQUAL GT GE LT LE
%left 			SUBTRACT ADD
%left			MULTIPLY DIVIDE
%right			POWER
%left			NOT NEG
%left			LBRACKET RBRACKET LPARENTHESES RPARENTHESES

%type<as_node> program
%type<as_node> scope
%type<as_node> declarations
%type<as_node> statements
%type<as_node> declaration
%type<as_node> statement
%type<as_node> else_statement
%type<as_node> type
%type<as_node> expression
%type<as_node> variable
%type<as_node> constructor
%type<as_node> function
%type<as_node> arguments_opt
%type<as_node> arguments

%start    program

%%

/***********************************************************************
 *  Yacc/Bison rules
 *  Phase 2:
 *    1. Replace grammar found here with something reflecting the source
 *       language grammar
 *    2. Implement the trace parser option of the compiler
 *  Phase 3:
 *    1. Add code to rules for construction of AST.
 ***********************************************************************/
program
  :   scope                                     { yTRACE("program -> scope\n"); ast = $1; }
  ;

scope
  :   LBRACE declarations statements RBRACE 	{ yTRACE("scope -> declarations statements\n"); $$ = ast_allocate(SCOPE_NODE, $2, $3); }
  ;

declarations
  :   declarations declaration                  { yTRACE("declarations -> declarations declaration\n"); $$ = ast_allocate(DECLARATIONS_NODE, $1, $2); }
  |   /*epsilon*/                               { yTRACE("declarations -> epsilon\n"); $$ = NULL; }
  ;

statements
  :   statements statement                      { yTRACE("statements -> statements statement\n"); $$ = ast_allocate(STATEMENTS_NODE, $1, $2);}
  |   /*epsilon*/                               { yTRACE("statements -> epsilon\n"); $$ = NULL;}
  ;

declaration
  :   type ID SEMICOLON					{ yTRACE("declaration -> type ID ;\n"); $$ = ast_allocate(DECLARATION_NODE, 0, $1, $2, NULL); }
  |   type ID ASSIGNMENT expression SEMICOLON		{ yTRACE("declaration -> type ID = expression ;\n"); $$ = ast_allocate(DECLARATION_NODE, 0, $1, $2, $4); }
  |   CONST type ID ASSIGNMENT expression SEMICOLON	{ yTRACE("declaration -> const type ID = expression ;\n"); $$ = ast_allocate(DECLARATION_NODE, 1, $2, $3, $5); }
  ;

statement
  :   variable ASSIGNMENT expression SEMICOLON				{ yTRACE("statment -> variable = expression ;\n"); $$ = ast_allocate(ASSIGNMENT_STATEMENT_NODE, $1, $3); }
  |   IF LPARENTHESES expression RPARENTHESES statement else_statement	{ yTRACE("statement -> if (expression) statement else_statement\n"); $$ = ast_allocate(IF_STATEMENT_NODE, $3, $5, $6); }
  |   WHILE LPARENTHESES expression RPARENTHESES statement		{ yTRACE("statement -> while (expression) statement\n"); /* Ignore. */}
  |   scope								{ yTRACE("statement -> scope\n"); $$ = ast_allocate(NESTED_SCOPE_NODE, $1); }
  |   SEMICOLON								{ yTRACE("statement -> ;\n"); $$ = NULL; }
  ;

else_statement
  :   ELSE statement                            { yTRACE("else_statement -> else statement\n"); $$ = ast_allocate(ELSE_STATEMENT_NODE, $2); }
  |   /*epsilon*/                               { yTRACE("else_statement -> epsilon\n"); $$ = NULL; }
  ;

type
  :   INT_T                                     { yTRACE("type -> int\n"); $$ = ast_allocate(TYPE_NODE, "int", 0); }
  |   FLOAT_T                                   { yTRACE("type -> float\n"); $$ = ast_allocate(TYPE_NODE, "float", 0); }
  |   BOOL_T                                    { yTRACE("type -> bool\n"); $$ = ast_allocate(TYPE_NODE, "bool", 0); }
  |   VEC_T                                     { yTRACE("type -> vec\n"); $$ = ast_allocate(TYPE_NODE, $1, 1); }
  ;

expression
  :   constructor				{ yTRACE("expression -> constructor\n"); }
  |   function					{ yTRACE("expression -> function\n"); }
  |   INT_C					{ yTRACE("expression -> int_c\n"); /*printf("[debug]int is: %d\n", $1);*/  $$ = ast_allocate(INT_NODE, $1);}
  |   FLOAT_C					{ yTRACE("expression -> float_c\n"); $$ = ast_allocate(FLOAT_NODE, $1); }
  |   BOOL_C					{ yTRACE("expression -> bool_c\n"); $$ = ast_allocate(BOOL_NODE, $1); }
  |   variable					{ yTRACE("expression -> variable\n"); }
  |   NOT expression %prec NEG			{ yTRACE("expression -> NOT expression\n"); $$ = ast_allocate(UNARY_EXPRESSION_NODE, NOTEQUAL, $2); }
  |   SUBTRACT expression %prec NEG		{ yTRACE("expression -> NEG expression\n"); $$ = ast_allocate(UNARY_EXPRESSION_NODE, SUBTRACT, $2); }
  |   expression AND expression			{ yTRACE("expression -> expression AND expression\n"); $$ = ast_allocate(BINARY_EXPRESSION_NODE, AND, $1, $3); }
  |   expression OR expression			{ yTRACE("expression -> expression OR expression\n"); $$ = ast_allocate(BINARY_EXPRESSION_NODE, OR, $1, $3); }
  |   expression EQUAL expression		{ yTRACE("expression -> expression EQUAL expression\n"); $$ = ast_allocate(BINARY_EXPRESSION_NODE, EQUAL, $1, $3); }
  |   expression NOTEQUAL expression		{ yTRACE("expression -> expression NOTEQUAL expression\n"); $$ = ast_allocate(BINARY_EXPRESSION_NODE, NOTEQUAL, $1, $3); }
  |   expression GT expression			{ yTRACE("expression -> expression GT expression\n"); $$ = ast_allocate(BINARY_EXPRESSION_NODE, GT, $1, $3); }
  |   expression GE expression			{ yTRACE("expression -> expression GE expression\n"); $$ = ast_allocate(BINARY_EXPRESSION_NODE, GE, $1, $3); }
  |   expression LT expression			{ yTRACE("expression -> expression LT expression\n"); $$ = ast_allocate(BINARY_EXPRESSION_NODE, LT, $1, $3); }
  |   expression LE expression			{ yTRACE("expression -> expression LE expression\n"); $$ = ast_allocate(BINARY_EXPRESSION_NODE, LE, $1, $3); }
  |   expression ADD expression			{ yTRACE("expression -> expression ADD expression\n"); $$ = ast_allocate(BINARY_EXPRESSION_NODE, ADD, $1, $3); }
  |   expression SUBTRACT expression		{ yTRACE("expression -> expression SUBTRACT expression\n"); $$ = ast_allocate(BINARY_EXPRESSION_NODE, SUBTRACT, $1, $3); }
  |   expression MULTIPLY expression		{ yTRACE("expression -> expression MULTIPLY expression\n"); $$ = ast_allocate(BINARY_EXPRESSION_NODE, MULTIPLY, $1, $3); }
  |   expression DIVIDE expression		{ yTRACE("expression -> expression DIVIDE expression\n"); $$ = ast_allocate(BINARY_EXPRESSION_NODE, DIVIDE, $1, $3); }
//  |   expression ASSIGNMENT expression		{ yTRACE("expression -> expression ASSIGNMENT expression\n"); 
//                                                                $$ = ast_allocate(BINARY_EXPRESSION_NODE, ASSIGNMENT, $1, $3); }
  |   expression POWER expression		{ yTRACE("expression -> expression POWER expression\n"); $$ = ast_allocate(BINARY_EXPRESSION_NODE, POWER, $1, $3); }
  |   LPARENTHESES expression RPARENTHESES	{ yTRACE("(expression)\n"); }
  ;

variable
  :   ID                                        { yTRACE("variable -> ID\n"); $$ = ast_allocate(VAR_NODE, $1, 0, -1); }
  |   ID LBRACKET INT_C RBRACKET                { yTRACE("variable -> ID [ int_c ]\n"); $$ = ast_allocate(VAR_NODE, $1, 1, $3); }
  ;

constructor
  :   type LPARENTHESES arguments RPARENTHESES		{ yTRACE("constructor -> type ( arguments)\n"); $$ = ast_allocate(CONSTRUCTOR_NODE, $1, $3); }
  ;

function
  :   FUNC_ID LPARENTHESES arguments_opt RPARENTHESES	{ yTRACE("function -> func_id ( arguments_opt )\n");
                                                          $$ = ast_allocate(FUNCTION_NODE, $1, $3); 
                                                        }
  ;

arguments_opt
  :   arguments                                 { yTRACE("arguments_opt -> arguments\n"); }
  |   /*epsilon*/                               { yTRACE("arguments_opt -> epsilon\n"); }
  ;

arguments
  :   arguments COMMA expression                { yTRACE("arguments -> arguments , expression\n"); $$ = ast_allocate(ARGS_NODE, $3, $1); }
  |   expression                                { yTRACE("arguments -> expression\n"); $$ = ast_allocate(ARGS_NODE, $1, NULL); }
  ; 


%%

/***********************************************************************ol
 * Extra C code.
 *
 * The given yyerror function should not be touched. You may add helper
 * functions as necessary in subsequent phases.
 ***********************************************************************/
void yyerror(const char* s) {
  if (errorOccurred)
    return;    /* Error has already been reported by scanner */
  else
    errorOccurred = 1;
        
  fprintf(errorFile, "\nPARSER ERROR, LINE %d",yyline);
  if (strcmp(s, "parse error")) {
    if (strncmp(s, "parse error, ", 13))
      fprintf(errorFile, ": %s\n", s);
    else
      fprintf(errorFile, ": %s\n", s+13);
  } else
    fprintf(errorFile, ": Reading token %s\n", yytname[YYTRANSLATE(yychar)]);
}

