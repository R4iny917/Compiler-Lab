#include"intermediate.h"
int var_num = 0;
int label_num = 0;
int temp_num = 0;
extern int struct_is_defined;
Variable var_head,var_tail;
CodeList intercodes;

//生成一条中间代码
InterCode new_InterCode(int type,Operand op1,Operand op2, Operand op3,char* name){
    InterCode temp = (InterCode)malloc(sizeof(struct InterCode_));
    temp->kind = type;
    switch (type)
    {
    case IC_ASSIGN:
        temp->u.assign.left = op1;
        temp->u.assign.right = op2;
        break;
    case IC_LABEL:
        temp->u.op = op1;
        break;
    case IC_PLUS:
        temp->u.binop.result = op1;
        temp->u.binop.op1 = op2;
        temp->u.binop.op2 = op3;
        break; 
    case IC_SUB:
        temp->u.binop.result = op1;
        temp->u.binop.op1 = op2;
        temp->u.binop.op2 = op3;
        break;
    case IC_MUL:
        temp->u.binop.result = op1;
        temp->u.binop.op1 = op2;
        temp->u.binop.op2 = op3;
        break;
    case IC_DIV:
        temp->u.binop.result = op1;
        temp->u.binop.op1 = op2;
        temp->u.binop.op2 = op3;
        break;
    case IC_DEC:
        temp->u.dec.result = op1;
        break;
    case IC_GOTO:
        temp->u.op = op1;
        break;
    case IC_IF_GOTO:
        temp->u.ifgoto.op1 = op1;
        temp->u.ifgoto.op2 = op2;
        temp->u.ifgoto.result = op3;
        temp->u.ifgoto.relop = name;
        break;
    case IC_FUNCTION:
        temp->u.op = op1;
        break;
    case IC_ARG:
        temp->u.op = op1;
        break;
    case IC_CALL:
        temp->u.assign.left = op1;
        temp->u.assign.right = op2;
        break; 
    case IC_PARAM:
        temp->u.op = op1;
        break; 
    case IC_READ:
        temp->u.op = op1;
        break; 
    case IC_WRITE:
        temp->u.op = op1;
        break; 
    case IC_RETURN:
        temp->u.op = op1;
        break; 
    default:
        break;
    }
    return temp;
}

Operand new_Operand(int type,int num){ //生成一个新的操作数
    Operand operand = (Operand)malloc(sizeof(struct Operand_));
    operand->kind = type;
    switch (type)
    {
    case OP_VARIABLE:
        operand->u.var_no = var_num;
        var_num++;
        break;
    case OP_CONSTANT:
        operand->u.const_val = num;
        break;
    case OP_LABEL:
        operand->u.label_no = label_num;
        label_num++;
        break;
    case OP_TEMP:
        operand->u.temp_no = temp_num;
        temp_num++;
        break;
    case OP_ARRAY:
        operand->u.var_no = var_num;
        var_num++;
        break;
    default:
        break;
    }
    return operand;
}

Operand new_constant(int num){
    return new_Operand(OP_CONSTANT,num);
}
//生成一个以目标操作数的地址为值的操作数
Operand new_addr(Operand op){
    Operand new_op = new_Operand(OP_GET_ADDR,-1);
    new_op->u.op = op;
    return new_op;
}
//生成一个以目标操作数为地址的值的操作数
Operand new_point(Operand op){
    Operand new_op = new_Operand(OP_GET_POINT,-1);
    new_op->op_addr = op;
    return new_op;
}
//产生一个临时变量
Operand new_temp(){
    return new_Operand(OP_TEMP,-1);
}
//产生一个跳转标记
Operand new_label(){
    return new_Operand(OP_LABEL,-1);
}
 //产生一个变量标记  
Operand new_var(char* name){
   Operand op =  new_Operand(OP_VARIABLE,-1);
   add_in_variable(name,op);
   return op;
}
//产生一个新的数组标记
Operand new_arr(char* name){
    Operand op =  new_Operand(OP_ARRAY,-1);
    Operand addr = new_temp();
    op->op_addr = addr;
    add_in_variable(name,op);
    return op;
}
//产生一个新的函数标记
Operand new_func(char* name){
    Operand op = new_Operand(OP_FUNCTION,-1);
    op->u.func_name = name;
    return op;
}

