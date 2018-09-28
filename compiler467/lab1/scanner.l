%{
/**********************************************************************
 *  Interface to the scanner module for CSC467 course project
 **********************************************************************/


#include "common.h"
#include "parser.tab.h"
#include "string.h"


#define DEBUG_MODE = 1
#define YY_USER_INIT { yyin = inputFile; }
#define	yyinput      input
#define yTRACE(x)    { if (traceScanner) fprintf(traceFile, "TOKEN %3d : %s\n", x, yytext); }
#define yERROR(x)    { fprintf(errorFile, "\nLEXICAL ERROR, LINE %d: %s\n", yyline, x); errorOccurred = TRUE; yyterminate(); }
#define yDEBUG(x)    { if (DEBUG_MODE) fprintf("%s", x); }

int CheckInt(void);
int yyline = 1;

%}
%option noyywrap

WS                          [ \t\n\v\r\f]

SIGN                        [+|-]
DIGIT                       [0-9]
ID                          [a-zA-Z][a-zA-Z0-9]*
TF                          (true|false)
VEC                         (b|i)?(vec)[234]

%%

{WS}                        { /*FIXME: maybe waste of time*/ /*return WS;*/ }

if                          { yTRACE(IF); return IF; }
else                        { yTRACE(ELSE); return ELSE; }

"="                         { yTRACE(ASSIGNMENT); return ASSIGNMENT; }
"+"                         { /*This should be an add operation*/ printf("ADD!: "); yTRACE(ADD); return ADD; }
"-"                         { /*This should be a subtract operation*/ printf("SUBTRACT!: "); yTRACE(SUBTRACT); return SUBTRACT; }
"*"                         { /*This should be a multiply operation*/ printf("MULTIPLY!: "); yTRACE(MULTIPLY); return MULTIPLY; }
"/"                         { /*This should be a divide operation*/ printf("DIVIDE!: "); yTRACE(DIVIDE); return DIVIDE; }
"^"                         { /*This should be a power operation*/ printf("POWER!: "); yTRACE(POWER); return POWER; }

"!"                         { yTRACE(NOT); return NOT; }
"=="                        { yTRACE(EQUAL); return EQUAL; }
"!="                        { yTRACE(NOTEQUAL); return NOTEQUAL; }
"&&"                        { yTRACE(AND); return AND; }
"||"                        { yTRACE(OR); return OR; }
">"                         { yTRACE(GT); return GT; }
">="                        { yTRACE(GE); return GE; }
"<"                         { yTRACE(LT); return LT; }
"<="                        { yTRACE(LE); return LE; }

"("                         { yTRACE(LPARENTHESES); return LPARENTHESES; }
")"                         { yTRACE(RPARENTHESES); return RPARENTHESES; }
"{"                         { yTRACE(LBRACE); return LBRACE; }
"}"                         { yTRACE(RBRACE); return RBRACE; }
"["                         { yTRACE(LBRACKET); return LBRACKET; }
"]"                         { yTRACE(RBRACKET); return RBRACKET; }

"."                         { yTRACE(DOT); return DOT; }
";"                         { yTRACE(SEMICOLON); return SEMICOLON; }
","                         { yTRACE(COMMA); return COMMA; }

const                       { yTRACE(CONST); return CONST; }
void                        { yTRACE(VOID_T); return VOID_T; }
bool                        { yTRACE(BOOL_T); return BOOL_T; }
int                         { yTRACE(INT_T); return INT_T; }
float                       { yTRACE(FLOAT_T); return FLOAT_T; }


{TF}                        { yTRACE(BOOL_C); return BOOL_C; }
{SIGN}?{DIGIT}+             {
                                /*FIXME: consider situation like 3+5*/
                                /*integer number*/
                                yTRACE(INT_C);
                                yylval.as_int = atoi(yytext);
                            #ifdef DEBUG_MODE
                                printf("[debug]yylval.as_int = %d\n", yylval.as_int);
                            #endif
                                return INT_C;
                            }
{SIGN}?{DIGIT}*"."{DIGIT}+  {
                                /*float number*/
                                yTRACE(FLOAT_C);
                                yylval.as_float = atof(yytext);
                            #ifdef DEBUG_MODE
                                printf("[debug]yylval.as_float = %f\n", yylval.as_float);
                            #endif
                                return FLOAT_C;
                            }

{VEC}                       {
                                yTRACE(VEC_T);

                                yylval.as_id = yytext;
                            #ifdef DEBUG_MODE
                                printf("[debug]yytext: %s\n", yytext);
                            #endif
                                return VEC_T;
                            }



{ID}                        {
                                /*identifier*/
                                yTRACE(yytext);
                                //printf("yyleng is %d\n", yyleng);
                                if (yylval.as_id == NULL) {
                                #ifdef DEBUG_MODE
                                    printf("[debug]yylval is not NULL\n");
                                #endif
                                    free(yylval.as_id);
                                }
                                //FIXME: potential mem leak
                                yylval.as_id = (char *)(malloc(sizeof(char)*(strlen(yytext)+1)));
                                strcpy(yylval.as_id, yytext);
                            #ifdef DEBUG_MODE
                                printf("[debug]yylval.as_id = %s\n", yylval.as_id);
                            #endif
                                return ID;
                            }


{DIGIT}+{ID}                {
                                /*FIXME: hacky checking...*/
                                yERROR("Unsupported Identifier\n");
                            }


"/*".*"*/"                  { /*FIXME: comment incorrect; work for /sdfs/ as well*/ yTRACE("Comment detected.\n"); printf("Comment detected\n");}


.                           { yERROR("Unknown token\n"); }

%%

