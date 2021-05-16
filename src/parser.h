#pragma once
#include <bits/stdc++.h>

#include "data.h"
#include "json.hpp"

using namespace std;

struct GrammarTreeNode {
    string raw, type, parserSymbol;
    int row, column;
    uint64_t ID;
    std::any prop;

    vector<GrammarTreeNode*> son;
    GrammarTreeNode(string raw, string type, string parserSymbol, int row, int column, uint64_t ID) : raw(raw), type(type), parserSymbol(parserSymbol), row(row), column(column), ID(ID), son{} {}
};

void init();

GlobalAST* parser_work(string file_name);