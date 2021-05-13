
#include "gen.h"

#include <algorithm>
#include <iostream>

#include "data.h"
#include "spdlog/spdlog.h"

void ASTDispatcher::genGlobalBegin(GlobalAST *ast) {
    // TODO:
    CodeCollector::begin_section("prelude");
    CodeCollector::push_back("#include <iostream>");
    CodeCollector::push_back("#include <string>");
    CodeCollector::push_back("using namespace std;");
    CodeCollector::push_back("void write_int(int x){cout<<x;}");
    SymbolTable::insertType(
        "write_int",
        new FunctionDescriptor(
            "write", {new VariableDescriptor("x", SymbolTable::lookforType(TYPE_BASIC_INT), false)},
            SymbolTable::lookforType("int")));
    CodeCollector::push_back("void write_str(string x){cout<<x;}");
    SymbolTable::insertType(
        "write_str",
        new FunctionDescriptor(
            "write", {new VariableDescriptor("x", SymbolTable::lookforType(TYPE_BASIC_STRING), false)},
            SymbolTable::lookforType("int")));
    CodeCollector::push_back("int read_int(){int x;cin>>x;return x;}");
    SymbolTable::insertType(
        "read_int",
        new FunctionDescriptor("read", {},
                               SymbolTable::lookforType(TYPE_BASIC_STRING)));
    CodeCollector::end_section();
}

void ASTDispatcher::genGlobalEnd(GlobalAST *ast) {
    // TODO:
}

void ASTDispatcher::genArrayTypeDecl(ArrayTypeDeclAST *ast) {
    auto descriptor = SymbolTable::create_array_type(ast->itemAST->_descriptor,
                                                     ast->rangeR - ast->rangeL);

    CodeCollector::begin_section("prelude");
    CodeCollector::src()<<"typedef "<<descriptor->itemDescriptor->name<<" "<<descriptor->name<<"["<<descriptor->sz<<"];";
    CodeCollector::push_back();
    CodeCollector::end_section();
    ast->_descriptor = descriptor;
}

void ASTDispatcher::genBasicType(BasicTypeAST *ast) {
    auto descriptor = SymbolTable::lookforType(ast->varType);
    if (descriptor == nullptr) {
        spdlog::error("undefined type `{}`, you basterd", ast->varType);
    }
    ast->_descriptor = descriptor;
}

void ASTDispatcher::genNumberExpr(NumberExprAST *ast) {
    VariableDescriptor *t = nullptr;

    switch (ast->const_type) {
        case CONSTANT_INT:
            t = SymbolTable::createVariableG(SymbolTable::lookforType(TYPE_BASIC_INT));
            break;
        case CONSTANT_REAL:
            t = SymbolTable::createVariableG(SymbolTable::lookforType(TYPE_BASIC_DOUBLE));
            break;
        default:
            spdlog::error("unknown constant type: {}", ast->const_type);
            break;
    }

    CodeCollector::begin_section("global_define");
    CodeCollector::src() << mapVariableType(t->varType) << " " << t->name << "=";
    if (ast->const_type == CONSTANT_REAL)
        CodeCollector::src() << ast->val_float << ";";
    if (ast->const_type == CONSTANT_INT)
        CodeCollector::src() << ast->val_int << ";";
    CodeCollector::push_back();
    CodeCollector::end_section();
    ast->value = t;
}

void ASTDispatcher::genStringExpr(StringExprAST *ast) {
    VariableDescriptor *t = SymbolTable::createVariableG(SymbolTable::lookforType(TYPE_BASIC_STRING));

    CodeCollector::begin_section("global_define");
    CodeCollector::src() << mapVariableType(t->varType) << " " << t->name
                         << " = \"" << ast->val << "\";";
    CodeCollector::push_back();
    CodeCollector::end_section();
    ast->value = t;
}

void ASTDispatcher::genVariableExpr(VariableExprAST *ast) {
    VariableDescriptor *t = SymbolTable::lookforVariable(ast->name);
    if (!t) {
        spdlog::error("variable {} not found", ast->name);
        return;
    }
    ast->value = t;
}

void ASTDispatcher::genReturn(ReturnAST *ast) {
    CodeCollector::src()
        << "return "
        << std::any_cast<VariableDescriptor *>(ast->expr->value)->name << ";";
    CodeCollector::push_back();
}

