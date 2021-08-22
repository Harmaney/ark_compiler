#include <sstream>
#include <string>
#include <vector>
#include <stack>

#include "data.h"
#include "symbols.h"

#ifndef _CPP_CCVM
#define _CPP_CCVM

std::string get_internal_function_name(std::string name,
                                       std::vector<SymbolDescriptor*> args);


class _SymbolTable {
    int next_slot;
    std::map<std::string, SymbolDescriptor*> refType;
    std::map<std::string, VariableDescriptor*> refVar;

    std::map<std::pair<SymbolDescriptor*, int>, std::string> hasArrayType;
    std::map<SymbolDescriptor*, std::string> hasPointerType;

   public:
    int group;
    _SymbolTable* parent;
    _SymbolTable(int group) : group(group), next_slot(0), parent(NULL) {}
    std::string get_slot();
    void insert_variable(std::string sig, VariableDescriptor* var);
    void insert_type(std::string sig, SymbolDescriptor* var);
    ArrayTypeDescriptor* create_array_type(SymbolDescriptor* item, int sz,
                                           int beg);
    PointerTypeDescriptor* create_pointer_type(SymbolDescriptor* item);
    VariableDescriptor* search_variable(std::string sig);
    SymbolDescriptor* search_type(std::string sig);
};

class SymbolTable {
   private:
     _SymbolTable* root;
     _SymbolTable* current;

   public:
     void init();
     void enter();
     void exit();
     VariableDescriptor* create_variable(std::string sig,
                                               SymbolDescriptor* type,
                                               bool isRef, bool isConst);
     VariableDescriptor* create_variable_G(std::string sig,
                                                 SymbolDescriptor* type,
                                                 bool isRef);
     VariableDescriptor* create_variable(SymbolDescriptor* type,
                                               bool isRef, bool isConst);
     VariableDescriptor* create_variable_G(SymbolDescriptor* type,
                                                 bool isRef);

     VariableDescriptor* lookfor_variable(std::string sig);

     SymbolDescriptor* insert_type(std::string sig, SymbolDescriptor* descriptor);
     void insert_function(std::string sig,
                                FunctionDescriptor* descriptor);
     ArrayTypeDescriptor* create_array_type(SymbolDescriptor* item,
                                                  int sz, int beg);
     PointerTypeDescriptor* create_pointer_type(SymbolDescriptor* item);
     SymbolDescriptor* lookfor_type(std::string sig);
     FunctionDescriptor* lookfor_function(
        std::string sig, std::vector<SymbolDescriptor*> args);
};

class VMAbstract {
   public:
    virtual std::string output() = 0;
};

class VMString : public VMAbstract {
    std::string str;
    bool newLine;

   public:
    VMString(std::string str,bool newLine=false) : str(str),newLine(newLine) {}
    std::string output() override { return str; }
};

class VMWhiteBlock : public VMAbstract {
   protected:
    std::vector<VMAbstract *> children;

   public:
    void push_back(VMAbstract *vm) { children.push_back(vm); }
    std::string output() override {
        std::stringstream ss;
        for (auto child : children) {
            ss << child->output() << std::endl;
        }
        return ss.str();
    }

    void clear() { children.clear(); }
};
class VMBlock : public VMWhiteBlock {
   public:
    std::string output() override {
        std::stringstream ss;
        ss << "{" << std::endl;
        for (auto child : children) {
            ss << child->output() << std::endl;
        }
        ss << "}" << std::endl;

        return ss.str();
    }
};

/// 直接存储代码
///
/// 看起来，至少在直接翻译到 C
/// 的情况下，并不会出现“回填”的情况，所以完全可以这么搞。
class CodeCollector {
   private:
    int next_slot;
    VMWhiteBlock *root;
    std::stack<VMWhiteBlock*> cur;

   public:
    void push_block(VMWhiteBlock *block);
    void pop_block();
    /// 将代码压入段尾
    void push_back(VMAbstract *vm);

    std::string create_tag_G();

    VMWhiteBlock* createWhiteBlock();
    VMBlock * createBlock();

    void createTypeDecl(ArrayTypeDescriptor *arrayTypeDescriptor);
    void createTypeDecl(PointerTypeDescriptor *pointTypeDescriptor);
    void createTypeDecl(StructDescriptor *structDescriptor);
    // finally there's a day i will remove this f**king stupid function
    void createAkaType(std::string newName,SymbolDescriptor *typeDescriptor);
    void createVariableDecl(Value *value);
    
    void createCondBr(Value *cond,VMWhiteBlock *ok, VMWhiteBlock *no);

    void createLoop(Value *cond,VMWhiteBlock *init,VMWhiteBlock *calCond,VMWhiteBlock *step,VMWhiteBlock *body);

    void createConstDecl(VariableDescriptor *var,int value);
    void createConstDecl(VariableDescriptor *var,double value);
    void createConstDecl(VariableDescriptor *var,char value);

    void createFunctionCall(VariableDescriptor *var, FunctionDescriptor *function, std::vector<VariableDescriptor*> args);

    void createReturn();
    void createReturn(VariableDescriptor *t);

    void createVarAssign(VariableDescriptor *lhs,VariableDescriptor *rhs,bool fetchRef=false);
    void createArrayAssign(Value *var,Value *array,Value *index,bool fetchRef=false);
    void createStructAssign(Value *var,Value *struct1,std::string index,bool fetchRef=false);

    void createOptBinary(VariableDescriptor *res,VariableDescriptor *a,VariableDescriptor *b,std::string opt);

    /// 清空段
    void clear();
    /// 输出代码
    void output(std::ostream &out);
    void output();
};

class VMContext {
   public:
    CodeCollector *code_collector;
    SymbolTable *symbalTable;

    VMContext(){
        this->code_collector=new CodeCollector();
        this->symbalTable=new SymbolTable();
    }
};
#endif