#include "codegen.h"
#include "lex.h"
#include "parser.h"
#include "test_ast.h"
using namespace std;

int main(int argc, char **argv) {
    std::string inputPas;
    if (argc <= 1) {
        std::cerr << "please give input.pas" << std::endl;
        std::cin >> inputPas;
    } else
        inputPas = argv[1];
    std::ifstream soureFileStream(inputPas);
    if (!soureFileStream) {
        FATAL(std::cerr << "file doesn't exist.\n";)
        return 0;
    }
    std::string sourceCode((std::istreambuf_iterator<char>(soureFileStream)),
                           (std::istreambuf_iterator<char>()));
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
    system("gcc out.c -o out");
    return 0;
}