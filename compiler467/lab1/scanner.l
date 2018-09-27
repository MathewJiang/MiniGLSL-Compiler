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

WS          [ \t\n\v\r\f]

SIGN        [+|-]
DIGIT       [0-9]
ID          [a-zA-Z][a-zA-Z0-9]*
TF          (true|false)

%%

{WS}			{ return WS; }

if			{ yTRACE(IF); return IF; }

else			{ yTRACE(ELSE); return ELSE; }

=                       { yTRACE(ASSIGNMENT); return ASSIGNMENT; }

bool                    { yTRACE(BOOL_T); return BOOL_T; }
{TF}                    { yTRACE(BOOL_C); return BOOL_C; }

int                     { yTRACE(INT_T); return INT_T; }
{SIGN}?{DIGIT}+			{
					/*integer number*/
					yTRACE(INT_C);
					yylval.as_int = atoi(yytext);
#ifdef DEBUG_MODE
					printf("[debug]yylval.as_int = %d\n", yylval.as_int);
#endif
					return INT_C;
				}

float                   { yTRACE(FLOAT_T); return FLOAT_T; }
{SIGN}?{DIGIT}*"."{DIGIT}+	{
					/*float number*/
					yTRACE(yytext);
					yylval.as_float = atof(yytext);
#ifdef DEBUG_MODE
					printf("[debug]yylval.as_float = %f\n", yylval.as_float);
#endif
                                        return FLOAT_C;
				}

{ID}			{
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


{DIGIT}+{ID}		{
				/*FIXME: hacky checking...*/
				yERROR("Unsupported Identifier\n");
			}


[/*].*[*/]			{ yTRACE("Comment detected.\n"); }

"!"				{ yTRACE("NOT op\n"); }
"&&"|"||"|"=="|"!="|">"|">="|"<"|"<="		{ yTRACE("Conditional op\n");}


.		{ yERROR("Unknown token\n"); }

%%
