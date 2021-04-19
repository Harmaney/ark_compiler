
#include "gen.h"

#include <iostream>

#include "data.h"
#include "spdlog/spdlog.h"

void ASTDispatcher::genGlobalBegin(GlobalAST *ast) {
    // TODO:
}

void ASTDispatcher::genGlobalEnd(GlobalAST *ast) {
    // TODO:
}

void ASTDispatcher::genNumberExpr(NumberExprAST *ast) {
    Variable *t = SymbolTable::createVariableG(REAL);

    // TODO: add integer
    CodeCollector::src() << "double " << t->sig << ";";
    CodeCollector::push_back();
    CodeCollector::src() << t->sig << "=" << ast->val << ";";
    CodeCollector::push_back();
    ast->value = t;
}

void ASTDispatcher::genVariableExpr(VariableExprAST *ast) {
    Variable *t = SymbolTable::lookfor(ast->name);
    if (!t) {
        spdlog::error("variable {} not found", ast->name);
        return;
    }
    ast->value = t;
}

void ASTDispatcher::genBinaryExpr(BinaryExprAST *ast) {
    Variable *lhs = static_cast<Variable *>(ast->LHS->value);
    Variable *rhs = static_cast<Variable *>(ast->RHS->value);

    switch (ast->op) {
        case '=': {
            CodeCollector::src() << lhs->sig << "=" << rhs->sig << ";";
            CodeCollector::push_back();
            // TODO: pascal 的 = 是否有返回值？
            break;
        }

        default: {
            Variable *t = SymbolTable::createVariable(REAL);
            genVariable(t);

            CodeCollector::src()
                << t->sig << "=" << lhs->sig << ast->op << rhs->sig << ";";
            CodeCollector::push_back();
            ast->value = t;
            break;
        }
    }
}

void ASTDispatcher::genCallExpr(CallExprAST *ast) {
    CodeCollector::src() << ast->callee << "(";
    for (auto arg : ast->args) {
        CodeCollector::src() << static_cast<Variable *>(arg->value)->sig << ",";
    }
    CodeCollector::src() << ");";
    CodeCollector::push_back();
}

void ASTDispatcher::genFunctionSignature(FunctionSignatureAST *ast) {
    CodeCollector::src() << mapVariableType(ast->result) << " ";
    CodeCollector::src() << ast->sig;
    CodeCollector::src() << "(";
    for (auto item : ast->args) {
        CodeCollector::src() << "double " << item->sig << ", ";
    }
    CodeCollector::src() << ")";
    CodeCollector::push_back();
}

void ASTDispatcher::genBlockBegin(BlockAST *ast) {
    CodeCollector::push_back("{");
}

void ASTDispatcher::genBlockEnd(BlockAST *ast) {
    CodeCollector::push_back("}");
}

void ASTDispatcher::genVariableDecl(VariableDeclAST *ast) {
    auto var = SymbolTable::createVariable(ast->sig->name, ast->type);
    genVariable(var);
}

std::string mapVariableType(VariableType type) {
    switch (type) {
        case REAL:
            return "double";
        case INT:
            return "int";
        default:
            spdlog::error("unknown type of variable: {}", type);
            break;
    }
    assert(false);
}

void genVariable(Variable *var) {
    // TODO: handle different type
    CodeCollector::src() << mapVariableType(var->type) << " " << var->sig
                         << ";";
    CodeCollector::push_back();
}

////////////////////////////

std::map<std::string, std::vector<std::string>> CodeCollector::codes;
std::stringstream CodeCollector::ss;
std::string CodeCollector::cur_section_name;
std::vector<std::string> CodeCollector::cur_section;
std::vector<std::string> CodeCollector::section_order;

void CodeCollector::push_back(std::string str) { cur_section.push_back(str); }
void CodeCollector::push_front(std::string str) {
    cur_section.insert(cur_section.begin(), str);
}

void CodeCollector::push_back() { cur_section.push_back(ss.str()); }
void CodeCollector::push_front() {
    cur_section.insert(cur_section.begin(), ss.str());
}

void CodeCollector::begin_section(std::string section_name) {
    if (section_name.empty()) {
        section_name = std::to_string(section_order.size());
    }

    cur_section.clear();
    // 重新打开对应代码段
    if(codes.count(section_name)){
        cur_section=codes[section_name];
    }
    cur_section_name = section_name;
    ss.str(std::string());
}

std::stringstream &CodeCollector::src() { return ss; }

void CodeCollector::end_section(PlaceHolder place) {
    codes[cur_section_name] = cur_section;
    cur_section.clear();
    switch (place) {
        case PLACE_BEGIN:
            section_order.insert(section_order.begin(), cur_section_name);
            break;

        case PLACE_END:
            section_order.push_back(cur_section_name);
            break;
        default:
            // TODO: wrong
            spdlog::error("unknown inserting place: {}", place);
            break;
    }
}

void CodeCollector::output() {
    for (auto sid :section_order){
        for (auto str : codes[sid]) {
            spdlog::debug(str);
        }
    }
}

void CodeCollector::clear() { codes.clear(); }