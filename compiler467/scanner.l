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

int CheckInt(void);
int yyline = 1;

%}
%option noyywrap

WS          [ \t\n]

DIGIT       [0-9]
ID          [a-zA-Z][a-zA-Z0-9]*

%%
{WS}	    { return WS; }

if	    { yTRACE(IF); return IF; }

else        { yTRACE(ELSE); return ELSE; }


{ID}			{
				  yTRACE(yytext);
				  if (yylval.as_id) {
#ifdef DEBUG_MODE
					printf("[debug]yylval is not NULL\n");
#endif
					free(yylval.as_id);
				  }
				  yylval.as_id = (char *)(malloc(sizeof(char)*(strlen(yytext)+1)));
				  strcpy(yylval.as_id, yytext);
#ifdef DEBUG_MODE
				  printf("[debug]yylval.as_id = %s\n", yylval.as_id);
#endif
				  return ID;
			}


{DIGIT}+		{
				yTRACE(yytext);
				yylval.as_int = atoi(yytext);
#ifdef DEBUG_MODE
				printf("[debug]yylval.as_int = %d\n", yylval.as_int);
#endif
				return INT_C;
			}


"/*"        { yERROR("Change this - comment unimplemented."); }

.           { yERROR("Unknown token"); }

%%