//将中间代码加入到中间代码链表尾部
void add_interCode(InterCode ic){
    CodeList codeList = (CodeList)malloc(sizeof(struct CodeList_));
    codeList->code = (InterCode)malloc(sizeof(struct InterCode_));
    codeList->code = ic;
    CodeList tmp_tail = intercodes->prev;
    intercodes->prev = codeList;
    codeList->next = intercodes;
    tmp_tail->next = codeList;
    codeList->prev = tmp_tail;
}

//将变量加入到变量链表中
void add_in_variable(char *name,Operand op){
    if(var_head->next == NULL){
        var_head->next = (Variable)malloc(sizeof(struct _Variable));
        var_head->next->name =name;
        var_head->next->op = op;
        var_tail = var_head->next;
    }
    else{
        var_tail->next = (Variable)malloc(sizeof(struct _Variable));
        var_tail->next->name = name;
        var_tail->next->op = op;
        var_tail = var_tail->next;
    }
    var_tail->next = NULL;
}
//从变量链表中查找对应操作数
Operand get_operand(char* name){
    Variable temp = var_head->next;
    while(temp != NULL){
        if(strcmp(name,temp->name) == 0)
            return temp->op;
        temp = temp->next;
    }
    assert(0);
}
//获得目标数组的size
int get_size(Type type){
    if(type == NULL)return 0;
    if(type->kind == BASIC)return 4;
    if(type->kind == ARRAY)return type->u.array.size* get_size(type->u.array.elem);
    assert(0);
}
//将操作数信息打印到文件中
void show_operand(Operand op, FILE* ic_out){
    switch (op->kind)
    {
     /* constant-expression */
    case OP_VARIABLE: // 变量（var）
        fprintf(ic_out,"v%d ",op->u.var_no);
        break;
    case OP_CONSTANT: // 常量（#1）
        fprintf(ic_out,"#%d ",op->u.const_val);
        break;
    case OP_LABEL: // 标签(LABEL label1:)
        fprintf(ic_out,"label%d ",op->u.label_no);
        break;
    case OP_TEMP: // 临时变量（t1）
        fprintf(ic_out,"t%d ",op->u.temp_no);
        break;
    case OP_FUNCTION://函数（Function)
        fprintf(ic_out,"%s ",op->u.func_name);
        break;
    case OP_ARRAY://数组
        fprintf(ic_out,"v%d ",op->u.var_no);
        break;
    case OP_GET_POINT://指针操作*t
        fprintf(ic_out,"*");
        show_operand(op->op_addr,ic_out);
        break;
    case OP_GET_ADDR:
        fprintf(ic_out,"&");//取地址操作&t
        show_operand(op->u.op,ic_out);
        break;
    default:
        break;
    }
}
//将中间代码信息打印到文件中
void show_intercode(InterCode ic,FILE* ic_out){
    switch (ic->kind)
    {   
    case IC_LABEL:    // LABEL x :
        fprintf(ic_out, "LABEL ");
        show_operand(ic->u.op, ic_out);
        fprintf(ic_out, ": ");
        break;
    case IC_FUNCTION:     // FUNCTION f :
        fprintf(ic_out, "FUNCTION ");
        show_operand(ic->u.op, ic_out);
        fprintf(ic_out, ": ");
        break;
    case IC_ASSIGN:   // x := y
        show_operand(ic->u.assign.left, ic_out);
        fprintf(ic_out, ":= ");
        show_operand(ic->u.assign.right, ic_out);
        break;
    case IC_PLUS:      // x := y+z
        show_operand(ic->u.binop.result, ic_out);
        fprintf(ic_out, ":= ");
        show_operand(ic->u.binop.op1, ic_out);
        fprintf(ic_out, "+ ");
        show_operand(ic->u.binop.op2, ic_out);
        break;
    case IC_SUB:      // x := y-z
        show_operand(ic->u.binop.result, ic_out);
        fprintf(ic_out, ":= ");
        show_operand(ic->u.binop.op1, ic_out);
        fprintf(ic_out, "- ");
        show_operand(ic->u.binop.op2, ic_out);
        break;
    case IC_MUL:      // x := y*z
        show_operand(ic->u.binop.result, ic_out);
        fprintf(ic_out, ":= ");
        show_operand(ic->u.binop.op1, ic_out);
        fprintf(ic_out, "* ");
        show_operand(ic->u.binop.op2, ic_out);
        break;
    case IC_DIV:      // x := y/z
        show_operand(ic->u.binop.result, ic_out);
        fprintf(ic_out, ":= ");
        show_operand(ic->u.binop.op1, ic_out);
        fprintf(ic_out, "/ ");
        show_operand(ic->u.binop.op2, ic_out);
        break;
    case IC_GOTO:     // GOTO x
        fprintf(ic_out, "GOTO ");
        show_operand(ic->u.op, ic_out);
        break;
    case IC_IF_GOTO:  // IF x [relop] y GOTO z
        fprintf(ic_out, "IF ");
        show_operand(ic->u.ifgoto.op1, ic_out);
        fprintf(ic_out, "%s ", ic->u.ifgoto.relop);
        show_operand(ic->u.ifgoto.op2, ic_out);
        fprintf(ic_out, "GOTO ");
        show_operand(ic->u.ifgoto.result, ic_out);
        break;
    case IC_RETURN:   // RETURN x
        fprintf(ic_out, "RETURN ");
        show_operand(ic->u.op, ic_out);
        break;
    case IC_DEC:      // DEC x [size]
        fprintf(ic_out, "DEC ");
        show_operand(ic->u.dec.result, ic_out);
        fprintf(ic_out, "%d", ic->u.dec.size);
        break;
    case IC_ARG:      // ARG x
        fprintf(ic_out, "ARG ");
        show_operand(ic->u.op, ic_out);
        break;
    case IC_CALL:     // x := CALL f
        show_operand(ic->u.assign.left, ic_out);
        fprintf(ic_out, ":= CALL ");
        show_operand(ic->u.assign.right, ic_out);
        break;
    case IC_PARAM:    // PARAM x
        fprintf(ic_out, "PARAM ");
        show_operand(ic->u.op, ic_out);
        break;
    case IC_READ:     // READ x
        fprintf(ic_out, "READ ");
        show_operand(ic->u.op, ic_out);
        break;
    case IC_WRITE:    // WRITE x   
        fprintf(ic_out, "WRITE ");
        show_operand(ic->u.op, ic_out);
        break;
    default:
        break;
    }
    fprintf(ic_out, "\n");
}
//逐步打印中间代码
void show_codelist(FILE* ic_out){
    assert(intercodes != NULL);
    assert(intercodes->next != NULL);
    CodeList cur = intercodes->next;
    while(cur!= intercodes){
        assert(cur != NULL);
        show_intercode(cur->code,ic_out);
        cur = cur->next;
    }
}

