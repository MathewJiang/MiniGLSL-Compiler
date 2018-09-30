%{
/**********************************************************************
 *  Interface to the scanner module for CSC467 course project
 **********************************************************************/


#include "common.h"
#include "parser.tab.h"
#include "string.h"


#define DEBUG_MODE 1
#define MAX_FLOAT 1E37
#define MIN_FLOAT -1E37

#define YY_USER_INIT { yyin = inputFile; }
#define	yyinput      input
#define yTRACE(x)    { if (traceScanner) fprintf(traceFile, "TOKEN %3d : %s\n", x, yytext); }
#define yERROR(x)    { fprintf(errorFile, "\nLEXICAL ERROR, LINE %d: %s\n", yyline, x); errorOccurred = TRUE; yyterminate(); }
#define yDEBUG(x)    { if (DEBUG_MODE) fprintf("%s", x); }

int CheckInt(void);
void bufCpy(char**, char*, int);
int yyline = 1;

%}
%option noyywrap

WS                          [ \t\v\r\f]

SIGN                        [+|-]
DIGIT                       [0-9]
ID                          [a-zA-Z][a-zA-Z0-9_]*
TF                          (true|false)
VEC                         (b|i)?(vec)[234]
FUNC_NAME                   (lit|dp3|rsq)

%%

{WS}                        { /*FIXME: maybe waste of time if return*/ /*return WS;*/ }
"\n"			    { ++yyline; }

if                          { yTRACE(IF); return IF; }
else                        { yTRACE(ELSE); return ELSE; }
while                       { yTRACE(WHILE); return WHILE; }

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

const                       { /*FIXME: may want to store in keywords*/yTRACE(CONST); return CONST; }
void                        { yTRACE(VOID_T); return VOID_T; }
bool                        { yTRACE(BOOL_T); return BOOL_T; }
int                         { yTRACE(INT_T); return INT_T; }
float                       { yTRACE(FLOAT_T); return FLOAT_T; }


{TF}                        { yTRACE(BOOL_C); return BOOL_C; }
{DIGIT}+             {
                                /*FIXME: consider situation like 3+5*/
                                /*integer number*/
				int temp = atoi(yytext);
				if (temp > MAX_INTEGER || yyleng > 6)
					yERROR("Integer out of range\n");
                                
				int i = 0;
				char c = yytext[0];
				while (c == '+' || c == '-') {
					i++;
					c = yytext[i];
				}
				if (c == '0') {
					yERROR("Octal representation not supported\n");
				}

				yTRACE(INT_C);
				yylval.as_int = temp;
                            #ifdef DEBUG_MODE
                                printf("[debug]yylval.as_int = %d\n", yylval.as_int);
                            #endif
                                return INT_C;
                            }



{VEC}                       {
				/*FIXME: Need to separate between vec2, vec3, etc*/
				/*FIXME: also may store it in the keyword field*/
                                yTRACE(VEC_T);
                                if (yylval.as_keyword != NULL) {
                                #ifdef DEBUG_MODE
                                    printf("[debug]yylval is not NULL\n");
                                #endif
                                    //free(yylval.as_keyword);
                                }

                                bufCpy(&yylval.as_keyword, yytext, yyleng);
                            #ifdef DEBUG_MODE
                                printf("[debug]vec yylval.as_keyword: %s\n", yylval.as_keyword);
                            #endif
                                return VEC_T;
                            }

{FUNC_NAME}                 {
				/*FIXME: do we need to check for func name length?*/
				/*FIXME: !!!should this be a keyword??*/
                                yTRACE(FUNC_ID);
                                if (yylval.as_id != NULL) {
                                #ifdef DEBUG_MODE
                                    printf("[debug]func yylval is not NULL\n");
                                #endif
                                    //free(yylval.as_id);
                                }

                                bufCpy(&yylval.as_id, yytext, yyleng);
                            #ifdef DEBUG_MODE
                                printf("[debug]func yylval.as_id: %s\n", yylval.as_id);
                            #endif
                                return FUNC_ID;

                            }


{ID}                        {
                                /*identifier*/
                                yTRACE(ID);

                                if (yylval.as_id != NULL) {
                                #ifdef DEBUG_MODE
                                    printf("[debug]vec yylval is not NULL\n");
                                #endif
                                    //free(yylval.as_id);
                                }
				if (yyleng > MAX_IDENTIFIER) {
				    yERROR("ID length exceeds 32 bytes\n");
				}
				
                                //FIXME: using normal array may cause stack overflow
                                //yylval.as_id = (char *)(malloc(sizeof(char)*(strlen(yytext)+1)));
                                //strcpy(yylval.as_id, yytext);
                                printf("????????????yytext: %s", yytext);
                                bufCpy(&yylval.as_id, yytext, yyleng);
                            #ifdef DEBUG_MODE
                                printf("[debug]yylval.as_id = %s\n", yylval.as_id);
                            #endif
                                return ID;
                            }

{DIGIT}*("."{DIGIT}*)?([Ee]{SIGN}?{DIGIT}*("."{DIGIT}*)?)?  {
                                /*float point number*/
                                if (strstr(yytext, ".e") == yytext ||
                                        strstr(yytext, ".E") == yytext) {
                                    yERROR("Invalid float value.\n");
                                }
                                if (strrchr(yytext, 'e') == (yytext + yyleng - 1) ||
                                        strrchr(yytext, 'E') == (yytext + yyleng - 1)) {
                                    yERROR("Invalid float value.\n")
                                }
                                if ((strrchr(yytext, '.') && strrchr(yytext, 'e') && strrchr(yytext, '.') > strrchr(yytext, 'e')) ||
                                         (strrchr(yytext, '.') && strrchr(yytext, 'E') && strrchr(yytext, '.') > strrchr(yytext, 'E'))) {
                                    yERROR("Unsupported float value: exponent must be an integer.\n");
                                }
				float temp = atof(yytext);
				if (temp > MAX_FLOAT) {
					yERROR("Float out of range\n");
				}

                                yTRACE(FLOAT_C);
                                yylval.as_float = temp;
                            #ifdef DEBUG_MODE
                                printf("[debug]yylval.as_float = %f\n", yylval.as_float);
                            #endif
                                return FLOAT_C;
                            }

{DIGIT}+{ID}                {
                                /*FIXME: hacky checking...*/
                                yERROR("Identifiers should not start with numbers\n");
                            }


"/*".*"*/"                  { /*FIXME: does longest matching rule works well*/ printf("Comment detected\n");}

"/*".*			    { yERROR("Open comment without closing\n"); }

.                           { yERROR("Unknown token\n"); }

%%

void bufCpy(char** dest, char* src, int len) {
    char buf[len + 1];
    strcpy(buf, src);
    *dest = buf;
}

