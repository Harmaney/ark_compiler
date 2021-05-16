#pragma once
#include <sstream>
#include <string>
#include <ostream>
#include <stack>
#include <vector>

#include "data.h"

class SymbolDescriptor;

class BlockAST;
class NumberExprAST;
class StringExprAST;

class ArrayTypeDeclAST;
class PointerTypeDeclAST;
class BasicTypeAST;

class VariableExprAST;
class ReturnAST;
class UnaryExprAST;
class BinaryExprAST;
class CallExprAST;
class VariableDeclAST;

class IfStatementAST;
class WhileStatementAST;
class ForStatementAST;

class StructDeclAST;

class GlobalAST;
class FunctionSignatureAST;
class FunctionAST;


struct VariableDescriptor;

class ADispatcher{
public:
    virtual void genGlobalBegin(GlobalAST *ast)=0;
    virtual void genGlobalEnd(GlobalAST *ast)=0;

    virtual void genArrayTypeDecl(ArrayTypeDeclAST *ast)=0;
    virtual void genBasicType(BasicTypeAST *ast)=0;
    virtual void genPointerTypeDecl(PointerTypeDeclAST *ast)=0;

    virtual void genNumberExpr(NumberExprAST *ast)=0;
    virtual void genStringExpr(StringExprAST *ast)=0;
    virtual void genVariableExpr(VariableExprAST *ast)=0;
    virtual void genReturn(ReturnAST *ast)=0;
    virtual void genUnaryExpr(UnaryExprAST *ast)=0;
    virtual void genBinaryExpr(BinaryExprAST *ast)=0;
    virtual void genCallExpr(CallExprAST *ast)=0;

    /// 只生产if，不包含block
    virtual void genIfStatementBegin(IfStatementAST *ast)=0;
    virtual void genIfStatementElse(IfStatementAST *ast)=0;
    virtual void genIfStatementEnd(IfStatementAST *ast)=0;
    /// 只生产while，不包含block
    virtual void genWhileStatementBegin(WhileStatementAST *ast)=0;
    virtual void genWhileStatementEnd(WhileStatementAST *ast)=0;
    /// 只生产for，不包含block
    virtual void genForStatementBegin(ForStatementAST *ast)=0;
    virtual void genForStatementEnd(ForStatementAST *ast)=0;

    virtual void genFunction(FunctionAST *ast)=0;
    virtual void genFunctionSignature(FunctionSignatureAST *ast)=0;

    virtual void genStruct(StructDeclAST *ast)=0;

    virtual void genBlockBegin(BlockAST *ast)=0;
    virtual void genBlockEnd(BlockAST *ast)=0;

    virtual void genVariableDecl(VariableDeclAST *ast)=0;
};

/// 输出代码的 visitor
///
/// 访问的规则**不在此定义**，而是在 data 里。
class ASTDispatcher: public ADispatcher {
   public:
    void genGlobalBegin(GlobalAST *ast) override;
    void genGlobalEnd(GlobalAST *ast) override;

    void genArrayTypeDecl(ArrayTypeDeclAST *ast) override;
    void genBasicType(BasicTypeAST *ast) override;
    void genPointerTypeDecl(PointerTypeDeclAST *ast) override;

    void genNumberExpr(NumberExprAST *ast) override;
    void genStringExpr(StringExprAST *ast) override;
    void genVariableExpr(VariableExprAST *ast) override;
    void genReturn(ReturnAST *ast) override;
    void genUnaryExpr(UnaryExprAST *ast) override;
    void genBinaryExpr(BinaryExprAST *ast) override;
    void genCallExpr(CallExprAST *ast) override;

    /// 只生产if，不包含block
    void genIfStatementBegin(IfStatementAST *ast) override;
    void genIfStatementElse(IfStatementAST *ast) override;
    void genIfStatementEnd(IfStatementAST *ast) override;
    /// 只生产while，不包含block
    void genWhileStatementBegin(WhileStatementAST *ast) override;
    void genWhileStatementEnd(WhileStatementAST *ast) override;
    /// 只生产for，不包含block
    void genForStatementBegin(ForStatementAST *ast) override;
    void genForStatementEnd(ForStatementAST *ast) override;

    void genFunction(FunctionAST *ast) override;
    void genFunctionSignature(FunctionSignatureAST *ast) override;

    void genStruct(StructDeclAST *ast) override;

    void genBlockBegin(BlockAST *ast) override;
    void genBlockEnd(BlockAST *ast) override;

    void genVariableDecl(VariableDeclAST *ast) override;
};

enum PlaceHolder{
    PLACE_VOID,PLACE_BEGIN=1,PLACE_END=2
};

/// 直接存储代码
///
/// 看起来，至少在直接翻译到 C
/// 的情况下，并不会出现“回填”的情况，所以完全可以这么搞。
class CodeCollector {
   private:
    static std::vector<std::string> section_order;
    static std::map<std::string, std::vector<std::string>*> codes;

    static std::vector<std::string> *cur_section;
    static std::stack<std::string> cur_section_name;

    static std::stringstream ss;

   public:
   /// 开启新的代码段
    static void begin_section(std::string section_name = std::string());
    /// 当前行的生成
    static std::stringstream &src();
    /// 将代码压入段尾
    static void push_back(std::string str);
    /// 将代码压入段首
    static void push_front(std::string str);
    /// 将代码压入段尾
    static void push_back();
    /// 将代码压入段首
    static void push_front();
    /// 结束段并压入全局
    static void end_section(PlaceHolder place=PLACE_END);

    /// 重排段序列
    static void rearrange_section(std::vector<std::string> order);
    static void rearrange_section(std::string section,int newPos);
    /// 清空段
    static void clear();
    /// 输出代码
    static void output(std::ostream &out);
    static void output();
};

/////////////////////////////////////

class VisualDispatcher:public ADispatcher{
    std::ostream &log;
    public:
    VisualDispatcher(std::ostream &log):log(log){}
    void genGlobalBegin(GlobalAST *ast) override;
    void genGlobalEnd(GlobalAST *ast) override;

    void genArrayTypeDecl(ArrayTypeDeclAST *ast) override;
    void genBasicType(BasicTypeAST *ast) override;
    void genPointerTypeDecl(PointerTypeDeclAST *ast) override;

    void genNumberExpr(NumberExprAST *ast) override;
    void genStringExpr(StringExprAST *ast) override;
    void genVariableExpr(VariableExprAST *ast) override;
    void genReturn(ReturnAST *ast) override;
    void genUnaryExpr(UnaryExprAST *ast) override;
    void genBinaryExpr(BinaryExprAST *ast) override;
    void genCallExpr(CallExprAST *ast) override;

    /// 只生产if，不包含block
    void genIfStatementBegin(IfStatementAST *ast) override;
    void genIfStatementEnd(IfStatementAST *ast) override;
    /// 只生产while，不包含block
    void genWhileStatementBegin(WhileStatementAST *ast) override;
    void genWhileStatementEnd(WhileStatementAST *ast) override;
    /// 只生产for，不包含block
    void genForStatementBegin(ForStatementAST *ast) override;
    void genForStatementEnd(ForStatementAST *ast) override;

    void genFunction(FunctionAST *ast) override;
    void genFunctionSignature(FunctionSignatureAST *ast) override;

    void genStruct(StructDeclAST *ast) override;

    void genBlockBegin(BlockAST *ast) override;
    void genBlockEnd(BlockAST *ast) override;

    void genVariableDecl(VariableDeclAST *ast) override;
};
