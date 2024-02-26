%option yylineno

%{
    #include <stdlib.h>
    #include <ctype.h>
    #include "tree.h"
    #define YYSTYPE struct Node*
    #include "syntax.tab.h"
    int yycolumn = 1;
    #define YY_USER_ACTION \
        yylloc.first_line = yylloc.last_line = yylineno; \
        yylloc.first_column = yycolumn; \
        yylloc.last_column = yycolumn + yyleng - 1; \
        yycolumn += yyleng;
    extern int lexical_errors;
%}

digit  [0-9]
letter_  [a-zA-Z_]
SPACE    [ \t\r\n]+

SEMI  ";"
COMMA  ","
ASSIGNOP  "="
RELOP  ">"|"<"|">="|"<="|"=="|"!="
PLUS  "+"
MINUS  "-"
STAR  "*"
DIV  "/"
AND  "&&"
OR  "||"
DOT  "."
NOT  "!"
LP  "("
RP  ")"
LB  "["
RB  "]"
LC  "{"
RC  "}"
TYPE  int|float
STRUCT  struct
RETURN  return
IF  if
ELSE  else
WHILE  while

DEC  0|[1-9]([0-9])*
BAD_ID {digit}+{letter_}+
OCT  0[0-7]+
HEX  0[xX][0-9a-fA-F]+


NOR_FLOAT  {digit}+\.{digit}+
EXP_FLOAT  (({digit}+\.{digit}*)|({digit}*\.{digit}+))[eE][-+]?{digit}+
FLOAT {NOR_FLOAT}|{EXP_FLOAT}
ID  {letter_}({letter_}|{digit})*

%%
{SPACE}     { }
{SEMI}      { yylval = tree_building("SEMI", LEX_OTHER, yylineno, 0, NULL);
              return SEMI; }
{COMMA}     { yylval = tree_building("COMMA", LEX_OTHER, yylineno, 0, NULL);
              return COMMA; }
{ASSIGNOP}  { yylval = tree_building("ASSIGNOP", LEX_OTHER, yylineno, 0, NULL);
              return ASSIGNOP; }
{PLUS}      { yylval = tree_building("PLUS", LEX_OTHER, yylineno, 0, NULL);
              return PLUS; }
{MINUS}     { yylval = tree_building("MINUS", LEX_OTHER, yylineno, 0, NULL);
              return MINUS; }
{STAR}      { yylval = tree_building("STAR", LEX_OTHER, yylineno, 0, NULL);
              return STAR; }
{DIV}       { yylval = tree_building("DIV", LEX_OTHER, yylineno, 0, NULL);
              return DIV; }
{AND}       { yylval = tree_building("AND", LEX_OTHER, yylineno, 0, NULL);
              return AND; }
{OR}        { yylval = tree_building("OR", LEX_OTHER, yylineno, 0, NULL);
              return OR; }
{DOT}       { yylval = tree_building("DOT", LEX_OTHER, yylineno, 0, NULL);
              return DOT; }
{NOT}       { yylval = tree_building("NOT", LEX_OTHER, yylineno, 0, NULL);
              return NOT; }
{LP}        { yylval = tree_building("LP", LEX_OTHER, yylineno, 0, NULL);
              return LP; }
{RP}        { yylval = tree_building("RP", LEX_OTHER, yylineno, 0, NULL);
              return RP; }
{LB}        { yylval = tree_building("LB", LEX_OTHER, yylineno, 0, NULL);
              return LB; }
{RB}        { yylval = tree_building("RB", LEX_OTHER, yylineno, 0, NULL);
              return RB; }
{LC}        { yylval = tree_building("LC", LEX_OTHER, yylineno, 0, NULL);
              return LC; }
{RC}        { yylval = tree_building("RC", LEX_OTHER, yylineno, 0, NULL);
              return RC; }
{STRUCT}    { yylval = tree_building("STRUCT", LEX_OTHER, yylineno, 0, NULL);
              return STRUCT; }
{RETURN}    { yylval = tree_building("RETURN", LEX_OTHER, yylineno, 0, NULL);
              return RETURN; }
{IF}        { yylval = tree_building("IF", LEX_OTHER, yylineno, 0, NULL); 
              return IF; }
{ELSE}      { yylval = tree_building("ELSE", LEX_OTHER, yylineno, 0, NULL);
              return ELSE; }
{WHILE}     { yylval = tree_building("WHILE", LEX_OTHER, yylineno, 0, NULL);
              return WHILE; }
{RELOP}     { yylval = tree_building("RELOP", LEX_OTHER, yylineno, 0, NULL);
              strcpy(yylval->str_val, yytext);
              return RELOP; }
{TYPE}      { yylval = tree_building("TYPE", LEX_TYPE, yylineno, 0, NULL);
              strcpy(yylval->str_val, yytext);
              return TYPE; }
{DEC}       { yylval = tree_building("INT", LEX_INT, yylineno, 0, NULL);
              sscanf(yytext,"%d",&yylval->int_val);
              return INT; }
{FLOAT}     { yylval = tree_building("FLOAT", LEX_FLOAT, yylineno, 0, NULL);
              sscanf(yytext,"%f",&yylval->float_val);
              return FLOAT; }
{ID}        { yylval = tree_building("ID", LEX_ID, yylineno, 0, NULL);
              strcpy(yylval->str_val, yytext);
              return ID; }
.           { lexical_errors++;
              printf("Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext); }
%%
