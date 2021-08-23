
#include "gen.h"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>

#include "data.h"
#include "err.h"
#include "logger.h"
#include "stdsupport.h"

Value *castValue(std::any x) { return std::any_cast<Value *>(x); }

std::any ASTDispatcher::gen_global(GlobalAST *ast) {
    VMWhiteBlock *global_block = new VMWhiteBlock();
    code()->push_block(global_block);
    ast->mainBlock->accept(*this);

    return nullptr;
}

std::any ASTDispatcher::gen_array_type_decl(ArrayTypeDeclAST *ast) {
    Value *rangeL = castValue(ast->rangeL->accept(*this));
    Value *rangeR = castValue(ast->rangeR->accept(*this));
    ast->itemAST->accept(*this);
    auto itemDescriptor = ast->itemAST->_descriptor;
    // 检查数组区间是否是整数
    if (rangeL->varType->name != TYPE_BASIC_INT ||
        rangeR->varType->name != TYPE_BASIC_INT) {
        throw TypeErrorException("wrong type in range of array",
                                 std::to_string(ast->rangeL->const_type),
                                 TYPE_BASIC_INT, 0, 0);
    }

    auto descriptor = symbolTable()->create_array_type(
        itemDescriptor, ast->rangeR->val_int - ast->rangeL->val_int + 1,
        ast->rangeL->val_int);
    ast->_descriptor=descriptor;

    // this is wrong when using custom type
    // to fix this, you should think about how to resolve typedef in struct too
    term_print.debug() << "generate typedef" << std::endl;

    code()->createTypeDecl(descriptor);
    return nullptr;
}

std::any ASTDispatcher::gen_pointer_type_decl(PointerTypeDeclAST *ast) {
    ast->ref->accept(*this);
    auto descriptor = symbolTable()->create_pointer_type(ast->ref->_descriptor);

    code()->createTypeDecl(descriptor);
    ast->_descriptor = descriptor;
    return nullptr;
}

std::any ASTDispatcher::gen_type_def(TypeDefAST *ast) {
    // 这看起来不太靠谱
    ast->newName->accept(*this);
    ast->oldName->accept(*this);
    auto descriptor = symbolTable()->insert_type(ast->newName->varType,
                                                 ast->oldName->_descriptor);

    code()->createAkaType(ast->newName->varType, descriptor);
    ast->_descriptor = descriptor;
    return nullptr;
}

std::any ASTDispatcher::gen_basic_type(BasicTypeAST *ast) {
    auto descriptor = symbolTable()->lookfor_type(ast->varType);
    if (descriptor == nullptr) {
        throw std::invalid_argument("undefined type " + ast->varType);
    }
    ast->_descriptor = descriptor;
    return nullptr;
}

std::any ASTDispatcher::gen_number_expr(NumberExprAST *ast) {
    VariableDescriptor *t = nullptr;

    switch (ast->const_type) {
        case CONSTANT_INT:
            t = symbolTable()->create_variable_G(
                symbolTable()->lookfor_type(TYPE_BASIC_INT), false);
            t->isLeftVar = false;
            break;
        case CONSTANT_REAL:
            t = symbolTable()->create_variable_G(
                symbolTable()->lookfor_type(TYPE_BASIC_DOUBLE), false);
            t->isLeftVar = false;
            break;
        default:
            throw InternalErrorException(
                "unknown constant type: " + ast->const_type, 0, 0);
            break;
    }

    // TODO: move those code to ccvm.h
    // CodeCollector::begin_section("global_define");
    // CodeCollector::src() << map_variable_type(t->varType) << " " << t->name
    //                      << "=";
    // if (ast->const_type == CONSTANT_REAL)
    //     CodeCollector::src() << ast->val_float << ";";
    // else if (ast->const_type == CONSTANT_INT)
    //     CodeCollector::src() << ast->val_int << ";";
    // else {
    //     throw InternalErrorException(
    //         "unknown constant type: " + ast->const_type, 0, 0);
    // }
    // CodeCollector::push_back();
    // CodeCollector::end_section();

    if (ast->const_type == CONSTANT_INT) {
        code()->createConstDecl(t, ast->val_int);
    } else if (ast->const_type == CONSTANT_REAL) {
        code()->createConstDecl(t, ast->val_float);
    } else {
        throw InternalErrorException(
            "unknown constant type: " + ast->const_type, 0, 0);
    }

    return t;
}

