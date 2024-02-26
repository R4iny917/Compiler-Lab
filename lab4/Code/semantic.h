#ifndef SEMANTIC_H_
#define SEMEANIC_H_
#include <stdio.h>
#include <stdlib.h>
#include<assert.h>
#include "tree.h"
typedef struct FieldList_* FieldList;
typedef struct Type_* Type;
typedef struct Structure_* Structure;
typedef struct Function_* Function;
typedef struct HashNode_* HashNode;
typedef struct HashTableLink_* HashTableLink;
#define HashSize 1024
HashTableLink hash_table_link;


struct Type_{
    enum{BASIC = 0,ARRAY = 1,STRUCTURE = 2,STRUCTTAG = 3,FUNCTION = 4}kind;
    union{
        int basic;//int(1),float(2)
        struct{
            Type elem; int size;
        }array;
        Structure structure;
        Function function;
    }u;
};

struct Structure_{
    char *name;
    FieldList domain;
};

struct Function_{
    char *name;//函数名字
    int line;//
    int para_num;
    Type type;
    FieldList param;
};

struct FieldList_ {
    char* name;      // 域的名字
    Type type;       // 域的类型
    FieldList tail;  // 下一个域
};

struct HashNode_ {
    char* name;
    Type type;
    HashNode link;
};

struct HashTableLink_{
    HashNode hashtable[HashSize+1];
    HashTableLink next;
    HashTableLink before;
};

//Hash Table
void check_step(char* name, int lineno);
int type_check(Type t1, Type t2);
int param_check(FieldList p1,FieldList p2);
void init_hashtable();               // initialize the hashtable
unsigned int hash_pjw(char* name);       // hash function
void insert_HashNode(HashNode hash_node);  // insert filed to hashtable
Type type_get_this_table(char* name);      // look up the item, if find return the index
Type type_get_all_table(char* name);
HashNode create_hash_node_function(Function func );

//Semantic Analysis
void semantic_check(struct Node* root);
void ExtDefList(struct Node *node);
void ExtDef(struct Node* node);
void ExtDecList(struct Node* root, Type type);

//Specifiers
Type Specifier(struct Node* root);
Type StructSpecifier(struct Node* root);
char* OptTag(struct Node* root);

//Declarators
FieldList VarDec(struct Node* root, Type type, int class);
Function FunDec(struct Node* root, Type type);
FieldList VarList(struct Node* root);
FieldList ParamDec(struct Node* root);

//Statements
void CompSt(struct Node* root, Type type);
void StmtList(struct Node* root, Type type);
void Stmt(struct Node* root, Type type);

//Local Definitions
FieldList DefList(struct Node* root, int class);
FieldList Def(struct Node* root, int class);
FieldList DecList(struct Node* root, Type type, int class);
FieldList Dec(struct Node* root, Type type, int class);

//Expressions
Type Exp(struct Node* root);
FieldList Args(struct Node* root); 
#endif