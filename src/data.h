#pragma once
#include <any>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "const.h"
#include "gen.h"
#include "logger.h"

class ASTDispatcher;

////////////////////////////////////////

/// 符号定义
class SymbolDescriptor {
   public:
    DescriptorType type;
    std::string name;
    SymbolDescriptor(DescriptorType type, std::string name)
        : type(type), name(name) {}
};

/// 类型定义。因为编译到C++，所以类型定义中并不需要携带任何关于大小和长度一类的信息
class TypeDescriptor : public SymbolDescriptor {
   public:
    TypeDescriptor(std::string name)
        : SymbolDescriptor(DESCRIPTOR_TYPE, name) {}
};

/// 指针定义
///
/// 一定注意，此处的指针定义是和Pascal中的指针对应的。变量中的ref标记，指的是某种pascal的写法，使得对于该变量的访问成为了引用格式的，翻译到代码时对应到C++指针。
class PointerTypeDescriptor : public SymbolDescriptor {
   public:
    SymbolDescriptor* ref;
    PointerTypeDescriptor(std::string name, SymbolDescriptor* ref)
        : SymbolDescriptor(DESCRIPTOR_POINTER_TYPE, name), ref(ref) {}
};

/// 数组定义
class ArrayTypeDescriptor : public SymbolDescriptor {
   public:
    int sz;
    int beg;
    SymbolDescriptor* itemDescriptor;
    ArrayTypeDescriptor(std::string name, SymbolDescriptor* itemDescriptor,
                        int sz, int beg)
        : SymbolDescriptor(DESCRIPTOR_ARRAY, name),
          itemDescriptor(itemDescriptor),
          sz(sz),
          beg(beg) {}
};

/// 变量定义
class VariableDescriptor : public SymbolDescriptor {
   public:
    SymbolDescriptor* varType;
    // FIX: i dont know if it is proper.
    bool isRef;
    // 现在没有用上
    bool isConst;
    bool isLeftVar;

    VariableDescriptor(std::string name, SymbolDescriptor* varType, bool isRef,
                       bool isConst, bool isLeftVar = true)
        : SymbolDescriptor(DESCRIPTOR_VARIABLE, name),
          varType(varType),
          isRef(isRef),
          isConst(isConst),
          isLeftVar(isLeftVar) {}
};

class StructDescriptor : public SymbolDescriptor {
   public:
    std::map<std::string, SymbolDescriptor*> refVar;

    StructDescriptor(std::string name,
                     std::map<std::string, SymbolDescriptor*> refVar)
        : SymbolDescriptor(DESCRIPTOR_STRUCT, name), refVar(refVar) {}

    void push(std::string sig, SymbolDescriptor* varDescriptor) {
        refVar[sig] = varDescriptor;
    }
};

class FunctionDescriptor : public SymbolDescriptor {
   public:
    std::vector<VariableDescriptor*> args;
    SymbolDescriptor* resultDescriptor;
    FunctionDescriptor(std::string name, std::vector<VariableDescriptor*> args,
                       SymbolDescriptor* resultDescriptor)
        : SymbolDescriptor(DESCRIPTOR_FUNCTION, name),
          args(args),
          resultDescriptor(resultDescriptor) {}
};

std::string get_internal_function_name(std::string name,
                                       std::vector<SymbolDescriptor*> args);

////////////////////////////////////////

class AST {
   public:
    ASTKind type;
    std::map<std::string, std::any> extraData;
    AST(ASTKind type) : type(type) {}
    virtual void accept(ASTDispatcher& dispatcher) = 0;
    void set_row(int row) { extraData["row"] = row; }
    void assign(AST* other) { extraData["row"] = other->extraData["row"]; }
    int get_row() {
        if (extraData.count("row")) return std::any_cast<int>(extraData["row"]);
        return 0;
    }
    void assign(std::any other);
};

/// Pascal 的 Block 看起来并不是 Expr
class BlockAST : public AST {
   public:
    std::vector<AST*> exprs;
    BlockAST(const std::vector<AST*> exprs) : AST(AST_BLOCK), exprs(exprs) {
        parser_info.push_back({{"ID", (uint64_t)this},
                               {"type", "BlockAST"},
                               {"son", Serialize(exprs)}});
    }
    void accept(ASTDispatcher& dispatcher) override;
};

/// Expr，具有返回值的表达式
class ExprAST : public AST {
   public:
    std::any value;
    ExprAST(ASTKind type) : AST(type) {}
    virtual ~ExprAST() {}
    void accept(ASTDispatcher& dispatcher) override;
};

