#pragma once
enum ASTKind {
    NUMBER_EXPR,
    VARIABLE_EXPR,
    BINARY_EXPR,
    CALL_EXPR,
    GLOBAL,
    FUNCTION_SIGNATURE,
    FUNCTION,
    BLOCK,
    VARIABLE_DECL,
};

enum VariableType {
    REAL=1,
    INT,
};

enum ConstantType{
    CONSTANT_REAL,
    CONSTANT_INT,
};