#include <stdio.h>
#include "tree.h"
#include"objectcode.h"
#include "intermediate.h"

                     // the file pointer from which the translator writes its output.
extern int yyrestart(FILE* f);
extern int yyparse();
extern int yylex();
extern void print_tree(struct Node* root, int depth);
extern struct Node* root;
extern CodeList intercodes;
int lexical_errors = 0;
int syntax_errors = 0;

int main(int argc, char** argv) {
    if (argc <= 1)
        return 1;
    FILE* f = fopen(argv[1], "r");
    FILE* asmff;
    //FILE* intercodeff = fopen("output.ir","w");
    if(argv[2] == NULL)asmff = fopen("output.s", "w");
    else asmff =fopen(argv[2], "w");
    if (!f) {
        perror(argv[1]);
        return 1;
    }
    yyrestart(f);
    yyparse();
    if (root != NULL && lexical_errors == 0 && syntax_errors == 0){
        //print_tree(root, 0);
        semantic_check(root);
        Intercode_analysis(root);
        //show_codelist(stdout);
        generate_asm(asmff);
    }
    fclose(asmff);
    return 0;
}