/// 类型定义AST的抽象类
class TypeDeclAST : public AST {
   public:
    SymbolDescriptor* _descriptor;
    TypeDeclAST(ASTKind kind) : AST(kind) {}
};

/// 基本类型。比如 int
class BasicTypeAST : public TypeDeclAST {
   public:
    std::string varType;
    BasicTypeAST(std::string varType)
        : TypeDeclAST(AST_BASIC_TYPE), varType(varType) {
        parser_info.push_back(
            {{"ID", (uint64_t)this}, {"type", "BasicTypeAST:" + varType}});
    }
    void accept(ASTDispatcher& dispatcher) override;
};

class TypeDefAST : public TypeDeclAST {
   public:
    BasicTypeAST* newName;
    TypeDeclAST* oldName;
    TypeDefAST(BasicTypeAST* newName, TypeDeclAST* oldName)
        : TypeDeclAST(AST_TYPE_DEF), newName(newName), oldName(oldName) {}
    void accept(ASTDispatcher& dispatcher) override;
};

/// 指针类型
class PointerTypeDeclAST : public TypeDeclAST {
   public:
    // do not support multiple pointer and pointer of array, i'm tired.
    BasicTypeAST* ref;
    PointerTypeDeclAST(BasicTypeAST* ref)
        : TypeDeclAST(AST_POINTER_TYPE), ref(ref) {
        parser_info.push_back({{"ID", (uint64_t)this},
                               {"type", "PointerTypeDeclAST"},
                               {"son", Serialize(ref)}});
    }
    void accept(ASTDispatcher& dispatcher) override;
};

/// 数字常量
class NumberExprAST : public ExprAST {
   public:
    double val_float;
    int val_int;
    ConstantType const_type;
    NumberExprAST(double val)
        : ExprAST(AST_NUMBER_EXPR), val_float(val), const_type(CONSTANT_REAL) {
        parser_info.push_back(
            {{"ID", (uint64_t)this},
             {"type", "NumberExprAST:" + std::to_string(val_float)}});
    }
    NumberExprAST(int val)
        : ExprAST(AST_NUMBER_EXPR), val_int(val), const_type(CONSTANT_INT) {
        parser_info.push_back(
            {{"ID", (uint64_t)this},
             {"type", "NumberExprAST:" + std::to_string(val_int)}});
    }

    void accept(ASTDispatcher& dispacher) override;
};

/// 数组类型。可以继续在它下面挂数组，但是我不知道翻译会不会出问题
class ArrayTypeDeclAST : public TypeDeclAST {
   public:
    // does not support pointer
    TypeDeclAST* itemAST;
    NumberExprAST *rangeL, *rangeR;
    ArrayTypeDeclAST(TypeDeclAST* itemAST, NumberExprAST* rangeL,
                     NumberExprAST* rangeR)
        : TypeDeclAST(AST_ARRAY_TYPE),
          itemAST(itemAST),
          rangeL(rangeL),
          rangeR(rangeR) {
        parser_info.push_back({{"ID", (uint64_t)this},
                               {"type", "ArrayTypeDeclAST"},
                               {"son", Serialize(itemAST, rangeL, rangeR)}});
    }
    void accept(ASTDispatcher& dispatcher) override;
};

/// 字符串常量
class StringExprAST : public ExprAST {
   public:
    std::string val;
    StringExprAST(std::string val) : ExprAST(AST_STRING_EXPR), val(val) {
        parser_info.push_back(
            {{"ID", (uint64_t)this}, {"type", "StringExprAST:" + val}});
    }
    void accept(ASTDispatcher& dispacher) override;
};

/// 字符常量
class CharExprAST : public ExprAST {
   public:
    char val;
    CharExprAST(char val) : ExprAST(AST_CHAR_EXPR), val(val) {
        parser_info.push_back(
            {{"ID", (uint64_t)this}, {"type", "CharExprAST:" + val}});
    }
    void accept(ASTDispatcher& dispacher) override;
};

/// VariableExprAST - Expression class for referencing a variable, like "a".
class VariableExprAST : public ExprAST {
   public:
    std::string name;
    VariableExprAST(const std::string& name)
        : ExprAST(AST_VARIABLE_EXPR), name(name) {
        parser_info.push_back(
            {{"ID", (uint64_t)this}, {"type", "VariableExprAST:" + name}});
    }
    void accept(ASTDispatcher& dispacher) override;
};

