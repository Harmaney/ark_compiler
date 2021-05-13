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
    return 0;
}