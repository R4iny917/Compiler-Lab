#include"semantic.h"
#define VAR 1
#define FIELD 2
//检查类型是否匹配
int type_check(Type t1, Type t2){
    if (t1 == t2)
        return 1;
    if (t1 == NULL && t2 == NULL)
        return 1; // 都为空一致
    if (t1->kind == BASIC){
        if (t1->u.basic == t2->u.basic)
            return 1; // 基本类型相同
        return 0; // 基本类型不同
    }
    if (t1->kind == ARRAY){ // 逐个比对数组元素类型
        if(t2->kind != ARRAY)
            return 0;
        else
            return type_check(t1->u.array.elem, t2->u.array.elem);
    }
    if (t1->kind == STRUCTURE){
        if (t2->kind != STRUCTURE )
            return 0;
        return t1->u.structure->name == t2->u.structure->name;
    }
    if(t1->kind == FUNCTION){
        if(t2->kind != FUNCTION)
            return 0;
        else{
            if(t1->u.function->para_num!=t2->u.function->para_num)
				return 0;
            FieldList a = t1->u.function->param;
            FieldList b = t2->u.function->param;
            return param_check(a,b);
        }
    }
    return 0;
}
//检查参数是否匹配
int param_check(FieldList p1,FieldList p2){
    while (p1 != NULL && p2 != NULL) {
        if (type_check(p1->type, p2->type) == 0)
            return 0; // 参数类型不一致
        p1 = p1->tail;
        p2 = p2->tail; // 获取下一个参数
    }
    if (p1 == NULL && p2 == NULL)
        return 1; // 检查结束，参数类型一致
    return 0; // 默认类型不一致
}

void print_func(char* name,int lineno,int paranum){
    printf("Function:%s at Line %d with %d params.\n",name,lineno,paranum);
}
//初始化哈希表
void init_hashtable(){
    for(int i = 0; i <= HashSize; ++i )
        hash_table_link->hashtable[i] = NULL;
}
//新建一个符号表
void new_hashtable(){
    hash_table_link->next = (HashTableLink)malloc(sizeof(struct HashTableLink_));
    hash_table_link->next->before = hash_table_link;
    hash_table_link = hash_table_link->next;
    init_hashtable();
}
//删除原先的栈顶符号表
void delete_hashtable(){
    hash_table_link = hash_table_link->before;
    hash_table_link->next = NULL;
    //free(hash_table_link->next);
}
//计算符号插入位置
unsigned int hash_pjw(char* name){
    unsigned int val = 0, i;
    for (; *name; name++) {
        val = (val << 2) + *name;
        if (i = val & ~HashSize) 
            val = (val ^ (i >> 12)) & HashSize;
    }
    assert(val <= HashSize);
    return val;
}

