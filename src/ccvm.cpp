#include "ccvm.h"

#include <string>

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
////////////////////////////

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
    } else
        return NULL;
}

SymbolDescriptor* _SymbolTable::search_type(std::string sig) {
    if (refType.count(sig)) {
        return refType[sig];
    } else
        return NULL;
}

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

SymbolDescriptor* SymbolTable::insert_type(std::string sig,
                                           SymbolDescriptor* descriptor) {
    term_print.debug() << "insert type " << sig << std::endl;
    current->insert_type(sig, descriptor);
    return descriptor;
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
    assert(false);
}

////////////////////////////
CodeCollector::CodeCollector() {
    root = new VMWhiteBlock();
    push_block(root);
}
void CodeCollector::push_block(VMWhiteBlock* block) { cur.push(block); }
void CodeCollector::pop_block() { cur.pop(); }

void CodeCollector::push_back(VMAbstract* vm) {
    assert(cur.size());
    cur.top()->push_back(vm);
}

void CodeCollector::output() {
    assert(root);
    term_print.debug() << root->output() << std::endl;
}

void CodeCollector::output(std::ostream& out) {
    assert(root);
    out << root->output() << std::endl;
}

void CodeCollector::clear() {
    assert(cur.size());
    cur.top()->clear();
}

std::string CodeCollector::create_tag_G() {
    return "L" + std::to_string(next_slot++);
}

VMWhiteBlock* CodeCollector::createWhiteBlock() { return new VMWhiteBlock(); }
VMBlock* CodeCollector::createBlock() { return new VMBlock(); }

std::string map_variable_type(SymbolDescriptor* type) {
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
        // throw std::invalid_argument("unknown map type " + type->name);
    }
}

void CodeCollector::createTypeDecl(ArrayTypeDescriptor* arrayTypeDescriptor) {
    std::stringstream ss;
    ss << "typedef " << map_variable_type(arrayTypeDescriptor->itemDescriptor)
       << " " << arrayTypeDescriptor->name << "[" << arrayTypeDescriptor->sz
       << "];" << std::endl;
    push_back(new VMString(ss.str()));
}

void CodeCollector::createTypeDecl(PointerTypeDescriptor* pointTypeDescriptor) {
    std::stringstream ss;
    ss << "typedef " << map_variable_type(pointTypeDescriptor->ref) << " "
       << "* " << pointTypeDescriptor->name << ";" << std::endl;
    push_back(new VMString(ss.str()));
}

void CodeCollector::createAkaType(std::string newName,
                                  SymbolDescriptor* typeDescriptor) {
    std::stringstream ss;
    ss << "typedef " << map_variable_type(typeDescriptor) << " " << newName
       << ";" << std::endl;
    push_back(new VMString(ss.str()));
}

std::string getVarialbeDecl(Value* value) {
    std::stringstream ss;
    // TODO: handle different type
    if (value->varType->type == DESCRIPTOR_STRUCT) {
        ss << "struct ";
    }
    ss << map_variable_type(value->varType);
    if (value->isRef) ss << "*";
    ss << " " << value->name;
    return ss.str();
}

std::string getVarialbeDecl(Value* value, std::string assign) {
    std::stringstream ss;
    // TODO: handle different type
    if (value->varType->type == DESCRIPTOR_STRUCT) {
        ss << "struct ";
    }
    ss << map_variable_type(value->varType);
    if (value->isRef) ss << "*";
    ss << " " << value->name;
    ss << "=" << assign;
    return ss.str();
}

std::string getVariableExpr(Value* value) {
    std::stringstream ss;
    ss<<"(";
    if (value->isRef) ss << "*";
    ss << value->name;
    ss<<")";
    return ss.str();
}

void CodeCollector::createVariableDecl(Value* value) {
    auto span = new VMString(getVarialbeDecl(value) + ";", true);
    push_back(span);
}

void CodeCollector::createTypeDecl(StructDescriptor* structDescriptor) {
    std::stringstream ss;
    ss << "struct " << structDescriptor->name << "{" << std::endl;
    for (auto item : structDescriptor->refVar) {
        // TODO: 重新定义形参
        ss << getVarialbeDecl(
                  new VariableDescriptor(item.first, item.second, false, false))
           << std::endl;
    }
    ss << "};";

    push_back(new VMString(ss.str()));
}

