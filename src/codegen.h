#pragma once
#include "data.h"
#include "gen.h"
#include "logger.h"

void init_code_generator() {
    SymbolTable::init();
    TagTable::init();
}

void init_basic_type() {
    SymbolTable::insert_type(
        TYPE_BASIC_VOID,
        new SymbolDescriptor(DESCRIPTOR_BASIC, TYPE_BASIC_VOID));
    SymbolTable::insert_type(
        TYPE_BASIC_INT, new SymbolDescriptor(DESCRIPTOR_BASIC, TYPE_BASIC_INT));
    SymbolTable::insert_type(
        TYPE_BASIC_DOUBLE,
        new SymbolDescriptor(DESCRIPTOR_BASIC, TYPE_BASIC_DOUBLE));
    SymbolTable::insert_type(
        TYPE_BASIC_STRING,
        new SymbolDescriptor(DESCRIPTOR_BASIC, TYPE_BASIC_STRING));
    SymbolTable::insert_type(
        TYPE_BASIC_CHAR,
        new SymbolDescriptor(DESCRIPTOR_BASIC, TYPE_BASIC_CHAR));
    SymbolTable::insert_type(
        TYPE_BASIC_LONGINT,
        new SymbolDescriptor(DESCRIPTOR_BASIC, TYPE_BASIC_LONGINT));
    SymbolTable::insert_type(
        TYPE_BASIC_INT64,
        new SymbolDescriptor(DESCRIPTOR_BASIC, TYPE_BASIC_INT64));
}

std::string code_gen_work(AST *ast) {
    init_code_generator();
    init_basic_type();

    CodeCollector::begin_section("main");

    ASTDispatcher dispatcher;
    ast->accept(dispatcher);

    CodeCollector::end_section();

    CodeCollector::begin_section("init_global_var");
    CodeCollector::push_front("void init_global_var_() {");
    CodeCollector::push_back("}");
    CodeCollector::end_section();

    CodeCollector::rearrange_section({"prelude", "global_define", "pre_struct",
                                      "pre_array", "struct", "init_global_var",
                                      "main"});

    std::ostringstream code_out;
    CodeCollector::output(code_out);
    return code_out.str();
}