#pragma once
#include <any>
#include <map>
#include <string>
#include <vector>

#include "const.h"
#include "gen.h"
#include "spdlog/spdlog.h"

class ASTDispatcher;

class AST {
   public:
    ASTKind type;
    std::map<std::string, std::any> extraData;
    AST(ASTKind type) : type(type) {}
    virtual void accept(ASTDispatcher &dispatcher) = 0;
};

/// Pascal 的 Block 看起来并不是 Expr
class BlockAST : public AST {
   public:
    std::vector<AST *> exprs;
    BlockAST() : AST(AST_BLOCK) {}
    void accept(ASTDispatcher &dispatcher) override;
};

/// Expr，具有返回值的表达式
class ExprAST : public AST {
   public:
    std::any value;
    ExprAST(ASTKind type) : AST(type) {}
    virtual ~ExprAST() {}
    void accept(ASTDispatcher &dispatcher) override;
};

class NumberExprAST : public ExprAST {
   public:
    double val_float;
    int val_int;
    ConstantType const_type;
    NumberExprAST(double val)
        : ExprAST(AST_NUMBER_EXPR), val_float(val), const_type(CONSTANT_REAL) {}
    NumberExprAST(int val)
        : ExprAST(AST_NUMBER_EXPR), val_int(val), const_type(CONSTANT_INT) {}

    void accept(ASTDispatcher &dispacher) override;
};

class StringExprAST : public ExprAST {
   public:
    std::string val;
    StringExprAST(std::string val) : ExprAST(AST_STRING_EXPR), val(val) {}
    void accept(ASTDispatcher &dispacher) override;
};

/// VariableExprAST - Expression class for referencing a variable, like "a".
class VariableExprAST : public ExprAST {
   public:
    std::string name;
    VariableExprAST(const std::string &name)
        : ExprAST(AST_VARIABLE_EXPR), name(name) {}
    void accept(ASTDispatcher &dispacher) override;
};

/// BinaryExprAST - Expression class for a binary operator.
class BinaryExprAST : public ExprAST {
   public:
    ExprAST *LHS, *RHS;
    char op;
    BinaryExprAST(char op, ExprAST *lhs, ExprAST *rhs)
        : ExprAST(AST_BINARY_EXPR), op(op), LHS(lhs), RHS(rhs) {}
    void accept(ASTDispatcher &dispacher) override;
};

/// CallExprAST - Expression class for function calls.
class CallExprAST : public ExprAST {
   public:
    std::string callee;
    std::vector<ExprAST *> args;
    CallExprAST(const std::string &callee, std::vector<ExprAST *> &args)
        : ExprAST(AST_CALL_EXPR), callee(callee), args(args) {}
    void accept(ASTDispatcher &dispacher) override;
};

class VariableDeclAST : public AST {
   public:
    VariableExprAST *sig;
    std::string varType;
    VariableDeclAST(VariableExprAST *sig, std::string type)
        : AST(AST_VARIABLE_DECL), sig(sig), varType(type) {}
    void accept(ASTDispatcher &dispatcher) override;
};

class ForStatementAST : public AST {
   public:
    VariableExprAST *itervar;
    NumberExprAST *rangeL, *rangeR;
    BlockAST *body;

    ForStatementAST(VariableExprAST *itervar, NumberExprAST *rangeL,
                    NumberExprAST *rangeR, BlockAST *body)
        : AST(AST_FOR_STATEMENT),
          itervar(itervar),
          rangeL(rangeL),
          rangeR(rangeR),
          body(body) {}
    void accept(ASTDispatcher &dispatcher) override;
};

class WhileStatementAST : public AST {
   public:
    ExprAST *condition;
    BlockAST *body;

    WhileStatementAST(ExprAST *condition, BlockAST *body)
        : AST(AST_WHILE_STATEMENT), condition(condition), body(body) {}
    void accept(ASTDispatcher &dispatcher) override;
};

class IfStatementAST : public AST {
   public:
    ExprAST *condition;
    BlockAST *body_true;
    BlockAST *body_false;

