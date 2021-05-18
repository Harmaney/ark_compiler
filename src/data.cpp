#include "data.h"

#include <iostream>
#include <stdexcept>

#include "err.h"
#include "logger.h"
#include "parser.h"

void AST::assign(std::any other) {
    using namespace NodeProperties;
    extraData["row"] = cast<AST*>(other)->extraData["row"];
}
void LOG_WALK(AST* ast) {
    //    WALK_AST<<"ARRIVE "<<ast<<std::endl;
    gen_info.push_back({ {"ARRIVE", (uint64_t)ast} });
}

void ExprAST::accept(ASTDispatcher& dispatcher) {
    LOG_WALK(this);

    switch (type) {
    case AST_BINARY_EXPR:
        dynamic_cast<BinaryExprAST*>(this)->accept(dispatcher);
        break;
    case AST_NUMBER_EXPR:
        dynamic_cast<NumberExprAST*>(this)->accept(dispatcher);
        break;
    case AST_VARIABLE_EXPR:
        dynamic_cast<VariableExprAST*>(this)->accept(dispatcher);
        break;
    default:
        throw std::invalid_argument("unknown type of expr");
        break;
    }
}

void BasicTypeAST::accept(ASTDispatcher& dispatcher) {
    LOG_WALK(this);
    dispatcher.gen_basic_type(this);
}

void TypeDefAST::accept(ASTDispatcher& dispatcher) {
    LOG_WALK(this);
    dispatcher.gen_type_def(this);
}

void ArrayTypeDeclAST::accept(ASTDispatcher& dispatcher) {
    LOG_WALK(this);
    switch (this->itemAST->type) {
    case AST_BASIC_TYPE:
        static_cast<BasicTypeAST*>(this->itemAST)->accept(dispatcher);
        break;
    case AST_ARRAY_TYPE:
        static_cast<ArrayTypeDeclAST*>(this->itemAST)->accept(dispatcher);
        break;
    default:
        throw std::invalid_argument("not support type in array");
        break;
    }
    dispatcher.gen_array_type_decl(this);
}

void PointerTypeDeclAST::accept(ASTDispatcher& dispatcher) {
    LOG_WALK(this);
    this->ref->accept(dispatcher);
    dispatcher.gen_pointer_type_decl(this);
}

void NumberExprAST::accept(ASTDispatcher& dispatcher) {
    LOG_WALK(this);
    term_print.detail() << "into number ast" << std::endl;
    dispatcher.gen_number_expr(this);
    term_print.detail() << "out number ast" << std::endl;
}

void StringExprAST::accept(ASTDispatcher& dispatcher) {
    LOG_WALK(this);
    term_print.detail() << "into string ast" << std::endl;
    dispatcher.gen_string_expr(this);
    term_print.detail() << "out string ast" << std::endl;
}

void CharExprAST::accept(ASTDispatcher& dispatcher) {
    LOG_WALK(this);
    term_print.detail() << "into char ast" << std::endl;
    dispatcher.gen_char_expr(this);
    term_print.detail() << "out char ast" << std::endl;
}

void VariableExprAST::accept(ASTDispatcher& dispatcher) {
    LOG_WALK(this);
    term_print.detail() << "into variable ast" << std::endl;
    dispatcher.gen_variable_expr(this);
    term_print.detail() << "out variable ast" << std::endl;
}

void BinaryExprAST::accept(ASTDispatcher& dispatcher) {
    LOG_WALK(this);
    LHS->accept(dispatcher);
    RHS->accept(dispatcher);

    dispatcher.gen_binary_expr(this);
}

void UnaryExprAST::accept(ASTDispatcher& dispatcher) {
    LOG_WALK(this);
    expr->accept(dispatcher);

    dispatcher.gen_unary_expr(this);
}

void ReturnAST::accept(ASTDispatcher& dispatcher) {
    LOG_WALK(this);
    if (this->expr) this->expr->accept(dispatcher);

    dispatcher.gen_return(this);
}

void CallExprAST::accept(ASTDispatcher& dispatcher) {
    LOG_WALK(this);
    for (auto arg : args) {
        arg->accept(dispatcher);
    }

    term_print.detail() << "into cell ast" << std::endl;
    dispatcher.gen_call_expr(this);
    term_print.detail() << "out cell ast" << std::endl;
}