void ASTDispatcher::genBinaryExpr(BinaryExprAST *ast) {
    VariableDescriptor *lhs =
        std::any_cast<VariableDescriptor *>(ast->LHS->value);
    VariableDescriptor *rhs =
        std::any_cast<VariableDescriptor *>(ast->RHS->value);

    switch (ast->op) {
        case '=': {
            CodeCollector::src() << lhs->name << "=" << rhs->name << ";";
            CodeCollector::push_back();
            // TODO: pascal 的 = 是否有返回值？
            break;
        }

        default: {
            VariableDescriptor *t =
                SymbolTable::createVariable(SymbolTable::lookforType(TYPE_BASIC_DOUBLE));
            genVariable(t);

            CodeCollector::src()
                << t->name << "=" << lhs->name << ast->op << rhs->name << ";";
            CodeCollector::push_back();
            ast->value = t;
            break;
        }
    }
}

void ASTDispatcher::genCallExpr(CallExprAST *ast) {
    // check whether function exists.
    SymbolDescriptor *raw_descriptor = SymbolTable::lookforType(ast->callee);
    if (!raw_descriptor || raw_descriptor->type != DESCRIPTOR_FUNCTION) {
        spdlog::error("try to call invalid function `{}`, you basterd.",
                      ast->callee);
    }
    FunctionDescriptor *descriptor =
        static_cast<FunctionDescriptor *>(raw_descriptor);

    if (descriptor->args.size() != ast->args.size()) {
        spdlog::error("lose args for function `{}`: {}/{}, you basterd.",
                      ast->callee, ast->args.size(), descriptor->args.size());
    }

    CodeCollector::src() << ast->callee << "(";
    for (int i = 0; i < ast->args.size() - 1; i++) {
        if (std::any_cast<VariableDescriptor *>(ast->args[i]->value)->varType !=
            descriptor->args[i]->varType) {
            spdlog::error(
                "wrong type of arg for function `{}` at place {}, you basterd.",
                ast->callee, i);
        }
        CodeCollector::src()
            << std::any_cast<VariableDescriptor *>(ast->args[i]->value)->name
            << ",";
    }
    if (std::any_cast<VariableDescriptor *>(ast->args.back()->value)->varType !=
        descriptor->args.back()->varType) {
        spdlog::error(
            "wrong type of arg for function `{}` at place {}, you basterd.",
            ast->callee, ast->args.size() - 1);
    }
    CodeCollector::src()
        << std::any_cast<VariableDescriptor *>(ast->args.back()->value)->name;
    CodeCollector::src() << ");";
    CodeCollector::push_back();
}

void ASTDispatcher::genIfStatementBegin(IfStatementAST *ast) {
    CodeCollector::src()
        << "if ("
        << std::any_cast<VariableDescriptor *>(ast->condition->value)->name
        << ")";

    std::string *L0 = TagTable::createTagG();
    CodeCollector::src() << "goto " << *L0 << ";";
    ast->extraData["L0"] = L0;
}

void ASTDispatcher::genForStatementBegin(ForStatementAST *ast) {
    ast->extraData["end"] = TagTable::createTagG();
    ast->extraData["begin"] = TagTable::createTagG();

    if (ast->rangeL->const_type != CONSTANT_INT) {
        spdlog::warn("the left range of `for` is not integer");
    }
    if (ast->rangeR->const_type != CONSTANT_INT) {
        spdlog::warn("the left range of `for` is not integer");
    }

    CodeCollector::src()
        << std::any_cast<VariableDescriptor *>(ast->itervar->value)->name
        << " = " << std::any_cast<VariableDescriptor *>(ast->rangeL->value)->name
        << ";";
    CodeCollector::push_back();

    CodeCollector::src()
        << "if ("
        << std::any_cast<VariableDescriptor *>(ast->itervar->value)->name << ">"
        << std::any_cast<VariableDescriptor *>(ast->rangeR->value)->name << ")";
    CodeCollector::src() << "goto "
                         << *std::any_cast<std::string *>(ast->extraData["end"])
                         << ";";
    CodeCollector::push_back();

    CodeCollector::src() << *std::any_cast<std::string *>(
                                ast->extraData["begin"])
                         << ":";
    CodeCollector::push_back();
}

void ASTDispatcher::genForStatementEnd(ForStatementAST *ast) {
    CodeCollector::src()
        << std::any_cast<VariableDescriptor *>(ast->itervar->value)->name
        << "++;";
    CodeCollector::push_back();

    CodeCollector::src()
        << "if ("
        << std::any_cast<VariableDescriptor *>(ast->itervar->value)->name
        << "<=" << std::any_cast<VariableDescriptor *>(ast->rangeR->value)->name
        << ")";
    CodeCollector::src() << "goto "
                         << *std::any_cast<std::string *>(
                                ast->extraData["begin"])
                         << ";";
    CodeCollector::push_back();

    CodeCollector::src() << *std::any_cast<std::string *>(ast->extraData["end"])
                         << ":";
    CodeCollector::push_back();
}

