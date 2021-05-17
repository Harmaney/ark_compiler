
#include "gen.h"

#include <algorithm>
#include <cassert>
#include <iostream>

#include "data.h"
#include "err.h"
#include "logger.h"

std::string mapVariableType(SymbolDescriptor *type) {
    if (type->name == TYPE_BASIC_DOUBLE) {
        return "double";
    } else if (type->name == TYPE_BASIC_INT) {
        return "int";
    } else if (type->name == TYPE_BASIC_STRING) {
        return "string";
    } else if (type->name == TYPE_BASIC_LONGINT) {
        return "int";
    } else if (type->name == TYPE_BASIC_INT64) {
        return "long long";
    } else {
        return type->name;
        // throw std::invalid_argument("unknown map type "+type->name);
    }
}

bool isPossibleToLevelUp(SymbolDescriptor *src, SymbolDescriptor *target) {
    std::vector<SymbolDescriptor *> level;
    level.push_back(SymbolTable::lookforType(TYPE_BASIC_INT));
    level.push_back(SymbolTable::lookforType(TYPE_BASIC_LONGINT));
    level.push_back(SymbolTable::lookforType(TYPE_BASIC_INT64));

    bool checked = false;
    for (auto ty : level) {
        if (src == ty) checked = true;
        if (checked && target == ty) return true;
    }
    return false;
}

void putVariableDecl(VariableDescriptor *var) {
    // TODO: handle different type
    CodeCollector::src() << mapVariableType(var->varType);
    if (var->isRef) CodeCollector::src() << "*";
    CodeCollector::src() << " " << var->name;
}

void putVariableExpr(VariableDescriptor *var) {
    CodeCollector::src() << "(";
    if (var->isRef) CodeCollector::src() << "*";
    CodeCollector::src() << var->name << ")";
}

void ASTDispatcher::genGlobalBegin(GlobalAST *ast) {
    // TODO:
    CodeCollector::begin_section("prelude");

    CodeCollector::push_back("#include <iostream>");
    CodeCollector::push_back("#include <string>");
    CodeCollector::push_back("using namespace std;");
    CodeCollector::push_back("int ____nouse=1;");

    // write 以及其符号
    CodeCollector::push_back("void write_integer(int x){cout<<x;}");
    SymbolTable::insertFunction(
        "write",
        new FunctionDescriptor(
            "write",
            {new VariableDescriptor(
                "x", SymbolTable::lookforType(TYPE_BASIC_INT), false, false)},
            SymbolTable::lookforType(TYPE_BASIC_VOID)));

    CodeCollector::push_back("void write_int64(long long x){cout<<x;}");
    SymbolTable::insertFunction(
        "write",
        new FunctionDescriptor(
            "write",
            {new VariableDescriptor(
                "x", SymbolTable::lookforType(TYPE_BASIC_INT64), false, false)},
            SymbolTable::lookforType(TYPE_BASIC_VOID)));

    CodeCollector::push_back(
        "void write_integer_string(int a1,string a2){cout<<a1<<\" \"<<a2;}");
    SymbolTable::insertFunction(
        "write",
        new FunctionDescriptor(
            "write",
            {new VariableDescriptor(
                 "a1", SymbolTable::lookforType(TYPE_BASIC_INT), false, false),
             new VariableDescriptor("a2",
                                    SymbolTable::lookforType(TYPE_BASIC_STRING),
                                    false, false)},
            SymbolTable::lookforType(TYPE_BASIC_VOID)));

    // read int以及其符号
    CodeCollector::push_back("void read_integer(int *x){scanf(\"%d\",x);}");
    SymbolTable::insertFunction(
        "read",
        new FunctionDescriptor(
            "read",
            {new VariableDescriptor(
                "x", SymbolTable::lookforType(TYPE_BASIC_INT), true, false)},
            SymbolTable::lookforType(TYPE_BASIC_VOID)));

    CodeCollector::push_back(
        "void read_int64(long long *x){scanf(\"%lld\",x);}");
    SymbolTable::insertFunction(
        "read",
        new FunctionDescriptor(
            "read",
            {new VariableDescriptor(
                "x", SymbolTable::lookforType(TYPE_BASIC_INT64), true, false)},
            SymbolTable::lookforType(TYPE_BASIC_VOID)));

    CodeCollector::end_section();
}

void ASTDispatcher::genGlobalEnd(GlobalAST *ast) {
    // TODO:
}