void BlockAST::accept(ASTDispatcher& dispatcher) {
    LOG_WALK(this);
    dispatcher.gen_block_begin(this);
    SymbolTable::enter();

    for (auto expr : exprs) {
        switch (expr->type) {
        case AST_BINARY_EXPR:
            static_cast<BinaryExprAST*>(expr)->accept(dispatcher);
            break;
        case AST_NUMBER_EXPR:
            static_cast<NumberExprAST*>(expr)->accept(dispatcher);
            break;
        case AST_VARIABLE_EXPR:
            static_cast<VariableExprAST*>(expr)->accept(dispatcher);
            break;
        case AST_STRUCT_DECL:
            static_cast<StructDeclAST*>(expr)->accept(dispatcher);
            break;
        case AST_RETURN:
            static_cast<ReturnAST*>(expr)->accept(dispatcher);
            break;
        case AST_CALL_EXPR:
            static_cast<CallExprAST*>(expr)->accept(dispatcher);
            break;
        case AST_VARIABLE_DECL:
            static_cast<VariableDeclAST*>(expr)->accept(dispatcher);
            break;
        case AST_FOR_STATEMENT:
            static_cast<ForStatementAST*>(expr)->accept(dispatcher);
            break;
        case AST_WHILE_STATEMENT:
            static_cast<WhileStatementAST*>(expr)->accept(dispatcher);
            break;
        case AST_IF_STATEMENT:
            static_cast<IfStatementAST*>(expr)->accept(dispatcher);
            break;
        default:
            throw std::invalid_argument("unknown type of AST in Block");
            break;
        }
    }

    SymbolTable::exit();
    dispatcher.gen_block_end(this);
}

void VariableDeclAST::accept(ASTDispatcher& dispatcher) {
    LOG_WALK(this);
    if (this->varType)
        this->varType->accept(dispatcher);
    else if (this->initVal)
        this->initVal->accept(dispatcher);
    else {
        throw TypeErrorException("missing type for variable " + this->sig->name,
            "<>", "<?>", 0, 0);
    }

    term_print.detail() << "into variable ast" << std::endl;
    dispatcher.gen_variable_decl(this);
    term_print.detail() << "out variable ast" << std::endl;
}

void ForStatementAST::accept(ASTDispatcher& dispatcher) {
    LOG_WALK(this);
    this->itervar->accept(dispatcher);
    this->rangeL->accept(dispatcher);
    this->rangeR->accept(dispatcher);

    term_print.detail() << "into for ast" << std::endl;
    dispatcher.gen_for_statement_begin(this);
    this->body->accept(dispatcher);
    dispatcher.gen_for_statement_end(this);
    term_print.detail() << "into for ast" << std::endl;
}

void WhileStatementAST::accept(ASTDispatcher& dispatcher) {
    LOG_WALK(this);
    term_print.detail() << "into condition of while" << std::endl;
    this->condition->accept(dispatcher);
    term_print.detail() << "out condition of while" << std::endl;

    dispatcher.gen_while_statement_begin(this);
    this->body->accept(dispatcher);

    // to get the new result of condition
    this->condition->accept(dispatcher);
    dispatcher.gen_while_statement_end(this);
}

void IfStatementAST::accept(ASTDispatcher& dispatcher) {
    LOG_WALK(this);
    term_print.detail() << "into if ast" << std::endl;
    this->condition->accept(dispatcher);
    dispatcher.gen_if_statement_begin(this);
    this->body_false->accept(dispatcher);
    dispatcher.gen_if_statement_else(this);
    this->body_true->accept(dispatcher);
    dispatcher.gen_if_statement_end(this);

    // TODO: body and else if
    term_print.detail() << "out if ast" << std::endl;
}

void GlobalAST::accept(ASTDispatcher& dispatcher) {
    LOG_WALK(this);
    term_print.detail() << "into global ast" << std::endl;
    dispatcher.gen_global_begin(this);

    for (auto var : this->vars) {
        var->accept(dispatcher);
    }

    // convert mainBlock into main function
    this->functions.push_back(new FunctionAST(
        new FunctionSignatureAST("main", {}, new BasicTypeAST(TYPE_BASIC_INT)),
        {}, this->mainBlock));
    // add return 0 to mainBlock
    this->mainBlock->exprs.push_back(new ReturnAST(new NumberExprAST(0)));
    this->mainBlock->extraData["IS_MAIN"] = 1;
    for (auto func : functions) {
        func->accept(dispatcher);
    }

    dispatcher.gen_global_end(this);
    term_print.detail() << "out global ast" << std::endl;
}

void FunctionAST::accept(ASTDispatcher& dispatcher) {
    LOG_WALK(this);
    // that's too bad
    this->sig->resultType->accept(dispatcher);
    this->sig->_resultType = sig->resultType->_descriptor;
    // This is for real var
    // for(auto arg:this->sig->args){
    //     arg->accept(dispatcher);
    // }

    // put all variable declaration into block
    this->body->exprs.insert(this->body->exprs.begin(), this->varDecls.begin(),
        this->varDecls.end());

    dispatcher.gen_function(this);
}

void FunctionSignatureAST::accept(ASTDispatcher& dispatcher) {}

void StructDeclAST::accept(ASTDispatcher& dispatcher) {
    LOG_WALK(this);
    dispatcher.gen_struct(this);
}

///////////////////////////////////////

std::string _SymbolTable::get_slot() {
    return "t" + std::to_string(group) + "_" + std::to_string(next_slot++);
}

