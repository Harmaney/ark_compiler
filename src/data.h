#pragma once
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
    AST(ASTKind type) : type(type) {}
    virtual void accept(ASTDispatcher &dispatcher) = 0;
};

/// Pascal 的 Block 看起来并不是 Expr
class BlockAST : public AST {
   public:
    std::vector<AST *> exprs;
    BlockAST() : AST(BLOCK) {}
    void accept(ASTDispatcher &dispatcher) override;
};

/// Expr，具有返回值的表达式
class ExprAST : public AST {
   public:
    void *value;
    ExprAST(ASTKind type) : AST(type) {}
    virtual ~ExprAST() {}
};

class NumberExprAST : public ExprAST {
   public:
    double val;
    NumberExprAST(double val) : ExprAST(NUMBER_EXPR), val(val) {}
    void accept(ASTDispatcher &dispacher) override;
};

/// VariableExprAST - Expression class for referencing a variable, like "a".
class VariableExprAST : public ExprAST {
   public:
    std::string name;
    VariableExprAST(const std::string &name)
        : ExprAST(VARIABLE_EXPR), name(name) {}
    void accept(ASTDispatcher &dispacher) override;
};

/// BinaryExprAST - Expression class for a binary operator.
class BinaryExprAST : public ExprAST {
   public:
    ExprAST *LHS, *RHS;
    char op;
    BinaryExprAST(char op, ExprAST *lhs, ExprAST *rhs)
        : ExprAST(BINARY_EXPR), op(op), LHS(lhs), RHS(rhs) {}
    void accept(ASTDispatcher &dispacher) override;
};

/// CallExprAST - Expression class for function calls.
class CallExprAST : public ExprAST {
   public:
    std::string callee;
    std::vector<ExprAST *> args;
    CallExprAST(const std::string &callee, std::vector<ExprAST *> &args)
        : ExprAST(CALL_EXPR), callee(callee), args(args) {}
    void accept(ASTDispatcher &dispacher) override;
};

class VariableDeclAST : public AST {
   public:
    VariableExprAST *sig;
    VariableType type;
    VariableDeclAST(VariableExprAST *sig, VariableType type)
        : AST(VARIABLE_DECL), sig(sig), type(type) {}
    void accept(ASTDispatcher &dispatcher) override;
};

class FunctionSignatureAST : public AST {
   public:
    std::string sig;
    std::vector<VariableDeclAST *> args;
    VariableType result;
    FunctionSignatureAST(std::string sig, std::vector<VariableDeclAST *> args,
                         VariableType result)
        : AST(FUNCTION_SIGNATURE), sig(sig), args(args), result(result) {}
    void accept(ASTDispatcher &dispatcher) override;
};

class FunctionAST : public AST {
   public:
    FunctionSignatureAST *sig;
    BlockAST *body;
    FunctionAST(FunctionSignatureAST *sig, BlockAST *body)
        : AST(FUNCTION), sig(sig), body(body) {}
    void accept(ASTDispatcher &dispatcher) override;
};

class GlobalAST : public AST {
   public:
    std::vector<FunctionAST *> functions;
    GlobalAST(std::vector<FunctionAST *> functions)
        : AST(GLOBAL), functions(functions) {}
    void accept(ASTDispatcher &dispatcher) override;
};

////////////////////////////////////////////

struct Variable {
    std::string sig;
    VariableType type;
    bool isConst;
};

class _SymbolTable {
    int nextSlot;
    std::map<std::string, Variable *> refVar;

   public:
    _SymbolTable *parent;
    _SymbolTable();
    std::string getSlot();
    void insert(std::string sig, Variable *var);
    Variable *search(std::string sig);
};

class SymbolTable {
   private:
    static _SymbolTable *root;
    static _SymbolTable *current;

   public:
    static void init();
    static void enter();
    static void exit();
    static Variable *createVariable(std::string sig, VariableType type);
    static Variable *createVariableG(std::string sig, VariableType type);
    static Variable *createVariable(VariableType type);
    static Variable *createVariableG(VariableType type);

    static Variable *lookfor(std::string sig);
};