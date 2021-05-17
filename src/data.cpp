#include "data.h"

#include <iostream>
#include <stdexcept>

#include "logger.h"

void LOG_WALK(AST *ast){
    WALK_AST<<"ARRIVE "<<ast<<std::endl;
}


void ExprAST::accept(ASTDispatcher &dispatcher) {
    LOG_WALK(this);

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
            throw std::invalid_argument("unknown type of expr");
            break;
    }
}

void BasicTypeAST::accept(ASTDispatcher &dispatcher) {
    LOG_WALK(this);
    dispatcher.genBasicType(this);
}

void TypeDefAST::accept(ASTDispatcher &dispatcher) {
    LOG_WALK(this);
    dispatcher.genTypeDef(this);
}

void ArrayTypeDeclAST::accept(ASTDispatcher &dispatcher) {
    LOG_WALK(this);
    switch (this->itemAST->type) {
        case AST_BASIC_TYPE:
            static_cast<BasicTypeAST *>(this->itemAST)->accept(dispatcher);
            break;
        case AST_ARRAY_TYPE:
            static_cast<ArrayTypeDeclAST *>(this->itemAST)->accept(dispatcher);
            break;
        default:
            throw std::invalid_argument("not support type in array");
            break;
    }
    dispatcher.genArrayTypeDecl(this);
}

void PointerTypeDeclAST::accept(ASTDispatcher &dispatcher) {
    LOG_WALK(this);
    this->ref->accept(dispatcher);
    dispatcher.genPointerTypeDecl(this);
}

void NumberExprAST::accept(ASTDispatcher &dispatcher) {
    LOG_WALK(this);
    TRACE(std::cerr << "into number ast" << std::endl;)
    dispatcher.genNumberExpr(this);
    TRACE(std::cerr << "out number ast" << std::endl;)
}

void StringExprAST::accept(ASTDispatcher &dispatcher) {
    LOG_WALK(this);
    TRACE(std::cerr << "into string ast" << std::endl;)
    dispatcher.genStringExpr(this);
    TRACE(std::cerr << "out string ast" << std::endl;)
}

void VariableExprAST::accept(ASTDispatcher &dispatcher) {
    LOG_WALK(this);
    TRACE(std::cerr << "into variable ast" << std::endl;)
    dispatcher.genVariableExpr(this);
    TRACE(std::cerr << "out variable ast" << std::endl;)
}

void BinaryExprAST::accept(ASTDispatcher &dispatcher) {
    LOG_WALK(this);
    LHS->accept(dispatcher);
    RHS->accept(dispatcher);

    dispatcher.genBinaryExpr(this);
}

void UnaryExprAST::accept(ASTDispatcher &dispatcher) {
    LOG_WALK(this);
    expr->accept(dispatcher);

    dispatcher.genUnaryExpr(this);
}

void ReturnAST::accept(ASTDispatcher &dispatcher) {
    LOG_WALK(this);
    if(this->expr)
        this->expr->accept(dispatcher);

    dispatcher.genReturn(this);
}

void CallExprAST::accept(ASTDispatcher &dispatcher) {
    LOG_WALK(this);
    for (auto arg : args) {
        arg->accept(dispatcher);
    }

    TRACE(std::cerr << "into cell ast" << std::endl;)
    dispatcher.genCallExpr(this);
    TRACE(std::cerr << "out cell ast" << std::endl;)
}

void BlockAST::accept(ASTDispatcher &dispatcher) {
    LOG_WALK(this);
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
            case AST_STRUCT_DECL:
                static_cast<StructDeclAST *>(expr)->accept(dispatcher);
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
                static_cast<WhileStatementAST *>(expr)->accept(dispatcher);
                break;
            case AST_IF_STATEMENT:
                static_cast<IfStatementAST *>(expr)->accept(dispatcher);
                break;
            default:
                throw std::invalid_argument("unknown type of AST in Block");
                break;
        }
    }

    SymbolTable::exit();
    dispatcher.genBlockEnd(this);
}

