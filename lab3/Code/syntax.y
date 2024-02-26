%locations
%define parse.error verbose

%{
    #include <stdarg.h>
    #include "lex.yy.c"
    struct Node* root = NULL;
    struct Node** construct_children(int child_num, struct Node* child1, ...);
    void yyerror(const char* msg);
    extern int syntax_errors;
    int yyerror_line = 0;
%}

%token  INT FLOAT ID 
%token  SEMI COMMA 
%token  RELOP PLUS MINUS STAR DIV ASSIGNOP
%token  AND OR DOT NOT
%token  LP RP LB RB LC RC
%token  TYPE STRUCT RETURN
%token  IF ELSE WHILE

%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT
%left  UMINUS 
%left LP RP LB RB DOT

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%%
/* High-level Definitions */
Program : ExtDefList                            { $$ = tree_building("Program", NOT_TEM, @$.first_line, 1, construct_children(1, $1));
                                                  root = $$; }
    ;
ExtDefList : ExtDef ExtDefList                  { $$ = tree_building("ExtDefList", NOT_TEM, @$.first_line, 2, construct_children(2, $1, $2)); }
    | /* empty */                               { $$ = NULL;}
    ;
ExtDef : Specifier ExtDecList SEMI              { $$ = tree_building("ExtDef", NOT_TEM, @$.first_line, 3, construct_children(3, $1, $2, $3)); }
    | Specifier SEMI                            { $$ = tree_building("ExtDef", NOT_TEM, @$.first_line, 2, construct_children(2, $1, $2)); }
    | Specifier FunDec CompSt                   { $$ = tree_building("ExtDef", NOT_TEM, @$.first_line, 3, construct_children(3, $1, $2, $3)); }
    | error SEMI                                {  }
    | Specifier error SEMI                      {  }
    | error Specifier SEMI                      {  }
    | error RP                                  {  }
    | error RC                                  {  }
    |error Specifier error CompSt               {  }
    ;
ExtDecList : VarDec                             { $$ = tree_building("ExtDecList", NOT_TEM, @$.first_line, 1, construct_children(1, $1)); }
    | VarDec COMMA ExtDecList                   { $$ = tree_building("ExtDecList", NOT_TEM, @$.first_line, 3, construct_children(3, $1, $2, $3)); }
    | VarDec error COMMA ExtDecList             { }
    ;

/* Specifiers */
Specifier : TYPE                                { $$ = tree_building("Specifier", NOT_TEM, @$.first_line, 1, construct_children(1, $1)); }
    | StructSpecifier                           { $$ = tree_building("Specifier", NOT_TEM, @$.first_line, 1, construct_children(1, $1)); }
    ;
StructSpecifier : STRUCT OptTag LC DefList RC   { $$ = tree_building("StructSpecifier", NOT_TEM, @$.first_line, 5, construct_children(5, $1, $2, $3, $4, $5)); }
    | STRUCT Tag                                { $$ = tree_building("StructSpecifier", NOT_TEM, @$.first_line, 2, construct_children(2, $1, $2)); }
    | STRUCT OptTag LC DefList error RC         {}
    ;
OptTag : ID                                     { $$ = tree_building("OptTag", NOT_TEM, @$.first_line, 1, construct_children(1, $1)); }
    | /* empty */                               { $$ = NULL;}
    ;
Tag : ID                                        { $$ = tree_building("Tag", NOT_TEM, @$.first_line, 1, construct_children(1, $1)); }
    ;

/* Declarators */
VarDec : ID                                     { $$ = tree_building("VarDec", NOT_TEM, @$.first_line, 1, construct_children(1, $1)); }
    | VarDec LB INT RB                          { $$ = tree_building("VarDec", NOT_TEM, @$.first_line , 4, construct_children(4, $1, $2, $3, $4)); }
    ;
FunDec : ID LP VarList RP                       { $$ = tree_building("FunDec", NOT_TEM, @$.first_line, 4, construct_children(4, $1, $2, $3, $4)); }
    | ID LP RP                                  { $$ = tree_building("FunDec", NOT_TEM, @$.first_line , 3, construct_children(3, $1, $2, $3)); }
    ;
