#include "codegen.h"
#include "lex.h"
#include "parser.h"
#include "test_ast.h"
using namespace std;

int main(int argc, char **argv) {
    lex_work("1.pas");
    auto ast = parser_work("lex_out.txt");

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