//struct错误处理，即代码中不应出现struct
void struct_error_handler() {
    printf("Cannot translate: Code contains variables or parameters of structure type.\n");
    exit(-1);
}
//中间代码生成分析
void Intercode_analysis(struct Node* root){
    //首先判断AST是否为空;
    //接着判断AST结构是否正确（满足C—文法要求）;
    if(root==NULL)return ;
    if(strcmp(root->name, "Program")!= 0 ) return;
    if(struct_is_defined == 1)struct_error_handler();
    //初始化全局变量中间代码变量等列表;
    intercodes = (CodeList)malloc(sizeof(struct CodeList_));
    intercodes->code = NULL;
    intercodes->prev = intercodes;
    intercodes->next = intercodes;
    var_head = (Variable)malloc(sizeof(struct _Variable));
    var_tail = (Variable)malloc(sizeof(struct _Variable));
    //设定变量、标签、临时变量标号;
    var_num = 1; label_num = 1; temp_num = 1;
    translate_Program(root);
}

//开始遍历语法树节点
void translate_Program(struct Node* root){
    translate_ExtDefList(root->children[0]);
}

    // ExtDefList → ExtDef ExtDefList
    // |e 
void translate_ExtDefList(struct Node* root){
    if(root != NULL && root->child_num == 2){
        translate_ExtDef(root->children[0]);
        translate_ExtDefList(root->children[1]);
    }
}
    // ExtDef → Specifier ExtDecList SEMI
    // | Specifier SEMI
    // | Specifier FunDec CompSt