std::any ASTDispatcher::gen_string_expr(StringExprAST *ast) {
    // TODO: implement this
    throw InternalErrorException("not implemented", 0, 0);
    return nullptr;
}

std::any ASTDispatcher::gen_char_expr(CharExprAST *ast) {
    VariableDescriptor *t = symbolTable()->create_variable_G(
        symbolTable()->lookfor_type(TYPE_BASIC_STRING), false);
    t->isLeftVar = false;

    // TODO: move those code to ccvm.h
    // CodeCollector::begin_section("global_define");
    // CodeCollector::src() << map_variable_type(t->varType) << " " << t->name
    //                      << " = \'" << ast->val << "\';";
    // CodeCollector::push_back();
    // CodeCollector::end_section();

    code()->createConstDecl(t, ast->val);
    return t;
}

std::any ASTDispatcher::gen_variable_expr(VariableExprAST *ast) {
    VariableDescriptor *t = symbolTable()->lookfor_variable(ast->name);

    // TODO: think twice about this
    if (!t) {
        // for pascal's strange return
        t = symbolTable()->lookfor_variable("__ret");
        if (!t)
            throw SymbolUndefinedException("undefined variable", ast->name, 0,
                                           0);
    }

    return t;
}

std::any ASTDispatcher::gen_return(ReturnAST *ast) {
    if (ast->expr != nullptr) {
        Value *t = castValue(ast->expr->accept(*this));

        code()->createReturn(t);

        // TODO: move this
        // CodeCollector::src()
        //     << "return "
        //     << std::any_cast<VariableDescriptor *>(ast->expr->value)->name
        //     << ";";
    } else {
        code()->createReturn();
        // TODO: move this
        // CodeCollector::src() << "return "
        //                      << ";";
    }
    return nullptr;
}

std::any ASTDispatcher::gen_binary_expr(BinaryExprAST *ast) {
    Value *lhs = castValue(ast->LHS->accept(*this));
    Value *rhs = castValue(ast->RHS->accept(*this));

    if (ast->op == ":=") {
        // TODO: solve level up
        if (lhs->varType != rhs->varType) {
            throw TypeErrorException("type does not match between `=`",
                                     rhs->varType->name, lhs->varType->name, 0,
                                     0);
        }
        if (lhs->isLeftVar == false) {
            throw TypeErrorException("try using `=` on right value",
                                     "right val", "left val", 0, 0);
        }
        if (lhs->isConst == true) {
            throw TypeErrorException("try using `:=` on const value",
                                     "const value", "variable", 0, 0);
        }

        code()->createVarAssign(lhs, rhs);
        // TODO: pascal 的 = 是否有返回值？
        return nullptr;
    } else if (ast->op == "[]") {
        if (lhs->varType->type != DESCRIPTOR_ARRAY) {
            throw TypeErrorException("try to use operator [] on invalid type",
                                     std::to_string(lhs->varType->type),
                                     std::to_string(DESCRIPTOR_ARRAY), 0, 0);
        }

        auto array = static_cast<ArrayTypeDescriptor *>(lhs->varType);

        VariableDescriptor *realid =
            symbolTable()->create_variable(rhs->varType, false, false);

        auto leftRangeDescriptor = symbolTable()->create_variable_G(
            symbolTable()->lookfor_type(TYPE_BASIC_INT), false);
        code()->createOptBinary(realid, realid, leftRangeDescriptor, "-");

        auto t =
            symbolTable()->create_variable(array->itemDescriptor, true, false);
        code()->createVariableDecl(t);

        code()->createArrayAssign(t, lhs, rhs,true);
        // CodeCollector::src() << t->name;
        // CodeCollector::src() << "=&";
        // put_variable_expr(lhs);
        // CodeCollector::src() << "[";
        // put_variable_expr(rhs);
        // CodeCollector::src() << "];";
        // CodeCollector::push_back();
        return t;
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
        VariableDescriptor *t = symbolTable()->create_variable(
            array->refVar[child_id], true, false);

        code()->createVariableDecl(t);

        code()->createStructAssign(t, lhs, child_id);
        // CodeCollector::src() << t->name;
        // CodeCollector::src() << "=&(";
        // put_variable_expr(lhs);
        // CodeCollector::src() << "." << child_id;
        // CodeCollector::src() << ");";
        // CodeCollector::push_back();
        return t;
    } else {
        if (ast->op == "<>") ast->op = "!=";
        if (ast->op == "=") ast->op = "==";
        if (ast->op == "div") ast->op = "/";
        if (ast->op == "and") ast->op = "&&";
        if (ast->op == "or") ast->op = "||";
        if (ast->op == "mod") ast->op = "%";
        if (ast->op == "xor") ast->op = "^";
        if (ast->op == "-" || ast->op == "+" || ast->op == "*" ||
            ast->op == "%" || ast->op == "^" || ast->op == "/" ||
            ast->op == "<<" || ast->op == ">>" || ast->op == "==" ||
            ast->op == "!=" || ast->op == "<" || ast->op == ">" ||
            ast->op == "<=" || ast->op == ">=" || ast->op == "||" ||
            ast->op == "&&") {
            // FIX: calculate type
            // TODO: level up
            if (lhs->varType != rhs->varType) {
                throw TypeErrorException("type of objects does not match",
                                         rhs->varType->name, lhs->varType->name,
                                         0, 0);
            }
            VariableDescriptor *t =
                symbolTable()->create_variable(lhs->varType, false, false);
            code()->createVariableDecl(t);

            code()->createOptBinary(t, lhs, rhs, ast->op);
            // if (lhs->varType->name == "string") {
            //     assert(rhs->varType->name == "string");
            //     assert(ast->op == "+");
            //     CodeCollector::src() << "assign_string_(&";
            //     put_variable_expr(t);
            //     CodeCollector::src() << ",add_string_(";
            //     put_variable_expr(lhs);
            //     CodeCollector::src() << ",";
            //     put_variable_expr(rhs);
            //     CodeCollector::src() << "));";
            //     CodeCollector::push_back();
            // } else {
            //     put_variable_expr(t);
            //     CodeCollector::src() << "=";
            //     put_variable_expr(lhs);
            //     CodeCollector::src() << ast->op;
            //     put_variable_expr(rhs);
            //     CodeCollector::src() << ";";
            //     CodeCollector::push_back();
            // }
            return t;
        } else {
            throw UndefinedBehaviorException("unknown operator " + ast->op, 0,
                                             0);
        }
    }
}