void ASTDispatcher::genArrayTypeDecl(ArrayTypeDeclAST *ast) {
    // 检查数组区间是否是整数
    if (ast->rangeL->const_type != CONSTANT_INT ||
        ast->rangeR->const_type != CONSTANT_INT) {
        throw TypeErrorException("wrong type in range of array",
                                 std::to_string(ast->rangeL->const_type),
                                 TYPE_BASIC_INT, 0, 0);
    }

    auto descriptor = SymbolTable::create_array_type(
        ast->itemAST->_descriptor, ast->rangeR->val_int - ast->rangeL->val_int);

    // this is wrong when using custom type
    // to fix this, you should think about how to resolve typedef in struct too
    DEBUG(std::cerr << "generate typedef" << std::endl;)
    CodeCollector::begin_section("pre_array");
    CodeCollector::src() << "typedef "
                         << mapVariableType(descriptor->itemDescriptor) << " "
                         << descriptor->name << "[" << descriptor->sz << "];";
    CodeCollector::push_back();
    CodeCollector::end_section();
    ast->_descriptor = descriptor;
}

void ASTDispatcher::genPointerTypeDecl(PointerTypeDeclAST *ast) {
    auto descriptor = SymbolTable::create_pointer_type(ast->ref->_descriptor);

    CodeCollector::begin_section("prelude");
    CodeCollector::src() << "typedef " << mapVariableType(descriptor->ref)
                         << "* " << descriptor->name << ";";
    CodeCollector::push_back();
    CodeCollector::end_section();
    ast->_descriptor = descriptor;
}

void ASTDispatcher::genTypeDef(TypeDefAST *ast) {
    // 这看起来不太靠谱
    SymbolTable::insertType(ast->newName->varType, ast->oldName->_descriptor);

    CodeCollector::src() << "typedef "
                         << mapVariableType(ast->oldName->_descriptor) << " "
                         << ast->newName->varType << ";";
    CodeCollector::push_back();
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
                SymbolTable::lookforType(TYPE_BASIC_INT64), false);
            t->isLeftVar = false;
            break;
        case CONSTANT_REAL:
            t = SymbolTable::createVariableG(
                SymbolTable::lookforType(TYPE_BASIC_DOUBLE), false);
            t->isLeftVar = false;
            break;
        default:
            throw InternalErrorException(
                "unknown constant type: " + ast->const_type, 0, 0);
            break;
    }

    CodeCollector::begin_section("global_define");
    CodeCollector::src() << mapVariableType(t->varType) << " " << t->name
                         << "=";
    if (ast->const_type == CONSTANT_REAL)
        CodeCollector::src() << ast->val_float << ";";
    else if (ast->const_type == CONSTANT_INT)
        CodeCollector::src() << ast->val_int << ";";
    else {
        throw InternalErrorException(
            "unknown constant type: " + ast->const_type, 0, 0);
    }
    CodeCollector::push_back();
    CodeCollector::end_section();
    ast->value = t;
}

void ASTDispatcher::genStringExpr(StringExprAST *ast) {
    VariableDescriptor *t = SymbolTable::createVariableG(
        SymbolTable::lookforType(TYPE_BASIC_STRING), false);
    t->isLeftVar = false;

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
        // for pascal's strange return
        t = SymbolTable::lookforVariable("__ret");
        if (!t)
            throw SymbolUndefinedException("undefined variable", ast->name, 0,
                                           0);
    }

    ast->value = t;
}

void ASTDispatcher::genReturn(ReturnAST *ast) {
    if (ast->expr != nullptr) {
        CodeCollector::src()
            << "return "
            << std::any_cast<VariableDescriptor *>(ast->expr->value)->name
            << ";";
    } else {
        CodeCollector::src() << "return "
                             << ";";
    }

    CodeCollector::push_back();
}

