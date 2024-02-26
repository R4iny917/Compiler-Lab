#include"objectcode.h"
extern CodeList intercodes;
char* reg_name[Register_Num] = {"$zero","$at","$v0","$v1","$a0","$a1","$a2","$a3","$t0","$t1","$t2","$t3","$t4","$t5","$t6","$t7",
                      "$s0","$s1","$s2","$s3","$s4","$s5","$s6","$s7","$t8","$t9","$k0","$k1","$gp","$sp","$fp","$ra"};
Register registers[Register_Num]; //32个寄存器
int local_offset; //被调用函数内存区对应偏移量
VarStructure var_head; //变量链表
int local_argnums; //调用函数前参数数目统计，调用后清零
//初始化文件中的write和read函数
void init_output_file(FILE* file){
   // 打印目标代码数据段
    fprintf(file,".data\n_prompt: .asciiz \"Enter an integer:\"\n_ret: .asciiz \"\\n\"\n.globl main\n.text\n");
    // 打印read函数定义
    fprintf(file,"\nread:\n");
    fprintf(file,"\tli $v0, 4\n");
    fprintf(file,"\tla $a0, _prompt\n");
    fprintf(file,"\tsyscall\n");
    fprintf(file,"\tli $v0, 5\n");
    fprintf(file,"\tsyscall\n");
    fprintf(file,"\tjr $ra\n");
    // 打印write函数定义
    fprintf(file,"\nwrite:\n");
    fprintf(file,"\tli $v0, 1\n");
    fprintf(file,"\tsyscall\n");
    fprintf(file,"\tli $v0, 4\n");
    fprintf(file,"\tla $a0, _ret\n");
    fprintf(file,"\tsyscall\n");
    fprintf(file,"\tmove $v0, $0\n");
    fprintf(file,"\tjr $ra\n\n");
}
//初始化32个寄存器
void init_registers(){
    for(int i = 0; i < Register_Num; ++i){
        registers[i].name = reg_name[i];
        registers[i].is_used = 0;
        registers[i].variable = NULL;
    }
}
//在内存区初始化变量
void insert_varstructure(Operand op){
    if(op->kind == OP_CONSTANT)
        return;
    if(find_varstructure(op) == NULL){
        if(op->kind != OP_ARRAY)
            local_offset += 4;
        VarStructure variable = (VarStructure)malloc(sizeof(struct _VarStructure));
        variable->offset = -local_offset;
        if(op->kind == OP_GET_ADDR || op->kind == OP_GET_POINT)
            variable->op = op->u.op;
        else
            variable->op = op;
        variable->next = var_head;
        var_head = variable;
    }
    
}
//Debugging Tools
void print_variable_list(){
    VarStructure var = var_head;
    while(var != NULL){
        printf("offset:%d\top:", var->offset);
        show_operand(var->op, stdout);
        printf("\n");
        var = var->next;
    }
}
//从内存区中获取变量
VarStructure find_varstructure(Operand op){
    VarStructure cur_variable = var_head;
    Operand op2 = op;
    if(op->kind == OP_GET_ADDR || op->kind == OP_GET_POINT)
        op2 = op->u.op;
    while(cur_variable != NULL){
        if(compare_operand(cur_variable->op,op2) == 1)
            return cur_variable;
        cur_variable = cur_variable->next;
    }
    return NULL;
}
//比较两个操作数，判断是否相同
int compare_operand(Operand op1, Operand op2){
    if(op1->kind == op2->kind){
        switch (op1->kind)
        {
        case OP_TEMP:
            return (op1->u.temp_no == op2->u.temp_no);
        case OP_VARIABLE:
        case OP_ARRAY:
            return(op1->u.var_no == op2->u.var_no);
        default:
            break;
        }
    }
    return 0;
}
//获取寄存器,可以处理所有类型，is_left表示传进来的操作数是否是左值
int getReg(Operand op, FILE* file,int is_left){
    int index = 0;
    for(int i = 8; i < 16; ++i){
        if(registers[i].is_used == 0){//寄存器空闲
            index = i;
            break;
        }
    }
    assert(index != 0);//肯定会有空闲的寄存器
    registers[index].is_used = 1;
    if(op->kind == OP_CONSTANT){//处理常量右值
        fprintf(file, "\tli %s, %d #将const值存入寄存器\n ",registers[index].name,op->u.const_val);
        return index;
    }
    else{
        VarStructure var_structrue = find_varstructure(op);
        registers[index].variable = var_structrue;
        if(op->kind == OP_GET_ADDR){//处理&x型，必为右值
            fprintf(file, "\taddi %s, $fp, %d #将x的地址取出\n ", registers[index].name, var_structrue->offset);
        }
        else if(op->kind == OP_GET_POINT){//处理*x型，需要判断是左值还是右值
            if(is_left == 1){ //判断当前操作数是不是左值
                fprintf(file, "\tlw %s, %d($fp) #左值取出地址\n", registers[index].name, var_structrue->offset);
            }
            else{
                fprintf(file, "\tlw %s, %d($fp)#右值取出地址保存值\n", registers[index].name, var_structrue->offset);
                fprintf(file, "\tlw %s, 0(%s)\n", registers[index].name, registers[index].name);
            }
        }
        else {
            fprintf(file, "\tlw %s, %d($fp) #将值存入寄存器\n", registers[index].name, var_structrue->offset);
        }
        return index;
    }
    return 0;
}
//将寄存器中的值写回内存,并清空当前调用的寄存器
void write_back(int index, FILE* file){
    fprintf(file, "\tsw %s, %d($fp)#将值写回寄存器\n", registers[index].name, registers[index].variable->offset);
    for(int i = 8; i < 16; ++i)
        registers[i].is_used = 0;
}
//根据每条中间代码生成对应的Mips指令
void generate_asm(FILE* file){
    init_registers();
    init_output_file(file);
    var_head = NULL;
    local_offset = 0;
    local_argnums = 0;
    CodeList cur = intercodes->next;
    while(cur!= intercodes){
        assert(cur != NULL);
        generate_IR_asm(cur,file);
        cur = cur->next;
    }
}