std::any ASTDispatcher::gen_unary_expr(UnaryExprAST *ast) {
    Value *var = castValue(ast->expr->accept(*this));

    if (ast->op == "*") {
        if (var->varType->type != DESCRIPTOR_POINTER_TYPE) {
            throw TypeErrorException(
                "try to def a variable that is not a pointer",
                var->varType->name, "a pointer", 0, 0);
        }
        auto t = symbolTable()->create_variable(
            static_cast<PointerTypeDescriptor *>(var->varType)->ref, true,
            false);

        code()->createVariableDecl(t);
        code()->createVarAssign(t, var);

        return t;
    } else {
        throw UndefinedBehaviorException("unknown unary operator, " + ast->op,
                                         0, 0);
    }
}

std::any ASTDispatcher::gen_call_expr(CallExprAST *ast) {
    // TODO: SPECIAL for exit()

    // check whether function exists.
    std::vector<SymbolDescriptor *> argsymbols;
    std::vector<Value *> argValues;
    for (auto arg : ast->args) {
        auto t = castValue(arg->accept(*this));
        argsymbols.push_back(t->varType);
        argValues.push_back(t);
    }
    // TODO: level up
    SymbolDescriptor *raw_descriptor =
        symbolTable()->lookfor_function(ast->callee, argsymbols);

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
        symbolTable()->lookfor_type(TYPE_BASIC_VOID)) {
        auto ret = symbolTable()->create_variable(descriptor->resultDescriptor,
                                                  false, false);
        code()->createVariableDecl(ret);

        // ast->value = ret;

        code()->createFunctionCall(ret, descriptor, argValues);
        return ret;
    } else {
        code()->createFunctionCall(nullptr, descriptor, argValues);
        return nullptr;
    }

    // CodeCollector::src() << descriptor->name << "(";
    // for (int i = 0; i < ast->args.size(); i++) {
    //     auto d = std::any_cast<VariableDescriptor *>(ast->args[i]->value);
    //     // if (d->varType != descriptor->args[i]->varType) {
    //     //     throw TypeErrorException(
    //     //         "wrong type of arg of function " + ast->callee,
    //     //         d->varType->name, descriptor->args[i]->varType->name,
    //     //         0, 0);
    //     // }
    //     // check whether the function wants ref or not
    //     // we use pointer to simulate this process in c
    //     CodeCollector::src() << (descriptor->args[i]->isRef ? "&" : "");
    //     put_variable_expr(d);
    //     if (i != ast->args.size() - 1) CodeCollector::src() << ",";
    // }

    // CodeCollector::src() << ");";
    // CodeCollector::push_back();
}

