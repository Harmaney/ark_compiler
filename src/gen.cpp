
#include "gen.h"

#include <algorithm>
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
    Variable *t = nullptr;

    switch (ast->const_type) {
        case CONSTANT_INT:
            t = SymbolTable::createVariableG(INT);
            break;
        case CONSTANT_REAL:
            t = SymbolTable::createVariableG(REAL);
            break;
        default:
            spdlog::error("unknown constant type: {}", ast->const_type);
            break;
    }

    CodeCollector::begin_section("global_define");
    CodeCollector::src() << mapVariableType(t->type) << " " << t->sig << ";";
    CodeCollector::push_back();
    CodeCollector::src() << t->sig << "=" << ast->val << ";";
    CodeCollector::push_back();
    CodeCollector::end_section();
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

std::map<std::string, std::vector<std::string> *> CodeCollector::codes;
std::stringstream CodeCollector::ss;
std::stack<std::string> CodeCollector::cur_section_name;
std::vector<std::string> *CodeCollector::cur_section;
std::vector<std::string> CodeCollector::section_order;

void CodeCollector::push_back(std::string str) { cur_section->push_back(str); }
void CodeCollector::push_front(std::string str) {
    cur_section->insert(cur_section->begin(), str);
}

void CodeCollector::push_back() {
    cur_section->push_back(ss.str());
    ss.str(std::string());
}
void CodeCollector::push_front() {
    cur_section->insert(cur_section->begin(), ss.str());
    ss.str(std::string());
}

void CodeCollector::begin_section(std::string section_name) {
    if (section_name.empty()) {
        section_name = std::to_string(section_order.size());
    }

    // 重新打开对应代码段
    if (codes.count(section_name)) {
        cur_section = codes[section_name];
    } else {
        cur_section = new std::vector<std::string>();
        codes[section_name] = cur_section;
    }
    cur_section_name.push(section_name);
    ss.str(std::string());
}

std::stringstream &CodeCollector::src() { return ss; }

void CodeCollector::end_section(PlaceHolder place) {
    assert(!cur_section_name.empty());
    if (std::find(section_order.begin(), section_order.end(),
                  cur_section_name.top()) == section_order.end()) {
        switch (place) {
            case PLACE_BEGIN:
                section_order.insert(section_order.begin(),
                                     cur_section_name.top());
                break;

            case PLACE_END:
                section_order.push_back(cur_section_name.top());
                break;
            default:
                // TODO: wrong
                spdlog::error("unknown inserting place: {}", place);
                break;
        }
    }
    cur_section_name.pop();
    if (!cur_section_name.empty()) {
        cur_section = codes[cur_section_name.top()];
    }
}

void CodeCollector::output() {
    for (auto sid : section_order) {
        spdlog::debug("section[{}]-----", sid);
        for (auto str : *codes[sid]) {
            spdlog::debug(str);
        }
    }
}

void CodeCollector::clear() { codes.clear(); }