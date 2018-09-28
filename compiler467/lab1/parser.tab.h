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
    ASSIGNMENT = 263,
    ADD = 264,
    SUBTRACT = 265,
    MULTIPLY = 266,
    DIVIDE = 267,
    POWER = 268,
    NOT = 269,
    EQUAL = 270,
    NOTEQUAL = 271,
    AND = 272,
    OR = 273,
    GT = 274,
    GE = 275,
    LT = 276,
    LE = 277,
    LPARENTHESES = 278,
    RPARENTHESES = 279,
    LBRACE = 280,
    RBRACE = 281,
    LBRACKET = 282,
    RBRACKET = 283,
    DOT = 284,
    SEMICOLON = 285,
    COMMA = 286,
    ID = 287,
    CONST = 288,
    SIGN = 289,
    VOID_T = 290,
    INT_T = 291,
    INT_C = 292,
    FLOAT_T = 293,
    FLOAT_C = 294,
    BOOL_T = 295,
    BOOL_C = 296
  };
#endif
/* Tokens.  */
#define myToken1 258
#define myToken2 259
#define WS 260
#define IF 261
#define ELSE 262
#define ASSIGNMENT 263
#define ADD 264
#define SUBTRACT 265
#define MULTIPLY 266
#define DIVIDE 267
#define POWER 268
#define NOT 269
#define EQUAL 270
#define NOTEQUAL 271
#define AND 272
#define OR 273
#define GT 274
#define GE 275
#define LT 276
#define LE 277
#define LPARENTHESES 278
#define RPARENTHESES 279
#define LBRACE 280
#define RBRACE 281
#define LBRACKET 282
#define RBRACKET 283
#define DOT 284
#define SEMICOLON 285
#define COMMA 286
#define ID 287
#define CONST 288
#define SIGN 289
#define VOID_T 290
#define INT_T 291
#define INT_C 292
#define FLOAT_T 293
#define FLOAT_C 294
#define BOOL_T 295
#define BOOL_C 296

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 55 "parser.y" /* yacc.c:1909  */

  int as_int;
  float as_float;
  
  char* as_id;
  char* as_keyword;

#line 144 "y.tab.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