std::any ASTDispatcher::gen_if_statement(IfStatementAST *ast) {
    auto blockCond = code()->createWhiteBlock();
    auto blockOk = code()->createWhiteBlock();
    auto blockNo = code()->createWhiteBlock();

    code()->push_block(blockCond);
    auto t = castValue(ast->condition->accept(*this));
    code()->pop_block();

    code()->push_block(blockOk);
    ast->body_true->accept(*this);
    code()->pop_block();

    code()->push_block(blockNo);
    ast->body_false->accept(*this);
    code()->pop_block();

    code()->createCondBr(t, blockOk, blockNo);

    return nullptr;
}

std::any ASTDispatcher::gen_for_statement(ForStatementAST *ast) {
    auto blockInit = code()->createWhiteBlock();
    auto blockCond = code()->createWhiteBlock();
    auto blockStep = code()->createWhiteBlock();
    auto blockBody = code()->createWhiteBlock();

    // if (std::any_cast<VariableDescriptor *>(ast->rangeL->value)->varType !=
    //         SymbolTable::lookfor_type(TYPE_BASIC_INT64) &&
    //     is_possible_to_level_up(
    //         std::any_cast<VariableDescriptor *>(ast->rangeL->value)->varType,
    //         SymbolTable::lookfor_type(TYPE_BASIC_INT64)) == false) {
    //     throw TypeErrorException(
    //         "left range of `for` is not an integer",
    //         std::any_cast<VariableDescriptor *>(ast->rangeL->value)
    //             ->varType->name,
    //         "a integer", 0, 0);
    // }
    // if (std::any_cast<VariableDescriptor *>(ast->rangeR->value)->varType !=
    //         SymbolTable::lookfor_type(TYPE_BASIC_INT64) &&
    //     is_possible_to_level_up(
    //         std::any_cast<VariableDescriptor *>(ast->rangeR->value)->varType,
    //         SymbolTable::lookfor_type(TYPE_BASIC_INT64)) == false) {
    //     throw TypeErrorException(
    //         "right range of `for` is not an integer",
    //         std::any_cast<VariableDescriptor *>(ast->rangeL->value)
    //             ->varType->name,
    //         "a integer", 0, 0);
    // }

    code()->push_block(blockInit);
    auto iterVar = castValue(ast->itervar->accept(*this));
    auto rangeL = castValue(ast->rangeL->accept(*this));
    code()->createVarAssign(iterVar, rangeL);
    code()->pop_block();

    code()->push_block(blockCond);
    auto rangeR = castValue(ast->rangeR->accept(*this));
    auto cond = symbolTable()->create_variable(
        symbolTable()->lookfor_type(TYPE_BASIC_INT), false, false);
    code()->createOptBinary(cond, iterVar, rangeR, "<=");
    code()->pop_block();

    code()->push_block(blockStep);
    auto const1 = symbolTable()->create_variable(
        symbolTable()->lookfor_type(TYPE_BASIC_INT), false, false);
    code()->createVariableDecl(const1);
    code()->createConstDecl(const1, 1);
    code()->createOptBinary(iterVar, iterVar, const1, "+");
    code()->pop_block();

    code()->push_block(blockBody);
    ast->body->accept(*this);
    code()->pop_block();

    code()->createLoop(cond, blockInit, blockCond, blockStep, blockBody);

    return nullptr;
}

std::any ASTDispatcher::gen_while_statement(WhileStatementAST *ast) {
    auto blockCond = code()->createWhiteBlock();
    auto blockBody = code()->createWhiteBlock();

    code()->push_block(blockCond);
    auto subCond = castValue(ast->condition->accept(*this));
    code()->pop_block();

    code()->push_block(blockBody);
    ast->body->accept(*this);
    code()->pop_block();

    code()->createLoop(subCond, nullptr, nullptr, nullptr, blockBody);

    return nullptr;
}

