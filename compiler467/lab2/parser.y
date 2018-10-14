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
//#include "ast.h"
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
  
  char* as_id;
  char* as_keyword;
}
// TODO:Replace myToken with your tokens, you can use these tokens in flex
%token		myToken1 myToken2
%token		WS
%token		IF ELSE
%token          WHILE

%token          ASSIGNMENT
%token          ADD
%token		SUBTRACT
%token		MULTIPLY
%token		DIVIDE
%token		POWER

%token		NOT
%token		EQUAL
%token		NOTEQUAL
%token		AND
%token		OR
%token		GT
%token		GE
%token		LT
%token		LE

%token		LPARENTHESES
%token		RPARENTHESES
%token		LBRACE
%token		RBRACE
%token		LBRACKET
%token		RBRACKET

%token		DOT
%token		SEMICOLON
%token		COMMA

%token		ID

%token		CONST
%token          SIGN
%token          VEC_T
%token          FUNC_ID
%token          VOID_T
%token		INT_T
%token<as_int>	INT_C
%token		FLOAT_T
%token		FLOAT_C
%token		BOOL_T
%token		BOOL_C

%left		OR
%left		AND
%nonassoc	EQUAL NOTEQUAL GT GE LT LE
%left 		SUBTRACT ADD
%left		MULTIPLY DIVIDE
%right		POWER
%left		NOT NEG
%left		LBRACKET RBRACKET LPARENTHESES RPARENTHESES

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
  :   scope     			{ yTRACE("Program -> Scope\n"); }
  |   /*epislon*/			{ yTRACE("Program -> epislon\n"); }
  ;

scope
  :   LBRACE declarations statements RBRACE 	{ yTRACE("scope -> declarations statements\n"); }
  ;

declarations
  :   declarations declaration		{ yTRACE("declarations -> declarations declarations\n"); }
  |   /*epislon*/			{ yTRACE("declarations -> epislon\n"); }
  ;

statements
  :   statements statement		{ yTRACE("statements -> statements statement\n"); }
  |   /*epislon*/			{ yTRACE("statements -> epislon\n"); }
  ;

declaration
  :   type ID SEMICOLON					{ yTRACE("declaration -> type ID ;\n"); }
  |   type ID ASSIGNMENT expression SEMICOLON		{ yTRACE("declaration -> type ID = expression ;\n"); }
  |   CONST type ID ASSIGNMENT expression SEMICOLON	{ yTRACE("declaration -> const type ID = expression ;\n"); }
  |   /*epislon*/					{ yTRACE("declarations -> epislon\n"); }
  ;

statement
  :   variable ASSIGNMENT expression SEMICOLON				{ yTRACE("statment -> variable = expression ;\n"); }
  |   IF LPARENTHESES expression RPARENTHESES statement else_statement	{ yTRACE("statement -> if (expression) statement else_statement\n"); }
  |   WHILE LPARENTHESES expression RPARENTHESES statement		{ yTRACE("statement -> while (expression) statement\n"); }
  |   scope								{ yTRACE("statement -> scope\n"); }
  |   SEMICOLON								{ yTRACE("statement -> ;\n"); }
  ;

else_statement
  :   ELSE statement			{ yTRACE("else_statement -> else statement\n"); }
  |   /*epislon*/			{ yTRACE("else_statement -> epislon\n"); }
  ;

type
  :   INT_T | FLOAT_T | BOOL_T | VEC_T	{ yTRACE("type -> int | float | bool | vec\n"); }
  ;

expression
  :   constructor				{ yTRACE("expression -> constructor\n"); }
  |   function					{ yTRACE("expression -> function\n"); }
  |   INT_C					{ yTRACE("expression -> int_c\n"); /*printf("[debug]int is: %d\n", $1);*/ }
  |   FLOAT_C					{ yTRACE("expression -> float_c\n"); }
  |   BOOL_C					{ yTRACE("expression -> bool_c\n"); }
  |   variable					{ yTRACE("expression -> variable\n"); }
  |   unary_op expression %prec NEG		{ yTRACE("expression -> unary_op expression\n"); }
  |   expression binary_op expression		{ yTRACE("expression -> expression binary_op expression\n"); }
  |   LPARENTHESES expression RPARENTHESES	{ yTRACE("(expression)\n"); }
  ;

variable
  :   ID				{ yTRACE("variable -> ID\n"); }
  |   ID LBRACKET INT_C RBRACKET	{ yTRACE("variable -> ID [ int_c ]\n"); }
  ;

unary_op
  :   NOT | SUBTRACT			{ yTRACE("unary_op -> ! | -\n"); }
  ;

binary_op
  :   AND | OR | EQUAL | NOTEQUAL | GT | GE		{ yTRACE("binary_op -> and | or | eq | noteq | gt | ge\n"); }
  |   LT | LE | ADD | SUBTRACT | MULTIPLY | DIVIDE	{ yTRACE("binary_op -> lt | le | add | subtract | multiply | divide\n"); }
  |   ASSIGNMENT | POWER				{ yTRACE("binary_op -> assignment | power\n"); } 
  ;

constructor
  :   type LPARENTHESES arguments RPARENTHESES		{ yTRACE("constructor -> type ( arguments)\n"); }
  ;

function
  :   FUNC_ID LPARENTHESES arguments_opt RPARENTHESES	{ yTRACE("function -> func_id ( arguments_opt )\n"); }
  ;

arguments_opt
  :   arguments				{ yTRACE("arguments_opt -> arguments\n"); }
  |   /*epislon*/			{ yTRACE("arguments_opt -> epislon\n"); }
  ;

arguments
  :   arguments COMMA expression	{ yTRACE("arguments -> arguments , expression\n"); }
  |   expression			{ yTRACE("arguments -> expression\n"); }
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

