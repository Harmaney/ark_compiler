#pragma once
#include <ostream>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

#include "data.h"

class SymbolDescriptor;

class BlockAST;
class NumberExprAST;
class StringExprAST;
class CharExprAST;

class ArrayTypeDeclAST;
class PointerTypeDeclAST;
class BasicTypeAST;
class TypeDefAST;

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

class ADispatcher {
   public:
    virtual void gen_global_begin(GlobalAST *ast) = 0;
    virtual void gen_global_end(GlobalAST *ast) = 0;

    virtual void gen_array_type_decl(ArrayTypeDeclAST *ast) = 0;
    virtual void gen_basic_type(BasicTypeAST *ast) = 0;
    virtual void gen_pointer_type_decl(PointerTypeDeclAST *ast) = 0;
    virtual void gen_type_def(TypeDefAST *ast) = 0;

    virtual void gen_number_expr(NumberExprAST *ast) = 0;
    virtual void gen_string_expr(StringExprAST *ast) = 0;
    virtual void gen_char_expr(CharExprAST *ast) = 0;
    virtual void gen_variable_expr(VariableExprAST *ast) = 0;
    virtual void gen_return(ReturnAST *ast) = 0;
    virtual void gen_unary_expr(UnaryExprAST *ast) = 0;
    virtual void gen_binary_expr(BinaryExprAST *ast) = 0;
    virtual void gen_call_expr(CallExprAST *ast) = 0;

    /// 只生产if，不包含block
    virtual void gen_if_statement_begin(IfStatementAST *ast) = 0;
    virtual void gen_if_statement_else(IfStatementAST *ast) = 0;
    virtual void gen_if_statement_end(IfStatementAST *ast) = 0;
    /// 只生产while，不包含block
    virtual void gen_while_statement_begin(WhileStatementAST *ast) = 0;
    virtual void gen_while_statement_end(WhileStatementAST *ast) = 0;
    /// 只生产for，不包含block
    virtual void gen_for_statement_begin(ForStatementAST *ast) = 0;
    virtual void gen_for_statement_end(ForStatementAST *ast) = 0;

    virtual void gen_function(FunctionAST *ast) = 0;
    virtual void gen_function_signature(FunctionSignatureAST *ast) = 0;

    virtual void gen_struct(StructDeclAST *ast) = 0;

    virtual void gen_block_begin(BlockAST *ast) = 0;
    virtual void gen_block_end(BlockAST *ast) = 0;

    virtual void gen_variable_decl(VariableDeclAST *ast) = 0;
};

/// 输出代码的 visitor
///
/// 访问的规则**不在此定义**，而是在 data 里。
class ASTDispatcher : public ADispatcher {
   public:
    void gen_global_begin(GlobalAST *ast) override;
    void gen_global_end(GlobalAST *ast) override;

    void gen_array_type_decl(ArrayTypeDeclAST *ast) override;
    void gen_basic_type(BasicTypeAST *ast) override;
    void gen_pointer_type_decl(PointerTypeDeclAST *ast) override;
    void gen_type_def(TypeDefAST *ast) override;

    void gen_number_expr(NumberExprAST *ast) override;
    void gen_string_expr(StringExprAST *ast) override;
    void gen_char_expr(CharExprAST *ast) override;
    void gen_variable_expr(VariableExprAST *ast) override;
    void gen_return(ReturnAST *ast) override;
    void gen_unary_expr(UnaryExprAST *ast) override;
    void gen_binary_expr(BinaryExprAST *ast) override;
    void gen_call_expr(CallExprAST *ast) override;

    /// 只生产if，不包含block
    void gen_if_statement_begin(IfStatementAST *ast) override;
    void gen_if_statement_else(IfStatementAST *ast) override;
    void gen_if_statement_end(IfStatementAST *ast) override;
    /// 只生产while，不包含block
    void gen_while_statement_begin(WhileStatementAST *ast) override;
    void gen_while_statement_end(WhileStatementAST *ast) override;
    /// 只生产for，不包含block
    void gen_for_statement_begin(ForStatementAST *ast) override;
    void gen_for_statement_end(ForStatementAST *ast) override;

    void gen_function(FunctionAST *ast) override;
    void gen_function_signature(FunctionSignatureAST *ast) override;

    void gen_struct(StructDeclAST *ast) override;

    void gen_block_begin(BlockAST *ast) override;
    void gen_block_end(BlockAST *ast) override;

    void gen_variable_decl(VariableDeclAST *ast) override;
};

enum PlaceHolder { PLACE_VOID, PLACE_BEGIN = 1, PLACE_END = 2 };

/// 直接存储代码
///
/// 看起来，至少在直接翻译到 C
/// 的情况下，并不会出现“回填”的情况，所以完全可以这么搞。
class CodeCollector {
   private:
    static std::vector<std::string> section_order;
    static std::map<std::string, std::vector<std::string> *> codes;

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
    static void end_section(PlaceHolder place = PLACE_END);

    /// 重排段序列
    static void rearrange_section(std::vector<std::string> order);
    static void rearrange_section(std::string section, int newPos);
    /// 清空段
    static void clear();
    /// 输出代码
    static void output(std::ostream &out);
    static void output();
};

/////////////////////////////////////

class VisualDispatcher : public ADispatcher {
    std::ostream &log;

   public:
    VisualDispatcher(std::ostream &log) : log(log) {}
    void gen_global_begin(GlobalAST *ast) override;
    void gen_global_end(GlobalAST *ast) override;

    void gen_array_type_decl(ArrayTypeDeclAST *ast) override;
    void gen_basic_type(BasicTypeAST *ast) override;
    void gen_pointer_type_decl(PointerTypeDeclAST *ast) override;

    void gen_number_expr(NumberExprAST *ast) override;
    void gen_string_expr(StringExprAST *ast) override;
    void gen_variable_expr(VariableExprAST *ast) override;
    void gen_return(ReturnAST *ast) override;
    void gen_unary_expr(UnaryExprAST *ast) override;
    void gen_binary_expr(BinaryExprAST *ast) override;
    void gen_call_expr(CallExprAST *ast) override;

    /// 只生产if，不包含block
    void gen_if_statement_begin(IfStatementAST *ast) override;
    void gen_if_statement_end(IfStatementAST *ast) override;
    /// 只生产while，不包含block
    void gen_while_statement_begin(WhileStatementAST *ast) override;
    void gen_while_statement_end(WhileStatementAST *ast) override;
    /// 只生产for，不包含block
    void gen_for_statement_begin(ForStatementAST *ast) override;
    void gen_for_statement_end(ForStatementAST *ast) override;

    void gen_function(FunctionAST *ast) override;
    void gen_function_signature(FunctionSignatureAST *ast) override;

    void gen_struct(StructDeclAST *ast) override;

    void gen_block_begin(BlockAST *ast) override;
    void gen_block_end(BlockAST *ast) override;

    void gen_variable_decl(VariableDeclAST *ast) override;
};
