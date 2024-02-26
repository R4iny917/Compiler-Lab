#ifndef INTERCODE_H
#define INTERCODE_H

#include <stdio.h>
#include <stdlib.h>
#include "semantic.h"

typedef struct Operand_* Operand;
typedef struct CodeList_* CodeList;
typedef struct InterCode_* InterCode;
typedef struct ArgList_* ArgList;
typedef struct _Variable* Variable;

#define true 1
#define false 0

struct Operand_{
    enum{
        OP_VARIABLE,    // 变量（var）
        OP_CONSTANT,    // 常量（#1）
        OP_LABEL,       // 标签(LABEL label1:)
        OP_TEMP,        // 临时变量（t1）
        OP_FUNCTION,    //函数
        OP_ARRAY,       //数组
        OP_GET_POINT,   //获得以目标操作数的值为地址的空间变量
        OP_GET_ADDR,    //获得目标操作数的值的地址
    }kind;
    union{ 
        int var_no; // 变量定义的序号
        int const_val; //操作数的值
        int label_no; // 标签序号
        int temp_no; // 临时变量序号（唯一性）
        char* func_name; //函数名字
        Operand op;     //目标操作数，对应GET_POINT,GET_ADDR,和数组元素
    } u;
    Type type; // 计算数组、结构体占用size
};

struct InterCode_{
    enum{
        IC_LABEL,    // LABEL x :
        IC_FUNCTION, // FUNCTION f :
        IC_ASSIGN,   // x := y
        IC_PLUS,     // x := y+z
        IC_SUB,      // x := y-z
        IC_MUL,      // x := y*z
        IC_DIV,      // x := y/z
        IC_GOTO,     // GOTO x
        IC_IF_GOTO,  // IF x [relop] y GOTO z
        IC_RETURN,   // RETURN x
        IC_DEC,      // DEC x [size]
        IC_ARG,      // ARG x
        IC_CALL,     // x := CALL f
        IC_PARAM,    // PARAM x
        IC_READ,     // READ x
        IC_WRITE,    // WRITE x      
    } kind; 
    union{
    Operand op; //单操作数中间代码
    struct{Operand right, left; } assign; //等于
    struct{Operand result, op1, op2; } binop; //三地址代码
    struct{Operand result;int size;} dec; //数组声明
    struct { Operand result, op1, op2; char* relop; } ifgoto;  //IF_GOTO语句
    }u;
};

struct CodeList_{
    InterCode code; // 中间代码列表实现方式
    CodeList prev, next;
};

struct _Variable{ // 变量的实现方式
    char* name;
    Operand op;
    Variable next;
};

InterCode new_InterCode(int type,Operand op1,Operand op2,Operand op3,char* name);//生成一条中间代码
Operand new_Operand(int type,int num); //生成一个新的操作数
Operand new_addr(Operand op); //生成一个以目标操作数的地址为值的操作数
Operand new_point(Operand op); //生成一个以目标操作数为地址的值的操作数
Operand new_temp(); //产生一个临时变量
Operand new_var(char* name);  //产生一个变量标记                               
Operand new_label();  //产生一个跳转标记
Operand new_arr(char* name);//产生一个新的数组标记
Operand new_func(char* name);//产生一个新的函数标记
void add_interCode(InterCode ic); //将中间代码加入到中间代码链表尾部
void add_in_variable(char *name,Operand op); //将变量加入到变量链表中

Operand get_operand(char* name); //从变量链表中查找对应操作数
int get_size(Type type); //获得目标数组的size

void show_operand(Operand op, FILE* ic_out); //将操作数信息打印到文件中
void show_intercode(InterCode ic,FILE* ic_out);//将中间代码信息打印到文件中
void show_codelist(FILE* ic_out);//逐步打印中间代码
void struct_error_handler(); //struct错误处理，即代码中不应出现struct

//translation
void Intercode_analysis(struct Node* root);
void translate_Program(struct Node* root);
void translate_ExtDefList(struct Node* root);
void translate_ExtDef(struct Node* root);

Operand translate_VarDec(struct Node* root);  // 返回变量OP
void translate_FunDec(struct Node* root);

void translate_CompSt(struct Node* root);
void translate_StmtList(struct Node* root);
void translate_Stmt(struct Node* root);

void translate_DefList(struct Node* root);
void translate_Def(struct Node* root);
void translate_DecList(struct Node* root);
void translate_Dec(struct Node* root);

void translate_Exp(struct Node* root, Operand place);
void translate_Args(struct Node* root, int write_func);

void translate_Cond(struct Node* root,Operand label_true, Operand label_false);


#endif