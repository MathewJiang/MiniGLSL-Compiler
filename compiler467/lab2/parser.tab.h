/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    myToken1 = 258,
    myToken2 = 259,
    WS = 260,
    IF = 261,
    ELSE = 262,
    WHILE = 263,
    ASSIGNMENT = 264,
    ADD = 265,
    SUBTRACT = 266,
    MULTIPLY = 267,
    DIVIDE = 268,
    POWER = 269,
    NOT = 270,
    EQUAL = 271,
    NOTEQUAL = 272,
    AND = 273,
    OR = 274,
    GT = 275,
    GE = 276,
    LT = 277,
    LE = 278,
    LPARENTHESES = 279,
    RPARENTHESES = 280,
    LBRACE = 281,
    RBRACE = 282,
    LBRACKET = 283,
    RBRACKET = 284,
    DOT = 285,
    SEMICOLON = 286,
    COMMA = 287,
    ID = 288,
    CONST = 289,
    SIGN = 290,
    VEC_T = 291,
    FUNC_ID = 292,
    VOID_T = 293,
    INT_T = 294,
    INT_C = 295,
    FLOAT_T = 296,
    FLOAT_C = 297,
    BOOL_T = 298,
    BOOL_C = 299
  };
#endif
/* Tokens.  */
#define myToken1 258
#define myToken2 259
#define WS 260
#define IF 261
#define ELSE 262
#define WHILE 263
#define ASSIGNMENT 264
#define ADD 265
#define SUBTRACT 266
#define MULTIPLY 267
#define DIVIDE 268
#define POWER 269
#define NOT 270
#define EQUAL 271
#define NOTEQUAL 272
#define AND 273
#define OR 274
#define GT 275
#define GE 276
#define LT 277
#define LE 278
#define LPARENTHESES 279
#define RPARENTHESES 280
#define LBRACE 281
#define RBRACE 282
#define LBRACKET 283
#define RBRACKET 284
#define DOT 285
#define SEMICOLON 286
#define COMMA 287
#define ID 288
#define CONST 289
#define SIGN 290
#define VEC_T 291
#define FUNC_ID 292
#define VOID_T 293
#define INT_T 294
#define INT_C 295
#define FLOAT_T 296
#define FLOAT_C 297
#define BOOL_T 298
#define BOOL_C 299

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 55 "parser.y" /* yacc.c:1909  */

  int as_int;
  float as_float;
  
  char* as_id;
  char* as_keyword;

#line 150 "y.tab.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
