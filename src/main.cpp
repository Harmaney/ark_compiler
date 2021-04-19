#include <iostream>
#include <string>

#include "data.h"
#include "gen.h"
#include "lexer.h"
#include "spdlog/spdlog.h"
using namespace std;

void TEST_lexer() {
    string test = "123+3";
    LexerScanner scanner(test);

    while (scanner.hasNextToken()) {
        cout << scanner.nextToken().str << endl;
    }
}

int main(int argc, char **argv) {
    spdlog::set_level(spdlog::level::debug);
    SymbolTable::init();

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

    ASTDispatcher dispacher;
    func->accept(dispacher);

    CodeCollector::output();

    return 0;
}
