#include "codegen.h"
#include "lex.h"
#include "parser.h"
#include "test_ast.h"
using namespace std;

int main(int argc, char **argv) {
    auto sourceCode = load_file("testcase/SegmentTree.pas");

    auto tq = lex_work(sourceCode);

    auto ast = parser_work(tq);

    init_code_generator();
    init_basic_type();

    CodeCollector::begin_section("main");

    ASTDispatcher dispatcher;
    ast->accept(dispatcher);

    CodeCollector::end_section(PLACE_END);
    ofstream codeOut("out.cpp");
    CodeCollector::output(codeOut);

    std::ofstream info("info.json");
    info << ((Json){
                 {"code", sourceCode},
                 {"lex", lexOutputer},
                 {"parser", parserOutputer},
                 {"gen", genOutputer},
             })
                .dump();
    return 0;
}