void translate_ExtDef(struct Node* root){
    //检查语法树是否为空;
    //检查是否为“；”;
    assert(root->child_num == 2 || root->child_num == 3);
    if(strcmp(root->children[1]->name, "SEMI")== 0 )
        return ;
    //检查是否有全局标识符定义;
    if(strcmp(root->children[1]->name, "ExtDecList")== 0 ) 
        return ;
    // 依次检查是否是函数定义;
    // 实验三不包含全局变量，结构体定义也已经在实验二中处理并添加到符号表中了
    if(strcmp(root->children[1]->name,"FunDec") == 0){
        translate_FunDec(root->children[1]);
        translate_CompSt(root->children[2]);
    // 合并函数声明和函数体
    } else{
        assert(0);
    }
}
    //VarDec → ID
    //| VarDec LB INT RB
Operand translate_VarDec(struct Node* root){
    if(root == NULL)return NULL;
    assert(root->child_num == 1 || root->child_num == 4);
    if(root->child_num == 1){
        Type type = type_get_this_table(root->children[0]->str_val);
        assert(type != NULL);
        if(type->kind == BASIC){
            Operand operand = new_var(root->children[0]->str_val);
            return operand;
        }
        if(type->kind == ARRAY){
            Operand arr_op = new_arr(root->children[0]->str_val);
            arr_op->type = type->u.array.elem;
            InterCode ic = new_InterCode(IC_DEC,arr_op,NULL,NULL,NULL);
            ic->u.dec.size = get_size(type);
            add_interCode(ic);
            add_interCode(new_InterCode(IC_ASSIGN,arr_op->op_addr,new_addr(arr_op),NULL,NULL));
            return arr_op;
        }
    }
    if(root->child_num == 4){
        return translate_VarDec(root->children[0]);
    }
    assert(0);
}

    //FunDec → ID LP VarList RP
    //| ID LP RP
void translate_FunDec(struct Node* root){  
    if (root == NULL) return;
    assert(root->child_num == 3 || root->child_num == 4);
    // 处理无参函数
    // 初始化函数信息，获取函数名
    char* func_name = root->children[0]->str_val;
    Operand op = new_func(func_name);
    add_interCode(new_InterCode(IC_FUNCTION,op,NULL,NULL,NULL));
    if(root->child_num == 4){// 处理有参函数
        Type type = type_get_this_table(func_name);
        FieldList params = type->u.function->param;
        int param_is_arr = 0;
        while (params!=NULL)
        {
         // 构造函数参数列表判断参数类型（选做内容中需要支持结构体和数组作为参数）
            Operand operand;
            if(params->type->kind==BASIC){ 
                operand = new_var(params->name);
                add_interCode(new_InterCode(IC_PARAM,operand,NULL,NULL,NULL));
            }
            if(params->type->kind == ARRAY){ // 处理数组或者结构体
                operand = new_arr(params->name);
                add_interCode(new_InterCode(IC_PARAM,operand->op_addr,NULL,NULL,NULL));
            }
            params = params->tail;
        }
        params = type->u.function->param;
        while(params != NULL){
            if(params->type->kind == ARRAY){
                Operand op = get_operand(params->name);
                add_interCode(new_InterCode(IC_ASSIGN,op,new_point(op->op_addr),NULL,NULL));
            }
            params = params->tail;
        }
    }
}

    //CompSt → LC DefList StmtList RC
void translate_CompSt(struct Node* root){
    if(root == NULL)
        return;
    assert(root->child_num == 4);
    translate_DefList(root->children[1]);
    translate_StmtList(root->children[2]);
};

    //StmtList → Stmt StmtList
    //| e
void translate_StmtList(struct Node* root){
    if(root == NULL)
        return;
    assert(root->child_num == 2);
    translate_Stmt(root->children[0]);
    translate_StmtList(root->children[1]);
}