//展示符号内容，调试用
void show_hashnode(HashNode hash_node){
    printf("Node name: %s\n",hash_node->name);
    if(hash_node->type->kind == BASIC)
        if(hash_node->type->u.basic == 1)
            printf("Node type: INT\n");
        else  
            printf("Node type: FLOAT\n");
    else if(hash_node->type->kind == STRUCTURE){
        printf("Node type: STRUCTURE\n");
    }
    else if(hash_node->type->kind == STRUCTTAG){
        printf("Node type: STRUCTTAG\n");
    }
    else if(hash_node->type->kind == ARRAY)
        printf("Node type: ARRAY\n"); 
    else if(hash_node->type->kind == FUNCTION){
        printf("Node type: FUNCTION\n");
        printf("Node paraNums:%d\n",hash_node->type->u.function->para_num);
    }
    printf("\n");         
}
//插入哈希符号
void insert_HashNode(HashNode hash_node){
    //计算哈希值;
    unsigned int pos = hash_pjw(hash_node->name);
    if(hash_node->type->kind == FUNCTION){
        hash_node->link = hash_table_link->before->hashtable[pos];
        hash_table_link->before->hashtable[pos] = hash_node;
    }
    else{
        hash_node->link = hash_table_link->hashtable[pos];
        hash_table_link->hashtable[pos] = hash_node;
    }
    //show_hashnode(hash_node);
}
//获得栈顶符号表的变量类型
Type type_get_this_table(char* name){
    unsigned int pos = hash_pjw(name);
    HashNode hash_node = hash_table_link->hashtable[pos];
    while(hash_node != NULL){
        if(strcmp(hash_node->name,name) == 0)
            return hash_node->type;
        hash_node = hash_node->link;
    }
    return NULL;
}
//获得所有符号表中最靠近栈顶符号表的变量类型
Type type_get_all_table(char* name){
    unsigned int pos = hash_pjw(name);
    HashTableLink tmp = hash_table_link;
    HashNode hash_node = (HashNode)malloc(sizeof(struct HashNode_));
    while(tmp != NULL){
        hash_node = tmp->hashtable[pos];
        while(hash_node != NULL){
            if(strcmp(hash_node->name,name) == 0)
                return hash_node->type;
            hash_node = hash_node->link;
        }
        tmp = tmp->before;
    }
    return NULL;
}
//创建函数符号
HashNode create_hash_node_function(Function func ){
    HashNode hash_node = (HashNode)malloc(sizeof(struct HashNode_));
    hash_node->name = func->name;
    hash_node->type = (Type)malloc(sizeof(struct Type_));
    hash_node->type->kind = FUNCTION;
    hash_node->type->u.function = func;
    return hash_node;
}
//创建结构定义类型符号
HashNode create_hash_node_struct(char* name){
    HashNode hash_node = (HashNode)malloc(sizeof(struct HashNode_));
    hash_node->type = (Type)malloc(sizeof(struct Type_));
    hash_node->type->u.structure = (Structure)malloc(sizeof(struct Structure_));
    hash_node->type->kind = STRUCTTAG;
    hash_node->type->u.structure->name = name;
    hash_node->name = name;
    return hash_node;
}

HashNode create_hash_node_var(char* name, Type type){
    HashNode hash_node = (HashNode)malloc(sizeof(struct HashNode_));
    hash_node->name = name;
    hash_node->type = (Type)malloc(sizeof(struct Type_));
    hash_node->type = type;
    return hash_node;
}

Type set_basic_type(Type type,int basic_type){
    type->kind = BASIC;
    type->u.basic = basic_type;
    return type;
}

//语义分析器代码
void check_step(char* name, int lineno){
    printf("Now in :%s at Line %d.\n",name,lineno );

}

void semantic_check(struct Node* root){
//初始化工作，如全局变量初始化;
    hash_table_link = (HashTableLink)malloc(sizeof(struct HashTableLink_));
    init_hashtable();
    if(root == NULL)return;
    //check_step("Program",root->lineno);
// 遍历语法树中的ExtDefList节点
    ExtDefList(root->children[0]);
    //check_step("Over",root->lineno);
}

void ExtDefList(struct Node *root){
    // ExtDefList → ExtDef ExtDefList
    // |e 
    if(root != NULL && root->child_num == 2){
        //check_step("ExtDefList",root->lineno);
        ExtDef(root->children[0]);
        ExtDefList(root->children[1]);
    }
}

void ExtDef(struct Node* root){
    // ExtDef → Specifier ExtDecList SEMI
    // | Specifier SEMI
    // | Specifier FunDec CompSt
    // 判断节点是否为空;
    if(root == NULL)return;
    assert(root->child_num == 2 || root->child_num == 3);
    //check_step("ExtDef",root->lineno);
    // 获取node的specifier类型type;
    Type type = Specifier(root->children[0]);
    if(root->child_num == 3){
        if(strcmp(root->children[1]->name,"ExtDecList") == 0){// ExtDef → Specifier ExtDecList SEMI
            ExtDecList(root->children[1], type);
        }
        if(strcmp(root->children[1]->name,"FunDec") == 0){// | Specifier FunDec CompSt
            new_hashtable();
            Function func = FunDec(root->children[1], type);
            if(func != NULL){
                HashNode hash_node = create_hash_node_function(func);
                insert_HashNode(hash_node);
                CompSt(root->children[2],type);
            }
            delete_hashtable();
        }
    }
}