    IfStatementAST(ExprAST *condition, BlockAST *body_true,
                   BlockAST *body_false)
        : AST(AST_IF_STATEMENT),
          condition(condition),
          body_true(body_true),
          body_false(body_false) {}
    void accept(ASTDispatcher &dispatcher) override;
};

class FunctionSignatureAST : public AST {
   public:
    std::string sig;
    std::vector<VariableDeclAST *> args;
    std::string resultType;
    FunctionSignatureAST(std::string sig, std::vector<VariableDeclAST *> args,
                         std::string result)
        : AST(AST_FUNCTION_SIGNATURE),
          sig(sig),
          args(args),
          resultType(result) {}
    void accept(ASTDispatcher &dispatcher) override;
};

class FunctionAST : public AST {
   public:
    FunctionSignatureAST *sig;
    BlockAST *body;
    FunctionAST(FunctionSignatureAST *sig, BlockAST *body)
        : AST(AST_FUNCTION), sig(sig), body(body) {}
    void accept(ASTDispatcher &dispatcher) override;
};

class StructDeclAST : public AST {
   public:
    std::string sig;
    std::vector<VariableDeclAST *> varDecl;
    StructDeclAST(std::string sig, std::vector<VariableDeclAST *> varDecl)
        : AST(AST_STRUCT_DECL), sig(sig), varDecl(varDecl) {}
    void accept(ASTDispatcher &dispatcher) override;
};

class GlobalAST : public AST {
   public:
    std::vector<VariableDeclAST *> vars;
    std::vector<FunctionAST *> functions;
    BlockAST *mainBlock;
    GlobalAST(std::vector<VariableDeclAST *> vars,
              std::vector<FunctionAST *> functions, BlockAST *mainBlock)
        : AST(AST_GLOBAL),
          vars(vars),
          functions(functions),
          mainBlock(mainBlock) {}
    void accept(ASTDispatcher &dispatcher) override;
};

////////////////////////////////////////////

class TypeDescriptor {
   public:
    DescriptorType type;
    TypeDescriptor(DescriptorType type) : type(type) {}
};

class VariableDescriptor : public TypeDescriptor {
   public:
    std::string sig;
    std::string varType;
    bool isConst;

    VariableDescriptor(std::string sig, std::string varType, bool isConst)
        : TypeDescriptor(DESCRIPTOR_VARIABLE),
          sig(sig),
          varType(varType),
          isConst(isConst) {}
};

class StructDescriptor : public TypeDescriptor {
   public:
    std::map<std::string, TypeDescriptor *> refVar;

    StructDescriptor() : TypeDescriptor(DESCRIPTOR_STRUCT) {}

    void push(std::string sig, TypeDescriptor *varDescriptor) {
        refVar[sig] = varDescriptor;
    }
};

class _SymbolTable {
    int nextSlot;
    std::map<std::string, TypeDescriptor *> refType;
    std::map<std::string, VariableDescriptor *> refVar;

   public:
    _SymbolTable *parent;
    _SymbolTable();
    std::string getSlot();
    void insert(std::string sig, VariableDescriptor *var);
    void insert(std::string sig, TypeDescriptor *var);
    VariableDescriptor *searchVariable(std::string sig);
    TypeDescriptor *searchType(std::string sig);
};

class SymbolTable {
   private:
    static _SymbolTable *root;
    static _SymbolTable *current;

   public:
    static void init();
    static void enter();
    static void exit();
    static VariableDescriptor *createVariable(std::string sig,
                                              std::string type);
    static VariableDescriptor *createVariableG(std::string sig,
                                               std::string type);
    static VariableDescriptor *createVariable(std::string type);
    static VariableDescriptor *createVariableG(std::string type);

    static VariableDescriptor *lookforVariable(std::string sig);

    static void insertType(std::string sig, TypeDescriptor *descriptor);
    static TypeDescriptor *lookforType(std::string sig);
};

class TagTable {
    static int nextSlot;

   public:
    static void init();
    static std::string *createTagG();
};