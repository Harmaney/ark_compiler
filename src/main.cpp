#include <iostream>
#include <string>

#include "data.h"
#include "gen.h"
#include "spdlog/spdlog.h"
using namespace std;

void init_code_generator() {
    SymbolTable::init();
    TagTable::init();
}

void TEST_for(){
    BlockAST *env=new BlockAST();
    env->exprs.push_back(new VariableDeclAST(new VariableExprAST("i"),INT));

    BlockAST *for_block = new BlockAST();

    for_block->exprs.push_back(new VariableDeclAST(new VariableExprAST("domjudge"),REAL));
    ForStatementAST *for_ast=new ForStatementAST(new VariableExprAST("i"),1,5,for_block);

    env->exprs.push_back(for_ast);

    CodeCollector::begin_section();
    ASTDispatcher dispacher;
    env->accept(dispacher);
    CodeCollector::end_section(PLACE_END);
    CodeCollector::output();
}

void TEST_funccall() {
    BlockAST *block = new BlockAST();

    VariableDeclAST *decl = new VariableDeclAST(new VariableExprAST("a"), REAL);
    block->exprs.push_back(decl);

    BinaryExprAST *t1 = new BinaryExprAST('=', new VariableExprAST("a"),
                                          new NumberExprAST(123));
    block->exprs.push_back(t1);

    vector<ExprAST *> callargs;
    callargs.push_back(
        new BinaryExprAST('+', new NumberExprAST(1.1), new NumberExprAST(2.2)));
    callargs.push_back(new NumberExprAST(114.514));
    callargs.push_back(new VariableExprAST("a"));
    CallExprAST *call = new CallExprAST("write", callargs);
    block->exprs.push_back(call);

    FunctionSignatureAST *funcsig =
        new FunctionSignatureAST("main", vector<VariableDeclAST *>(), INT);
    FunctionAST *func = new FunctionAST(funcsig, block);

    GlobalAST *global = new GlobalAST({func});

    ASTDispatcher dispacher;
    CodeCollector::begin_section();
    global->accept(dispacher);
    CodeCollector::end_section(PLACE_END);

    CodeCollector::rearrange_section("global_define", 0);
    CodeCollector::rearrange_section("prelude", 0);
    CodeCollector::output();
}

int main(int argc, char **argv) {
    spdlog::set_level(spdlog::level::debug);
    init_code_generator();

    TEST_for();

    return 0;
}
