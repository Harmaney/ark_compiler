#include <iostream>
#include <string>

#include "data.h"
#include "gen.h"
#include "lex.h"
#include "parser.h"
#include "spdlog/spdlog.h"
using namespace std;

void init_code_generator() {
    SymbolTable::init();
    TagTable::init();
}

void init_basic_type() {
    SymbolTable::insertType(TYPE_BASIC_INT,
                            new SymbolDescriptor(DESCRIPTOR_BASIC));
    SymbolTable::insertType(TYPE_BASIC_DOUBLE,
                            new SymbolDescriptor(DESCRIPTOR_BASIC));
}

void TEST_struct() {
    std::vector<VariableDeclAST *> t;
    t.push_back(new VariableDeclAST(new VariableExprAST("a"), "int"));
    StructDeclAST *ast = new StructDeclAST("Test", t);

    ASTDispatcher dispacher;
    ast->accept(dispacher);
}

void TEST_while() {
    BlockAST *env = new BlockAST({});
    env->exprs.push_back(
        new VariableDeclAST(new VariableExprAST("i"), TYPE_BASIC_INT));

    BlockAST *while_block = new BlockAST({});

    while_block->exprs.push_back(
        new VariableDeclAST(new VariableExprAST("domjudge"), TYPE_BASIC_INT));
    WhileStatementAST *while_ast = new WhileStatementAST(
        new BinaryExprAST('<', new VariableExprAST("i"), new NumberExprAST(5)),
        while_block);

    env->exprs.push_back(while_ast);

    ASTDispatcher dispacher;
    env->accept(dispacher);
}

void TEST_for() {
    BlockAST *env = new BlockAST({});
    env->exprs.push_back(
        new VariableDeclAST(new VariableExprAST("i"), TYPE_BASIC_INT));

    BlockAST *for_block = new BlockAST({});

    for_block->exprs.push_back(new VariableDeclAST(
        new VariableExprAST("domjudge"), TYPE_BASIC_DOUBLE));
    ForStatementAST *for_ast =
        new ForStatementAST(new VariableExprAST("i"), new NumberExprAST(1),
                            new NumberExprAST(5), for_block);

    env->exprs.push_back(for_ast);

    ASTDispatcher dispacher;
    env->accept(dispacher);
}

void TEST_case1() {
    GlobalAST *global = new GlobalAST(
        {}, {},
        new BlockAST(
            {new CallExprAST("write", {new StringExprAST("hello, world")})}));
    
    ASTDispatcher dispacher;
    global->accept(dispacher);
}

void TEST_funccall() {
    BlockAST *block = new BlockAST({});

    VariableDeclAST *decl =
        new VariableDeclAST(new VariableExprAST("a"), TYPE_BASIC_DOUBLE);
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

    FunctionSignatureAST *funcsig = new FunctionSignatureAST(
        "main", vector<VariableDeclAST *>(), TYPE_BASIC_DOUBLE);
    FunctionAST *func = new FunctionAST(funcsig, block);

    GlobalAST *global = new GlobalAST({}, {func}, {});

    ASTDispatcher dispacher;
    global->accept(dispacher);
}

int main(int argc, char **argv) {
    std::ifstream ifff("../files/keywords.txt");
    std::string x;ifff>>x;
    std::cerr<<x<<'\n';
    
    ofstream of("../files/text.txt");
    of<<"???"<<endl;
    std::cerr<<"ffff";
    of.close();
	// lex_work("../files/1.pas"); 
	// parser_work("../files/lex_out.txt");

    /*spdlog::set_level(spdlog::level::debug);
    init_code_generator();
    init_basic_type();

    CodeCollector::begin_section();
    // TEST_while();
    // TEST_funccall();
    // TEST_struct();
    TEST_case1();

    CodeCollector::end_section(PLACE_END);

    CodeCollector::rearrange_section("global_define", 0);
    CodeCollector::rearrange_section("prelude", 0);
    CodeCollector::output();*/

    return 0;
}
