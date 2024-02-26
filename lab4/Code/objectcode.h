#ifndef __OBJECTCODE_H__
#define __OBJECTCODE_H__
#define Register_Num 32
#include "intermediate.h"
typedef struct _Register Register;
typedef struct _VarStructure* VarStructure;


struct _VarStructure{
    Operand op;//变量名
    int offset;    //变量在内存中的存储位置
    VarStructure next; //变量链表;
    // int reg 变量存放的寄存器，由于采用朴素寄存器分配法，每次调用后都会写回内存，所以该变量无效
};

struct _Register{
    int is_used;          // 是否使用
    char *name;         // 寄存器名
    VarStructure variable; //关联的变量VarStructure var
};

void generate_asm(FILE* file);//逐步打印中间代码
void generate_IR_asm(CodeList codelists,FILE* file);//将中间代码信息打印到文件中
void init_output_file(FILE* file); //初始化文件中的write和read函数
void init_registers(); //初始化32个寄存器
void insert_varstructure(Operand op); //在内存区初始化变量
VarStructure find_varstructure(Operand op); //从内存区中获取变量
int compare_operand(Operand op1, Operand op2); //比较两个操作数
int getReg(Operand op, FILE* file,int is_left);//处理操作数，返回操作数存储的寄存器编号
void write_back(int index, FILE* file);//将寄存器中的值写回内存


void generate_label_objcode(CodeList codelists, FILE* file);  // LABEL x :
void generate_function_objcode(CodeList codelists, FILE* file); // FUNCTION f :
void generate_assign_objcode(CodeList codelists, FILE* file); // x := y
void generate_plus_objcode(CodeList codelists, FILE* file); //x := y+z
void generate_sub_objcode(CodeList codelists, FILE* file);  // x := y-z
void generate_mul_objcode(CodeList codelists, FILE* file); // x := y*z
void generate_div_objcode(CodeList codelists, FILE* file);  // x := y/z
void generate_goto_objcode(CodeList codelists, FILE* file); // GOTO x
void generate_if_goto_objcode(CodeList codelists, FILE* file); // IF x [relop] y GOTO z
void generate_return_objcode(CodeList codelists, FILE* file); // RETURN x
void generate_dec_objcode(CodeList codelists, FILE* file); // DEC x [size]
void generate_arg_objcode(CodeList codelists, FILE* file); // ARG x
void generate_call_objcode(CodeList codelists, FILE* file); // x := CALL f
void generate_param_objcode(CodeList codelists, FILE* file); // PARAM x
void generate_write_objcode(CodeList codelists, FILE* file); // WRITE x
void generate_read_objcode(CodeList codelists, FILE* file); // READ x
#endif