void _SymbolTable::insert_variable(std::string sig, VariableDescriptor* var) {
    refVar[sig] = var;
}
void _SymbolTable::insert_type(std::string sig, SymbolDescriptor* var) {
    refType[sig] = var;
}
ArrayTypeDescriptor* _SymbolTable::create_array_type(SymbolDescriptor* item,
    int sz, int beg) {
    if (this->hasArrayType.count(std::make_pair(item, sz))) {
        return static_cast<ArrayTypeDescriptor*>(
            this->search_type(this->hasArrayType[std::make_pair(item, sz)]));
    }
    auto slot = get_slot();
    ArrayTypeDescriptor* arrayDescriptor =
        new ArrayTypeDescriptor(slot, item, sz, beg);
    this->insert_type(slot, arrayDescriptor);
    return arrayDescriptor;
}

PointerTypeDescriptor* _SymbolTable::create_pointer_type(
    SymbolDescriptor* item) {
    if (this->hasPointerType.count(item)) {
        return static_cast<PointerTypeDescriptor*>(
            this->search_type(this->hasPointerType[item]));
    }
    auto slot = get_slot();
    PointerTypeDescriptor* pointerDescriptor =
        new PointerTypeDescriptor(slot, item);
    this->insert_type(slot, pointerDescriptor);
    return pointerDescriptor;
}

VariableDescriptor* _SymbolTable::search_variable(std::string sig) {
    if (refVar.count(sig)) {
        return refVar[sig];
    }
    else
        return NULL;
}

SymbolDescriptor* _SymbolTable::search_type(std::string sig) {
    if (refType.count(sig)) {
        return refType[sig];
    }
    else
        return NULL;
}

_SymbolTable* SymbolTable::current;
_SymbolTable* SymbolTable::root;

void SymbolTable::init() {
    root = new _SymbolTable(0);
    current = root;
}

void SymbolTable::enter() {
    _SymbolTable* t = new _SymbolTable(current->group + 1);
    t->parent = current;
    current = t;
}

void SymbolTable::exit() {
    _SymbolTable* t = current;
    current = t->parent;
    delete t;
}

VariableDescriptor* SymbolTable::create_variable(std::string sig,
    SymbolDescriptor* type,
    bool isRef, bool isConst) {
    current->insert_variable(sig,
        new VariableDescriptor(sig, type, isRef, isConst));
    return current->search_variable(sig);
}

VariableDescriptor* SymbolTable::create_variable_G(std::string sig,
    SymbolDescriptor* type,
    bool isRef) {
    root->insert_variable(sig, new VariableDescriptor(sig, type, isRef, false));
    return root->search_variable(sig);
}

VariableDescriptor* SymbolTable::create_variable(SymbolDescriptor* type,
    bool isRef, bool isConst) {
    std::string sig = current->get_slot();
    current->insert_variable(sig,
        new VariableDescriptor(sig, type, isRef, isConst));
    return current->search_variable(sig);
}

VariableDescriptor* SymbolTable::create_variable_G(SymbolDescriptor* type,
    bool isRef) {
    std::string sig = root->get_slot();
    root->insert_variable(sig, new VariableDescriptor(sig, type, isRef, false));
    return root->search_variable(sig);
}

VariableDescriptor* SymbolTable::lookfor_variable(std::string sig) {
    _SymbolTable* table = current;
    while (table) {
        if (table->search_variable(sig)) return table->search_variable(sig);
        table = table->parent;
    }
    return NULL;
}

void SymbolTable::insert_type(std::string sig, SymbolDescriptor* descriptor) {
    term_print.debug() << "insert type " << sig << std::endl;
    current->insert_type(sig, descriptor);
}

void SymbolTable::insert_function(std::string sig,
    FunctionDescriptor* descriptor) {
    std::vector<SymbolDescriptor*> symbols;
    for (auto arg : descriptor->args) {
        symbols.push_back(arg->varType);
    }
    auto name = get_internal_function_name(sig, symbols);
    descriptor->name = name;
    term_print.debug() << "???? " << name << " " << descriptor->name
        << std::endl;
    insert_type(name, descriptor);
}

ArrayTypeDescriptor* SymbolTable::create_array_type(SymbolDescriptor* item,
    int sz, int beg) {
    // array type are seen as global type
    return root->create_array_type(item, sz, beg);
}

PointerTypeDescriptor* SymbolTable::create_pointer_type(
    SymbolDescriptor* item) {
    return root->create_pointer_type(item);
}

SymbolDescriptor* SymbolTable::lookfor_type(std::string sig) {
    _SymbolTable* table = current;
    while (table) {
        if (table->search_type(sig)) return table->search_type(sig);
        table = table->parent;
    }
    return NULL;
}

FunctionDescriptor* SymbolTable::lookfor_function(
    std::string sig, std::vector<SymbolDescriptor*> args) {
    auto name = get_internal_function_name(sig, args);
    return static_cast<FunctionDescriptor*>(lookfor_type(name));
}

std::string get_internal_function_name(std::string name,
    std::vector<SymbolDescriptor*> args) {
    std::stringstream ss;
    ss << name;
    for (auto arg : args) {
        ss << "_" << arg->name;
    }
    // ss<<"__"<<res->name;
    return ss.str();
}

///////////////////////////////////////////////

int TagTable::next_slot;

void TagTable::init() { TagTable::next_slot = 0; }

std::string* TagTable::create_tag_G() {
    return new std::string("L" + std::to_string(TagTable::next_slot++));
}