void generate_IR_asm(CodeList codelists, FILE* file){
    
    switch (codelists->code->kind)
    {
        
    case IC_LABEL:  // LABEL x :
        generate_label_objcode(codelists,file);
        break;
    case IC_FUNCTION: // FUNCTION f :
        generate_function_objcode(codelists,file);
        break;
    case IC_ASSIGN:   // x := y
        generate_assign_objcode(codelists,file);
        break;
    case IC_PLUS:     // x := y+z
        generate_plus_objcode(codelists,file);
        break;
    case IC_SUB:      // x := y-z
        generate_sub_objcode(codelists,file);
        break;
    case IC_MUL:      // x := y*z
        generate_mul_objcode(codelists,file);
        break;
    case IC_DIV:      // x := y/z
        generate_div_objcode(codelists,file);
        break;
    case IC_GOTO:     // GOTO x
        generate_goto_objcode(codelists,file);
        break;
    case IC_IF_GOTO:  // IF x [relop] y GOTO z
        generate_if_goto_objcode(codelists,file);
        break;
    case IC_RETURN:   // RETURN x
        generate_return_objcode(codelists,file);
        break;
    case IC_DEC:      // DEC x [size]
        generate_dec_objcode(codelists,file);
        break;
    case IC_ARG:      // ARG x
        generate_arg_objcode(codelists,file);
        break;
    case IC_CALL:     // x := CALL f
        generate_call_objcode(codelists,file);
        break;
    case IC_PARAM:    // PARAM x
        generate_param_objcode(codelists,file);
        break;
    case IC_READ:    // READ x
        generate_read_objcode(codelists,file);
        break;
    case IC_WRITE:    // WRITE x  
        generate_write_objcode(codelists,file);
        break;
    default:
        break;
    }
}
void generate_label_objcode(CodeList codelists, FILE* file){
    fprintf(file,"label%d:\n",codelists->code->u.op->u.label_no);
}