void ExtDecList(struct Node* root, Type type){
    /*ExtDecList -> VarDec
    | VarDec COMMA ExtDecList
    */
    if(root == NULL)return;
    assert(root->child_num == 1 || root->child_num == 3);
    //check_step("ExtDecList",root->lineno);
    if(root->child_num == 1){//ExtDecList -> VarDec
        VarDec(root->children[0],type,VAR);
    }
    if(root->child_num == 3){//VarDec COMMA ExtDecList
        VarDec(root->children[0],type,VAR);
        ExtDecList(root->children[2],type);
    }
}

//Specifiers
Type Specifier(struct Node* root){
    //Specifier → TYPE
    //| StructSpecifier
    //check_step("Specifier",root->lineno);
    Type type = (Type)malloc(sizeof(struct Type_));
    if(strcmp(root->children[0]->name,"TYPE")==0){
        type->kind = BASIC;
        //printf("type:%s\n",root->children[0]->str_val);
        if(strcmp(root->children[0]->str_val,"int") == 0)
            type->u.basic = 1;//int(1)
        if(strcmp(root->children[0]->str_val,"float") == 0)
            type->u.basic = 2;//float(1)
        return type;
    }
    if(strcmp(root->children[0]->name,"StructSpecifier")==0){
        return StructSpecifier(root->children[0]);
    }
}


Type StructSpecifier(struct Node *root){
    // StructSpecifier → STRUCT OptTag LC DefList RC
    // | STRUCT Tag
    if(root == NULL)return NULL;
    assert(root->child_num == 2 || root->child_num == 5);
    //check_step("StructSpecifer",root->lineno);
    Type type = (Type)malloc(sizeof(struct Type_));
    type->u.structure = (Structure)malloc(sizeof(struct Structure_));
    type->kind = STRUCTURE;
    FieldList field = (FieldList)malloc(sizeof(struct FieldList_));;
    if (root->child_num == 5) {
        char* opttag = OptTag(root->children[1]);
        opttag = (opttag == NULL)? " ":opttag;
        HashNode hash_node = create_hash_node_struct(opttag);
        if(type_get_all_table(opttag) != NULL && opttag != " "){
            printf("Error type 16 at Line %d: Duplicated name \"%s\".\n", root->lineno, opttag);
            return NULL;
        }
        hash_node->type->u.structure->domain = DefList(root->children[3],FIELD);
        insert_HashNode(hash_node); 
        type->u.structure->name = opttag;
        type->u.structure->domain = hash_node->type->u.structure->domain;          
    } else if (root->child_num == 2) { 
        char* tag = root->children[1]->children[0]->str_val;
        Type ret_type = type_get_all_table(tag);
        if (ret_type == NULL || ret_type->kind != STRUCTTAG) {
            printf("Error type 17 at Line %d: Undefined structure \"%s\".\n", root->children[1]->lineno, tag);
            return NULL;
        }
        type->u.structure = ret_type->u.structure;
    }
    return type;
}

char* OptTag(struct Node* root){
    if (root == NULL ) return NULL;
    // OptTag -> ID
    return root->children[0]->str_val;
}

// char* Tag(struct Node* root) {
//     if (root == NULL) return NULL;
//     // Tag -> ID
//     assert(root->child_num == 1);
//     return root->children[0]->str_val;
// }