void translate_Stmt(struct Node* root){
    if(root == NULL)return;
    assert(root->child_num == 1 || root->child_num == 2 || root->child_num == 3 || root->child_num == 4 || root->child_num == 5 || root->child_num == 7);
    if(strcmp(root->children[0]->name, "CompSt")==0) // 处理复合语句   CompSt
        translate_CompSt(root->children[0]);
    if(strcmp(root->children[0]->name, "Exp")==0) // 处理表达式 Exp SEMI
        translate_Exp(root->children[0], new_temp());
    if(strcmp(root->children[0]->name, "RETURN")==0){ // 处理返回语句 RETURN Exp SEMI
        Operand t1 = new_temp();
        translate_Exp(root->children[1],t1);
        add_interCode(new_InterCode(IC_RETURN,t1,NULL,NULL,NULL));
        // translate_Exp + ic
    }
    if(strcmp(root->children[0]->name,"IF")==0){ 
        if(root->child_num == 5){ // | IF LP Exp RP Stmt
            Operand label1 = new_label();
            Operand label2 = new_label();
            translate_Cond(root->children[2], label1, label2);
            add_interCode(new_InterCode(IC_LABEL,label1,NULL,NULL,NULL));
            translate_Stmt(root->children[4]);
            add_interCode(new_InterCode(IC_LABEL,label2,NULL,NULL,NULL));
            //translate_Cond + [LABEL label1] + translate_Stmt + [LABEL label2]
        }else{ // | IF LP Exp RP Stmt ELSE Stmt
            Operand label1 = new_label();
            Operand label2 = new_label();
            Operand label3 = new_label();
            translate_Cond(root->children[2], label1, label2);
            add_interCode(new_InterCode(IC_LABEL,label1,NULL,NULL,NULL));
            translate_Stmt(root->children[4]);
            add_interCode(new_InterCode(IC_GOTO,label3,NULL,NULL,NULL));      
            add_interCode(new_InterCode(IC_LABEL,label2,NULL,NULL,NULL));
            translate_Stmt(root->children[6]);
            add_interCode(new_InterCode(IC_LABEL,label3,NULL,NULL,NULL));
            //translate_Cond + [LABEL label1] + translate_Stmt + [GOTO label3] + [LABEL label2] + translate_Stmt + [LABEL label3]
        }
    }
    if(strcmp(root->children[0]->name,"WHILE")==0){ // while语句处理   WHILE LP Exp RP Stmt
        Operand label1 = new_label();
        Operand label2 = new_label();
        Operand label3 = new_label();
        add_interCode(new_InterCode(IC_LABEL,label1,NULL,NULL,NULL));       
        translate_Cond(root->children[2], label2, label3);       
        add_interCode(new_InterCode(IC_LABEL,label2,NULL,NULL,NULL));
        translate_Stmt(root->children[4]);
        add_interCode(new_InterCode(IC_GOTO,label1,NULL,NULL,NULL));
        add_interCode(new_InterCode(IC_LABEL,label3,NULL,NULL,NULL));
        // [LABEL label1] + translate_Cond + [LABEL label2] + translate_Cond + [GOTO label1] + [LABEL label3]
    }
    return;
}

// DefList → Def DefList
// | e
void translate_DefList(struct Node* root){
    if(root == NULL)return;
    assert(root->child_num == 2);
    translate_Def(root->children[0]);
    translate_DefList(root->children[1]);
}

    //Def → Specifier DecList SEMI
void translate_Def(struct Node* root){
    if(root == NULL)return;
    assert(root->child_num == 3);
    translate_DecList(root->children[1]);
}

//  DecList → Dec
//  | Dec COMMA DecList
void translate_DecList(struct Node* root){
    if(root == NULL)return;
    assert(root->child_num == 1 || root->child_num == 3);
    translate_Dec(root->children[0]);
    if(root->child_num == 3)
        translate_DecList(root->children[2]);
}

    // Dec -> VarDec: 给出FieldList里面的内容,给出数组或者ID
	//| VarDec ASSIGNOP Exp/
void translate_Dec(struct Node* root){
    if(root == NULL)return;
    assert(root->child_num == 1 || root->child_num == 3);
    Operand var = translate_VarDec(root->children[0]);
    if(root->child_num == 3){
        Operand temp = new_temp();
        translate_Exp(root->children[2],temp);
        add_interCode(new_InterCode(IC_ASSIGN,var,temp,NULL,NULL));
    }
}