void generate_function_objcode(CodeList codelists, FILE *file){
    fprintf(file,"\n%s: #函数初始化\n",codelists->code->u.op->u.func_name);
    fprintf(file, "\taddi $sp, $sp, -8\n");//
    fprintf(file, "\tsw $fp, 0($sp)\n");
    fprintf(file, "\tsw $ra, 4($sp)\n");
    fprintf(file, "\tmove $fp, $sp #函数初始化结束\n");
    local_offset = 0;
    int param_num = 0;
    CodeList cur_codelist = codelists->next;
    while(cur_codelist->code->kind == IC_PARAM){ //遍历参数列表，将所有参数添加到堆区中，所以offset为正
        VarStructure param = (VarStructure)malloc(sizeof(struct _VarStructure));
        param->offset = 8 + 4 * param_num;
        param->op = cur_codelist->code->u.op;
        param->next = var_head;
        var_head = param;
        cur_codelist = cur_codelist->next;
        param_num++;
    } 
    while(cur_codelist != intercodes && cur_codelist->code->kind != IC_FUNCTION){ //遇到一个新函数体时，遍历完整个函数内部的中间代码，将变量加载到内存区，方便后续写回内存
        switch (cur_codelist->code->kind)
        {
        case IC_ASSIGN:
            insert_varstructure(cur_codelist->code->u.assign.left);
            insert_varstructure(cur_codelist->code->u.assign.right);
            break;
        case IC_PLUS:
        case IC_SUB:
        case IC_MUL:
        case IC_DIV:
            insert_varstructure(cur_codelist->code->u.binop.result);
            insert_varstructure(cur_codelist->code->u.binop.op1);
            insert_varstructure(cur_codelist->code->u.binop.op2);
            break;
        case IC_IF_GOTO:
            insert_varstructure(cur_codelist->code->u.ifgoto.op1);
            insert_varstructure(cur_codelist->code->u.ifgoto.op2);
            break;
        case IC_DEC:
            local_offset += cur_codelist->code->u.dec.size;
            insert_varstructure(cur_codelist->code->u.dec.result);
            break;
        case IC_CALL:
            insert_varstructure(cur_codelist->code->u.assign.left);
            break;
        case IC_READ:
        case IC_WRITE:
        case IC_ARG:
            insert_varstructure(cur_codelist->code->u.op);
            break;
        default:
            break;
        }
        cur_codelist = cur_codelist->next;
    }
    fprintf(file,"\taddi $sp, $sp, -%d #移动帧指针\n",local_offset);
    for(int i = 8; i < 16; ++i)//所有被调用者寄存器均空闲
        registers[i].is_used = 0;
}

void generate_assign_objcode(CodeList codelists, FILE* file){
    // 左侧操作数有三种可能的类型：OP_VARIABLE, OP_TEMP, OP_GET_POINT
    Operand left = codelists->code->u.assign.left;
    // 右侧操作数有五种可能的类型：OP_VARIABLE, OP_TEMP, OP_GET_POINT,OP_CONSTANT,OP_GET_ADDR
    Operand right = codelists->code->u.assign.right;
    int left_index = getReg(left,file,1);
    int right_index = getReg(right,file,0);
    if(left->kind == OP_GET_POINT){ //*x = ?
        fprintf(file,"\tsw %s, 0(%s) #指针存值\n",registers[right_index].name,registers[left_index].name);
    }
    else{// x = ?
        fprintf(file,"\tmove %s, %s #变量存值\n",registers[left_index].name,registers[right_index].name);
    }
    write_back(left_index,file);
}

void generate_plus_objcode(CodeList codelists, FILE* file){
    int result_index = getReg(codelists->code->u.binop.result,file,1);
    int op1_index = getReg(codelists->code->u.binop.op1,file,0);
    int op2_index = getReg(codelists->code->u.binop.op2,file,0);
    fprintf(file,"\tadd %s, %s, %s \n", registers[result_index].name,registers[op1_index].name,registers[op2_index].name);
    write_back(result_index,file);
}

void generate_sub_objcode(CodeList codelists, FILE* file){
    int result_index = getReg(codelists->code->u.binop.result,file,1);
    int op1_index = getReg(codelists->code->u.binop.op1,file,0);
    int op2_index = getReg(codelists->code->u.binop.op2,file,0);
    fprintf(file,"\tsub %s, %s, %s\n", registers[result_index].name,registers[op1_index].name,registers[op2_index].name);
    write_back(result_index,file);
}

void generate_mul_objcode(CodeList codelists, FILE* file){
    int result_index = getReg(codelists->code->u.binop.result,file,1);
    int op1_index = getReg(codelists->code->u.binop.op1,file,0);
    int op2_index = getReg(codelists->code->u.binop.op2,file,0);
    fprintf(file,"\tmul %s, %s, %s\n", registers[result_index].name,registers[op1_index].name,registers[op2_index].name);
    write_back(result_index,file);
}

void generate_div_objcode(CodeList codelists, FILE* file){
    int result_index = getReg(codelists->code->u.binop.result,file,1);
    int op1_index = getReg(codelists->code->u.binop.op1,file,0);
    int op2_index = getReg(codelists->code->u.binop.op2,file,0);
    fprintf(file,"\tdiv %s, %s, %s\n", registers[result_index].name,registers[op1_index].name,registers[op2_index].name);
    write_back(result_index,file);
}

void generate_goto_objcode(CodeList codelists, FILE* file){
    fprintf(file, "\tj label%d\n", codelists->code->u.op->u.label_no);
}

