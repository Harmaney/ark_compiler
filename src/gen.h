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
class BasicTypeAST;
class VariableExprAST;
class ReturnAST;
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

/// 输出代码的 visitor
///
/// 访问的规则**不在此定义**，而是在 data 里。
class ASTDispatcher {
   public:
    void genGlobalBegin(GlobalAST *ast);
    void genGlobalEnd(GlobalAST *ast);

    void genArrayTypeDecl(ArrayTypeDeclAST *ast);
    void genBasicType(BasicTypeAST *ast);

    void genNumberExpr(NumberExprAST *ast);
    void genStringExpr(StringExprAST *ast);
    void genVariableExpr(VariableExprAST *ast);
    void genReturn(ReturnAST *ast);
    void genBinaryExpr(BinaryExprAST *ast);
    void genCallExpr(CallExprAST *ast);

    /// 只生产if，不包含block
    void genIfStatementBegin(IfStatementAST *ast);
    void genIfStatementEnd(IfStatementAST *ast);
    /// 只生产while，不包含block
    void genWhileStatementBegin(WhileStatementAST *ast);
    void genWhileStatementEnd(WhileStatementAST *ast);
    /// 只生产for，不包含block
    void genForStatementBegin(ForStatementAST *ast);
    void genForStatementEnd(ForStatementAST *ast);

    void genFunctionSignature(FunctionSignatureAST *ast);

    void genStruct(StructDeclAST *ast);

    void genBlockBegin(BlockAST *ast);
    void genBlockEnd(BlockAST *ast);

    void genVariableDecl(VariableDeclAST *ast);
};

std::string mapVariableType(SymbolDescriptor *type);

void genVariable(VariableDescriptor *var);

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