//Declarators
FieldList VarDec(struct Node* root, Type type,int class){
    //VarDec → ID
    //| VarDec LB INT RB
    if(root == NULL)return NULL;
    assert(root->child_num == 1 || root->child_num == 4);
    //提取node的孩子节点n;
    //check_step("VarDec",root->lineno);
    struct Node* n = root->children[0];
    FieldList f = (FieldList)malloc(sizeof(struct FieldList_));    //构建FieldList变量 f;
    if (strcmp(n->name,"ID") == 0) {     //VarDec → ID 变量定义
        Type ret_type = type_get_this_table(n->str_val);
        Type ret_struct_type = type_get_all_table(n->str_val);
        if (class == VAR && (ret_type != NULL || (ret_struct_type != NULL && ret_struct_type->kind == STRUCTTAG))){
            printf("Error type 3 at Line %d: Redefined variable \"%s\".\n", root->lineno, root->children[0]->str_val);
            return NULL;
        }
        if (class == FIELD &&  (ret_type != NULL || (ret_struct_type != NULL && ret_struct_type->kind == STRUCTTAG))){  
            printf("Error type 15 at Line %d: Redefined field \"%s\".\n", root->lineno, root->children[0]->str_val);
            return NULL;
        }
        f->name = n->str_val;    //初始化f的名称、类型、tail域信息;
        f->type = type;
        f->tail = NULL;
        if(type != NULL){    //n的符号名及类型插入全局符号表中;
            HashNode hash_node = create_hash_node_var(n->str_val,type);
            insert_HashNode(hash_node);
        }
        return f;
    } 
    else { // 数组定义  VarDec LB INT RB
        Type arrType = (Type)malloc(sizeof(struct Type_));    //构建Type变量arrType;
        arrType->kind = ARRAY;    //初始化arrType的kind、u.array的size/elem类型;
        arrType->u.array.elem = type;
        arrType->u.array.size = root->children[2]->int_val;
        return VarDec(root->children[0],arrType,class);    //递归调用VarDec(n, arrType,class)返回FieldList变量f;
    }
}

Function FunDec(struct Node *root, Type type){
    //FunDec → ID LP VarList RP
    //| ID LP RP
    if (root == NULL) return NULL;
    assert(root->child_num == 3 || root->child_num == 4);
    //check_step("FunDec",root->lineno);
    struct Node* n = root->children[0];    //提取node的孩子节点n;
    Function func = (Function)malloc(sizeof(struct Function_));    //构建函数类型变量Fucntion func;
    func->name = n->str_val;    //初始化func的name、line、type（返回值类型）;
    func->line = n->lineno;
    func->para_num = 0;
    func->type = type;
    Type ret_type = type_get_all_table(n->str_val);
    if (root->child_num == 4) {
        func->param = VarList(root->children[2]);  //调用VarList遍历n的参数并返回FieldList变量 ,设置func的param域的值为FieldList变量;
        FieldList num_count = func->param;
        while(num_count != NULL){
            func->para_num++;
            num_count = num_count->tail;
        }
    } 
    else // 无参
        func->param = NULL;     //设置func的param域的值为NULL;
    if (ret_type != NULL && ret_type->kind == FUNCTION) {  //函数已存在符号表中
        printf("Error type 4 at Line %d: Redefined function \"%s\".\n", root->lineno, root->children[0]->str_val);
        return NULL;
    }
    return func;//返回func;
}

FieldList VarList(struct Node* root){
    // VarList → ParamDec COMMA VarList
    // | ParamDec
    if (root == NULL) return NULL;
    assert(root->child_num == 1 || root->child_num == 3);
    //check_step("VarList",root->lineno);
    FieldList field = ParamDec(root->children[0]);
    if(root->child_num == 3){
        if(field == NULL)
            field = VarList(root->children[2]);
        else
            field->tail = VarList(root->children[2]);
    }
    return field;
}

FieldList ParamDec(struct Node* root){
    //ParamDec → Specifier VarDec
    //check_step("ParamDec",root->lineno);
    Type type = Specifier(root->children[0]);
    return VarDec(root->children[1],type,FIELD);
}

//Statements
void CompSt(struct Node* root, Type type){
    //CompSt → LC DefList StmtList RC
    if(root == NULL)return ;
    assert(root->child_num == 4);
    //check_step("CompSt",root->lineno);
    DefList(root->children[1],VAR);
    StmtList(root->children[2],type);
}

