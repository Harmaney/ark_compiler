
#include "gen.h"

#include <algorithm>
#include <cassert>
#include <iostream>

#include "data.h"
#include "logger.hpp"

std::string mapVariableType(SymbolDescriptor *type) { return type->name; }

void putVariableDecl(VariableDescriptor *var) {
    // TODO: handle different type
    CodeCollector::src() << mapVariableType(var->varType);
    if (var->isRef) CodeCollector::src() << "*";
    CodeCollector::src() << " " << var->name;
}

void putVariableExpr(VariableDescriptor *var) {
    CodeCollector::src()<<"(";
    if (var->isRef) CodeCollector::src() << "*";
    CodeCollector::src() << var->name<<")";
}

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
            "write",
            {new VariableDescriptor(
                "x", SymbolTable::lookforType(TYPE_BASIC_INT), false, false)},
            SymbolTable::lookforType(TYPE_BASIC_VOID)));
    CodeCollector::push_back("void write_str(string x){cout<<x;}");
    SymbolTable::insertType(
        "write_str", new FunctionDescriptor(
                         "write",
                         {new VariableDescriptor(
                             "x", SymbolTable::lookforType(TYPE_BASIC_STRING),
                             false, false)},
                         SymbolTable::lookforType(TYPE_BASIC_VOID)));
    CodeCollector::push_back("int read_int(){int x;cin>>x;return x;}");
    SymbolTable::insertType(
        "read_int",
        new FunctionDescriptor("read_int", {},
                               SymbolTable::lookforType(TYPE_BASIC_INT)));
    CodeCollector::end_section();
}

void ASTDispatcher::genGlobalEnd(GlobalAST *ast) {
    // TODO:
}

void ASTDispatcher::genArrayTypeDecl(ArrayTypeDeclAST *ast) {
    auto descriptor = SymbolTable::create_array_type(ast->itemAST->_descriptor,
                                                     ast->rangeR - ast->rangeL);

    // this is wrong when using custom type
    // to fix this, you should think about how to resolve typedef in struct too
    DEBUG(std::cerr<<"generate typedef"<<std::endl;)
    CodeCollector::begin_section("pre_array");
    CodeCollector::src() << "typedef " << descriptor->itemDescriptor->name
                         << " " << descriptor->name << "[" << descriptor->sz
                         << "];";
    CodeCollector::push_back();
    CodeCollector::end_section();
    ast->_descriptor = descriptor;
}

void ASTDispatcher::genPointerTypeDecl(PointerTypeDeclAST *ast) {
    auto descriptor = SymbolTable::create_pointer_type(ast->ref->_descriptor);

    CodeCollector::begin_section("prelude");
    CodeCollector::src() << "typedef " << descriptor->ref->name << "* "
                         << descriptor->name << ";";
    CodeCollector::push_back();
    CodeCollector::end_section();
    ast->_descriptor = descriptor;
}

void ASTDispatcher::genBasicType(BasicTypeAST *ast) {
    auto descriptor = SymbolTable::lookforType(ast->varType);
    if (descriptor == nullptr) {
        throw std::invalid_argument("undefined type " + ast->varType);
        ;
    }
    ast->_descriptor = descriptor;
}

void ASTDispatcher::genNumberExpr(NumberExprAST *ast) {
    VariableDescriptor *t = nullptr;

    switch (ast->const_type) {
        case CONSTANT_INT:
            t = SymbolTable::createVariableG(
                SymbolTable::lookforType(TYPE_BASIC_INT), false);
            break;
        case CONSTANT_REAL:
            t = SymbolTable::createVariableG(
                SymbolTable::lookforType(TYPE_BASIC_DOUBLE), false);
            break;
        default:
            throw std::invalid_argument("unknown constant type: " +
                                        ast->const_type);
            break;
    }

    CodeCollector::begin_section("global_define");
    CodeCollector::src() << mapVariableType(t->varType) << " " << t->name
                         << "=";
    if (ast->const_type == CONSTANT_REAL)
        CodeCollector::src() << ast->val_float << ";";
    if (ast->const_type == CONSTANT_INT)
        CodeCollector::src() << ast->val_int << ";";
    CodeCollector::push_back();
    CodeCollector::end_section();
    ast->value = t;
}

