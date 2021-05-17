#pragma once
#include "data.h"
#include "lex.h"
#include "logger.h"
using namespace std;

namespace NodeProperties {
    using S = GlobalAST*;
    using ProgramStruct = GlobalAST*;
    using ProgramBody = map<std::string, std::vector<std::any>>;
    using Component = map<std::string, std::vector<std::any>>;
    using IDList = std::vector<std::string>;
    using ConstDeclaration = std::vector<VariableDeclAST*>;  // new
    using ConstValue = ExprAST*;
    using ActualType = TypeDeclAST*;
    using VarDeclaration = std::vector<VariableDeclAST*>;
    using Type = TypeDeclAST*;
    using BasicType = BasicTypeAST*;
    using Period = std::vector<std::pair<NumberExprAST*, NumberExprAST*>>;
    using Subprogram = FunctionAST*;
    using SubprogramHead = FunctionSignatureAST*;
    using FormalParameter = std::vector<VariableDeclAST*>;
    using ParameterList = std::vector<VariableDeclAST*>;
    using Parameter = std::vector<VariableDeclAST*>;
    using VarParameter = std::vector<VariableDeclAST*>;
    using ValueParameter = std::vector<VariableDeclAST*>;
    using SubprogramBody = std::pair<std::vector<VariableDeclAST*>, BlockAST*>;
    using SubComponent = std::vector<VariableDeclAST*>;
    using CompoundStatement = BlockAST*;
    using StatementList = std::vector<AST*>;
    using Statement = AST*;
    using Variable = ExprAST*;
    using IDVarpart = std::vector<ExprAST*>;
    using ProcedureCall = CallExprAST*;
    using ElsePart = BlockAST*;
    using ExpressionList = std::vector<ExprAST*>;
    using Expression = ExprAST*;
    using SimpleExpression = ExprAST*;
    using Term = ExprAST*;
    using Factor = ExprAST*;
    using ComposedVariable = ExprAST*;
    using Num = NumberExprAST*;
    using Digits = NumberExprAST*;
    using addOP = std::string;
    using TypeDeclaration = std::vector<TypeDefAST*>;
    using Value = ExprAST*;
    template <typename A>
    inline A cast(std::any arg) {
        if (typeid(A) == typeid(AST*)) {
            if (arg.type() == typeid(BlockAST*))
                return A(std::any_cast<BlockAST*>(arg));
            else if (arg.type() == typeid(ExprAST*))
                return A(std::any_cast<ExprAST*>(arg));
            else if (arg.type() == typeid(TypeDeclAST*))
                return A(std::any_cast<TypeDeclAST*>(arg));
            else if (arg.type() == typeid(BasicTypeAST*))
                return A(std::any_cast<BasicTypeAST*>(arg));
            else if (arg.type() == typeid(PointerTypeDeclAST*))
                return A(std::any_cast<PointerTypeDeclAST*>(arg));
            else if (arg.type() == typeid(NumberExprAST*))
                return A(std::any_cast<NumberExprAST*>(arg));
            else if (arg.type() == typeid(ArrayTypeDeclAST*))
                return A(std::any_cast<ArrayTypeDeclAST*>(arg));
            else if (arg.type() == typeid(StringExprAST*))
                return A(std::any_cast<StringExprAST*>(arg));
            else if (arg.type() == typeid(VariableExprAST*))
                return A(std::any_cast<VariableExprAST*>(arg));
            else if (arg.type() == typeid(UnaryExprAST*))
                return A(std::any_cast<UnaryExprAST*>(arg));
            else if (arg.type() == typeid(BinaryExprAST*))
                return A(std::any_cast<BinaryExprAST*>(arg));
            else if (arg.type() == typeid(CallExprAST*))
                return A(std::any_cast<CallExprAST*>(arg));
            else if (arg.type() == typeid(VariableDeclAST*))
                return A(std::any_cast<VariableDeclAST*>(arg));
            else if (arg.type() == typeid(ForStatementAST*))
                return A(std::any_cast<ForStatementAST*>(arg));
            else if (arg.type() == typeid(WhileStatementAST*))
                return A(std::any_cast<WhileStatementAST*>(arg));
            else if (arg.type() == typeid(IfStatementAST*))
                return A(std::any_cast<IfStatementAST*>(arg));
            else if (arg.type() == typeid(FunctionSignatureAST*))
                return A(std::any_cast<FunctionSignatureAST*>(arg));
            else if (arg.type() == typeid(FunctionAST*))
                return A(std::any_cast<FunctionAST*>(arg));
            else if (arg.type() == typeid(StructDeclAST*))
                return A(std::any_cast<StructDeclAST*>(arg));
            else if (arg.type() == typeid(GlobalAST*))
                return A(std::any_cast<GlobalAST*>(arg));
            else if (arg.type() == typeid(TypeDefAST*))
                return A(std::any_cast<TypeDefAST*>(arg));
        }
        else if (typeid(A) == typeid(TypeDeclAST*)) {
            if (arg.type() == typeid(BasicTypeAST*))
                return A(std::any_cast<BasicTypeAST*>(arg));
            else if (arg.type() == typeid(PointerTypeDeclAST*))
                return A(std::any_cast<PointerTypeDeclAST*>(arg));
            else if (arg.type() == typeid(ArrayTypeDeclAST*))
                return A(std::any_cast<ArrayTypeDeclAST*>(arg));
            else if (arg.type() == typeid(TypeDefAST*))
                return A(std::any_cast<TypeDefAST*>(arg));
        }
        else if (typeid(A) == typeid(ExprAST*)) {
            if (arg.type() == typeid(NumberExprAST*))
                return A(std::any_cast<NumberExprAST*>(arg));
            else if (arg.type() == typeid(StringExprAST*))
                return A(std::any_cast<StringExprAST*>(arg));
            else if (arg.type() == typeid(VariableExprAST*))
                return A(std::any_cast<VariableExprAST*>(arg));
            else if (arg.type() == typeid(UnaryExprAST*))
                return A(std::any_cast<UnaryExprAST*>(arg));
            else if (arg.type() == typeid(BinaryExprAST*))
                return A(std::any_cast<BinaryExprAST*>(arg));
            else if (arg.type() == typeid(CallExprAST*))
                return A(std::any_cast<CallExprAST*>(arg));
        }
        // std::cerr << "TYPE: " << typeid(A).name() << std::endl;
        // std::cerr << "TYPE: " << arg.type().name() << std::endl;
        return std::any_cast<A>(arg);
    }
    template <>
    inline std::vector<TypeDefAST*> cast(std::any arg) {  // new
        return std::any_cast<std::vector<TypeDefAST*>>(arg);
    }
    template <>
    inline map<std::string, std::vector<std::any>> cast(std::any arg) {
        return std::any_cast<map<std::string, std::vector<std::any>>>(arg);
    }
    template <>
    inline std::vector<ExprAST*> cast(std::any arg) {
        return std::any_cast<std::vector<ExprAST*>>(arg);
    }
    template <>
    inline std::vector<AST*> cast(std::any arg) {
        return std::any_cast<std::vector<AST*>>(arg);
    }
    template <>
    inline std::vector<std::string> cast(std::any arg) {
        return std::any_cast<std::vector<std::string>>(arg);
    }
    template <>
    inline std::string cast(std::any arg) {
        return std::any_cast<std::string>(arg);
    }
    template <>
    inline std::vector<VariableDeclAST*> cast(std::any arg) {
        return std::any_cast<std::vector<VariableDeclAST*>>(arg);
    }
    template <>
    inline std::pair<std::vector<VariableDeclAST*>, BlockAST*> cast(std::any arg) {
        return std::any_cast<std::pair<std::vector<VariableDeclAST*>, BlockAST*>>(
            arg);
    }
    template <>
    inline std::vector<std::pair<NumberExprAST*, NumberExprAST*>> cast(
        std::any arg) {
        return std::any_cast<
            std::vector<std::pair<NumberExprAST*, NumberExprAST*>>>(arg);
    }
};  // namespace NodeProperties

struct GrammarTreeNode {
    string raw, type, parserSymbol;
    int row, column;
    uint64_t ID;
    std::any prop;

    vector<GrammarTreeNode*> son;
    GrammarTreeNode(string raw, string type, string parserSymbol, int row,
        int column, uint64_t ID)
        : raw(raw),
        type(type),
        parserSymbol(parserSymbol),
        row(row),
        column(column),
        ID(ID),
        son{} {}

    void Report() {
        Json j = { {"type", "grammar"},    {"raw", raw},
                  {"type", type},         {"parserSymbol", parserSymbol},
                  {"row", row},           {"column", column},
                  {"ID", (uint64_t)this}, {"son", Json::array()} };
        for (auto s : son) j["son"].push_back((uint64_t)s);
        parserOutputer.push_back(j);
    }
};

void init();
GlobalAST* parser_work(TokenQueue);