/// 一元运算符
class UnaryExprAST : public ExprAST {
   public:
    ExprAST* expr;
    std::string op;
    UnaryExprAST(std::string op, ExprAST* expr)
        : ExprAST(AST_UNARY_EXPR), expr(expr), op(op) {
        parser_info.push_back({{"ID", (uint64_t)this},
                               {"type", "UnaryExprAST:" + op},
                               {"son", Serialize(expr)}});
    }
    void accept(ASTDispatcher& dispacher) override;
};

/// BinaryExprAST - Expression class for a binary operator.
class BinaryExprAST : public ExprAST {
   public:
    std::string op;
    ExprAST *LHS, *RHS;

    BinaryExprAST(std::string op, ExprAST* lhs, ExprAST* rhs)
        : ExprAST(AST_BINARY_EXPR), op(op), LHS(lhs), RHS(rhs) {
        parser_info.push_back({{"ID", (uint64_t)this},
                               {"type", "BinaryExprAST:" + op},
                               {"son", Serialize(LHS, RHS)}});
    }
    void accept(ASTDispatcher& dispacher) override;
};

/// 返回表达式
class ReturnAST : public AST {
   public:
    ExprAST* expr;
    ReturnAST(ExprAST* expr) : AST(AST_RETURN), expr(expr) {}
    ReturnAST() : AST(AST_RETURN), expr(nullptr) {}
    void accept(ASTDispatcher& dispacher) override;
};

/// CallExprAST - Expression class for function calls.
class CallExprAST : public ExprAST {
   public:
    std::string callee;
    std::vector<ExprAST*> args;
    CallExprAST(const std::string& callee, const std::vector<ExprAST*>& args)
        : ExprAST(AST_CALL_EXPR), callee(callee), args(args) {
        parser_info.push_back({{"ID", (uint64_t)this},
                               {"type", "CallExprAST:" + callee},
                               {"son", Serialize(args)}});
    }
    void accept(ASTDispatcher& dispacher) override;
};

/// 变量声明表达式
class VariableDeclAST : public AST {
   public:
    VariableExprAST* sig;
    TypeDeclAST* varType;
    SymbolDescriptor* _varType;
    // 这个东西的意义并不和指针等价！
    // 它只是在C中使用指针模拟，用于处理pascal里的引用类型
    bool isRef, isConst;

    NumberExprAST* initVal;

    VariableDeclAST(VariableExprAST* sig, TypeDeclAST* varType, bool isRef,
                    bool isConst = false, NumberExprAST* initVal = nullptr)
        : AST(AST_VARIABLE_DECL),
          sig(sig),
          varType(varType),
          isRef(isRef),
          isConst(isConst),
          initVal(initVal) {
        assert(varType);
        parser_info.push_back({{"ID", (uint64_t)this},
                               {"type", "VariableDeclAST"},
                               {"son", Serialize(sig, varType)}});
    }
    void accept(ASTDispatcher& dispatcher) override;
};

/// for表达式
class ForStatementAST : public AST {
   public:
    VariableExprAST* itervar;
    ExprAST *rangeL, *rangeR;
    BlockAST* body;

    ForStatementAST(VariableExprAST* itervar, ExprAST* rangeL, ExprAST* rangeR,
                    BlockAST* body)
        : AST(AST_FOR_STATEMENT),
          itervar(itervar),
          rangeL(rangeL),
          rangeR(rangeR),
          body(body) {
        parser_info.push_back(
            {{"ID", (uint64_t)this},
             {"type", "ForStatementAST"},
             {"son", Serialize(itervar, rangeL, rangeR, body)}});
    }
    void accept(ASTDispatcher& dispatcher) override;
};

class WhileStatementAST : public AST {
   public:
    ExprAST* condition;
    BlockAST* body;

    WhileStatementAST(ExprAST* condition, BlockAST* body)
        : AST(AST_WHILE_STATEMENT), condition(condition), body(body) {
        parser_info.push_back({{"ID", (uint64_t)this},
                               {"type", "WhileStatementAST"},
                               {"son", Serialize(condition, body)}});
    }
    void accept(ASTDispatcher& dispatcher) override;
};

class IfStatementAST : public AST {
   public:
    ExprAST* condition;
    BlockAST* body_true;
    BlockAST* body_false;

