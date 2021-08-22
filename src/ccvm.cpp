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

void TagTable::init() { next_slot = 0; }

std::string* TagTable::create_tag_G() {
    return new std::string("L" + std::to_string(next_slot++));
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

////////////////////////////
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
        // return type->name;
        throw std::invalid_argument("unknown map type " + type->name);
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

void CodeCollector::createVariableDecl(Value* value) {
    std::stringstream ss;
    // TODO: handle different type
    if (value->varType->type == DESCRIPTOR_STRUCT) {
        ss << "struct ";
    }
    ss << map_variable_type(value->varType);
    if (value->isRef) ss << "*";
    ss << " " << value->name;

    auto span = new VMString(ss.str());
    push_back(span);
}