void VariableDeclAST::accept(ASTDispatcher &dispatcher) {
    LOG_WALK(this);
    this->varType->accept(dispatcher);

    TRACE(std::cerr << "into variable ast" << std::endl;)
    dispatcher.genVariableDecl(this);
    TRACE(std::cerr << "out variable ast" << std::endl;)
}

void ForStatementAST::accept(ASTDispatcher &dispatcher) {
    LOG_WALK(this);
    this->itervar->accept(dispatcher);
    this->rangeL->accept(dispatcher);
    this->rangeR->accept(dispatcher);

    TRACE(std::cerr << "into for ast" << std::endl;)
    dispatcher.genForStatementBegin(this);
    this->body->accept(dispatcher);
    dispatcher.genForStatementEnd(this);
    TRACE(std::cerr << "into for ast" << std::endl;)
}

void WhileStatementAST::accept(ASTDispatcher &dispatcher) {
    LOG_WALK(this);
    TRACE(std::cerr << "into condition of while" << std::endl;)
    this->condition->accept(dispatcher);
    TRACE(std::cerr << "out condition of while" << std::endl;)

    dispatcher.genWhileStatementBegin(this);
    this->body->accept(dispatcher);

    // to get the new result of condition
    this->condition->accept(dispatcher);
    dispatcher.genWhileStatementEnd(this);
}

void IfStatementAST::accept(ASTDispatcher &dispatcher) {
    LOG_WALK(this);
    TRACE(std::cerr << "into if ast" << std::endl;)
    this->condition->accept(dispatcher);
    dispatcher.genIfStatementBegin(this);
    this->body_false->accept(dispatcher);
    dispatcher.genIfStatementElse(this);
    this->body_true->accept(dispatcher);
    dispatcher.genIfStatementEnd(this);

    // TODO: body and else if
    TRACE(std::cerr << "out if ast" << std::endl;)
}

void GlobalAST::accept(ASTDispatcher &dispatcher) {
    LOG_WALK(this);
    TRACE(std::cerr << "into global ast" << std::endl;)
    dispatcher.genGlobalBegin(this);

    for (auto var : this->vars) {
        var->accept(dispatcher);
    }

    // convert mainBlock into main function
    this->functions.push_back(new FunctionAST(
        new FunctionSignatureAST("main", {}, new BasicTypeAST(TYPE_BASIC_INT)),{},
        this->mainBlock));
    // add return 0 to mainBlock
    this->mainBlock->exprs.push_back(new ReturnAST(new NumberExprAST(0)));

    for (auto func : functions) {
        func->accept(dispatcher);
    }

    dispatcher.genGlobalEnd(this);
    TRACE(std::cerr << "out global ast" << std::endl;)
}

void FunctionAST::accept(ASTDispatcher &dispatcher) {
    LOG_WALK(this);
    // that's too bad
    this->sig->resultType->accept(dispatcher);
    this->sig->_resultType = sig->resultType->_descriptor;
    // This is for real var
    // for(auto arg:this->sig->args){
    //     arg->accept(dispatcher);
    // }

    // put all variable declaration into block
    this->body->exprs.insert(this->body->exprs.begin(),
                             this->varDecls.begin(),
                             this->varDecls.end());

    dispatcher.genFunction(this);
}

void FunctionSignatureAST::accept(ASTDispatcher &dispatcher) {}

void StructDeclAST::accept(ASTDispatcher &dispatcher) {
    LOG_WALK(this);
    dispatcher.genStruct(this);
}

///////////////////////////////////////

std::string _SymbolTable::getSlot() {
    return "t" + std::to_string(group) + "_" + std::to_string(nextSlot++);
}

void _SymbolTable::insert_variable(std::string sig, VariableDescriptor *var) {
    refVar[sig] = var;
}
void _SymbolTable::insert_type(std::string sig, SymbolDescriptor *var) {
    refType[sig] = var;
}
ArrayTypeDescriptor *_SymbolTable::create_array_type(SymbolDescriptor *item,
                                                     int sz) {
    if (this->hasArrayType.count(std::make_pair(item, sz))) {
        return static_cast<ArrayTypeDescriptor *>(
            this->searchType(this->hasArrayType[std::make_pair(item, sz)]));
    }
    auto slot = getSlot();
    ArrayTypeDescriptor *arrayDescriptor =
        new ArrayTypeDescriptor(slot, item, sz);
    this->insert_type(slot, arrayDescriptor);
    return arrayDescriptor;
}