void ASTDispatcher::genStringExpr(StringExprAST *ast) {
    VariableDescriptor *t = SymbolTable::createVariableG(
        SymbolTable::lookforType(TYPE_BASIC_STRING), false);

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
        throw std::invalid_argument("undefined variable");
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

    if (ast->op == "=") {
        if (lhs->varType != rhs->varType) {
            throw std::invalid_argument("type does not match between `=`");
        }
        putVariableExpr(lhs);
        CodeCollector::src() << "=";
        putVariableExpr(rhs);
        CodeCollector::src() << ";";
        CodeCollector::push_back();
        // TODO: pascal 的 = 是否有返回值？

    } else if (ast->op == "[]") {
        if (lhs->varType->type != DESCRIPTOR_ARRAY) {
            throw std::domain_error("try to use operator [] on invalid type");
        }

        auto array = static_cast<ArrayTypeDescriptor *>(lhs->varType);

        VariableDescriptor *t =
            SymbolTable::createVariable(array->itemDescriptor, true);
        putVariableDecl(t);
        CodeCollector::src() << ";";
        CodeCollector::push_back();

        // FIX: too ugly
        CodeCollector::src() << t->name;
        CodeCollector::src() << "=&";
        putVariableExpr(lhs);
        CodeCollector::src() << "[";
        putVariableExpr(rhs);
        CodeCollector::src() << "];";
        CodeCollector::push_back();
        ast->value = t;
    } else if (ast->op == ".") {
        if (lhs->varType->type != DESCRIPTOR_STRUCT) {
            throw std::invalid_argument(
                "try to use operator `.` on invalid variable");
        }
        if (ast->RHS->type != AST_STRING_EXPR) {
            throw std::invalid_argument("operator `.` takes a string as arg 2");
        }
        auto child_id = static_cast<StringExprAST *>(ast->RHS)->val;
        auto array = static_cast<StructDescriptor *>(lhs->varType);
        if (!array->refVar.count(child_id)) {
            throw std::invalid_argument("no member `" + child_id +
                                        "` in struct `" + array->name + "`");
        }
        VariableDescriptor *t =
            SymbolTable::createVariable(array->refVar[child_id], true);

        putVariableDecl(t);
        CodeCollector::src() << ";";
        CodeCollector::push_back();

        // FIX: too ugly
        CodeCollector::src() << t->name;
        CodeCollector::src() << "=&(";
        putVariableExpr(lhs);
        CodeCollector::src() << "." << child_id;
        CodeCollector::src() << ");";
        CodeCollector::push_back();

        ast->value = t;
    } else {
        // FIX: calculate type
        VariableDescriptor *t =
            SymbolTable::createVariable(lhs->varType, false);
        putVariableDecl(t);
        CodeCollector::src() << ";";
        CodeCollector::push_back();

        putVariableExpr(t);
        CodeCollector::src() << "=";
        putVariableExpr(lhs);
        CodeCollector::src() << ast->op;
        putVariableExpr(rhs);
        CodeCollector::src() << ";";
        CodeCollector::push_back();
        ast->value = t;
    }
}

void ASTDispatcher::genUnaryExpr(UnaryExprAST *ast) {
    VariableDescriptor *var =
        std::any_cast<VariableDescriptor *>(ast->expr->value);

    if (ast->op == "*") {
        if (var->varType->type != DESCRIPTOR_POINTER_TYPE) {
            throw std::invalid_argument(
                "try to def a variable that is not a pointer");
        }
        VariableDescriptor *t = SymbolTable::createVariable(
            static_cast<PointerTypeDescriptor *>(var->varType)->ref, true);
        putVariableDecl(t);
        CodeCollector::src() << ";";
        CodeCollector::push_back();

        putVariableExpr(t);
        CodeCollector::src() << "=";
        putVariableExpr(var);
        CodeCollector::src() << ";";
        CodeCollector::push_back();

        ast->value = t;

    } else {
        throw std::invalid_argument("unknown unary operator");
    }
}

void ASTDispatcher::genCallExpr(CallExprAST *ast) {
    // check whether function exists.
    SymbolDescriptor *raw_descriptor = SymbolTable::lookforType(ast->callee);
    if (!raw_descriptor || raw_descriptor->type != DESCRIPTOR_FUNCTION) {
        throw std::invalid_argument("try to call invalid function " +
                                    ast->callee);
    }
    FunctionDescriptor *descriptor =
        static_cast<FunctionDescriptor *>(raw_descriptor);

    if (descriptor->args.size() != ast->args.size()) {
        throw std::invalid_argument("args not matched for function " +
                                    ast->callee);
    }

    // gen var to catch result val
    if (descriptor->resultDescriptor !=
        SymbolTable::lookforType(TYPE_BASIC_VOID)) {
        auto ret =
            SymbolTable::createVariable(descriptor->resultDescriptor, false);
    ast->value = ret;
        putVariableDecl(ret);
        CodeCollector::src() << ";";
        CodeCollector::push_back();

        putVariableExpr(ret);
    CodeCollector::src() << "=";
    }

    CodeCollector::src() << ast->callee << "(";
    for (int i = 0; i < ast->args.size(); i++) {
        auto d = std::any_cast<VariableDescriptor *>(ast->args[i]->value);
        if (d->varType != descriptor->args[i]->varType) {
            throw std::invalid_argument("wrong type of arg of function " +
                                        ast->callee + " at pos " +
                                        std::to_string(i));
        }
        // check whether the function wants ref or not
        // we use pointer to simulate this process in c
        CodeCollector::src()
            << (descriptor->args[i]->isRef ? "&" : "") << d->name;
        if (i != ast->args.size() - 1) CodeCollector::src() << ",";
    }

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
        throw std::invalid_argument(
            "the left range of `for` is not an integer");
    }
    if (ast->rangeR->const_type != CONSTANT_INT) {
        throw std::invalid_argument(
            "the right range of `for` is not an integer");
    }

    CodeCollector::src()
        << std::any_cast<VariableDescriptor *>(ast->itervar->value)->name
        << " = "
        << std::any_cast<VariableDescriptor *>(ast->rangeL->value)->name << ";";
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

