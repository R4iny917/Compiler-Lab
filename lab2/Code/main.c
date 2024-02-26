#include <stdio.h>
#include "tree.h"
#include "semantic.h"

extern int yyrestart(FILE* f);
extern int yyparse();
extern int yylex();
extern void print_tree(struct Node* root, int depth);
extern struct Node* root;
int lexical_errors = 0;
int syntax_errors = 0;

int main(int argc, char** argv) {
    if (argc <= 1)
        return 1;
    FILE* f = fopen(argv[1], "r");
    if (!f) {
        perror(argv[1]);
        return 1;
    }
    yyrestart(f);
    yyparse();
    if (root != NULL && lexical_errors == 0 && syntax_errors == 0){
        //print_tree(root, 0);
        semantic_check(root);
    }
    return 0;
}