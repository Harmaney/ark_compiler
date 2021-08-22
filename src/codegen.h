#pragma once
#include "data.h"
#include "gen.h"
#include "logger.h"
#include "ccvm.h"

VMContext* init_context() {
    auto context=new VMContext();
    
    return context;
}

void init_basic_type(VMContext *context) {
    context->symbalTable->insert_type(
        TYPE_BASIC_VOID,
        new SymbolDescriptor(DESCRIPTOR_BASIC, TYPE_BASIC_VOID));
    context->symbalTable->insert_type(
        TYPE_BASIC_INT, new SymbolDescriptor(DESCRIPTOR_BASIC, TYPE_BASIC_INT));
    context->symbalTable->insert_type(
        TYPE_BASIC_DOUBLE,
        new SymbolDescriptor(DESCRIPTOR_BASIC, TYPE_BASIC_DOUBLE));
    context->symbalTable->insert_type(
        TYPE_BASIC_STRING,
        new SymbolDescriptor(DESCRIPTOR_BASIC, TYPE_BASIC_STRING));
    context->symbalTable->insert_type(
        TYPE_BASIC_CHAR,
        new SymbolDescriptor(DESCRIPTOR_BASIC, TYPE_BASIC_CHAR));
    context->symbalTable->insert_type(
        TYPE_BASIC_LONGINT,
        new SymbolDescriptor(DESCRIPTOR_BASIC, TYPE_BASIC_LONGINT));
    context->symbalTable->insert_type(
        TYPE_BASIC_INT64,
        new SymbolDescriptor(DESCRIPTOR_BASIC, TYPE_BASIC_INT64));
}

std::string code_gen_work(AST *ast) {
    auto context=init_context();
    init_basic_type(context);


    ASTDispatcher dispatcher(context);
    ast->accept(dispatcher);



    std::ostringstream code_out;
    context->code_collector->output(code_out);
    return code_out.str();
}