void ASTDispatcher::genWhileStatementBegin(WhileStatementAST *ast) {
    CodeCollector::src()
        << "if (!"
        << std::any_cast<VariableDescriptor *>(ast->condition->value)->name
        << ")";

    std::string *L0 = TagTable::createTagG();
    CodeCollector::src() << "goto " << *L0 << ";";
    CodeCollector::push_back();
    ast->extraData["end"] = L0;

    std::string *begin = TagTable::createTagG();
    CodeCollector::src() << *begin << ":";
    CodeCollector::push_back();
    ast->extraData["begin"] = begin;
}

void ASTDispatcher::genWhileStatementEnd(WhileStatementAST *ast) {
    CodeCollector::src()
        << "if ("
        << std::any_cast<VariableDescriptor *>(ast->condition->value)->name
        << ")";
    CodeCollector::src() << "goto "
                         << *std::any_cast<std::string *>(
                                ast->extraData["begin"])
                         << ";";

    CodeCollector::push_back();
    CodeCollector::src() << *std::any_cast<std::string *>(ast->extraData["end"])
                         << ":";
    CodeCollector::push_back();
}

void ASTDispatcher::genFunctionSignature(FunctionSignatureAST *ast) {
    CodeCollector::src() << mapVariableType(ast->resultType) << " ";
    CodeCollector::src() << ast->sig;
    CodeCollector::src() << "(";
    for (int i = 0; i < (int)ast->args.size() - 1; i++) {
        CodeCollector::src() << mapVariableType(ast->args[i]->_varType) << " "
                             << ast->args[i]->sig->name << ", ";
    }
    if (!ast->args.empty())
        CodeCollector::src() << mapVariableType(ast->args.back()->_varType)
                             << " " << ast->args.back()->sig->name;
    CodeCollector::src() << ")";
    CodeCollector::push_back();
}

void ASTDispatcher::genBlockBegin(BlockAST *ast) {
    CodeCollector::push_back("{");
}

void ASTDispatcher::genBlockEnd(BlockAST *ast) {
    CodeCollector::push_back("}");
}

void ASTDispatcher::genStruct(StructDeclAST *ast) {
    StructDescriptor *structD = new StructDescriptor(ast->sig,{});

    CodeCollector::src() << "struct " << ast->sig << "{";
    CodeCollector::push_back();

    for (auto var : ast->varDecl) {
        // FIX: fuck this
        var->accept(*this);
        auto typeDescriptor = var->_varType;
        structD->push(var->sig->name, typeDescriptor);
    }

    SymbolTable::insertType(ast->sig, structD);
    CodeCollector::push_back("};");
}

void ASTDispatcher::genVariableDecl(VariableDeclAST *ast) {
    if (ast->varType.type() == typeid(BasicTypeAST *)) {
        ast->_varType =
            std::any_cast<BasicTypeAST *>(ast->varType)->_descriptor;
    } else if (ast->varType.type() == typeid(ArrayTypeDeclAST *)) {
        ast->_varType =
            std::any_cast<ArrayTypeDeclAST *>(ast->varType)->_descriptor;
    }
    auto var = SymbolTable::createVariable(ast->sig->name, ast->_varType);
    genVariable(var);
}

std::string mapVariableType(SymbolDescriptor *type) {
    return type->name;
}

void genVariable(VariableDescriptor *var) {
    // TODO: handle different type
    CodeCollector::src() << mapVariableType(var->varType) << " " << var->name
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

void CodeCollector::output(std::ostream &out) {
    for (auto sid : section_order) {
        out << "//" << sid << std::endl;
        for (auto str : *codes[sid]) {
            out << str << std::endl;
        }
    }
}

void CodeCollector::rearrange_section(std::vector<std::string> order) {
    section_order = order;
}
void CodeCollector::rearrange_section(std::string section, int newPos) {
    if (std::find(section_order.begin(), section_order.end(), section) !=
        section_order.end()) {
        section_order.erase(
            std::find(section_order.begin(), section_order.end(), section));
        section_order.insert(section_order.begin() + newPos, section);
    } else {
        spdlog::warn("rearranging section[{}] failed: no such section",
                     section);
    }
}

void CodeCollector::clear() { codes.clear(); }