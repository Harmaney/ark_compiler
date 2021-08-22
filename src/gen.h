#pragma once
#include <ostream>
#include <sstream>
#include <stack>
#include <string>
#include <vector>

#include "ccvm.h"
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
    virtual Value *gen_global(GlobalAST *ast) = 0;

    virtual Value *gen_array_type_decl(ArrayTypeDeclAST *ast) = 0;
    virtual Value *gen_basic_type(BasicTypeAST *ast) = 0;
    virtual Value *gen_pointer_type_decl(PointerTypeDeclAST *ast) = 0;
    virtual Value *gen_type_def(TypeDefAST *ast) = 0;

    virtual Value *gen_number_expr(NumberExprAST *ast) = 0;
    virtual Value *gen_string_expr(StringExprAST *ast) = 0;
    virtual Value *gen_char_expr(CharExprAST *ast) = 0;
    virtual Value *gen_variable_expr(VariableExprAST *ast) = 0;
    virtual Value *gen_return(ReturnAST *ast) = 0;
    virtual Value *gen_unary_expr(UnaryExprAST *ast) = 0;
    virtual Value *gen_binary_expr(BinaryExprAST *ast) = 0;
    virtual Value *gen_call_expr(CallExprAST *ast) = 0;

    virtual Value *gen_if_statement(IfStatementAST *ast) = 0;
    virtual Value *gen_while_statement(WhileStatementAST *ast) = 0;
    virtual Value *gen_for_statement(ForStatementAST *ast) = 0;

    virtual Value *gen_function(FunctionAST *ast) = 0;
    virtual Value *gen_function_signature(FunctionSignatureAST *ast) = 0;

    virtual Value *gen_struct(StructDeclAST *ast) = 0;

    virtual Value *gen_block(BlockAST *ast) = 0;

    virtual Value *gen_variable_decl(VariableDeclAST *ast) = 0;
};

/// 输出代码的 visitor
///
/// 访问的规则**不在此定义**，而是在 data 里。
class ASTDispatcher : public ADispatcher {
   private:
    VMContext *context;

    CodeCollector *code() { return context->code_collector; }
    SymbolTable *symbolTable() { return context->symbalTable; }
    TagTable *tagTable() { return context->tag_table; }

   public:
    ASTDispatcher(VMContext *context) : context(context) {}

    Value *gen_global(GlobalAST *ast) override;

    Value *gen_array_type_decl(ArrayTypeDeclAST *ast) override;
    Value *gen_basic_type(BasicTypeAST *ast) override;
    Value *gen_pointer_type_decl(PointerTypeDeclAST *ast) override;
    Value *gen_type_def(TypeDefAST *ast) override;

    Value *gen_number_expr(NumberExprAST *ast) override;
    Value *gen_string_expr(StringExprAST *ast) override;
    Value *gen_char_expr(CharExprAST *ast) override;
    Value *gen_variable_expr(VariableExprAST *ast) override;
    Value *gen_return(ReturnAST *ast) override;
    Value *gen_unary_expr(UnaryExprAST *ast) override;
    Value *gen_binary_expr(BinaryExprAST *ast) override;
    Value *gen_call_expr(CallExprAST *ast) override;

    Value *gen_if_statement(IfStatementAST *ast) override;
    Value *gen_while_statement(WhileStatementAST *ast) override;
    Value *gen_for_statement(ForStatementAST *ast) override;

    Value *gen_function(FunctionAST *ast) override;
    Value *gen_function_signature(FunctionSignatureAST *ast) override;

    Value *gen_struct(StructDeclAST *ast) override;

    Value *gen_block(BlockAST *ast) override;

    Value *gen_variable_decl(VariableDeclAST *ast) override;
};

enum PlaceHolder { PLACE_VOID, PLACE_BEGIN = 1, PLACE_END = 2 };