    IfStatementAST(ExprAST* condition, BlockAST* body_true,
                   BlockAST* body_false)
        : AST(AST_IF_STATEMENT),
          condition(condition),
          body_true(body_true),
          body_false(body_false) {
        parser_info.push_back(
            {{"ID", (uint64_t)this},
             {"type", "IfStatementAST"},
             {"son", Serialize(condition, body_true, body_false)}});
    }
    void accept(ASTDispatcher& dispatcher) override;
};

/// 函数的签名，指函数名和函数参数
class FunctionSignatureAST : public AST {
   public:
    std::string sig;
    std::vector<VariableDeclAST*> args;
    SymbolDescriptor* _resultType;
    TypeDeclAST* resultType;
    FunctionSignatureAST(std::string sig, std::vector<VariableDeclAST*> args,
                         TypeDeclAST* result)
        : AST(AST_FUNCTION_SIGNATURE),
          sig(sig),
          args(args),
          resultType(result) {
        parser_info.push_back({{"ID", (uint64_t)this},
                               {"type", "FunctionSignatureAST:" + sig},
                               {"son", Serialize(args, resultType)}});
    }
    void accept(ASTDispatcher& dispatcher) override;
};

class FunctionAST : public AST {
   public:
    FunctionSignatureAST* sig;
    std::vector<VariableDeclAST*> varDecls;
    BlockAST* body;
    FunctionAST(FunctionSignatureAST* sig,
                std::vector<VariableDeclAST*> varDecls, BlockAST* body)
        : AST(AST_FUNCTION), sig(sig), varDecls(varDecls), body(body) {
        parser_info.push_back({{"ID", (uint64_t)this},
                               {"type", "FunctionAST"},
                               {"son", Serialize(sig, varDecls, body)}});
    }
    void accept(ASTDispatcher& dispatcher) override;
};

class StructDeclAST : public TypeDeclAST {
   public:
    std::string sig;
    std::vector<VariableDeclAST*> varDecl;
    StructDeclAST(std::string sig, std::vector<VariableDeclAST*> varDecl)
        : TypeDeclAST(AST_STRUCT_DECL), sig(sig), varDecl(varDecl) {}
    void accept(ASTDispatcher& dispatcher) override;
};

/// 代表程序代码全局的AST
class GlobalAST : public AST {
   public:
    std::vector<VariableDeclAST*> vars;
    std::vector<TypeDefAST*> typeDefs;
    std::vector<FunctionAST*> functions;
    BlockAST* mainBlock;
    GlobalAST(std::vector<VariableDeclAST*> vars,
              std::vector<TypeDefAST*> typeDefs,
              std::vector<FunctionAST*> functions, BlockAST* mainBlock)
        : AST(AST_GLOBAL),
          typeDefs(typeDefs),
          vars(vars),
          functions(functions),
          mainBlock(mainBlock) {
        parser_info.push_back({{"ID", (uint64_t)this},
                               {"type", "Global"},
                               {"son", Serialize(vars, functions, mainBlock)}});
    }
    void accept(ASTDispatcher& dispatcher) override;
};

////////////////////////////////////////////

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
    static _SymbolTable* root;
    static _SymbolTable* current;

   public:
    static void init();
    static void enter();
    static void exit();
    static VariableDescriptor* create_variable(std::string sig,
                                               SymbolDescriptor* type,
                                               bool isRef, bool isConst);
    static VariableDescriptor* create_variable_G(std::string sig,
                                                 SymbolDescriptor* type,
                                                 bool isRef);
    static VariableDescriptor* create_variable(SymbolDescriptor* type,
                                               bool isRef, bool isConst);
    static VariableDescriptor* create_variable_G(SymbolDescriptor* type,
                                                 bool isRef);

    static VariableDescriptor* lookfor_variable(std::string sig);

    static void insert_type(std::string sig, SymbolDescriptor* descriptor);
    static void insert_function(std::string sig,
                                FunctionDescriptor* descriptor);
    static ArrayTypeDescriptor* create_array_type(SymbolDescriptor* item,
                                                  int sz, int beg);
    static PointerTypeDescriptor* create_pointer_type(SymbolDescriptor* item);
    static SymbolDescriptor* lookfor_type(std::string sig);
    static FunctionDescriptor* lookfor_function(
        std::string sig, std::vector<SymbolDescriptor*> args);
};

class TagTable {
    static int next_slot;

   public:
    static void init();
    static std::string* create_tag_G();
};

FunctionDescriptor* levelup_lookfor_function(
    std::string sig, std::vector<SymbolDescriptor*> args, int idx = 0);