void generate_if_goto_objcode(CodeList codelists, FILE* file){
    int left_index = getReg(codelists->code->u.ifgoto.op1,file,0);
    int right_index = getReg(codelists->code->u.ifgoto.op2,file,0);
    char* relop = NULL;
    if(strcmp(codelists->code->u.ifgoto.relop, "==")==0)
        relop = "beq";
    else if(strcmp(codelists->code->u.ifgoto.relop, "!=")==0)
        relop = "bne";
    else if(strcmp(codelists->code->u.ifgoto.relop, ">")==0)
        relop = "bgt";
    else if(strcmp(codelists->code->u.ifgoto.relop, "<")==0)
        relop = "blt";
    else if(strcmp(codelists->code->u.ifgoto.relop, ">=")==0) // “<=”与其类似
        relop = "bge";
    else if(strcmp(codelists->code->u.ifgoto.relop, "<=")==0) // “<=”与其类似
        relop = "ble";
        
    fprintf(file, "\t%s %s, %s, label%d #if_goto\n", relop,registers[left_index].name,registers[right_index].name,codelists->code->u.ifgoto.result->u.label_no);
    //清空寄存器
    registers[left_index].is_used = 0;
    registers[right_index].is_used = 0;
}

void generate_return_objcode(CodeList codelists, FILE* file){ // RETURN x
    fprintf(file, "\tlw $ra, 4($fp)#开始return指令\n");
    fprintf(file, "\taddi $sp, $fp, 8\n");
    int index = getReg(codelists->code->u.op,file,0);
    fprintf(file, "\tmove $v0, %s \n",registers[index].name);
    fprintf(file, "\tlw $fp, 0($fp)\n");
    fprintf(file, "\tjr $ra #return指令结束\n");
    for(int i = 8; i < 16; ++i)
        registers[i].is_used = 0;//返回后清空所有$t寄存器
}

void generate_dec_objcode(CodeList codelists, FILE* file){} // DEC x [size],数组声明，已经在初始化局部变量空间时处理过

void generate_arg_objcode(CodeList codelists, FILE* file){
    int index = getReg(codelists->code->u.op, file,0);
    fprintf(file, "\taddi $sp, $sp, -4 #计算传入参数\n");
    fprintf(file, "\tsw %s, 0($sp)\n", registers[index].name);
    local_argnums++;
    registers[index].is_used = 0;
}

void generate_call_objcode(CodeList codelists, FILE* file){
    fprintf(file,"\tjal %s #调用函数\n",codelists->code->u.assign.right->u.func_name);
    fprintf(file,"\taddi $sp, $sp, %d\n",local_argnums * 4);
    local_argnums = 0;
    int index = getReg(codelists->code->u.assign.left,file,1);
    fprintf(file, "\tmove %s, $v0\n", registers[index].name);
    write_back(index,file);
}

void generate_read_objcode(CodeList codelists, FILE* file){
    fprintf(file, "\taddi $sp, $sp, -4\n");
    fprintf(file, "\tsw $ra, 0($sp)\n"); // 保存$ra寄存器内容
    fprintf(file, "\tjal read\n");
    //为read参数x分配寄存器;
    int index = getReg(codelists->code->u.op,file,1);
    fprintf(file, "\tmove %s, $v0\n",registers[index].name);
    //将x关联到的变量值溢出到栈上并标记偏移量;
    fprintf(file, "\tlw $ra, 0($sp)\n");
    fprintf(file, "\taddi $sp, $sp, 4\n"); // 恢复$ra寄存器

    write_back(index,file);
}

void generate_write_objcode(CodeList codelists, FILE* file){
    fprintf(file,"\taddi $sp, $sp, -4\n");
    fprintf(file,"\tsw $ra, 0($sp)\n"); // 保存$ra寄存器内容
    //为x分配寄存器;
    int index = getReg(codelists->code->u.op,file,0);
    fprintf(file, "\tmove $a0, %s #进行写操作\n", registers[index].name);
    fprintf(file,"\tjal write\n"); // 跳转到write函数
    //将寄存器x关联的变量值溢出到栈中记录偏移量;
    fprintf(file,"\tlw $ra, 0($sp)\n");
    fprintf(file,"\taddi $sp, $sp, 4\n"); // 恢复$ra内容
    for(int i = 8; i < 16; ++i)
        registers[i].is_used = 0;
}

void generate_param_objcode(CodeList codelists, FILE* file){} // PARAM x，在初始化function时已处理
