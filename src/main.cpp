#include <fstream>
#include <iostream>

#include "codegen.h"
#include "lex.h"
#include "logger.h"
#include "parser.h"

int main(int argc, char **argv) {
    std::string input_pas;
    if (argc <= 1) {
        std::cerr << "please give input.pas" << std::endl;
        std::cin >> input_pas;
    } else
        input_pas = argv[1];
    std::ifstream src_file_stream(input_pas);
    if (!src_file_stream) {
        term_print.fatal() << "file doesn't exist.\n";
        abort();
    }
    std::string src_code((std::istreambuf_iterator<char>(src_file_stream)),
                         (std::istreambuf_iterator<char>()));

    term_print.info() << "lexing...\n";
    auto tq = lex_work(src_code);
    term_print.info() << "parsing...\n";
    auto ast = parser_work(tq);
    term_print.info() << "generating...\n";
    auto ccode = code_gen_work(ast);

    std::ofstream ccode_file("out.c");
    ccode_file << ccode;
    std::cerr << "finished!\n";

    std::ofstream info("info.json");
    info << (Json{
                 {"code", src_code},
                 {"lex", lex_info},
                 {"parser", parser_info},
                 {"gen", gen_info},
             })
                .dump();
    ccode_file.flush();
    ccode_file.close();
    system("gcc out.c -std=c99 -o out");
    return 0;
}