VarList : ParamDec COMMA VarList                { $$ = tree_building("VarList", NOT_TEM, @$.first_line, 3, construct_children(3, $1, $2, $3)); }
    | ParamDec                                  { $$ = tree_building("VarList", NOT_TEM, @$.first_line, 1, construct_children(1, $1)); }
    ;
ParamDec : Specifier VarDec                     { $$ = tree_building("ParamDec", NOT_TEM, @$.first_line , 2, construct_children(2, $1, $2)); }
    ;

/* Statements */
CompSt : LC DefList StmtList RC                 { $$ = tree_building("CompSt", NOT_TEM, @$.first_line, 4, construct_children(4, $1, $2, $3, $4)); }
    ;
StmtList : Stmt StmtList                        { $$ = tree_building("StmtList", NOT_TEM, @$.first_line, 2, construct_children(2, $1, $2)); }
    | /* empty */                               { $$ = NULL; }
    ;
Stmt : Exp SEMI                                 { $$ = tree_building("Stmt", NOT_TEM, @$.first_line, 2, construct_children(2, $1, $2)); }
    | CompSt                                    { $$ = tree_building("Stmt", NOT_TEM, @$.first_line, 1, construct_children(1, $1)); }
    | RETURN Exp SEMI                           { $$ = tree_building("Stmt", NOT_TEM, @$.first_line, 3, construct_children(3, $1, $2, $3)); }
    | IF LP Exp RP Stmt %prec LOWER_THAN_ELSE   { $$ = tree_building("Stmt", NOT_TEM, @$.first_line, 5, construct_children(5, $1, $2, $3, $4, $5)); }
    | IF LP Exp RP Stmt ELSE Stmt               { $$ = tree_building("Stmt", NOT_TEM, @$.first_line , 7, construct_children(7, $1, $2, $3, $4, $5, $6, $7)); }
    | WHILE LP Exp RP Stmt                      { $$ = tree_building("Stmt", NOT_TEM, @$.first_line, 5, construct_children(5, $1, $2, $3, $4, $5)); }
    | error SEMI                                { }
    | error Stmt                                { }
    | Exp error                                 { }
    | IF LP error RP Stmt %prec LOWER_THAN_ELSE { }
    | IF LP error RP Stmt ELSE Stmt             { }
    | WHILE LP error RP Stmt                    { }
    ;

/* Local Definitions */
DefList : Def DefList                           { $$ = tree_building("DefList", NOT_TEM, @$.first_line , 2, construct_children(2, $1, $2)); }
    | /* empty */                               { $$ = NULL; }
    ;
Def : Specifier DecList SEMI                    { $$ = tree_building("Def", NOT_TEM, @$.first_line, 3, construct_children(3, $1, $2, $3)); }
    | Specifier DecList error SEMI              {  }
    | Specifier error SEMI                      {  }
    
    ;
DecList : Dec                                   { $$ = tree_building("DecList", NOT_TEM, @$.first_line, 1, construct_children(1, $1)); }
    | Dec COMMA DecList                         { $$ = tree_building("DecList", NOT_TEM, @$.first_line, 3, construct_children(3, $1, $2, $3)); }
    ;
Dec : VarDec                                    { $$ = tree_building("Dec", NOT_TEM, @$.first_line, 1, construct_children(1, $1)); }
    | VarDec ASSIGNOP Exp                       { $$ = tree_building("Dec", NOT_TEM, @$.first_line, 3, construct_children(3, $1, $2, $3)); }
    ;

