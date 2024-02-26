#include <stdio.h>
#include <string.h>

enum node_type{
    NOT_TEM = 1,
    LEX_ID,
    LEX_TYPE,
    LEX_INT,
    LEX_FLOAT,
    LEX_OTHER
} ;

struct Node{
    char name[32]; 
    enum node_type node_type;  
    int lineno; 
    union { 
        char str_val[32];    
        int int_val;
        float float_val;
    };
    int child_num;   
    struct Node** children; 
};

struct Node* tree_building(char* name, enum node_type node_type, int lineno, int child_num, struct Node** children);
void print_tree(struct Node* root, int depth);