void ASTDispatcher::genBinaryExpr(BinaryExprAST *ast) {
    VariableDescriptor *lhs =
        std::any_cast<VariableDescriptor *>(ast->LHS->value);
    VariableDescriptor *rhs =
        std::any_cast<VariableDescriptor *>(ast->RHS->value);

    if (ast->op == ":=") {
        if (lhs->varType != rhs->varType &&
            isPossibleToLevelUp(rhs->varType, lhs->varType)) {
            throw TypeErrorException("type does not match between `=`",
                                     rhs->varType->name, lhs->varType->name, 0,
                                     0);
        }
        if (lhs->isLeftVar == false) {
            throw TypeErrorException("try using `=` on right value",
                                     "right val", "left val", 0, 0);
        }
        putVariableExpr(lhs);
        CodeCollector::src() << "=";
        putVariableExpr(rhs);
        CodeCollector::src() << ";";
        CodeCollector::push_back();
        // TODO: pascal 的 = 是否有返回值？

    } else if (ast->op == "[]") {
        if (lhs->varType->type != DESCRIPTOR_ARRAY) {
            throw TypeErrorException("try to use operator [] on invalid type",
                                     std::to_string(lhs->varType->type),
                                     std::to_string(DESCRIPTOR_ARRAY), 0, 0);
        }

        auto array = static_cast<ArrayTypeDescriptor *>(lhs->varType);

        VariableDescriptor *t =
            SymbolTable::createVariable(array->itemDescriptor, true, false);
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
            throw TypeErrorException(
                "try to use operator `.` on invalid variable",
                lhs->varType->name, "a record", 0, 0);
        }
        if (ast->RHS->type != AST_STRING_EXPR) {
            throw TypeErrorException("operator `.` takes a string as arg 2",
                                     std::to_string(ast->RHS->type), "a string",
                                     0, 0);
        }
        auto child_id = static_cast<StringExprAST *>(ast->RHS)->val;
        auto array = static_cast<StructDescriptor *>(lhs->varType);
        if (!array->refVar.count(child_id)) {
            throw SymbolUndefinedException("no member in record", child_id, 0,
                                           0);
        }
        VariableDescriptor *t =
            SymbolTable::createVariable(array->refVar[child_id], true, false);

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
        if (ast->op == "<>") ast->op = "!=";
        if (ast->op == "=") ast->op = "==";
        if (ast->op == "div") ast->op = "/";
        if (ast->op == "and") ast->op = "&&";
        if (ast->op == "or") ast->op = "||";
        if (ast->op == "-" || ast->op == "+" || ast->op == "*" ||
            ast->op == "/" || ast->op == "==" || ast->op == "!=" ||
            ast->op == "<" || ast->op == ">" || ast->op == "<=" ||
            ast->op == ">=" || ast->op == "||" || ast->op == "&&") {
            // FIX: calculate type
            if (lhs->varType != rhs->varType) {
                throw TypeErrorException("type of objects does not match",
                                         rhs->varType->name, lhs->varType->name,
                                         0, 0);
            }
            VariableDescriptor *t =
                SymbolTable::createVariable(lhs->varType, false, false);
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
        } else {
            throw UndefinedBehaviorException("unknown operator " + ast->op, 0,
                                             0);
        }
    }
}

void ASTDispatcher::genUnaryExpr(UnaryExprAST *ast) {
    VariableDescriptor *var =
        std::any_cast<VariableDescriptor *>(ast->expr->value);

    if (ast->op == "*") {
        if (var->varType->type != DESCRIPTOR_POINTER_TYPE) {
            throw TypeErrorException(
                "try to def a variable that is not a pointer",
                var->varType->name, "a pointer", 0, 0);
        }
        VariableDescriptor *t = SymbolTable::createVariable(
            static_cast<PointerTypeDescriptor *>(var->varType)->ref, true,
            false);
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
        throw UndefinedBehaviorException("unknown unary operator, " + ast->op,
                                         0, 0);
    }
}