void translate_Exp(struct Node* root, Operand place){ // place表示值
    if (root == NULL) return;
    assert(root->child_num == 1 || root->child_num == 2 || root->child_num == 3 || root->child_num == 4);
    if(strcmp(root->children[0]->name,"INT")==0){ //INT
        // place := #value
        int val = root->children[0]->int_val;
        place->kind = OP_CONSTANT;
        place->u.const_val = val;
    }
    if(strcmp(root->children[0]->name,"ID")==0){
        if(root->child_num == 1){// 处理标识符 ID
            char* name = root->children[0]->str_val;
            Type type = type_get_this_table(name); // 查找标识符
            Operand op = get_operand(name);
            if(type->kind==ARRAY){ // 数组or结构体返回地址 wait
                place->kind = op->kind;
                place->u = op->u;
                place->type = type->u.array.elem;
                place->op_addr = op->op_addr;
            }
            if(type->kind == BASIC){ // 一般变量直接赋值              
                place->kind = OP_VARIABLE;
                place->u.var_no = op->u.var_no;
            }
        }
        if(root->child_num == 3){//ID LP RP
            Type type = type_get_this_table(root->children[0]->str_val);
            if(strcmp(type->u.function->name,"read") == 0 )
                add_interCode(new_InterCode(IC_READ,place,NULL,NULL,NULL));
            else{
                Operand op = new_func(type->u.function->name);
                add_interCode(new_InterCode(IC_CALL,place,op,NULL,NULL));
            }
        }
        if(root->child_num == 4){//	| ID LP Args RP
            Type type = type_get_this_table(root->children[0]->str_val);
            if(strcmp(type->u.function->name,"write") == 0){
                translate_Args(root->children[2],true);
                place->kind = OP_CONSTANT;
                place->u.const_val = 0;
            }
            else{
                translate_Args(root->children[2],false);
                Operand op = new_func(type->u.function->name);
                add_interCode(new_InterCode(IC_CALL,place,op,NULL,NULL)); 
            }
        }
    }
    if(strcmp(root->children[0]->name,"LP")==0){ //LP Exp RP
        translate_Exp(root->children[1],place);
    }
    if(strcmp(root->children[0]->name,"Exp") == 0){
        if(strcmp(root->children[1]->name,"ASSIGNOP") == 0){// Exp ASSIGNOP Exp
            Operand left = new_temp();
            translate_Exp(root->children[0],left);
            Operand right = new_temp();
            translate_Exp(root->children[2],right);
            if(left->kind != OP_ARRAY){
                add_interCode(new_InterCode(IC_ASSIGN,left,right,NULL,NULL));
                place->kind = right->kind;
                place->u = right->u;
            }
            else{ // 直接将右侧数组第一个地址赋给左侧
                Operand temp = get_operand(root->children[0]->children[0]->str_val);
                temp->op_addr = right->op_addr;
                place->kind = right->kind;
                place->u = right->u;
            }
        }
        if(strcmp(root->children[1]->name,"PLUS") == 0|| strcmp(root->children[1]->name,"MINUS") == 0 // 	| Exp PLUS Exp| Exp MINUS Exp | Exp STAR Exp | Exp DIV Exp
        || strcmp(root->children[1]->name,"STAR") == 0|| strcmp(root->children[1]->name,"DIV") == 0){
            Operand t1 = new_temp();
            Operand t2 = new_temp();
            translate_Exp(root->children[0],t1);
            translate_Exp(root->children[2],t2);
            int kind = -1;
            if(strcmp(root->children[1]->name,"PLUS") == 0)
                kind = IC_PLUS;
            if(strcmp(root->children[1]->name,"MINUS") == 0)
                kind = IC_SUB;
            if(strcmp(root->children[1]->name,"STAR") == 0)
                kind = IC_MUL;
            if(strcmp(root->children[1]->name,"DIV") == 0)
                kind = IC_DIV;
            add_interCode(new_InterCode(kind,place,t1,t2,NULL));
        }
        if(strcmp(root->children[1]->name,"LB") == 0){ //| Exp LB Exp RB
            Operand tmp1 = new_temp();
            Operand tmp2 = new_temp();
            Operand tmp3 = new_temp();
            translate_Exp(root->children[0],tmp1);
            translate_Exp(root->children[2],tmp2);
            int size = get_size(tmp1->type);
            Operand base = new_constant(size);
            add_interCode(new_InterCode(IC_MUL,tmp3,tmp2,base,NULL));
            add_interCode(new_InterCode(IC_PLUS,tmp3,tmp1->op_addr,tmp3,NULL));
            place->kind = OP_GET_POINT;
            place->u.op = tmp3;
            place->type = tmp1->type->u.array.elem;
            place->op_addr = tmp3;
        }

    }
    if(strcmp(root->children[0]->name,"MINUS") == 0){ //MINUS Exp
        Operand t1 = new_temp();
        translate_Exp(root->children[1], t1);
        Operand op = new_constant(0);
        add_interCode(new_InterCode(IC_SUB,place,op,t1,NULL));
        // translate_Exp + [place := #0 - t1]
    }
    if (root->child_num >= 2 && (
        strcmp(root->children[0]->name, "NOT") == 0 ||    //NOT Exp | Exp RELOP Exp | Exp AND Exp | Exp OR Exp
        strcmp(root->children[1]->name, "RELOP") == 0 ||
        strcmp(root->children[1]->name, "AND") == 0 ||
        strcmp(root->children[1]->name, "OR") == 0)){
            Operand label1 = new_label();
            Operand label2 = new_label();
            //[place := #0]
            add_interCode(new_InterCode(IC_ASSIGN,place,new_constant(0),NULL,NULL));
            translate_Cond(root, label1, label2);
            //[LABEL label1] + [place := #1]
            add_interCode(new_InterCode(IC_LABEL,label1,NULL,NULL,NULL));
            add_interCode(new_InterCode(IC_ASSIGN,place,new_constant(1),NULL,NULL));
            //[LABEL label2]
            add_interCode(new_InterCode(IC_LABEL,label2,NULL,NULL,NULL));
            // [place := #0] + translate_Cond + [LABEL label1] + [place := #1] + [LABEL label2]
    }
}