/* Expchildrensions */
Exp : Exp ASSIGNOP Exp                          { $$ = tree_building("Exp", NOT_TEM, @$.first_line, 3, construct_children(3, $1, $2, $3)); }
    | Exp AND Exp                               { $$ = tree_building("Exp", NOT_TEM, @$.first_line , 3, construct_children(3, $1, $2, $3)); }
    | Exp OR Exp                                { $$ = tree_building("Exp", NOT_TEM, @$.first_line, 3, construct_children(3, $1, $2, $3)); }
    | Exp RELOP Exp                             { $$ = tree_building("Exp", NOT_TEM, @$.first_line , 3, construct_children(3, $1, $2, $3)); }
    | Exp PLUS Exp                              { $$ = tree_building("Exp", NOT_TEM, @$.first_line, 3, construct_children(3, $1, $2, $3)); }
    | Exp MINUS Exp                             { $$ = tree_building("Exp", NOT_TEM, @$.first_line, 3, construct_children(3, $1, $2, $3)); }
    | Exp STAR Exp                              { $$ = tree_building("Exp", NOT_TEM, @$.first_line, 3, construct_children(3, $1, $2, $3)); }
    | Exp DIV Exp                               { $$ = tree_building("Exp", NOT_TEM, @$.first_line, 3, construct_children(3, $1, $2, $3)); }
    | LP Exp RP                                 { $$ = tree_building("Exp", NOT_TEM, @$.first_line, 3, construct_children(3, $1, $2, $3)); }
    | MINUS Exp %prec UMINUS                    { $$ = tree_building("Exp", NOT_TEM, @$.first_line, 2, construct_children(2, $1, $2)); }
    | NOT Exp                                   { $$ = tree_building("Exp", NOT_TEM, @$.first_line, 2, construct_children(2, $1, $2)); }
    | ID LP Args RP                             { $$ = tree_building("Exp", NOT_TEM, @$.first_line, 4, construct_children(4, $1, $2, $3, $4)); }
    | ID LP RP                                  { $$ = tree_building("Exp", NOT_TEM, @$.first_line, 3, construct_children(3, $1, $2, $3)); }
    | Exp LB Exp RB                             { $$ = tree_building("Exp", NOT_TEM, @$.first_line, 4, construct_children(4, $1, $2, $3, $4)); }
    | Exp DOT ID                                { $$ = tree_building("Exp", NOT_TEM, @$.first_line, 3, construct_children(3, $1, $2, $3)); }
    | ID                                        { $$ = tree_building("Exp", NOT_TEM, @$.first_line, 1, construct_children(1, $1)); }
    | INT                                       { $$ = tree_building("Exp", NOT_TEM, @$.first_line, 1, construct_children(1, $1)); }
    | FLOAT                                     { $$ = tree_building("Exp", NOT_TEM, @$.first_line, 1, construct_children(1, $1)); }
    | Exp ASSIGNOP error                        { }
    | Exp AND error                             { }
    | Exp OR error                              { }
    | Exp RELOP error                           { }
    | Exp PLUS error                            { }
    | Exp MINUS error                           { }
    | Exp STAR error                            { }
    | Exp DIV error                             { }
    | LP error RP                               { }   
    | LP Exp error                              { }    
    | MINUS error %prec UMINUS                  { }           
    | NOT error                                 { }  
    | ID LP error RP                            { }  
    | Exp LB error RB                           { }  
    ;
Args : Exp COMMA Args                           { $$ = tree_building("Args", NOT_TEM, @$.first_line, 3, construct_children(3, $1, $2, $3)); }
    | Exp                                       { $$ = tree_building("Args", NOT_TEM, @$.first_line, 1, construct_children(1, $1)); }
    | error COMMA Exp                           { }
    ;
%%
void yyerror(const char* msg) { 
    if(yyerror_line==yylineno)return;
    yyerror_line=yylineno;
    syntax_errors++;
    printf("Error type B at Line %d: %s, near \"%s\".\n",yylineno,msg,yytext);
}

struct Node** construct_children(int child_num, struct Node* child1, ...) {
    va_list ap;
    va_start(ap, child1);
    struct Node** children = (struct Node**)malloc(sizeof(struct Node*) * child_num);
    children[0] = child1;
    for (int i = 1; i < child_num; i++)
        children[i] = va_arg(ap, struct Node*);
    return children;
}
