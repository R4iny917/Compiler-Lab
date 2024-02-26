#ifndef TREE_H_
#define TREE_H_
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
    char name[32]; // 节点名称
    enum node_type node_type;  // 节点类型
    int lineno; // 该节点对应语法/词法单元的行号
    union { // 该节点需要存储的值信息
        char str_val[32];    
        int int_val;
        float float_val;
    };
    int child_num;   // 该节点的子节点个数
    struct Node** children; // 该节点的子节点数组
};
//树的构建与打印
struct Node* tree_building(char* name, enum node_type node_type, int lineno, int child_num, struct Node** children);
void print_tree(struct Node* root, int depth);
#endif