// Args → Exp COMMA Args
// | Exp
void translate_Args(struct Node* root, int write_func){
    if (root == NULL) return;
    assert(root->child_num == 1 || root->child_num == 3);
    if(root->child_num == 3)
        translate_Args(root->children[2],write_func);
    Operand t1 = new_temp();
    translate_Exp(root->children[0],t1);
    if(write_func == true)//写函数
        add_interCode(new_InterCode(IC_WRITE,t1,NULL,NULL,NULL));
    else{
        if(t1->kind != OP_ARRAY)
            add_interCode(new_InterCode(IC_ARG,t1,NULL,NULL,NULL));
        else
            add_interCode(new_InterCode(IC_ARG,t1->op_addr,NULL,NULL,NULL));
    }
}

void translate_Cond(struct Node* root,Operand label_true, Operand label_false){
    if (root == NULL) return;
    assert(root->child_num == 1 || root->child_num == 2 || root->child_num == 3 || root->child_num == 4);
    if(root->child_num == 2 && strcmp(root->children[0]->name,"NOT") == 0) //NOT Exp
        translate_Cond(root->children[1],label_false,label_true);
    else if(root->child_num == 3 && strcmp(root->children[1]->name,"RELOP") == 0){ //Exp1 RELOP Exp
        Operand t1 = new_temp();
        Operand t2 = new_temp();
        translate_Exp(root->children[0],t1);
        translate_Exp(root->children[2],t2);
        char* relop = root->children[1]->str_val;
        add_interCode(new_InterCode(IC_IF_GOTO,t1,t2,label_true,relop));
        add_interCode(new_InterCode(IC_GOTO,label_false,NULL,NULL,NULL));
        // translate_Exp + translate_Exp + [IF t1 op t2 GOTO label_true] + [GOTO label_false]
    }
    else if(root->child_num == 3 && strcmp(root->children[1]->name,"AND") == 0){ //Exp AND Exp
        Operand label1 = new_label();
        translate_Cond(root->children[0], label1, label_false);
        add_interCode(new_InterCode(IC_LABEL,label1,NULL,NULL,NULL));
        translate_Cond(root->children[2], label_true, label_false);
        // code1 + [LABEL label1] + code2
    }
    else if(root->child_num == 3 && strcmp(root->children[1]->name,"OR") == 0){ //Exp OR Exp
        Operand label1 = new_label();
        translate_Cond(root->children[0], label_true, label1);
        add_interCode(new_InterCode(IC_LABEL,label1,NULL,NULL,NULL));
        translate_Cond(root->children[2], label_true, label_false);
        //code1 + [LABEL label1] + code2
    }
    else{
        Operand t1 = new_temp();
        translate_Exp(root,t1);
        add_interCode(new_InterCode(IC_IF_GOTO,t1,new_constant(0),label_true,"!="));
        add_interCode(new_InterCode(IC_GOTO,label_false,NULL,NULL,NULL));
        //translate_Exp +  [IF t1 != #0 GOTO label_true] + [GOTO label_false]
    }
}