PointerTypeDescriptor *_SymbolTable::create_pointer_type(
    SymbolDescriptor *item) {
    if (this->hasPointerType.count(item)) {
        return static_cast<PointerTypeDescriptor *>(
            this->searchType(this->hasPointerType[item]));
    }
    auto slot = getSlot();
    PointerTypeDescriptor *pointerDescriptor =
        new PointerTypeDescriptor(slot, item);
    this->insert_type(slot, pointerDescriptor);
    return pointerDescriptor;
}

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
    root = new _SymbolTable(0);
    current = root;
}

void SymbolTable::enter() {
    _SymbolTable *t = new _SymbolTable(current->group + 1);
    t->parent = current;
    current = t;
}

void SymbolTable::exit() {
    _SymbolTable *t = current;
    current = t->parent;
    delete t;
}

VariableDescriptor *SymbolTable::createVariable(std::string sig,
                                                SymbolDescriptor *type,
                                                bool isRef,bool isConst) {
    current->insert_variable(sig,
                             new VariableDescriptor(sig, type, isRef, isConst));
    return current->searchVariable(sig);
}

VariableDescriptor *SymbolTable::createVariableG(std::string sig,
                                                 SymbolDescriptor *type,
                                                 bool isRef) {
    root->insert_variable(sig, new VariableDescriptor(sig, type, isRef, false));
    return root->searchVariable(sig);
}

VariableDescriptor *SymbolTable::createVariable(SymbolDescriptor *type,
                                                bool isRef,bool isConst) {
    std::string sig = current->getSlot();
    current->insert_variable(sig,
                             new VariableDescriptor(sig, type, isRef, isConst));
    return current->searchVariable(sig);
}

VariableDescriptor *SymbolTable::createVariableG(SymbolDescriptor *type,
                                                 bool isRef) {
    std::string sig = root->getSlot();
    root->insert_variable(sig, new VariableDescriptor(sig, type, isRef, false));
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

void SymbolTable::insertType(std::string sig, SymbolDescriptor *descriptor) {
    DEBUG(std::cerr << "insert type " << sig << std::endl;)
    current->insert_type(sig, descriptor);
}

void SymbolTable::insertFunction(std::string sig,FunctionDescriptor *descriptor){
    std::vector<SymbolDescriptor*> symbols;
    for(auto arg:descriptor->args){
        symbols.push_back(arg->varType);
    }
    auto name=get_internal_function_name(sig,symbols);
    descriptor->name=name;
    std::cerr<<"???? "<<name<<" "<<descriptor->name<<std::endl;
    insertType(name,descriptor);
}


ArrayTypeDescriptor *SymbolTable::create_array_type(SymbolDescriptor *item,
                                                    int sz) {
    // array type are seen as global type
    return root->create_array_type(item, sz);
}

PointerTypeDescriptor *SymbolTable::create_pointer_type(
    SymbolDescriptor *item) {
    return root->create_pointer_type(item);
}

SymbolDescriptor *SymbolTable::lookforType(std::string sig) {
    _SymbolTable *table = current;
    while (table) {
        if (table->searchType(sig)) return table->searchType(sig);
        table = table->parent;
    }
    return NULL;
}

FunctionDescriptor* SymbolTable::lookforFunction(std::string sig,std::vector<SymbolDescriptor*> args){
    auto name=get_internal_function_name(sig,args);
    return static_cast<FunctionDescriptor*>(lookforType(name));
}

std::string get_internal_function_name(std::string name,std::vector<SymbolDescriptor *> args){
    std::stringstream ss;
    ss<<name;
    for(auto arg:args){
        ss<<"_"<<arg->name;
    }
    //ss<<"__"<<res->name;
    return ss.str();
}

///////////////////////////////////////////////

int TagTable::nextSlot;

void TagTable::init() { TagTable::nextSlot = 0; }

std::string *TagTable::createTagG() {
    return new std::string("L" + std::to_string(TagTable::nextSlot++));
}