void StmtList(struct Node* root, Type type){
    //StmtList → Stmt StmtList
    //| e
    if (root == NULL) return;
    assert(root->child_num == 2);
    //check_step("StmtList",root->lineno);
    Stmt(root->children[0], type);
    StmtList(root->children[1], type);
}

    // Stmt → Exp SEMI
    // | CompSt
    // | RETURN Exp SEMI
    // | IF LP Exp RP Stmt
    // | IF LP Exp RP Stmt ELSE Stmt
    // | WHILE LP Exp RP Stmt
void Stmt(struct Node* root, Type type){
    //判断节点是否为空;
    if (root == NULL) return;
    assert(root->child_num == 1 || root->child_num == 2 || root->child_num == 3 || root->child_num == 4 || root->child_num == 5 || root->child_num == 7);
    //check_step("Stmt",root->lineno);
    if (strcmp(root->children[0]->name,"Exp") == 0) { //Exp SEMI
        Exp(root->children[0]); 
    } else if (strcmp(root->children[0]->name,"CompSt") == 0) { //CompSt
        new_hashtable();//2.2附加内容，遇见CompSt在栈顶创建新符号表
        CompSt(root->children[0],type);
        delete_hashtable();//2.2附加内容，执行CompSt结束后在退栈
    } else if (strcmp(root->children[0]->name,"RETURN") == 0) { //RETURN Exp SEMI
        Type ret_type = Exp(root->children[1]);    // 返回值类型
        if(ret_type != NULL && type_check(type,ret_type) == 0){
            printf("Error type 8 at Line %d: Type mismatched for return.\n", root->lineno);
        }
    } else if (strcmp(root->children[0]->name,"WHILE") == 0) { // WHILE LP Exp RP Stmt
        Exp(root->children[2]);
        Stmt(root->children[4],type);
    }
    else if (strcmp(root->children[0]->name,"IF") == 0) { // IF LP Exp RP Stmt ELSE Stmt或IF LP Exp RP Stmt
        Exp(root->children[2]);
        Stmt(root->children[4],type);
        if(root->child_num == 7)
            Stmt(root->children[6],type);
    }
}

//Local Definitions
FieldList DefList(struct Node* root, int class){
// DefList → Def DefList
// | e
    if(root == NULL)return NULL;
    //check_step("DefList",root->lineno);
    FieldList field = Def(root->children[0],class);
    if(field == NULL)
        field = DefList(root->children[1],class);
    else{
        FieldList tmp = field;
        while(tmp->tail != NULL)
            tmp = tmp->tail;//移至fieldlist尾部
        tmp->tail = DefList(root->children[1],class);
    }
    return field;
}

FieldList Def(struct Node* root, int class){
    //Def → Specifier DecList SEMI
    //check_step("Def",root->lineno);
    Type type = Specifier(root->children[0]);
    FieldList field = DecList(root->children[1],type,class);
    return field;
}

FieldList DecList(struct Node* root, Type type, int class){
//  DecList → Dec
//  | Dec COMMA DecList
    //check_step("DecList",root->lineno);
    FieldList field = Dec(root->children[0],type,class);
    if(root->child_num == 3){
        if(field == NULL)
            field = DecList(root->children[2],type,class);
        else{
            FieldList tmp = field;
            while(tmp->tail != NULL)tmp = tmp->tail;
            tmp->tail = DecList(root->children[2],type,class);
        }
    }
    return field;
}

FieldList Dec(struct Node* root, Type type, int class){
    // Dec -> VarDec: 给出FieldList里面的内容,给出数组或者ID
	//| VarDec ASSIGNOP Exp/
    if(root == NULL)return NULL;
    //check_step("Dec",root->lineno);
    assert(root->child_num == 1 || root->child_num == 3);
    FieldList fieldlist = VarDec(root->children[0], type, class);
    if(fieldlist != NULL && root->child_num == 3){
        if (class == FIELD) {
            printf("Error type 15 at Line %d: Initialized field \"%s\".\n", root->lineno, fieldlist->name);
            return NULL;
        }
        if(class == VAR){
            Type exp_type = Exp(root->children[2]);
            if(exp_type != NULL && !type_check(type,exp_type)){
                printf("Error type 5 at Line %d: Type mismatched.\n", root->lineno);
                return NULL;
            }
        }
    }
   
    return fieldlist;
}