void CodeCollector::createCondBr(Value* cond, VMWhiteBlock* ok,
                                 VMWhiteBlock* no) {
    auto tagElse = create_tag_G();
    auto tagEnd = create_tag_G();
    {
        std::stringstream ss;
        ss << "if(!" << getVariableExpr(cond) << ") goto " << tagElse << ";"
           << std::endl;
        push_back(new VMString(ss.str()));
    }
    {
        auto block = new VMBlock();
        block->push_back(ok);
        push_back(block);

        std::stringstream ss;
        ss << "goto " << tagEnd << ";" << std::endl;
        ss << tagElse << ":" << std::endl;
        push_back(new VMString(ss.str()));
    }
    {
        auto block = new VMBlock();
        block->push_back(no);
        push_block(block);
        std::stringstream ss;
        ss << tagEnd << ":" << std::endl;
        push_back(new VMString(ss.str()));
    }
}

void CodeCollector::createLoop(Value* cond, VMWhiteBlock* init,
                               VMWhiteBlock* calCond, VMWhiteBlock* step,
                               VMWhiteBlock* body) {
    push_back(init);
    auto tagCalCond = create_tag_G();
    auto tagEnd = create_tag_G();
    push_back(new VMString(tagCalCond + ":", true));
    push_back(calCond);
    {
        std::stringstream ss;
        ss << "if(!" << getVariableExpr(cond) << ") goto " << tagEnd << ";"
           << std::endl;
        push_back(new VMString(ss.str()));
    }
    push_back(body);
    push_back(step);
    push_back(new VMString("goto " + tagCalCond + ";", true));
    push_back(new VMString(tagEnd + ":", true));
}

void CodeCollector::createConstDecl(VariableDescriptor* var, int value) {
    push_back(
        new VMString(getVarialbeDecl(var, std::to_string(value)) + ";", true));
}
void CodeCollector::createConstDecl(VariableDescriptor* var, double value) {
    push_back(
        new VMString(getVarialbeDecl(var, std::to_string(value)) + ";", true));
}
void CodeCollector::createConstDecl(VariableDescriptor* var, char value) {
    push_back(
        new VMString(getVarialbeDecl(var, std::to_string(value)) + ";", true));
}

void CodeCollector::createFunctionCall(VariableDescriptor* var,
                                       FunctionDescriptor* function,
                                       std::vector<VariableDescriptor*> args) {
    push_back(new VMString("function call not implemented"));
}

void CodeCollector::createReturn() {
    push_back(new VMString("return not implemented"));
}
void CodeCollector::createReturn(VariableDescriptor* t) {
    push_back(new VMString("return not implemented"));
}

void CodeCollector::createVarAssign(VariableDescriptor* lhs,
                                    VariableDescriptor* rhs, bool fetchRef) {
    std::stringstream ss;
    if (lhs->isRef && !fetchRef) ss << "*";
    ss << lhs->name;

    ss << "=";

    if (fetchRef) ss << "&";
    ss << getVariableExpr(rhs);
    ss << ";";

    push_back(new VMString(ss.str(), true));
}
void CodeCollector::createArrayAssign(Value* var, Value* array, Value* index,
                                      bool fetchRef) {
    std::stringstream ss;
    if (var->isRef && !fetchRef) ss << "*";
    ss << var->name;

    ss << "=";

    if (fetchRef) ss << "&";
    ss << "(" << getVariableExpr(array);
    ss << "[" << getVariableExpr(index) << "]";
    ss << ");";

    push_back(new VMString(ss.str(), true));
}
void CodeCollector::createStructAssign(Value* var, Value* struct1,
                                       std::string index, bool fetchRef) {
    push_back(new VMString("struct assign not implemented"));
}
void CodeCollector::createOptBinary(VariableDescriptor* res,
                                    VariableDescriptor* a,
                                    VariableDescriptor* b, std::string opt) {
    std::stringstream ss;
    ss << getVariableExpr(res) << "=";
    ss << getVariableExpr(a);
    ss << opt;
    ss << getVariableExpr(b);
    ss << ";";

    push_back(new VMString(ss.str(), true));
}