std::any ASTDispatcher::gen_function(FunctionAST *ast) {
    // // FIX: too ugly but work
    // // the reason for this is that we should inject args descriptor to its
    // block
    // // 这段代码太裂了

    // // add symbol for function
    // // and also there is need to create new node for processing 形参
    // for (auto arg : ast->sig->args) {
    //     arg->varType->accept(*this);
    //     arg->_varType = arg->varType->_descriptor;
    // }

    // std::vector<VariableDescriptor *> argsDescriptors;
    // for (int i = 0; i < (int)ast->sig->args.size(); i++) {
    //     // FIX: too ugly
    //     auto tempDescriptor = new VariableDescriptor(
    //         ast->sig->args[i]->sig->name, ast->sig->args[i]->_varType,
    //         ast->sig->args[i]->isRef, false);
    //     argsDescriptors.push_back(tempDescriptor);
    // }
    // auto functiondesciptor = new FunctionDescriptor(
    //     ast->sig->sig, argsDescriptors, ast->sig->_resultType);
    // SymbolTable::insert_function(ast->sig->sig, functiondesciptor);

    // CodeCollector::src() << map_variable_type(ast->sig->_resultType) << " ";
    // CodeCollector::src() << functiondesciptor->name;
    // CodeCollector::src() << "(";

    // for (int i = 0; i < (int)ast->sig->args.size(); i++) {
    //     // FIX: too ugly
    //     auto tempDescriptor = new VariableDescriptor(
    //         ast->sig->args[i]->sig->name, ast->sig->args[i]->_varType,
    //         ast->sig->args[i]->isRef, false);
    //     put_variable_decl(tempDescriptor);
    //     if (i + 1 != ast->sig->args.size()) CodeCollector::src() << ", ";
    // }
    // CodeCollector::src() << ")";
    // CodeCollector::push_back();

    // SymbolTable::enter();
    // for (auto arg : ast->sig->args) {
    //     SymbolTable::create_variable(arg->sig->name, arg->_varType,
    //     arg->isRef,
    //                                  false);
    // }
    // // used for pascal's strange return
    // if (ast->sig->_resultType != SymbolTable::lookfor_type(TYPE_BASIC_VOID))
    // {
    //     ast->body->exprs.insert(
    //         ast->body->exprs.begin(),
    //         new VariableDeclAST(new VariableExprAST("__ret"),
    //                             ast->sig->resultType, false, false));
    //     // add result call to body
    //     ast->body->exprs.push_back(new ReturnAST(new
    //     VariableExprAST("__ret")));
    // } else {
    //     //无用指令
    //     ast->body->exprs.push_back(new VariableDeclAST(
    //         new VariableExprAST("__ret"), new BasicTypeAST(TYPE_BASIC_INT),
    //         false, false));
    // }

    // // I am nearly throwing up
    // ast->body->accept(*this);
    // SymbolTable::exit();

    return nullptr;
}

std::any ASTDispatcher::gen_function_signature(FunctionSignatureAST *ast) {
    assert(false);
}

std::any ASTDispatcher::gen_block(BlockAST *ast) {
    VMBlock *block = code()->createBlock();
    code()->push_block(block);
    for (auto expr : ast->exprs) {
        expr->accept(*this);
    }
    code()->pop_block();

    code()->push_back(block);

    return nullptr;
}

std::any ASTDispatcher::gen_struct(StructDeclAST *ast) {
    StructDescriptor *structD = new StructDescriptor(ast->sig, {});

    for (auto var : ast->varDecl) {
        var->accept(*this);
        auto typeDescriptor = var->varType->_descriptor;
        structD->push(var->sig->name, typeDescriptor);
    }

    symbolTable()->insert_type(ast->sig, structD);
    code()->createTypeDecl(structD);
    ast->_descriptor = structD;
    return nullptr;
}

std::any ASTDispatcher::gen_variable_decl(VariableDeclAST *ast) {
    VariableDescriptor *var = nullptr;
    if (ast->varType) {
        ast->varType->accept(*this);
        auto varType = ast->varType->_descriptor;
        var = symbolTable()->create_variable(ast->sig->name, varType,
                                             ast->isRef, ast->isConst);
    } else if (ast->initVal) {
        auto initVal = castValue(ast->initVal->accept(*this));
        auto varType = initVal->varType;
        var = symbolTable()->create_variable(ast->sig->name, varType,
                                             ast->isRef, ast->isConst);
    } else {
        throw TypeErrorException("missing type for variable " + ast->sig->name,
                                 "<>", "<?>", 0, 0);
    }

    if (ast->initVal) {
        if (ast->initVal->const_type == CONSTANT_INT)
            code()->createConstDecl(var, ast->initVal->val_int);
        else if (ast->initVal->const_type == CONSTANT_REAL)
            code()->createConstDecl(var, ast->initVal->val_float);
        else {
            assert(false);
        }
    } else {
        code()->createVariableDecl(var);
    }

    return nullptr;
}