//Expressions
/*Exp -> Exp ASSIGNOP Exp
	| Exp AND Exp
	| Exp OR Exp
	| Exp RELOP Exp
 	| Exp PLUS Exp
	| Exp MINUS Exp
	| Exp STAR Exp
	| Exp DIV Exp
	| LP Exp RP
	| MINUS Exp
	| NOT Exp
	| ID LP Args RP
	| ID LP RP
	| Exp LB Exp RB
	| Exp DOT ID;
	| ID
	| INT
	| FLOAT
	*/
Type Exp(struct Node *root){
    if(root == NULL)return NULL;
    //提取node的孩子节点n;
    assert(root->child_num == 1 || root->child_num == 2 || root->child_num == 3 || root->child_num == 4);
    //check_step("Exp",root->lineno);
    struct Node* n = root->children[0];
    Type ret_type = (Type)malloc(sizeof(struct Type_));
    if(strcmp(n->name,"INT") == 0){//INT
        ret_type = set_basic_type(ret_type,1);
        return ret_type;
    }
    if(strcmp(n->name,"FLOAT") == 0){//FLOAT
        ret_type = set_basic_type(ret_type,2);
        return ret_type;
    }
    if(strcmp(n->name,"LP") == 0){
        return Exp(root->children[1]);
    } 
    if (strcmp(n->name,"ID") == 0) {
        if (root->child_num == 1){ //| ID
            ret_type = type_get_all_table(n->str_val);
            if ( ret_type == NULL){ // 变量不在符号表中
                printf("Error type 1 at Line %d: Undefined variable \"%s\".\n", root->lineno, root->children[0]->str_val);
                return NULL;
            }
            else
                return ret_type;
        }
        else{
            Type funcType = type_get_all_table(n->str_val);
            if (funcType == NULL){
                printf("Error type 2 at Line %d: Undefined function \"%s\".\n", root->lineno,  n->str_val);
                return NULL;
            }
            if (funcType->kind!= FUNCTION){
                printf("Error type 11 at Line %d: \"%s\" is not a function.\n", root->lineno, n->str_val);
                return NULL;
            }
            if(root->child_num == 4){//| ID LP Args RP
                FieldList param = Args(root->children[2]);
                if (!param_check(param, funcType->u.function->param)) {
                    printf("Error type 9 at Line %d: Function: %s arguments do not match.\n", root->lineno,  n->str_val);
                    return NULL; 
                }
            }
            if(root->child_num == 3){//| ID LP RP
                if(!param_check(NULL,funcType->u.function->param)){
                    printf("Error type 9 at Line %d: Function: %s arguments do not match.\n", root->lineno, n->str_val);
                    return NULL; 
                }              
            }
            return funcType->u.function->type; //类型为函数返回类型
        }
    }
    else if(strcmp(n->name,"Exp") == 0){
        if(strcmp(root->children[1]->name, "ASSIGNOP") == 0){//Exp ASSIGNOP Exp
            //获取赋值运算符左右操作数类型left, right;
            Type left = NULL;
            Type right = Exp(root->children[2]);
            if ((root->children[0]->child_num == 1 && strcmp(root->children[0]->children[0]->name, "ID") == 0) ||
				(root->children[0]->child_num == 4 && strcmp(root->children[0]->children[1]->name, "LB") == 0) ||
			    (root->children[0]->child_num == 3 && strcmp(root->children[0]->children[1]->name, "DOT") == 0))
				left = Exp(root->children[0]);
			else {
				printf("Error type 6 at Line %d: The left-hand side of an assignment must be a variable.\n", root->lineno);
                return NULL;
            }
            if (left != NULL && right != NULL &&!type_check(left, right)){
                printf("Error type 5 at Line %d: Type mismatched for assignment.\n", root->lineno);
                return NULL;
            }
            return left;
        }
        else if(strcmp(root->children[1]->name, "AND") == 0 || strcmp(root->children[1]->name, "OR") == 0       
              ||strcmp(root->children[1]->name, "RELOP") == 0 || strcmp(root->children[1]->name, "PLUS") == 0
              ||strcmp(root->children[1]->name, "MINUS") == 0 || strcmp(root->children[1]->name, "STAR") == 0
              ||strcmp(root->children[1]->name, "DIV") == 0 ){
                Type left = Exp(root->children[0]);
                Type right = Exp(root->children[2]);
                if (left == NULL || right == NULL)
                    return NULL;
                //操作数类型不匹配
                if (!type_check(left, right)) {
                    printf("Error type 7 at Line %d: Type mismatched for operands.\n", root->lineno);
				    return NULL;
                }
                if(strcmp(root->children[1]->name, "RELOP") == 0){
                    ret_type->kind = BASIC;
                    ret_type->u.basic = 1;
                    return ret_type;
                }
                if(strcmp(root->children[1]->name, "AND") == 0 || strcmp(root->children[1]->name, "OR") == 0){
                    if(left->kind != BASIC || left->u.basic != 1){
                        printf("Error type 7 at Line %d: Non-int type cannot perform logical operations.\n", root->lineno);
				        return NULL;                      
                    }
                    ret_type->kind = BASIC;
                    ret_type->u.basic = 1;
                    return ret_type;
                }
                return left;
            }
        else if(strcmp(root->children[1]->name, "LB") == 0) { //Exp LB Exp RB
            Type arr = Exp(n);
            if(arr != NULL){
                if (arr->kind != ARRAY) {
                    printf("Error type 10 at Line %d: Variable is not an array.\n", root->lineno);
                    return NULL;
                }
                Type index = Exp(root->children[2]);
                if (index == NULL || index->kind != BASIC || index->u.basic != 1) {
                    printf("Error type 12 at Line %d: Array index is not an integer.\n", root->lineno);
				    return NULL;
                }
                return arr->u.array.elem;
            }
            return arr;
        }
        else if(strcmp(root->children[1]->name, "DOT") == 0){
            Type left = Exp(n);
            if(left == NULL){}
            else if (left->kind != STRUCTURE){// 不是结构体类型
                printf("Error type 13 at Line %d: Illegal use of \".\".\n", root->lineno);
                return NULL;
            }
            else {
                FieldList head = left->u.structure->domain;
                Type ans = NULL;
                while (head != NULL) {
                    if (strcmp(root->children[2]->str_val, head->name) == 0) {
                        ans = head->type;
                        break;
                    }
                    head = head->tail;
                }
                if (ans == NULL) {  // 域名不存在
                    printf("Error type 14 at Line %d: None-existent field \"%s\".\n", root->lineno, root->children[2]->str_val);
                    return NULL;
                }
                return ans;
            }
            return left;
        }
    }
    else if (strcmp(root->children[0]->name, "MINUS") == 0) {
        Type res_type = Exp(root->children[1]);
        // 如果res为NULL应该是Exp有错，这里就不再报连锁错误
        if (res_type != NULL)
            if (res_type->kind != BASIC) {
                printf("Error type 7 at Line %d: Variable after MINUS is not Basic type.\n", root->lineno);
                return NULL;
            }
        return res_type;
    }
    else if (strcmp(root->children[0]->name, "NOT") == 0) {
        ret_type = Exp(root->children[1]);
        // 如果res为NULL应该是Exp有错，这里就不再报连锁错误
        if (ret_type != NULL)
            if (ret_type->kind != BASIC || ret_type->u.basic != 1) {
                printf("Error type 7 at Line %d: Variable after NOT is not INT type.\n", root->lineno);
                return NULL;
            }
        return ret_type;
    }
}

FieldList Args(struct Node* root){
// Args → Exp COMMA Args
// | Exp
    if (root == NULL) return NULL;
    //check_step("Args",root->lineno);
    assert(root->child_num == 1 || root->child_num == 3);
    FieldList field = (FieldList)malloc(sizeof(struct FieldList_));
    field->type = Exp(root->children[0]);
    if(root->child_num == 3)
        field->tail = Args(root->children[2]);
    return field;
}



