#include "data.h"

#include "spdlog/spdlog.h"

void ExprAST::accept(ASTDispatcher &dispatcher) {
    switch (type) {
        case AST_BINARY_EXPR:
            dynamic_cast<BinaryExprAST *>(this)->accept(dispatcher);
            break;
        case AST_NUMBER_EXPR:
            dynamic_cast<NumberExprAST *>(this)->accept(dispatcher);
            break;
        case AST_VARIABLE_EXPR:
            dynamic_cast<VariableExprAST *>(this)->accept(dispatcher);
            break;
        default:
            spdlog::error("unknown type of expr: {}", type);
            break;
    }
}
void NumberExprAST::accept(ASTDispatcher &dispatcher) {
    spdlog::trace("into number ast");
    dispatcher.genNumberExpr(this);
    spdlog::trace("out number ast");
}

void StringExprAST::accept(ASTDispatcher &dispatcher) {
    spdlog::trace("into string ast");
    dispatcher.genStringExpr(this);
    spdlog::trace("out string ast");
}

void VariableExprAST::accept(ASTDispatcher &dispatcher) {
    spdlog::trace("into variable ast");
    dispatcher.genVariableExpr(this);
    spdlog::trace("out variable ast");
}

void BinaryExprAST::accept(ASTDispatcher &dispatcher) {
    LHS->accept(dispatcher);
    RHS->accept(dispatcher);

    dispatcher.genBinaryExpr(this);
}

void ReturnAST::accept(ASTDispatcher &dispatcher) {
    this->expr->accept(dispatcher);

    dispatcher.genReturn(this);
}

void CallExprAST::accept(ASTDispatcher &dispatcher) {
    for (auto arg : args) {
        arg->accept(dispatcher);
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
            case AST_BINARY_EXPR:
                static_cast<BinaryExprAST *>(expr)->accept(dispatcher);
                break;
            case AST_NUMBER_EXPR:
                static_cast<NumberExprAST *>(expr)->accept(dispatcher);
                break;
            case AST_VARIABLE_EXPR:
                static_cast<VariableExprAST *>(expr)->accept(dispatcher);
                break;
            case AST_RETURN:
                static_cast<ReturnAST *>(expr)->accept(dispatcher);
                break;
            case AST_CALL_EXPR:
                static_cast<CallExprAST *>(expr)->accept(dispatcher);
                break;
            case AST_VARIABLE_DECL:
                static_cast<VariableDeclAST *>(expr)->accept(dispatcher);
                break;
            case AST_FOR_STATEMENT:
                static_cast<ForStatementAST *>(expr)->accept(dispatcher);
                break;
            case AST_WHILE_STATEMENT:
                static_cast<WhileStatementAST*>(expr)->accept(dispatcher);
                break;
            default:
                spdlog::error("unknown type of AST in Block: {}", expr->type);
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

void ForStatementAST::accept(ASTDispatcher &dispatcher) {
    this->itervar->accept(dispatcher);
    this->rangeL->accept(dispatcher);
    this->rangeR->accept(dispatcher);

    spdlog::trace("into for ast");
    dispatcher.genForStatementBegin(this);
    this->body->accept(dispatcher);
    dispatcher.genForStatementEnd(this);
    spdlog::trace("out for ast");
}

void WhileStatementAST::accept(ASTDispatcher &dispatcher) {
    spdlog::trace("into variable ast");
    this->condition->accept(dispatcher);

    dispatcher.genWhileStatementBegin(this);
    this->body->accept(dispatcher);

    this->condition->accept(dispatcher);
    dispatcher.genWhileStatementEnd(this);
    spdlog::trace("out variable ast");
}

void IfStatementAST::accept(ASTDispatcher &dispatcher) {
    spdlog::trace("into variable ast");
    this->condition->accept(dispatcher);
    dispatcher.genIfStatementBegin(this);
    // TODO: body and else if
    spdlog::trace("out variable ast");
}

void GlobalAST::accept(ASTDispatcher &dispatcher) {
    spdlog::trace("into global ast");
    dispatcher.genGlobalBegin(this);

    for(auto var:this->vars){
        var->accept(dispatcher);
    }

    // convert mainBlock into main function
    this->functions.push_back(new FunctionAST(new FunctionSignatureAST("main",{},TYPE_BASIC_INT),this->mainBlock));
    // add return 0 to mainBlock
    this->mainBlock->exprs.push_back(new ReturnAST(new NumberExprAST(0)));

    for (auto func : functions) {
        func->accept(dispatcher);
    }

    dispatcher.genGlobalEnd(this);
    spdlog::trace("out global ast");
}

void FunctionAST::accept(ASTDispatcher &dispatcher) {
    // TODO: symboltable
    sig->accept(dispatcher);
    body->accept(dispatcher);
}

void FunctionSignatureAST::accept(ASTDispatcher &dispatcher) {
    dispatcher.genFunctionSignature(this);
}

void StructDeclAST::accept(ASTDispatcher &dispatcher) {
    dispatcher.genStruct(this);
}

///////////////////////////////////////

_SymbolTable::_SymbolTable() {
    nextSlot = 0;
    parent = NULL;
}

std::string _SymbolTable::getSlot() { return "t" + std::to_string(nextSlot++); }

void _SymbolTable::insert_variable(std::string sig, VariableDescriptor *var) { refVar[sig] = var; }
void _SymbolTable::insert_type(std::string sig, SymbolDescriptor *var) { refType[sig] = var; }

VariableDescriptor *_SymbolTable::searchVariable(std::string sig) {
    if (refVar.count(sig)) {
        return refVar[sig];
    } else
        return NULL;
}


SymbolDescriptor *_SymbolTable::searchType(std::string sig) {
    if (refType.count(sig)) {
        return refType[sig];
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

VariableDescriptor *SymbolTable::createVariable(std::string sig, std::string type) {
    current->insert_variable(sig, new VariableDescriptor(sig, type, false));
    return current->searchVariable(sig);
}

VariableDescriptor *SymbolTable::createVariableG(std::string sig, std::string type) {
    root->insert_variable(sig, new VariableDescriptor(sig, type, false));
    return root->searchVariable(sig);
}

VariableDescriptor *SymbolTable::createVariable(std::string type) {
    std::string sig = current->getSlot();
    current->insert_variable(sig, new VariableDescriptor(sig, type, false));
    return current->searchVariable(sig);
}

VariableDescriptor *SymbolTable::createVariableG(std::string type) {
    std::string sig = root->getSlot();
    root->insert_variable(sig, new VariableDescriptor(sig, type, false));
    return root->searchVariable(sig);
}

VariableDescriptor *SymbolTable::lookforVariable(std::string sig) {
    _SymbolTable *table = current;
    while (table) {
        if (table->searchVariable(sig)) return table->searchVariable(sig);
        table = table->parent;
    }
    return NULL;
}

void SymbolTable::insertType(std::string sig,SymbolDescriptor *descriptor) {
    spdlog::debug("insert type `{}`",sig);
    current->insert_type(sig,descriptor);
}

SymbolDescriptor *SymbolTable::lookforType(std::string sig) {
    _SymbolTable *table = current;
    while (table) {
        if (table->searchType(sig)) return table->searchType(sig);
        table = table->parent;
    }
    return NULL;
}

///////////////////////////////////////////////

int TagTable::nextSlot;

void TagTable::init() {
    TagTable::nextSlot=0;
}

std::string* TagTable::createTagG() {
    return new std::string("L"+std::to_string(TagTable::nextSlot++));
}