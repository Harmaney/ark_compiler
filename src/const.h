#pragma once
enum ASTKind {
    AST_BASIC_TYPE,
    AST_POINTER_TYPE,
    AST_ARRAY_TYPE,
    AST_TYPE_DEF,
    AST_NUMBER_EXPR,
    AST_STRING_EXPR,
    AST_VARIABLE_EXPR,
    AST_BINARY_EXPR,
    AST_UNARY_EXPR,
    AST_RETURN,
    AST_CALL_EXPR,
    AST_GLOBAL,
    AST_FUNCTION_SIGNATURE,
    AST_FUNCTION,
    AST_BLOCK,
    AST_VARIABLE_DECL,
    AST_STRUCT_DECL,
    AST_FOR_STATEMENT,
    AST_WHILE_STATEMENT,
    AST_IF_STATEMENT,
};

#define TYPE_BASIC_DOUBLE "double"
#define TYPE_BASIC_INT "integer"
#define TYPE_BASIC_LONGINT "longint"
#define TYPE_BASIC_INT64 "int64"
// in fact this is somehow not basic
#define TYPE_BASIC_STRING "string"
#define TYPE_BASIC_VOID "void"

enum ConstantType{
    CONSTANT_REAL,
    CONSTANT_INT,
};

enum DescriptorType{
    DESCRIPTOR_BASIC,
    DESCRIPTOR_ARRAY,
    DESCRIPTOR_POINTER_TYPE,
    DESCRIPTOR_VARIABLE,
    DESCRIPTOR_STRUCT,
    DESCRIPTOR_FUNCTION,
    DESCRIPTOR_TYPE
};