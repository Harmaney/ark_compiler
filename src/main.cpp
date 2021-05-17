#include "codegen.h"
#include "lex.h"
#include "parser.h"
#include "test_ast.h"
using namespace std;

int main(int argc, char **argv) {
    auto tq = lex_work("testcase/1.pas");

    auto ast = parser_work(tq);

    init_code_generator();
    init_basic_type();

    CodeCollector::begin_section("main");

    ASTDispatcher dispatcher;
    ast->accept(dispatcher);

    CodeCollector::end_section(PLACE_END);
    ofstream codeOut("out.cpp");
    CodeCollector::output(codeOut);

    return 0;
}