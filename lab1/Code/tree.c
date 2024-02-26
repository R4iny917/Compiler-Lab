#include "tree.h"
#include "malloc.h"

struct Node* tree_building(char* name, enum node_type node_type, int lineno, int child_num, struct Node** children) {
    struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));
    sscanf(name, "%s", new_node->name);
    new_node->node_type = node_type;
    new_node->lineno = lineno;
    new_node->child_num = child_num;
    new_node->children = children;
    return new_node;
}

void print_tree(struct Node* root, int depth) {
    if(root == NULL)
        return;
    for (int i = 0; i < depth; i++)
        printf("  ");
    switch (root->node_type) {
        case LEX_ID:
            printf("%s: %s\n", root->name, root->str_val);
            break;
        case LEX_TYPE:
            printf("%s: %s\n", root->name, root->str_val);
            break;
        case LEX_INT:
            printf("%s: %d\n", root->name, root->int_val);
            break;
        case LEX_FLOAT:
            printf("%s: %f\n", root->name, root->float_val);
            break;
        case LEX_OTHER:
            printf("%s\n", root->name);
            break;
        case NOT_TEM:
            printf("%s (%d)\n", root->name, root->lineno);
            break;
    }
    for (int i = 0; i < root->child_num; i++) {
        print_tree(root->children[i], depth + 1);
    }
}