void ASTDispatcher::genFunction(FunctionAST *ast) {
    // FIX: too ugly but work
    // the reason for this is that we should inject args descriptor to its block
    CodeCollector::src() << mapVariableType(ast->sig->_resultType) << " ";
    CodeCollector::src() << ast->sig->sig;
    CodeCollector::src() << "(";

    // and also there is need to create new node for processing 形参
    for (auto arg : ast->sig->args) {
        arg->varType->accept(*this);
        arg->_varType = arg->varType->_descriptor;
    }
    std::vector<VariableDescriptor *> argsDescriptors;
    for (int i = 0; i < (int)ast->sig->args.size() - 1; i++) {
        // FIX: too ugly
        auto tempDescriptor = new VariableDescriptor(
            ast->sig->args[i]->sig->name, ast->sig->args[i]->_varType,
            ast->sig->args[i]->isRef, false);
        argsDescriptors.push_back(tempDescriptor);

        putVariableDecl(tempDescriptor);
        CodeCollector::src() << ", ";
    }
    if (!ast->sig->args.empty()) {
        auto tempDescriptor = new VariableDescriptor(
            ast->sig->args.back()->sig->name, ast->sig->args.back()->_varType,
            ast->sig->args.back()->isRef, false);
        argsDescriptors.push_back(tempDescriptor);

        putVariableDecl(tempDescriptor);
    }
    CodeCollector::src() << ")";
    CodeCollector::push_back();

    // add symbol for function
    SymbolTable::insertType(
        ast->sig->sig, new FunctionDescriptor(ast->sig->sig, argsDescriptors,
                                              ast->sig->_resultType));

    SymbolTable::enter();
    for (auto arg : ast->sig->args) {
        SymbolTable::createVariable(arg->sig->name, arg->_varType, arg->isRef);
    }
    // I am nearly throwing up
    ast->body->accept(*this);
    SymbolTable::exit();
}

void ASTDispatcher::genBlockBegin(BlockAST *ast) {
    CodeCollector::push_back("{");
}

void ASTDispatcher::genBlockEnd(BlockAST *ast) {
    CodeCollector::push_back("}");
}

void ASTDispatcher::genStruct(StructDeclAST *ast) {
    StructDescriptor *structD = new StructDescriptor(ast->sig, {});

    // 解决循环嵌套
    CodeCollector::begin_section("pre_struct");
    CodeCollector::src() << "struct " << ast->sig << ";";
    CodeCollector::push_back();
    CodeCollector::end_section();

    // 为了解决结构中有数组定义时，数组类型被限制在其内部，无法被外界使用的问题
    CodeCollector::begin_section("struct");
    SymbolTable::enter();
    CodeCollector::src() << "struct " << ast->sig << "{";
    CodeCollector::push_back();

    for (auto var : ast->varDecl) {
        // FIX: fuck this
        var->accept(*this);
        auto typeDescriptor = var->_varType;
        structD->push(var->sig->name, typeDescriptor);
    }

    CodeCollector::push_back("};");
    SymbolTable::exit();
    CodeCollector::end_section();
    SymbolTable::insertType(ast->sig, structD);
}

void ASTDispatcher::genVariableDecl(VariableDeclAST *ast) {
    ast->_varType = ast->varType->_descriptor;
    auto var =
        SymbolTable::createVariable(ast->sig->name, ast->_varType, ast->isRef);
    putVariableDecl(var);
    CodeCollector::src() << ";";
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
                throw std::invalid_argument(
                    "unknown inserting place in `end_section` of "
                    "CodeCollector");
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
        DEBUG(std::cerr << "// section " << sid << std::endl;)
        for (auto str : *codes[sid]) {
            DEBUG(std::cerr << str << std::endl;)
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
        WARN(std::cerr << "rearranging section `" << section
                       << "` failed: no such section" << std::endl;)
    }
}

void CodeCollector::clear() { codes.clear(); }