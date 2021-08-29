#include "dispatcher_vis.h"

#include "templatelogger.hpp"

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
    LOG(ast->varType);
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
    LOG(ast->const_type);
    return nullptr;
}
std::any VisDispatcher::gen_string_expr(StringExprAST *ast) {
    LOG(ast->val);
    return nullptr;
}
std::any VisDispatcher::gen_char_expr(CharExprAST *ast) {
    LOG(ast->val);
    return nullptr;
}
std::any VisDispatcher::gen_variable_expr(VariableExprAST *ast) {
    LOG(ast->name);
    return nullptr;
}
std::any VisDispatcher::gen_return(ReturnAST *ast) {
    LOG("return");
    return nullptr;
}
std::any VisDispatcher::gen_unary_expr(UnaryExprAST *ast) {
    ast->expr->accept(this);
    LOG(ast->op);
    return nullptr;
}
std::any VisDispatcher::gen_binary_expr(BinaryExprAST *ast) {
    ast->LHS->accept(this);
    LOG(ast->op);
    ast->RHS->accept(this);
    return nullptr;
}
std::any VisDispatcher::gen_call_expr(CallExprAST *ast) {
    LOG(ast->callee);
    for (auto arg : ast->args) {
        arg->accept(this);
    }
    return nullptr;
}

std::any VisDispatcher::gen_if_statement(IfStatementAST *ast) {
    ast->condition->accept(this);
    ast->body_true->accept(this);
    ast->body_false->accept(this);
    return nullptr;
}
std::any VisDispatcher::gen_while_statement(WhileStatementAST *ast) {
    ast->condition->accept(this);
    ast->body->accept(this);
    return nullptr;
}
std::any VisDispatcher::gen_for_statement(ForStatementAST *ast) {
    ast->itervar->accept(this);
    ast->rangeL->accept(this);
    ast->rangeR->accept(this);
    ast->body->accept(this);
    return nullptr;
}

std::any VisDispatcher::gen_function(FunctionAST *ast) {
    ast->sig->accept(this);
    ast->body->accept(this);
    return nullptr;
}
std::any VisDispatcher::gen_function_signature(FunctionSignatureAST *ast) {
    LOG(ast->sig);
    for (auto arg : ast->args) {
        arg->accept(this);
    }
    return nullptr;
}

std::any VisDispatcher::gen_struct(StructDeclAST *ast) {
    LOG(ast->sig);
    for (auto arg : ast->varDecl) {
        arg->accept(this);
    }
    return nullptr;
}

std::any VisDispatcher::gen_block(BlockAST *ast) {
    for (auto expr : ast->exprs) {
        expr->accept(this);
    }
    return nullptr;
}

std::any VisDispatcher::gen_variable_decl(VariableDeclAST *ast) {
    ast->sig->accept(this);
    return nullptr;
}