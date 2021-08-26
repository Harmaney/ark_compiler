#include "data.h"

#include <iostream>
#include <stdexcept>

#include "err.h"
#include "logger.h"
#include "parser.h"


std::any ExprAST::accept(ADispatcher* dispatcher) {
    throw std::invalid_argument("unknown type of expr");
}

std::any BasicTypeAST::accept(ADispatcher* dispatcher) {
    return dispatcher->gen_basic_type(this);
}

std::any TypeDefAST::accept(ADispatcher* dispatcher) {
    return dispatcher->gen_type_def(this);
}

std::any ArrayTypeDeclAST::accept(ADispatcher* dispatcher) {
    return dispatcher->gen_array_type_decl(this);
}

std::any PointerTypeDeclAST::accept(ADispatcher* dispatcher) {
    return dispatcher->gen_pointer_type_decl(this);
}

std::any NumberExprAST::accept(ADispatcher* dispatcher) {
    return dispatcher->gen_number_expr(this);
}

std::any StringExprAST::accept(ADispatcher* dispatcher) {
    return dispatcher->gen_string_expr(this);
}

std::any CharExprAST::accept(ADispatcher* dispatcher) {
    return dispatcher->gen_char_expr(this);
}

std::any VariableExprAST::accept(ADispatcher* dispatcher) {
    return dispatcher->gen_variable_expr(this);
}

std::any BinaryExprAST::accept(ADispatcher* dispatcher) {
    return dispatcher->gen_binary_expr(this);
}

std::any UnaryExprAST::accept(ADispatcher* dispatcher) {
    return dispatcher->gen_unary_expr(this);
}

std::any ReturnAST::accept(ADispatcher* dispatcher) {
    return dispatcher->gen_return(this);
}

std::any CallExprAST::accept(ADispatcher* dispatcher) {
    return dispatcher->gen_call_expr(this);
}

std::any BlockAST::accept(ADispatcher* dispatcher) {
    return dispatcher->gen_block(this);
}

std::any VariableDeclAST::accept(ADispatcher* dispatcher) {
    return dispatcher->gen_variable_decl(this);
}

std::any ParameterDeclAST::accept(ADispatcher* dispatcher){
    return dispatcher->gen_parameter_decl(this);
}

std::any ForStatementAST::accept(ADispatcher* dispatcher) {
    return dispatcher->gen_for_statement(this);
}

std::any WhileStatementAST::accept(ADispatcher* dispatcher) {
    return dispatcher->gen_while_statement(this);
}

std::any IfStatementAST::accept(ADispatcher* dispatcher) {
    return dispatcher->gen_if_statement(this);
}

std::any GlobalAST::accept(ADispatcher* dispatcher) {
    return dispatcher->gen_global(this);
}

std::any FunctionAST::accept(ADispatcher* dispatcher) {
    return dispatcher->gen_function(this);
}

std::any FunctionSignatureAST::accept(ADispatcher* dispatcher) {
    return dispatcher->gen_function_signature(this);
}

std::any StructDeclAST::accept(ADispatcher* dispatcher) {
    return dispatcher->gen_struct(this);
}

///////////////////////////////////////



// FunctionDescriptor* levelup_lookfor_function(std::string sig, std::vector<SymbolDescriptor*> args,int idx){
//     if(SymbolTable::lookfor_function(sig,args)){
//         return SymbolTable::lookfor_function(sig,args);
//     }
//     if(idx>=args.size())return nullptr;
//     std::vector<SymbolDescriptor *> level;
//     level.push_back(SymbolTable::lookfor_type(TYPE_BASIC_INT));
//     level.push_back(SymbolTable::lookfor_type(TYPE_BASIC_LONGINT));
//     level.push_back(SymbolTable::lookfor_type(TYPE_BASIC_INT64));

//     bool canrep=false;
//     for(auto dp:level){
//         if(dp==args[idx])canrep=true;
//         auto old=args[idx];
//         args[idx]=dp;
//         auto res=levelup_lookfor_function(sig,args,idx+1);
//         if(res)return res;
//         args[idx]=old;
//     }

//     return nullptr;
// }

// FunctionDescriptor* SymbolTable::lookfor_function(
//     std::string sig, std::vector<SymbolDescriptor*> args) {
//     auto name = get_internal_function_name(sig, args);
//     return static_cast<FunctionDescriptor*>(lookfor_type(name));
// }



///////////////////////////////////////////////