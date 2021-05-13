#include <fstream>
#include <iostream>
#include <string>


#include "lex.h"
using namespace std;

int main(int argc, char **argv) {
    // std::ifstream ifff("../files/keywords.txt");
    // std::string x;ifff>>x;
    // std::cerr<<x<<'\n';

    // ofstream of("../files/text.txt");
    // of<<"???"<<endl;
    // std::cerr<<"ffff";
    // of.close();
    lex_work("../files/SegmentTree.pas");
    parser_work("../files/lex_out.txt");

    // init_code_generator();
    // init_basic_type();

    // CodeCollector::begin_section();
    // TEST_vardecl();
    // TEST_arraydecl();
    TEST_arrayofarray();
    // TEST_while();
    // TEST_funccall();
    // TEST_struct();
    // TEST_pointer();
    // TEST_case1();

    // CodeCollector::end_section(PLACE_END);

    // CodeCollector::rearrange_section("global_define", 0);
    // CodeCollector::rearrange_section("prelude", 0);

    // ofstream codeOut("out.cpp");
    // CodeCollector::output();

    return 0;
}