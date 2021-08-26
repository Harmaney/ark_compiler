#include "dispatcher_vis.h"

std::any VisDispatcher::gen_global(GlobalAST *ast) {
    for (auto var : ast->vars) {
        var->accept(this);
    }
    for (auto func : ast->functions) {
        func->accept(this);
    }
    ast->mainBlock->accept(this);
    return nullptr;
}

std::any VisDispatcher::gen_array_type_decl(ArrayTypeDeclAST *ast) {
    ast->itemAST->accept(this);
    ast->rangeL->accept(this);
    ast->rangeR->accept(this);
    return nullptr;
}
std::any VisDispatcher::gen_basic_type(BasicTypeAST *ast) {
    logger->raw(ast->varType);
    return nullptr;
}
std::any VisDispatcher::gen_pointer_type_decl(PointerTypeDeclAST *ast) {
    ast->ref->accept(this);
    return nullptr;
}
std::any VisDispatcher::gen_type_def(TypeDefAST *ast) {
    ast->newName->accept(this);
    ast->oldName->accept(this);
    return nullptr;
}

std::any VisDispatcher::gen_number_expr(NumberExprAST *ast) {
    logger->raw("#", ast->const_type);
    return nullptr;
}
std::any VisDispatcher::gen_string_expr(StringExprAST *ast) {
    logger->raw(ast->val);
    return nullptr;
}
std::any VisDispatcher::gen_char_expr(CharExprAST *ast) {
    logger->raw("#", ast->val);
    return nullptr;
}
std::any VisDispatcher::gen_variable_expr(VariableExprAST *ast) {
    return nullptr;
}
std::any VisDispatcher::gen_return(ReturnAST *ast) { return nullptr; }
std::any VisDispatcher::gen_unary_expr(UnaryExprAST *ast) { return nullptr; }
std::any VisDispatcher::gen_binary_expr(BinaryExprAST *ast) { return nullptr; }
std::any VisDispatcher::gen_call_expr(CallExprAST *ast) { return nullptr; }

std::any VisDispatcher::gen_if_statement(IfStatementAST *ast) {
    return nullptr;
}
std::any VisDispatcher::gen_while_statement(WhileStatementAST *ast) {
    return nullptr;
}
std::any VisDispatcher::gen_for_statement(ForStatementAST *ast) {
    return nullptr;
}

std::any VisDispatcher::gen_function(FunctionAST *ast) { return nullptr; }
std::any VisDispatcher::gen_function_signature(FunctionSignatureAST *ast) {
    return nullptr;
}

std::any VisDispatcher::gen_struct(StructDeclAST *ast) { return nullptr; }

std::any VisDispatcher::gen_block(BlockAST *ast) { return nullptr; }

std::any VisDispatcher::gen_variable_decl(VariableDeclAST *ast) {
    return nullptr;
}