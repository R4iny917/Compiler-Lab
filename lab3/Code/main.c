#include <stdio.h>
#include "tree.h"

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
    FILE* ir_out = fopen(argv[2],"w");   
    if (!f) {
        perror(argv[1]);
        return 1;
    }
    if (!ir_out) {
        perror(argv[2]);
        return 1;
    }
    yyrestart(f);
    yyparse();
    if (root != NULL && lexical_errors == 0 && syntax_errors == 0){
        //print_tree(root, 0);
        semantic_check(root);
        //printf("semantic check over.\n");
        Intercode_analysis(root);
        show_codelist(ir_out);
    }
    return 0;
}