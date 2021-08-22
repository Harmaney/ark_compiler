#pragma once
#include <any>
#include <map>
#include <sstream>
#include <string>
#include <vector>

#include "const.h"
#include "gen.h"
#include "logger.h"
#include "symbols.h"


class ASTDispatcher;



////////////////////////////////////////

class AST {
   public:
    ASTKind type;
    std::map<std::string, std::any> extraData;
    AST(ASTKind type) : type(type) {}
    virtual std::any accept(ASTDispatcher& dispatcher) = 0;
};

/// Pascal 的 Block 看起来并不是 Expr
class BlockAST : public AST {
   public:
    std::vector<AST*> exprs;
    BlockAST(const std::vector<AST*> exprs) : AST(AST_BLOCK), exprs(exprs) {}
    std::any accept(ASTDispatcher& dispatcher) override;
};

/// Expr，具有返回值的表达式
class ExprAST : public AST {
   public:
    ExprAST(ASTKind type) : AST(type) {}
    virtual ~ExprAST() {}
    std::any accept(ASTDispatcher& dispatcher) override;
};

/// 类型定义AST的抽象类
class TypeDeclAST : public AST {
   public:
    TypeDeclAST(ASTKind kind) : AST(kind) {}
    SymbolDescriptor *_descriptor;
};

/// 基本类型。比如 int
class BasicTypeAST : public TypeDeclAST {
   public:
    std::string varType;
    BasicTypeAST(std::string varType)
        : TypeDeclAST(AST_BASIC_TYPE), varType(varType) {}
    std::any accept(ASTDispatcher& dispatcher) override;
};

class TypeDefAST : public TypeDeclAST {
   public:
    BasicTypeAST* newName;
    TypeDeclAST* oldName;
    TypeDefAST(BasicTypeAST* newName, TypeDeclAST* oldName)
        : TypeDeclAST(AST_TYPE_DEF), newName(newName), oldName(oldName) {}
    std::any accept(ASTDispatcher& dispatcher) override;
};

/// 指针类型
class PointerTypeDeclAST : public TypeDeclAST {
   public:
    // do not support multiple pointer and pointer of array, i'm tired.
    BasicTypeAST* ref;
    PointerTypeDeclAST(BasicTypeAST* ref)
        : TypeDeclAST(AST_POINTER_TYPE), ref(ref) {}
    std::any accept(ASTDispatcher& dispatcher) override;
};

/// 数字常量
class NumberExprAST : public ExprAST {
   public:
    double val_float;
    int val_int;
    ConstantType const_type;
    NumberExprAST(double val)
        : ExprAST(AST_NUMBER_EXPR), val_float(val), const_type(CONSTANT_REAL) {}
    NumberExprAST(int val)
        : ExprAST(AST_NUMBER_EXPR), val_int(val), const_type(CONSTANT_INT) {}

    std::any accept(ASTDispatcher& dispacher) override;
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
          rangeR(rangeR) {}
    std::any accept(ASTDispatcher& dispatcher) override;
};

/// 字符串常量
class StringExprAST : public ExprAST {
   public:
    std::string val;
    StringExprAST(std::string val) : ExprAST(AST_STRING_EXPR), val(val) {}
    std::any accept(ASTDispatcher& dispacher) override;
};

/// 字符常量
class CharExprAST : public ExprAST {
   public:
    char val;
    CharExprAST(char val) : ExprAST(AST_CHAR_EXPR), val(val) {}
    std::any accept(ASTDispatcher& dispacher) override;
};

/// VariableExprAST - Expression class for referencing a variable, like "a".
class VariableExprAST : public ExprAST {
   public:
    std::string name;
    VariableExprAST(const std::string& name)
        : ExprAST(AST_VARIABLE_EXPR), name(name) {}
    std::any accept(ASTDispatcher& dispacher) override;
};

/// 一元运算符
class UnaryExprAST : public ExprAST {
   public:
    ExprAST* expr;
    std::string op;
    UnaryExprAST(std::string op, ExprAST* expr)
        : ExprAST(AST_UNARY_EXPR), expr(expr), op(op) {}
    std::any accept(ASTDispatcher& dispacher) override;
};

/// BinaryExprAST - Expression class for a binary operator.
class BinaryExprAST : public ExprAST {
   public:
    std::string op;
    ExprAST *LHS, *RHS;

    BinaryExprAST(std::string op, ExprAST* lhs, ExprAST* rhs)
        : ExprAST(AST_BINARY_EXPR), op(op), LHS(lhs), RHS(rhs) {}
    std::any accept(ASTDispatcher& dispacher) override;
};

/// 返回表达式
class ReturnAST : public AST {
   public:
    ExprAST* expr;
    ReturnAST(ExprAST* expr) : AST(AST_RETURN), expr(expr) {}
    ReturnAST() : AST(AST_RETURN), expr(nullptr) {}
    std::any accept(ASTDispatcher& dispacher) override;
};

/// CallExprAST - Expression class for function calls.
class CallExprAST : public ExprAST {
   public:
    std::string callee;
    std::vector<ExprAST*> args;
    CallExprAST(const std::string& callee, const std::vector<ExprAST*>& args)
        : ExprAST(AST_CALL_EXPR), callee(callee), args(args) {}
    std::any accept(ASTDispatcher& dispacher) override;
};

/// 变量声明表达式
class VariableDeclAST : public AST {
   public:
    VariableExprAST* sig;
    TypeDeclAST* varType;
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
    }
    std::any accept(ASTDispatcher& dispatcher) override;
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
          body(body) {}
    std::any accept(ASTDispatcher& dispatcher) override;
};

class WhileStatementAST : public AST {
   public:
    ExprAST* condition;
    BlockAST* body;

    WhileStatementAST(ExprAST* condition, BlockAST* body)
        : AST(AST_WHILE_STATEMENT), condition(condition), body(body) {}
    std::any accept(ASTDispatcher& dispatcher) override;
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
          body_false(body_false) {}
    std::any accept(ASTDispatcher& dispatcher) override;
};

/// 函数的签名，指函数名和函数参数
class FunctionSignatureAST : public AST {
   public:
    std::string sig;
    std::vector<VariableDeclAST*> args;
    TypeDeclAST* resultType;
    FunctionSignatureAST(std::string sig, std::vector<VariableDeclAST*> args,
                         TypeDeclAST* result)
        : AST(AST_FUNCTION_SIGNATURE),
          sig(sig),
          args(args),
          resultType(result) {}
    std::any accept(ASTDispatcher& dispatcher) override;
};

class FunctionAST : public AST {
   public:
    FunctionSignatureAST* sig;
    std::vector<VariableDeclAST*> varDecls;
    BlockAST* body;
    FunctionAST(FunctionSignatureAST* sig,
                std::vector<VariableDeclAST*> varDecls, BlockAST* body)
        : AST(AST_FUNCTION), sig(sig), varDecls(varDecls), body(body) {}
    std::any accept(ASTDispatcher& dispatcher) override;
};

class StructDeclAST : public TypeDeclAST {
   public:
    std::string sig;
    std::vector<VariableDeclAST*> varDecl;
    StructDeclAST(std::string sig, std::vector<VariableDeclAST*> varDecl)
        : TypeDeclAST(AST_STRUCT_DECL), sig(sig), varDecl(varDecl) {}
    std::any accept(ASTDispatcher& dispatcher) override;
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
          mainBlock(mainBlock) {}
    std::any accept(ASTDispatcher& dispatcher) override;
};