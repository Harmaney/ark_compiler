
#include "gen.h"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>

#include "data.h"
#include "err.h"
#include "logger.h"
#include "stdsupport.h"

std::string map_variable_type(SymbolDescriptor *type) {
    if (type->name == TYPE_BASIC_DOUBLE) {
        return "double";
    } else if (type->name == TYPE_BASIC_INT) {
        return "int";
    } else if (type->name == TYPE_BASIC_CHAR) {
        return "char";
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
// std

std::map<std::string, std::string> read_write_overloads_code;

void init_read_write() {
    static const std::vector<std::pair<std::string, std::string>> basic_type = {
        {"lf", TYPE_BASIC_DOUBLE},
        {"d", TYPE_BASIC_INT},
        {"d", TYPE_BASIC_LONGINT},
        {"lld", TYPE_BASIC_INT64},
        {"s", TYPE_BASIC_STRING}};
    std::function<void(std::string, int, std::string, std::string, std::string,
                       std::string, std::string,
                       std::vector<VariableDescriptor *>)>
        dfs_all = [&](std::string func, int lev, std::string funcSuffix,
                      std::string argList, std::string preMalloc,
                      std::string formatString, std::string idList,
                      std::vector<VariableDescriptor *> args) {
            if (lev == 0) {
                read_write_overloads_code[func + funcSuffix] =
                    func == "write"
                        ? "void " + func + funcSuffix + "(" + argList +
                              ") { printf(\"" + formatString + "\"" + idList +
                              "); }"
                        : "void " + func + funcSuffix + "(" + argList + ") { " +
                              preMalloc + "scanf(\"" + formatString + "\"" +
                              idList + "); }";
                SymbolTable::insert_function(
                    func, new FunctionDescriptor(
                              func, args,
                              SymbolTable::lookfor_type(TYPE_BASIC_VOID)));
            } else {
                for (auto [format, type] : basic_type) {
                    std::string _funcSuffix, _argList, _formatString, _idList,
                        _preMalloc;
                    std::vector<VariableDescriptor *> _args = args;
                    _funcSuffix = funcSuffix + "_" + type;
                    std::string varName = "a" + std::to_string(lev);
                    _argList =
                        argList + (argList == "" ? "" : ",") +
                        map_variable_type(SymbolTable::lookfor_type(type)) +
                        (func == "read" ? "* " : "") + " " + varName;
                    _preMalloc = preMalloc;
                    if (type == TYPE_BASIC_STRING)
                        _preMalloc += "if(*" + varName + ") free(" + "*" +
                                      varName + ");" + "*" + varName +
                                      "=malloc(255);";
                    _formatString = formatString + "%" + format;
                    _idList =
                        idList + ", " +
                        (func == "read" && type == TYPE_BASIC_STRING ? "*"
                                                                     : "") +
                        varName;
                    _args.push_back(new VariableDescriptor(
                        varName, SymbolTable::lookfor_type(type),
                        (func == "read"), false));
                    dfs_all(func, lev - 1, _funcSuffix, _argList, _preMalloc,
                            _formatString, _idList, _args);
                }
            }
        };
    for (int i = 1; i <= 3; ++i) {
        dfs_all("write", i, "", "", "", "", "", {});
        dfs_all("read", i, "", "", "", "", "", {});
    }
}
///

bool is_possible_to_level_up(SymbolDescriptor *src, SymbolDescriptor *target) {
    std::vector<SymbolDescriptor *> level;
    level.push_back(SymbolTable::lookfor_type(TYPE_BASIC_INT));
    level.push_back(SymbolTable::lookfor_type(TYPE_BASIC_LONGINT));
    level.push_back(SymbolTable::lookfor_type(TYPE_BASIC_INT64));

    bool checked = false;
    for (auto ty : level) {
        if (src == ty) checked = true;
        if (checked && target == ty) return true;
    }
    return false;
}

void put_variable_decl(VariableDescriptor *var) {
    // TODO: handle different type
    if(var->varType->type==DESCRIPTOR_STRUCT){
        CodeCollector::src()<<"struct ";
    }
    CodeCollector::src() << map_variable_type(var->varType);
    if (var->isRef) CodeCollector::src() << "*";
    CodeCollector::src() << " " << var->name;
}

void put_variable_expr(VariableDescriptor *var) {
    CodeCollector::src() << "(";
    if (var->isRef) CodeCollector::src() << "*";
    CodeCollector::src() << var->name << ")";
}

void ASTDispatcher::gen_global_begin(GlobalAST *ast) {
    // TODO:
    CodeCollector::begin_section("prelude");

    std::ifstream prel("prelude.h");

    CodeCollector::push_back(std::string((std::istreambuf_iterator<char>(prel)),
                                         (std::istreambuf_iterator<char>())));

    init_read_write();

    CodeCollector::end_section();
}

void ASTDispatcher::gen_global_end(GlobalAST *ast) {
    // TODO:
}

void ASTDispatcher::gen_array_type_decl(ArrayTypeDeclAST *ast) {
    // 检查数组区间是否是整数
    if (ast->rangeL->const_type != CONSTANT_INT ||
        ast->rangeR->const_type != CONSTANT_INT) {
        throw TypeErrorException("wrong type in range of array",
                                 std::to_string(ast->rangeL->const_type),
                                 TYPE_BASIC_INT,ast->get_row(), 0);
    }

    auto descriptor = SymbolTable::create_array_type(
        ast->itemAST->_descriptor, ast->rangeR->val_int - ast->rangeL->val_int,
        ast->rangeL->val_int);

    // this is wrong when using custom type
    // to fix this, you should think about how to resolve typedef in struct too
    term_print.debug() << "generate typedef" << std::endl;
    CodeCollector::begin_section("pre_array");
    CodeCollector::src() << "typedef "
                         << map_variable_type(descriptor->itemDescriptor) << " "
                         << descriptor->name << "[" << descriptor->sz << "];";
    CodeCollector::push_back();
    CodeCollector::end_section();
    ast->_descriptor = descriptor;
}

void ASTDispatcher::gen_pointer_type_decl(PointerTypeDeclAST *ast) {
    auto descriptor = SymbolTable::create_pointer_type(ast->ref->_descriptor);

    CodeCollector::begin_section("prelude");
    CodeCollector::src() << "typedef " << map_variable_type(descriptor->ref)
                         << "* " << descriptor->name << ";";
    CodeCollector::push_back();
    CodeCollector::end_section();
    ast->_descriptor = descriptor;
}

void ASTDispatcher::gen_type_def(TypeDefAST *ast) {
    // 这看起来不太靠谱
    SymbolTable::insert_type(ast->newName->varType, ast->oldName->_descriptor);

    CodeCollector::src() << "typedef "
                         << map_variable_type(ast->oldName->_descriptor) << " "
                         << ast->newName->varType << ";";
    CodeCollector::push_back();
}

void ASTDispatcher::gen_basic_type(BasicTypeAST *ast) {
    auto descriptor = SymbolTable::lookfor_type(ast->varType);
    if (descriptor == nullptr) {
        throw std::invalid_argument("undefined type " + ast->varType);
        ;
    }
    ast->_descriptor = descriptor;
}

void ASTDispatcher::gen_number_expr(NumberExprAST *ast) {
    VariableDescriptor *t = nullptr;

    switch (ast->const_type) {
        case CONSTANT_INT:
            t = SymbolTable::create_variable_G(
                SymbolTable::lookfor_type(TYPE_BASIC_INT), false);
            t->isLeftVar = false;
            break;
        case CONSTANT_REAL:
            t = SymbolTable::create_variable_G(
                SymbolTable::lookfor_type(TYPE_BASIC_DOUBLE), false);
            t->isLeftVar = false;
            break;
        default:
            throw InternalErrorException(
                "unknown constant type: " + ast->const_type, ast->get_row(), 0);
            break;
    }

    CodeCollector::begin_section("global_define");
    CodeCollector::src() << map_variable_type(t->varType) << " " << t->name
                         << "=";
    if (ast->const_type == CONSTANT_REAL)
        CodeCollector::src() << ast->val_float << ";";
    else if (ast->const_type == CONSTANT_INT)
        CodeCollector::src() << ast->val_int << ";";
    else {
        throw InternalErrorException(
            "unknown constant type: " + ast->const_type,ast->get_row(), 0);
    }
    CodeCollector::push_back();
    CodeCollector::end_section();

    ast->value = t;
}

void ASTDispatcher::gen_string_expr(StringExprAST *ast) {
    VariableDescriptor *t = SymbolTable::create_variable_G(
        SymbolTable::lookfor_type(TYPE_BASIC_STRING), false);
    t->isLeftVar = false;

    CodeCollector::begin_section("global_define");
    CodeCollector::src() << map_variable_type(t->varType) << " " << t->name
                         << " = NULL;";
    CodeCollector::push_back();
    CodeCollector::end_section();

    CodeCollector::begin_section("init_global_var");
    CodeCollector::src() << t->name << "= new_string_(\"" << ast->val << "\");";
    CodeCollector::push_back();
    CodeCollector::end_section();
    ast->value = t;
}

void ASTDispatcher::gen_char_expr(CharExprAST *ast) {
    VariableDescriptor *t = SymbolTable::create_variable_G(
        SymbolTable::lookfor_type(TYPE_BASIC_STRING), false);
    t->isLeftVar = false;
    CodeCollector::begin_section("global_define");
    CodeCollector::src() << map_variable_type(t->varType) << " " << t->name
                         << " = \'" << ast->val << "\';";
    CodeCollector::push_back();
    CodeCollector::end_section();
    ast->value = t;
}

void ASTDispatcher::gen_variable_expr(VariableExprAST *ast) {
    VariableDescriptor *t = SymbolTable::lookfor_variable(ast->name);
    if (!t) {
        // for pascal's strange return
        t = SymbolTable::lookfor_variable("__ret");
        if (!t)
            throw SymbolUndefinedException("undefined variable", ast->name, ast->get_row(),
                                           0);
    }

    ast->value = t;
}

void ASTDispatcher::gen_return(ReturnAST *ast) {
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

void ASTDispatcher::gen_binary_expr(BinaryExprAST *ast) {
    VariableDescriptor *lhs =
        std::any_cast<VariableDescriptor *>(ast->LHS->value);
    VariableDescriptor *rhs =
        std::any_cast<VariableDescriptor *>(ast->RHS->value);

    if (ast->op == ":=") {
        if (lhs->varType != rhs->varType &&
            is_possible_to_level_up(rhs->varType, lhs->varType) == false) {
            throw TypeErrorException("type does not match between `=`",
                                     rhs->varType->name, lhs->varType->name, ast->get_row(),
                                     0);
        }
        if (lhs->isLeftVar == false) {
            throw TypeErrorException("try using `=` on right value",
                                     "right val", "left val", ast->get_row(), 0);
        }
        if (lhs->varType->name == "string") {
            assert(rhs->varType->name == "string");
            CodeCollector::src() << "assign_string_(&";
            put_variable_expr(lhs);
            CodeCollector::src() << ", ";
            put_variable_expr(rhs);
            CodeCollector::src() << ")";
        } else {
            put_variable_expr(lhs);
            CodeCollector::src() << "=";
            put_variable_expr(rhs);

            // TODO: pascal 的 = 是否有返回值？
        }
        CodeCollector::src() << ";";
        CodeCollector::push_back();
    } else if (ast->op == "[]") {
        if (lhs->varType->type != DESCRIPTOR_ARRAY) {
            throw TypeErrorException("try to use operator [] on invalid type",
                                     std::to_string(lhs->varType->type),
                                     std::to_string(DESCRIPTOR_ARRAY), ast->get_row(), 0);
        }

        auto array = static_cast<ArrayTypeDescriptor *>(lhs->varType);

        VariableDescriptor *realid =
            SymbolTable::create_variable(rhs->varType, false, false);
        put_variable_decl(realid);
        CodeCollector::src() << "=";
        put_variable_expr(realid);
        CodeCollector::src() << "-" << array->beg << ";";
        CodeCollector::push_back();

        VariableDescriptor *t =
            SymbolTable::create_variable(array->itemDescriptor, true, false);
        put_variable_decl(t);
        CodeCollector::src() << ";";
        CodeCollector::push_back();

        // FIX: too ugly
        CodeCollector::src() << t->name;
        CodeCollector::src() << "=&";
        put_variable_expr(lhs);
        CodeCollector::src() << "[";
        put_variable_expr(rhs);
        CodeCollector::src() << "];";
        CodeCollector::push_back();
        ast->value = t;
    } else if (ast->op == ".") {
        if (lhs->varType->type != DESCRIPTOR_STRUCT) {
            throw TypeErrorException(
                "try to use operator `.` on invalid variable",
                lhs->varType->name, "a record", ast->get_row(), 0);
        }
        if (ast->RHS->type != AST_STRING_EXPR) {
            throw TypeErrorException("operator `.` takes a string as arg 2",
                                     std::to_string(ast->RHS->type), "a string",
                                     ast->get_row(), 0);
        }
        auto child_id = static_cast<StringExprAST *>(ast->RHS)->val;
        auto array = static_cast<StructDescriptor *>(lhs->varType);
        if (!array->refVar.count(child_id)) {
            throw SymbolUndefinedException("no member in record", child_id, ast->get_row(),
                                           0);
        }
        VariableDescriptor *t =
            SymbolTable::create_variable(array->refVar[child_id], true, false);

        put_variable_decl(t);
        CodeCollector::src() << ";";
        CodeCollector::push_back();

        // FIX: too ugly
        CodeCollector::src() << t->name;
        CodeCollector::src() << "=&(";
        put_variable_expr(lhs);
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
        if (ast->op == "mod") ast->op = "%";
        if (ast->op == "-" || ast->op == "+" || ast->op == "*" ||
            ast->op == "%" || ast->op == "/" || ast->op == "==" ||
            ast->op == "!=" || ast->op == "<" || ast->op == ">" ||
            ast->op == "<=" || ast->op == ">=" || ast->op == "||" ||
            ast->op == "&&") {
            // FIX: calculate type
            if (lhs->varType != rhs->varType &&
                !is_possible_to_level_up(lhs->varType, rhs->varType) &&
                !is_possible_to_level_up(rhs->varType, lhs->varType)) {
                throw TypeErrorException("type of objects does not match",
                                         rhs->varType->name, lhs->varType->name,
                                         ast->get_row(), 0);
            }
            VariableDescriptor *t =
                SymbolTable::create_variable(lhs->varType, false, false);
            put_variable_decl(t);
            CodeCollector::src()
                << (lhs->varType->name == "string" ? "=NULL" : "") << ";";
            CodeCollector::push_back();
            if (lhs->varType->name == "string") {
                assert(rhs->varType->name == "string");
                assert(ast->op == "+");
                CodeCollector::src() << "assign_string_(&";
                put_variable_expr(t);
                CodeCollector::src() << ",add_string_(";
                put_variable_expr(lhs);
                CodeCollector::src() << ",";
                put_variable_expr(rhs);
                CodeCollector::src() << "));";
                CodeCollector::push_back();
            } else {
                put_variable_expr(t);
                CodeCollector::src() << "=";
                put_variable_expr(lhs);
                CodeCollector::src() << ast->op;
                put_variable_expr(rhs);
                CodeCollector::src() << ";";
                CodeCollector::push_back();
            }
            ast->value = t;
        } else {
            throw UndefinedBehaviorException("unknown operator " + ast->op,ast->get_row(),
                                             0);
        }
    }
}

void ASTDispatcher::gen_unary_expr(UnaryExprAST *ast) {
    VariableDescriptor *var =
        std::any_cast<VariableDescriptor *>(ast->expr->value);

    if (ast->op == "*") {
        if (var->varType->type != DESCRIPTOR_POINTER_TYPE) {
            throw TypeErrorException(
                "try to def a variable that is not a pointer",
                var->varType->name, "a pointer", ast->get_row(), 0);
        }
        VariableDescriptor *t = SymbolTable::create_variable(
            static_cast<PointerTypeDescriptor *>(var->varType)->ref, true,
            false);
        put_variable_decl(t);
        CodeCollector::src() << ";";
        CodeCollector::push_back();

        put_variable_expr(t);
        CodeCollector::src() << "=";
        put_variable_expr(var);
        CodeCollector::src() << ";";
        CodeCollector::push_back();

        ast->value = t;

    } else {
        throw std::invalid_argument("unknown unary operator");
        throw UndefinedBehaviorException("unknown unary operator, " + ast->op,
                                         ast->get_row(), 0);
    }
}

void ASTDispatcher::gen_call_expr(CallExprAST *ast) {
    // SPECIAL for exit()
    if (ast->callee == "exit") {
        if (SymbolTable::lookfor_variable("__ret")) {
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
        SymbolTable::lookfor_function(ast->callee, argsimbols);

    if (!raw_descriptor || raw_descriptor->type != DESCRIPTOR_FUNCTION) {
        throw SymbolUndefinedException("try to call undefined function",
                                       ast->callee, ast->get_row(), 0);
    }
    FunctionDescriptor *descriptor =
        static_cast<FunctionDescriptor *>(raw_descriptor);
    // special for read_write
    auto itrn_name = descriptor->name;
    if (read_write_overloads_code.count(itrn_name)) {
        CodeCollector::begin_section("prelude");
        CodeCollector::push_back(read_write_overloads_code[itrn_name]);
        CodeCollector::end_section();
        read_write_overloads_code.erase(itrn_name);
    }
    if (descriptor->args.size() != ast->args.size()) {
        throw TypeErrorException("args not matched for function " + ast->callee,
                                 std::to_string(ast->args.size()),
                                 std::to_string(descriptor->args.size()), ast->get_row(), 0);
    }

    // gen var to catch result val
    if (descriptor->resultDescriptor !=
        SymbolTable::lookfor_type(TYPE_BASIC_VOID)) {
        auto ret = SymbolTable::create_variable(descriptor->resultDescriptor,
                                                false, false);
        ast->value = ret;
        put_variable_decl(ret);
        CodeCollector::src() << ";";
        CodeCollector::push_back();

        put_variable_expr(ret);
        CodeCollector::src() << "=";
    }

    CodeCollector::src() << descriptor->name << "(";
    for (int i = 0; i < ast->args.size(); i++) {
        auto d = std::any_cast<VariableDescriptor *>(ast->args[i]->value);
        if (d->varType != descriptor->args[i]->varType) {
            throw TypeErrorException(
                "wrong type of arg of function " + ast->callee,
                d->varType->name, descriptor->args[i]->varType->name, ast->get_row(), 0);
        }
        // check whether the function wants ref or not
        // we use pointer to simulate this process in c
        CodeCollector::src() << (descriptor->args[i]->isRef ? "&" : "");
        put_variable_expr(d);
        if (i != ast->args.size() - 1) CodeCollector::src() << ",";
    }

    CodeCollector::src() << ");";
    CodeCollector::push_back();
}

void ASTDispatcher::gen_if_statement_begin(IfStatementAST *ast) {
    CodeCollector::src()
        << "if ("
        << std::any_cast<VariableDescriptor *>(ast->condition->value)->name
        << ")";

    std::string *L0 = TagTable::create_tag_G();
    CodeCollector::src() << "goto " << *L0 << ";";
    CodeCollector::push_back();
    ast->extraData["L0"] = L0;
}

void ASTDispatcher::gen_if_statement_else(IfStatementAST *ast) {
    std::string *END = TagTable::create_tag_G();
    CodeCollector::src() << "goto " << *END << ";";
    CodeCollector::push_back();
    ast->extraData["END"] = END;
    CodeCollector::src() << *std::any_cast<std::string *>(ast->extraData["L0"])
                         << ": ____nouse=1;;";
    CodeCollector::push_back();
}

void ASTDispatcher::gen_if_statement_end(IfStatementAST *ast) {
    CodeCollector::src() << *std::any_cast<std::string *>(ast->extraData["END"])
                         << ": ____nouse=1;;";
    CodeCollector::push_back();
}

void ASTDispatcher::gen_for_statement_begin(ForStatementAST *ast) {
    ast->extraData["end"] = TagTable::create_tag_G();
    ast->extraData["begin"] = TagTable::create_tag_G();

    if (std::any_cast<VariableDescriptor *>(ast->rangeL->value)->varType !=
            SymbolTable::lookfor_type(TYPE_BASIC_INT64) &&
        is_possible_to_level_up(
            std::any_cast<VariableDescriptor *>(ast->rangeL->value)->varType,
            SymbolTable::lookfor_type(TYPE_BASIC_INT64))==false) {
        throw TypeErrorException(
            "left range of `for` is not an integer",
            std::any_cast<VariableDescriptor *>(ast->rangeL->value)
                ->varType->name,
            "a integer",ast->get_row(), 0);
    }
    if (std::any_cast<VariableDescriptor *>(ast->rangeR->value)->varType !=
            SymbolTable::lookfor_type(TYPE_BASIC_INT64) &&
        is_possible_to_level_up(
            std::any_cast<VariableDescriptor *>(ast->rangeR->value)->varType,
            SymbolTable::lookfor_type(TYPE_BASIC_INT64))==false) {
        throw TypeErrorException(
            "right range of `for` is not an integer",
            std::any_cast<VariableDescriptor *>(ast->rangeL->value)
                ->varType->name,
            "a integer",ast->get_row(), 0);
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

void ASTDispatcher::gen_for_statement_end(ForStatementAST *ast) {
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

void ASTDispatcher::gen_while_statement_begin(WhileStatementAST *ast) {
    CodeCollector::src()
        << "if (!"
        << std::any_cast<VariableDescriptor *>(ast->condition->value)->name
        << ")";

    std::string *L0 = TagTable::create_tag_G();
    CodeCollector::src() << "goto " << *L0 << ";";
    CodeCollector::push_back();
    ast->extraData["end"] = L0;

    std::string *begin = TagTable::create_tag_G();
    CodeCollector::src() << *begin << ": ____nouse=1;;";
    CodeCollector::push_back();
    ast->extraData["begin"] = begin;
}

void ASTDispatcher::gen_while_statement_end(WhileStatementAST *ast) {
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

void ASTDispatcher::gen_function_signature(FunctionSignatureAST *ast) {
    term_print.warn() << "todo: function sig end" << std::endl;
}

void ASTDispatcher::gen_function(FunctionAST *ast) {
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
    SymbolTable::insert_function(ast->sig->sig, functiondesciptor);

    CodeCollector::src() << map_variable_type(ast->sig->_resultType) << " ";
    CodeCollector::src() << functiondesciptor->name;
    CodeCollector::src() << "(";

    for (int i = 0; i < (int)ast->sig->args.size(); i++) {
        // FIX: too ugly
        auto tempDescriptor = new VariableDescriptor(
            ast->sig->args[i]->sig->name, ast->sig->args[i]->_varType,
            ast->sig->args[i]->isRef, false);
        put_variable_decl(tempDescriptor);
        if (i + 1 != ast->sig->args.size()) CodeCollector::src() << ", ";
    }
    CodeCollector::src() << ")";
    CodeCollector::push_back();

    SymbolTable::enter();
    for (auto arg : ast->sig->args) {
        SymbolTable::create_variable(arg->sig->name, arg->_varType, arg->isRef,
                                     false);
    }
    // used for pascal's strange return
    if (ast->sig->_resultType != SymbolTable::lookfor_type(TYPE_BASIC_VOID)) {
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

void ASTDispatcher::gen_block_begin(BlockAST *ast) {
    CodeCollector::push_back("{");
    if (ast->extraData.count("IS_MAIN"))
        CodeCollector::push_back("init_global_var_();");
}

void ASTDispatcher::gen_block_end(BlockAST *ast) {
    CodeCollector::push_back("}");
}

void ASTDispatcher::gen_struct(StructDeclAST *ast) {
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
    SymbolTable::insert_type(ast->sig, structD);

    ast->_descriptor = structD;
}

void ASTDispatcher::gen_variable_decl(VariableDeclAST *ast) {
    VariableDescriptor *var = nullptr;
    if (ast->varType) {
        ast->_varType = ast->varType->_descriptor;
        var = SymbolTable::create_variable(ast->sig->name, ast->_varType,
                                           ast->isRef, ast->isConst);
    } else if (ast->initVal) {
        ast->_varType =
            std::any_cast<VariableDescriptor *>(ast->initVal->value)->varType;
        var = SymbolTable::create_variable(ast->sig->name, ast->_varType,
                                           ast->isRef, ast->isConst);
    } else {
        throw TypeErrorException("missing type for variable " + ast->sig->name,
                                 "<>", "<?>", ast->get_row(), 0);
    }
    put_variable_decl(var);

    if (ast->initVal) {
        /*CodeCollector::begin_section("init_global_var");
        put_variable_expr(var);
        CodeCollector::src() << "=";
        ast->initVal.
        put_variable_expr(
            std::any_cast<VariableDescriptor *>(ast->initVal->value));
        CodeCollector::src() << ";";
        CodeCollector::push_back();
        CodeCollector::end_section();*/
        CodeCollector::src() << "="
                             << (ast->initVal->const_type == CONSTANT_REAL
                                     ? ast->initVal->val_float
                                     : ast->initVal->val_int);
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
    // WALK_AST << cur_section_name.top() << " " << ss.str() << std::endl;
    gen_info.push_back(
        {{"GEN", {{"TO", cur_section_name.top()}, {"CODE", ss.str()}}}});
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
        term_print.debug() << "// section " << sid << std::endl;
        if (codes.count(sid))
            for (auto str : *codes[sid]) {
                term_print.debug() << str << std::endl;
            }
    }
}

void CodeCollector::output(std::ostream &out) {
    for (auto sid : section_order) {
        out << "//" << sid << std::endl;
        if (codes.count(sid))
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
        term_print.warn() << "rearranging section `" << section
                          << "` failed: no such section" << std::endl;
    }
}

void CodeCollector::clear() { codes.clear(); }

////////////////////////////////////////////////////////////////

void VisualDispatcher::gen_global_begin(GlobalAST *ast) {}
void VisualDispatcher::gen_global_end(GlobalAST *ast) {}

void VisualDispatcher::gen_array_type_decl(ArrayTypeDeclAST *ast) {}
void VisualDispatcher::gen_basic_type(BasicTypeAST *ast) {}
void VisualDispatcher::gen_pointer_type_decl(PointerTypeDeclAST *ast) {}

void VisualDispatcher::gen_number_expr(NumberExprAST *ast) {}
void VisualDispatcher::gen_string_expr(StringExprAST *ast) {}
void VisualDispatcher::gen_variable_expr(VariableExprAST *ast) {}
void VisualDispatcher::gen_return(ReturnAST *ast) {}
void VisualDispatcher::gen_unary_expr(UnaryExprAST *ast) {}
void VisualDispatcher::gen_binary_expr(BinaryExprAST *ast) {}
void VisualDispatcher::gen_call_expr(CallExprAST *ast) {}

/// 只生产if，不包含block
void VisualDispatcher::gen_if_statement_begin(IfStatementAST *ast) {}
void VisualDispatcher::gen_if_statement_end(IfStatementAST *ast) {}
/// 只生产while，不包含block
void VisualDispatcher::gen_while_statement_begin(WhileStatementAST *ast) {}
void VisualDispatcher::gen_while_statement_end(WhileStatementAST *ast) {}
/// 只生产for，不包含block
void VisualDispatcher::gen_for_statement_begin(ForStatementAST *ast) {}
void VisualDispatcher::gen_for_statement_end(ForStatementAST *ast) {}

void VisualDispatcher::gen_function(FunctionAST *ast) {}
void VisualDispatcher::gen_function_signature(FunctionSignatureAST *ast) {}

void VisualDispatcher::gen_struct(StructDeclAST *ast) {}

void VisualDispatcher::gen_block_begin(BlockAST *ast) {}
void VisualDispatcher::gen_block_end(BlockAST *ast) {}

void VisualDispatcher::gen_variable_decl(VariableDeclAST *ast) {}