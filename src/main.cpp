#include "codegen.h"
#include "lex.h"
#include "parser.h"
#include "test_ast.h"
using namespace std;

int main(int argc, char **argv) {
    /*if (argc <= 1) {
        FATAL(std::cerr << "please give input.pas" << std::endl;)
        return 0;
    }
    auto sourceCode = load_file(argv[1]);
    */
    auto sourceCode = load_file("testcase/string.pas");
    std::cerr << sourceCode << '\n';

    auto tq = lex_work(sourceCode);

    auto ast = parser_work(tq);

    init_code_generator();
    init_basic_type();

    CodeCollector::begin_section("main");

    ASTDispatcher dispatcher;
    ast->accept(dispatcher);

    CodeCollector::end_section();

    CodeCollector::begin_section("init_string");
    CodeCollector::push_front("void init_string_() {");
    CodeCollector::push_back("}");
    CodeCollector::end_section();

    CodeCollector::rearrange_section({"prelude", "global_define", "pre_struct",
                                      "pre_array", "struct", "init_string",
                                      "main"});

    ofstream codeOut("out.c");
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