#include <fstream>
#include <iostream>

#include "codegen.h"
#include "lex.h"
#include "logger.h"
#include "parser.h"
#include "test_ast.h"
#include "templatelogger.hpp"
int main(int argc, char **argv) {
    RUN_TEST();
    return 0;


    std::string input_pas, out_c = "out.c";
    if (argc == 1) {
        std::cerr << "Usage: p2c input.pas [-o out.c] [-l LOG_LEVEL(0-5)].\n";
        exit(0);
    } else
        input_pas = argv[1];

    for (int i = 2; i < argc; i += 2) {
        if (strcmp(argv[i], "-o") == 0) {
            out_c = argv[i + 1];
        } else if (strcmp(argv[i], "-l") == 0) {
            term_print.log_level = std::stoi(argv[i + 1]);
            if (term_print.log_level < 0 || term_print.log_level > 5) {
                term_print.fatal() << "Invalid info level.\n";
                exit(0);
            }
        }
    }
    std::ifstream src_file_stream(input_pas);
    if (!src_file_stream) {
        term_print.fatal() << "File doesn't exist.\n";
        exit(0);
    }
    std::string src_code((std::istreambuf_iterator<char>(src_file_stream)),
                         (std::istreambuf_iterator<char>()));

    term_print.info() << "Lexing...\n";
    auto tq = lex_work(src_code);
    term_print.info() << "Parsing...\n";
    auto ast = parser_work(tq);
    term_print.info() << "Generating...\n";
    auto ccode = code_gen_work(ast);

    std::ofstream ccode_file(out_c);
    ccode_file << ccode;
    term_print.info() << "Finished!\n";

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
    system(("gcc " + out_c + " -std=c99 -o out").c_str());
    return 0;
}