void ASTDispatcher::genCallExpr(CallExprAST *ast) {
    // SPECIAL for exit()
    if (ast->callee == "exit") {
        if (SymbolTable::lookforVariable("__ret")) {
            auto t = new ReturnAST(new VariableExprAST("__ret"));
            t->accept(*this);
        } else {
            auto t = new ReturnAST();
            t->accept(*this);
        }
        return;
    }

    // check whether function exists.
    std::vector<SymbolDescriptor *> argsimbols;
    for (auto arg : ast->args) {
        argsimbols.push_back(
            std::any_cast<VariableDescriptor *>(arg->value)->varType);
    }
    SymbolDescriptor *raw_descriptor =
        SymbolTable::lookforFunction(ast->callee, argsimbols);

    if (!raw_descriptor || raw_descriptor->type != DESCRIPTOR_FUNCTION) {
        throw SymbolUndefinedException("try to call undefined function",
                                       ast->callee, 0, 0);
    }
    FunctionDescriptor *descriptor =
        static_cast<FunctionDescriptor *>(raw_descriptor);

    if (descriptor->args.size() != ast->args.size()) {
        throw TypeErrorException("args not matched for function " + ast->callee,
                                 std::to_string(ast->args.size()),
                                 std::to_string(descriptor->args.size()), 0, 0);
    }

    // gen var to catch result val
    if (descriptor->resultDescriptor !=
        SymbolTable::lookforType(TYPE_BASIC_VOID)) {
        auto ret = SymbolTable::createVariable(descriptor->resultDescriptor,
                                               false, false);
        ast->value = ret;
        putVariableDecl(ret);
        CodeCollector::src() << ";";
        CodeCollector::push_back();

        putVariableExpr(ret);
        CodeCollector::src() << "=";
    }

    CodeCollector::src() << descriptor->name << "(";
    for (int i = 0; i < ast->args.size(); i++) {
        auto d = std::any_cast<VariableDescriptor *>(ast->args[i]->value);
        if (d->varType != descriptor->args[i]->varType) {
            throw TypeErrorException(
                "wrong type of arg of function " + ast->callee,
                d->varType->name, descriptor->args[i]->varType->name, 0, 0);
        }
        // check whether the function wants ref or not
        // we use pointer to simulate this process in c
        CodeCollector::src() << (descriptor->args[i]->isRef ? "&" : "");
        putVariableExpr(d);
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
    CodeCollector::push_back();
    ast->extraData["L0"] = L0;
}

void ASTDispatcher::genIfStatementElse(IfStatementAST *ast) {
    std::string *END = TagTable::createTagG();
    CodeCollector::src() << "goto " << *END << ";";
    CodeCollector::push_back();
    ast->extraData["END"] = END;
    CodeCollector::src() << *std::any_cast<std::string *>(ast->extraData["L0"])
                         << ": ____nouse=1;;";
    CodeCollector::push_back();
}

void ASTDispatcher::genIfStatementEnd(IfStatementAST *ast) {
    CodeCollector::src() << *std::any_cast<std::string *>(ast->extraData["END"])
                         << ": ____nouse=1;;";
    CodeCollector::push_back();
}

void ASTDispatcher::genForStatementBegin(ForStatementAST *ast) {
    ast->extraData["end"] = TagTable::createTagG();
    ast->extraData["begin"] = TagTable::createTagG();

    if (std::any_cast<VariableDescriptor *>(ast->rangeL->value)->varType !=
            SymbolTable::lookforType(TYPE_BASIC_INT64) &&
        isPossibleToLevelUp(
            std::any_cast<VariableDescriptor *>(ast->rangeL->value)->varType,
            SymbolTable::lookforType(TYPE_BASIC_INT64))) {
        throw TypeErrorException(
            "left range of `for` is not an integer",
            std::any_cast<VariableDescriptor *>(ast->rangeL->value)
                ->varType->name,
            "a integer", 0, 0);
    }
    if (std::any_cast<VariableDescriptor *>(ast->rangeR->value)->varType !=
            SymbolTable::lookforType(TYPE_BASIC_INT64) &&
        isPossibleToLevelUp(
            std::any_cast<VariableDescriptor *>(ast->rangeR->value)->varType,
            SymbolTable::lookforType(TYPE_BASIC_INT64))) {
        throw TypeErrorException(
            "right range of `for` is not an integer",
            std::any_cast<VariableDescriptor *>(ast->rangeL->value)
                ->varType->name,
            "a integer", 0, 0);
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
                         << ": ____nouse=1;;";
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
                         << ": ____nouse=1;;";
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
    CodeCollector::src() << *begin << ": ____nouse=1;;";
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
                         << ": ____nouse=1;;";
    CodeCollector::push_back();
}

void ASTDispatcher::genFunctionSignature(FunctionSignatureAST *ast) {
    WARN(std::cerr << "todo: function sig end" << std::endl;)
}

void ASTDispatcher::genFunction(FunctionAST *ast) {
    // FIX: too ugly but work
    // the reason for this is that we should inject args descriptor to its block
    // 这段代码太裂了

    // add symbol for function
    // and also there is need to create new node for processing 形参
    for (auto arg : ast->sig->args) {
        arg->varType->accept(*this);
        arg->_varType = arg->varType->_descriptor;
    }

    std::vector<VariableDescriptor *> argsDescriptors;
    for (int i = 0; i < (int)ast->sig->args.size(); i++) {
        // FIX: too ugly
        auto tempDescriptor = new VariableDescriptor(
            ast->sig->args[i]->sig->name, ast->sig->args[i]->_varType,
            ast->sig->args[i]->isRef, false);
        argsDescriptors.push_back(tempDescriptor);
    }
    auto functiondesciptor = new FunctionDescriptor(
        ast->sig->sig, argsDescriptors, ast->sig->_resultType);
    SymbolTable::insertFunction(ast->sig->sig, functiondesciptor);

    CodeCollector::src() << mapVariableType(ast->sig->_resultType) << " ";
    CodeCollector::src() << functiondesciptor->name;
    CodeCollector::src() << "(";

    for (int i = 0; i < (int)ast->sig->args.size(); i++) {
        // FIX: too ugly
        auto tempDescriptor = new VariableDescriptor(
            ast->sig->args[i]->sig->name, ast->sig->args[i]->_varType,
            ast->sig->args[i]->isRef, false);
        putVariableDecl(tempDescriptor);
        if (i + 1 != ast->sig->args.size()) CodeCollector::src() << ", ";
    }
    CodeCollector::src() << ")";
    CodeCollector::push_back();

    SymbolTable::enter();
    for (auto arg : ast->sig->args) {
        SymbolTable::createVariable(arg->sig->name, arg->_varType, arg->isRef,
                                    false);
    }
    // used for pascal's strange return
    if (ast->sig->_resultType != SymbolTable::lookforType(TYPE_BASIC_VOID)) {
        ast->body->exprs.insert(
            ast->body->exprs.begin(),
            new VariableDeclAST(new VariableExprAST("__ret"),
                                ast->sig->resultType, false, false));
        // add result call to body
        ast->body->exprs.push_back(new ReturnAST(new VariableExprAST("__ret")));
    } else {
        //无用指令
        ast->body->exprs.push_back(new VariableDeclAST(
            new VariableExprAST("__ret"), new BasicTypeAST(TYPE_BASIC_INT),
            false, false));
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
    VariableDescriptor *var = nullptr;
    if (!ast->initVal) {
        auto var = SymbolTable::createVariable(ast->sig->name, ast->_varType,
                                               ast->isRef, ast->isConst);
    }else{
        auto var = SymbolTable::createVariable(ast->sig->name, std::any_cast<VariableDescriptor*>(ast->initVal->value)->varType,
                                               ast->isRef, ast->isConst);
    }
    putVariableDecl(var);
    if (ast->initVal) {
        CodeCollector::src() << "=";
        putVariableExpr(
            std::any_cast<VariableDescriptor *>(ast->initVal->value));
    }
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
    WALK_AST << cur_section_name.top() << " " << ss.str() << std::endl;
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

////////////////////////////////////////////////////////////////

void VisualDispatcher::genGlobalBegin(GlobalAST *ast) {}
void VisualDispatcher::genGlobalEnd(GlobalAST *ast) {}

void VisualDispatcher::genArrayTypeDecl(ArrayTypeDeclAST *ast) {}
void VisualDispatcher::genBasicType(BasicTypeAST *ast) {}
void VisualDispatcher::genPointerTypeDecl(PointerTypeDeclAST *ast) {}

void VisualDispatcher::genNumberExpr(NumberExprAST *ast) {}
void VisualDispatcher::genStringExpr(StringExprAST *ast) {}
void VisualDispatcher::genVariableExpr(VariableExprAST *ast) {}
void VisualDispatcher::genReturn(ReturnAST *ast) {}
void VisualDispatcher::genUnaryExpr(UnaryExprAST *ast) {}
void VisualDispatcher::genBinaryExpr(BinaryExprAST *ast) {}
void VisualDispatcher::genCallExpr(CallExprAST *ast) {}

/// 只生产if，不包含block
void VisualDispatcher::genIfStatementBegin(IfStatementAST *ast) {}
void VisualDispatcher::genIfStatementEnd(IfStatementAST *ast) {}
/// 只生产while，不包含block
void VisualDispatcher::genWhileStatementBegin(WhileStatementAST *ast) {}
void VisualDispatcher::genWhileStatementEnd(WhileStatementAST *ast) {}
/// 只生产for，不包含block
void VisualDispatcher::genForStatementBegin(ForStatementAST *ast) {}
void VisualDispatcher::genForStatementEnd(ForStatementAST *ast) {}

void VisualDispatcher::genFunction(FunctionAST *ast) {}
void VisualDispatcher::genFunctionSignature(FunctionSignatureAST *ast) {}

void VisualDispatcher::genStruct(StructDeclAST *ast) {}

void VisualDispatcher::genBlockBegin(BlockAST *ast) {}
void VisualDispatcher::genBlockEnd(BlockAST *ast) {}

void VisualDispatcher::genVariableDecl(VariableDeclAST *ast) {}