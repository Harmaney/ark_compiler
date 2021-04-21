#include "data.h"

#include "spdlog/spdlog.h"

void NumberExprAST::accept(ASTDispatcher &dispatcher) {
    spdlog::trace("into number ast");
    dispatcher.genNumberExpr(this);
    spdlog::trace("out number ast");
}

void VariableExprAST::accept(ASTDispatcher &dispatcher) {
    spdlog::trace("into variable ast");
    dispatcher.genVariableExpr(this);
    spdlog::trace("out variable ast");
}

void BinaryExprAST::accept(ASTDispatcher &dispatcher) {
    switch (LHS->type) {
        case BINARY_EXPR:
            dynamic_cast<BinaryExprAST *>(LHS)->accept(dispatcher);
            break;
        case NUMBER_EXPR:
            dynamic_cast<NumberExprAST *>(LHS)->accept(dispatcher);
            break;
        case VARIABLE_EXPR:
            dynamic_cast<VariableExprAST *>(LHS)->accept(dispatcher);
            break;
        default:
            break;
    }
    switch (RHS->type) {
        case BINARY_EXPR:
            dynamic_cast<BinaryExprAST *>(RHS)->accept(dispatcher);
            break;
        case NUMBER_EXPR:
            dynamic_cast<NumberExprAST *>(RHS)->accept(dispatcher);
            break;
        case VARIABLE_EXPR:
            dynamic_cast<VariableExprAST *>(RHS)->accept(dispatcher);
            break;
        default:
            break;
    }

    dispatcher.genBinaryExpr(this);
}

void CallExprAST::accept(ASTDispatcher &dispatcher) {
    for (auto arg : args) {
        switch (arg->type) {
            case BINARY_EXPR:
                dynamic_cast<BinaryExprAST *>(arg)->accept(dispatcher);
                break;
            case NUMBER_EXPR:
                dynamic_cast<NumberExprAST *>(arg)->accept(dispatcher);
                break;
            case VARIABLE_EXPR:
                dynamic_cast<VariableExprAST *>(arg)->accept(dispatcher);
                break;
            default:
                break;
        }
    }

    spdlog::trace("into call ast");
    dispatcher.genCallExpr(this);
    spdlog::trace("out call ast");
}

void BlockAST::accept(ASTDispatcher &dispatcher) {
    dispatcher.genBlockBegin(this);
    SymbolTable::enter();

    for (auto expr : exprs) {
        switch (expr->type) {
            case BINARY_EXPR:
                static_cast<BinaryExprAST *>(expr)->accept(dispatcher);
                break;
            case NUMBER_EXPR:
                static_cast<NumberExprAST *>(expr)->accept(dispatcher);
                break;
            case VARIABLE_EXPR:
                static_cast<VariableExprAST *>(expr)->accept(dispatcher);
                break;
            case CALL_EXPR:
                static_cast<CallExprAST *>(expr)->accept(dispatcher);
                break;
            case VARIABLE_DECL:
                static_cast<VariableDeclAST *>(expr)->accept(dispatcher);
                break;
            default:
                break;
        }
    }

    SymbolTable::exit();
    dispatcher.genBlockEnd(this);
}

void VariableDeclAST::accept(ASTDispatcher &dispatcher) {
    spdlog::trace("into variable ast");
    dispatcher.genVariableDecl(this);
    spdlog::trace("out variable ast");
}

void GlobalAST::accept(ASTDispatcher &dispatcher) {
    spdlog::trace("into global ast");
    dispatcher.genGlobalBegin(this);

    for(auto func:functions){
        func->accept(dispatcher);
    }

    dispatcher.genGlobalEnd(this);
    spdlog::trace("out global ast");
}

void FunctionAST::accept(ASTDispatcher &dispatcher){
    // TODO: symboltable
    sig->accept(dispatcher);
    body->accept(dispatcher);
}

void FunctionSignatureAST::accept(ASTDispatcher &dispatcher){
    dispatcher.genFunctionSignature(this);
}

///////////////////////////////////////

_SymbolTable::_SymbolTable() {
    nextSlot = 0;
    parent = NULL;
}

std::string _SymbolTable::getSlot() { return "t" + std::to_string(nextSlot++); }

void _SymbolTable::insert(std::string sig, Variable *var) { refVar[sig] = var; }

Variable *_SymbolTable::search(std::string sig) {
    if (refVar.count(sig)) {
        return refVar[sig];
    } else
        return NULL;
}

_SymbolTable *SymbolTable::current;
_SymbolTable *SymbolTable::root;

void SymbolTable::init() {
    root = new _SymbolTable();
    current = root;
}

void SymbolTable::enter() {
    _SymbolTable *t = new _SymbolTable();
    t->parent = current;
    current = t;
}

void SymbolTable::exit() {
    _SymbolTable *t = current;
    current = t->parent;
    delete t;
}

Variable *SymbolTable::createVariable(std::string sig, VariableType type) {
    current->insert(sig, new Variable{sig, type, false});
    return current->search(sig);
}

Variable *SymbolTable::createVariableG(std::string sig, VariableType type) {
    root->insert(sig, new Variable{sig, type, false});
    return root->search(sig);
}

Variable *SymbolTable::createVariable(VariableType type) {
    std::string sig = current->getSlot();
    current->insert(sig, new Variable{sig, type, false});
    return current->search(sig);
}

Variable *SymbolTable::createVariableG(VariableType type) {
    std::string sig = current->getSlot();
    root->insert(sig, new Variable{sig, type, false});
    return root->search(sig);
}

Variable *SymbolTable::lookfor(std::string sig) {
    _SymbolTable *table = current;
    while (table) {
        if (table->search(sig)) return table->search(sig);
        table = table->parent;
    }
    return NULL;
}