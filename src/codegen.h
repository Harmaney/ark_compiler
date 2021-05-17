#pragma once
#include "data.h"
#include "gen.h"
#include "logger.h"

void init_code_generator() {
    SymbolTable::init();
    TagTable::init();
}

void init_basic_type() {
    SymbolTable::insertType(
        TYPE_BASIC_VOID,
        new SymbolDescriptor(DESCRIPTOR_BASIC, TYPE_BASIC_VOID));
    SymbolTable::insertType(
        TYPE_BASIC_INT, new SymbolDescriptor(DESCRIPTOR_BASIC, TYPE_BASIC_INT));
    SymbolTable::insertType(
        TYPE_BASIC_DOUBLE,
        new SymbolDescriptor(DESCRIPTOR_BASIC, TYPE_BASIC_DOUBLE));
    SymbolTable::insertType(
        TYPE_BASIC_STRING,
        new SymbolDescriptor(DESCRIPTOR_BASIC, TYPE_BASIC_STRING));
    SymbolTable::insertType(
        TYPE_BASIC_LONGINT,
        new SymbolDescriptor(DESCRIPTOR_BASIC, TYPE_BASIC_LONGINT));
        SymbolTable::insertType(
        TYPE_BASIC_INT64,
        new SymbolDescriptor(DESCRIPTOR_BASIC, TYPE_BASIC_INT64));
}