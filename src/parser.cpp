#include "parser.h"

#include <algorithm>
#include <any>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <random>
#include <set>
#include <vector>

#include "data.h"
#include "logger.h"
#define inset(y, x) x.find(y) != x.end()
#define table_exist 1

std::string globalS;
std::set<std::string> terminal, non_terminal;
typedef std::pair<std::string, std::vector<std::string>> Expr;
std::vector<Expr> productions;
std::map<int, Expr> production_content;
std::map<Expr, int> production_ID;
int production_num = 0;
std::vector<std::string> empty_vec;
std::map<std::string, std::set<std::string>> first;
std::map<std::string, std::set<std::vector<std::string>>> rhs_set;
std::set<std::string> get_epsilon;
void init() {
    std::ifstream input("../files/grammar.txt");
    if (!input) {
        term_print.fatal() << "grammar.txt not found." << std::endl;
        abort();
    }
    int mode = 0;
    std::string lhs;
    std::vector<std::string> rhs;
    while (!input.eof()) {
        std::string str;
        input >> str;
        if (str.empty()) continue;
        if (str == "->")
            mode = 1;
        else if (str == "@" || str == "|") {
            rhs_set[lhs].insert(rhs);

            Expr prod = make_pair(lhs, rhs);
            productions.push_back(prod);
            production_ID[prod] = ++production_num;
            production_content[production_num] = prod;

            if (rhs.empty()) get_epsilon.insert(lhs);
            rhs.clear();
            if (str == "@") mode = 0;
        }
        else if (mode == 0) {
            lhs = str, non_terminal.insert(str);
            if (globalS.empty()) globalS = lhs;
        }
        else
            rhs.push_back(str), terminal.insert(str);
    }
    for (auto str : non_terminal) terminal.erase(str);

    //	for(auto str:Nonterminal){
    //		cout<<str<<"->";
    //		for(auto rhs:rhs_set[str]){
    //			for(auto x:rhs) cout<<x<<" ";
    //			cout<<" | ";
    //		}
    //		cout<<endl;
    //	}

    //	for(auto str:Nonterminal) cout<<str<<endl;
    //	cout<<"terminal\n";
    //	for(auto str:terminal) cout<<str<<endl;
}
template <typename T>
bool merge(const std::set<T>& A, std::set<T>& B) {
    bool flg = false;
    for (auto x : A) {
        if (B.find(x) == B.end()) {
            flg = true;
            B.insert(x);
        }
    }
    return flg;
}
std::set<std::string> get_first(std::vector<std::string> strlist) {
    std::set<std::string> res;
    std::vector<std::string>::iterator it = strlist.begin();
    std::string Head = *it;
    while (1) {
        merge(first[Head], res);
        if (inset(Head, terminal)) break;
        if (inset(Head, get_epsilon)) {
            it++;
            if (it == strlist.end()) {
                merge(first[""], res);
                break;
            }
            else {
                Head = *it;
            }
        }
        else
            break;
    }
    return res;
}
void get_first() {
    for (auto x : terminal) first[x].insert(x);
    first["$"].insert("$");
    first[""].insert("");
    bool changed = true;
    while (changed) {
        changed = false;
        for (auto lhs : non_terminal) {
            for (auto rhs : rhs_set[lhs]) {
                if (rhs.empty()) continue;
                changed |= merge(get_first(rhs), first[lhs]);
            }
        }
    }

    //	for(auto str:terminal){
    //		cout<<str<<":";
    //		for(auto x:First[str]) cout<<x<<" ";
    //		cout<<endl;
    //	}
    //
    //	for(auto str:Nonterminal){
    //		cout<<str<<":";
    //		for(auto x:First[str]) cout<<x<<" ";
    //		cout<<endl;
    //	}
}
// typedef tuple<string,vector<string>,vector<string>,string>
// Item;//0A->1a.2Bb,3x
bool operator<(std::vector<std::string> A, std::vector<std::string> B) {
    for (int i = 0; i < A.size() && i < B.size(); i++) {
        if (A[i] != B[i]) return A[i] < B[i];
    }
    if (A.size() != B.size()) return A.size() < B.size();
}
bool operator==(std::vector<std::string> A, std::vector<std::string> B) {
    if (A.size() != B.size()) return false;
    for (int i = 0; i < A.size() && i < B.size(); i++) {
        if (A[i] != B[i]) return false;
    }
    return true;
}
struct Item {
    std::string lhs;
    std::string look_ahead;
    std::vector<std::string> previous, next;
    Item() {}
    Item(std::string lhs, std::vector<std::string> previous,
        std::vector<std::string> next, std::string look_ahead)
        : lhs(lhs), previous(previous), next(next), look_ahead(look_ahead) {}
    bool operator<(const Item other) const {
        if (lhs != other.lhs) {
            return lhs < other.lhs;
        }
        else if (!(previous == other.previous)) {
            return previous < other.previous;
        }
        else if (!(next == other.next)) {
            return next < other.next;
        }
        else
            return look_ahead < other.look_ahead;
    }
    bool operator==(const Item other) const {
        if (lhs != other.lhs) {
            return false;
        }
        else if (!(previous == other.previous)) {
            return false;
        }
        else if (!(next == other.next)) {
            return false;
        }
        else if (look_ahead != other.look_ahead)
            return false;
        return true;
    }
};
std::map<std::set<Item>, int> item_set;
std::map<int, std::set<Item>> item_content;
int item_num = 0;
std::set<Item> get_closure(Item I) {
    std::set<Item> J;
    J.insert(I);
    bool changed = true;
    do {
        changed = false;
        std::set<Item> J_new;
        std::set<Item> J_add;
        merge(J, J_new);
        for (auto item : J_new) {
            std::vector<std::string> next = item.next;
            if (next.empty()) continue;
            std::string B = *next.begin();  // B
            std::vector<std::string> temp = next;
            temp.erase(temp.begin());
            temp.push_back(item.look_ahead);  // temp = bx

            std::set<std::string> bx_first = get_first(temp);

            for (auto rhs : rhs_set[B]) {
                for (auto str : bx_first) {
                    Item new_item = Item(B, empty_vec, rhs, str);  // B->.y,bx
                    J_add.insert(new_item);
                }
            }
        }
        changed |= merge(J_add, J);
    } while (changed);
    return J;
}
std::set<Item> go(std::set<Item> I, std::string X) {
    std::set<Item> J;
    for (auto item : I) {
        // A->a.Bb,x
        if (item.next.empty()) continue;
        if (*item.next.begin() == X) {
            item.previous.push_back(*item.next.begin());
            item.next.erase(item.next.begin());
            merge(get_closure(item), J);
        }
    }
    return J;
}
void get_items() {
    item_content[item_num] =
        get_closure(Item(globalS, empty_vec, *rhs_set[globalS].begin(), "$"));
    item_set[get_closure(
        Item(globalS, empty_vec, *rhs_set[globalS].begin(), "$"))] = item_num++;
    std::set<std::string> sign;
    merge(terminal, sign);
    merge(non_terminal, sign);
    // ofstream itemout("item.txt");
    bool changed = true;
    do {
        changed = false;
        for (auto I : item_set) {
            for (auto X : sign) {
                if (X.empty()) continue;
                std::set<Item> go_ix = go(I.first, X);
                if (!go_ix.empty() && item_set.count(go_ix) == 0) {
                    item_content[item_num] = go_ix;
                    item_set[go_ix] = item_num++;
                    changed = true;
                    // itemout<<"================\n";
                    // for(auto x:Goix){
                    // 	itemout<<x.lhs<<"-> ";
                    // 	for(auto str:x.previous) itemout<<str<<" ";
                    // 	itemout<<". ";
                    // 	for(auto str:x.next) itemout<<str<<" ";
                    // 	itemout<<", ";
                    // 	itemout<<x.look_ahead<<endl;
                    // }
                }
            }
        }
    } while (changed);
    printf("%d\n", item_num);
}
enum ACTION { SHIFT = 1, REDUCE, ACC };
std::map<std::pair<int, std::string>, std::pair<ACTION, int>> action_table;
std::map<std::pair<int, std::string>, int> goto_table;
std::map<std::pair<int, std::string>, std::string> parser_err_table;
void add_action(std::pair<ACTION, int> act, std::pair<int, std::string> pos) {
    if (action_table.count(pos)) {
        if (action_table[pos] != act) {
            // if (actionTable[pos].first == SHIFT) return;
            /*std::cerr << "???" << endl;
            cerr << actionTable[pos].first << " " << actionTable[pos].second
                << endl;
            cerr << act.first << " " << act.second << endl;
            cerr << pos.first << " " << pos.second << endl;*/
        }
    }
    action_table[pos] = act;
}
void load_table() {
    std::ifstream inf("../files/analyse_table.txt");
    if (!inf) {
        term_print.fatal() << "analyse_table.txt not found." << std::endl;
        abort();
    }
    int I, id;
    std::string a, data;
    while (!inf.eof()) {
        inf >> I >> a >> data >> id;
        if (id == -1) {
            goto_table[std::make_pair(I, a)] = stoi(data);
        }
        else if (id == -2) {
            parser_err_table[std::make_pair(I, a)] = data;
        }
        else {
            enum ACTION ACT;
            if (stoi(data) == 1)
                ACT = SHIFT;
            else if (stoi(data) == 2)
                ACT = REDUCE;
            else if (stoi(data) == 3)
                ACT = ACC;
            else
                assert(0);
            action_table[std::make_pair(I, a)] = std::make_pair(ACT, id);
        }
    }
}
void generate_table() {
    if (table_exist) {
        load_table();
        return;
    }
    std::cerr << "generating table\n";
    get_items();
    for (auto I : item_set) {
        for (auto it : I.first) {
            std::vector<std::string> next = it.next;
            if (!next.empty()) {
                add_action(
                    std::make_pair(SHIFT, item_set[go(I.first, *next.begin())]),
                    std::make_pair(I.second, *next.begin()));
            }
            else if (it.lhs != globalS) {
                add_action(
                    std::make_pair(
                        REDUCE, production_ID[make_pair(it.lhs, it.previous)]),
                    std::make_pair(I.second, it.look_ahead));
            }
            else {
                add_action(std::make_pair(ACC, 0),
                    std::make_pair(I.second, it.look_ahead));
            }
        }
        for (auto A : non_terminal) {
            std::set<Item> GoIA = go(I.first, A);
            if (GoIA.empty()) continue;
            goto_table[make_pair(I.second, A)] = item_set[GoIA];
        }
        bool flg = false;
        for (auto it : I.first) {
            if (!it.next.empty()) {
                if (*it.next.begin() == ";") flg = true;
            }else {
                if (it.look_ahead == ";") flg = true;
            }
        }
        if (flg) {
            for (auto x : terminal) {
                if (action_table.count(std::make_pair(I.second, x)) == 0) {
                    parser_err_table[std::make_pair(I.second, x)] = ";";
                }
            }
        }
    }
    std::ofstream of("analyse_table.txt");
    if (!of) {
        std::cerr << "找不到文件analyse_table.txt" << std::endl;
        abort();
    }
    for (auto I : item_set) {
        for (auto a : terminal) {
            if (action_table.count(std::make_pair(I.second, a))) {
                of << I.second << " " << a << " "
                    << action_table[std::make_pair(I.second, a)].first << " "
                    << action_table[std::make_pair(I.second, a)].second
                    << std::endl;
            }
            else {
                if (parser_err_table.count(std::make_pair(I.second, a)))
                    of << I.second << " " << a << " "
                    << parser_err_table[make_pair(I.second, a)] << " -2"
                    << std::endl;
            }
        }
        if (action_table.count(std::make_pair(I.second, "$"))) {
            of << I.second << " "
                << "$"
                << " " << action_table[std::make_pair(I.second, "$")].first
                << " " << action_table[std::make_pair(I.second, "$")].second
                << std::endl;
        }
        for (auto A : non_terminal) {
            if (goto_table.count(std::make_pair(I.second, A))) {
                of << I.second << " " << A << " "
                    << goto_table[std::make_pair(I.second, A)] << " -1"
                    << std::endl;
            }
        }
    }
    std::cout << "table ready" << std::endl;
}

struct TokenItem {
    std::string raw, type, parser_symbol;
    int row, column;
    TokenItem() {}
    TokenItem(std::string raw, std::string type, std::string parser_symbol,
        int row, int column)
        : raw(raw),
        type(type),
        parser_symbol(parser_symbol),
        row(row),
        column(column) {}
    void load(std::tuple<std::string, int, int, std::string> i) {
        auto [_raw, _row, _column, _type] = i;
        if (_raw == "")
            *this = { "", "", "$", 0, 0 };
        else {
            std::string _parser_symbol;
            if (_type == "keyword" || _type == "punc") {
                if (_raw == ":=")
                    _parser_symbol = "assignOP";
                else if (_raw == "<>" || _raw == ">" || _raw == "<" ||
                    _raw == ">=" || _raw == "<=")
                    _parser_symbol = "relOP";
                else if (_raw == "*" || _raw == "/" || _raw == "div" ||
                    _raw == "mod" || _raw == "and")
                    _parser_symbol = "mulOP";
                else
                    _parser_symbol = _raw;
            }
            else if (_type == "identify")
                _parser_symbol = "ID";
            else if (_type == "intVal" || _type == "realVal" ||
                _type == "stringVal")
                _parser_symbol = _type;
            else
                _parser_symbol = _raw;
            *this = { _raw, _type, _parser_symbol, _row, _column };
        }
    }
};

std::string rand_name() {
    static int index = 0;
    std::string ret = "anonymous_" + std::to_string(index);
    index += 1;
    return ret;
}

GrammarTreeNode* analyse(TokenQueue& tq) {
    using namespace NodeProperties;

    //记录下来每个node->prop的类型
    std::vector<GrammarTreeNode*> unlinked_nodes;
    int cur_state;
    std::vector<int> states;
    std::vector<std::string> symbols;
    auto do_reduce = [&](Expr expr) {
        auto eval_const = [&](AST* node) {
            int flag = 1;
            NumberExprAST* number;
            if (typeid(node) == typeid(UnaryExprAST*)) {
                auto ptr = dynamic_cast<UnaryExprAST*>(node);
                if (ptr->op == "-") flag = -1;
                number = dynamic_cast<NumberExprAST*>(ptr->expr);
            }
            else
                number = dynamic_cast<NumberExprAST*>(node);
            assert(number);
            if (number->const_type == CONSTANT_INT)
                return new NumberExprAST(number->val_int * flag);
            else
                return new NumberExprAST(number->val_float * flag);
        };
        auto update_properties = [&](GrammarTreeNode* node) {
            if (node->type == "S") {  // S -> ProgramStruct
                node->prop = cast<S>(node->son[0]->prop);
            }
            else if (node->type == "ProgramStruct") {
                auto M = cast<ProgramBody>(node->son[2]->prop);
                std::vector<VariableDeclAST*> vars;
                for (auto i : M["var"])
                    vars.push_back(cast<VariableDeclAST*>(i));
                std::vector<FunctionAST*> functions;
                for (auto i : M["function"])
                    functions.push_back(cast<FunctionAST*>(i));
                TypeDeclaration types;
                for (auto i : M["type"]) types.push_back(cast<TypeDefAST*>(i));
                assert(M["body"].size() == 1);
                node->prop = new std::remove_pointer<ProgramStruct>::type(
                    vars, types, functions,
                    cast<BlockAST*>((M["body"]).front()));
            }
            else if (node->type == "ProgramHead") {
                node->prop = nullptr;
            }
            else if (node->type ==
                "ProgramBody") {  // ProgramBody -> Component
                                  // CompoundStatement @
                auto M = cast<ProgramBody>(node->son[0]->prop);
                M["body"].push_back(
                    cast<CompoundStatement>(node->son[1]->prop));
                node->prop = M;
            }
            else if (node->type == "Component") {
                Component prop;
                if (node->son.empty()) {
                    // Do nothing.
                }
                else if (node->son[0]->raw ==
                    "const") {  // Component -> const ConstDeclaration ;
                                // Component
                    prop = cast<Component>(node->son[3]->prop);
                    for (auto i : cast<ConstDeclaration>(node->son[1]->prop))
                        prop["var"].push_back(i);
                }
                else if (node->son[0]->raw ==
                    "type") {  // Component -> type TypeDeclaration ;
                               // Component
                    prop = cast<Component>(node->son[3]->prop);
                    auto list = cast<TypeDeclaration>(node->son[1]->prop);
                    for (auto iter = list.rbegin(); iter != list.rend(); ++iter)
                        prop["type"].insert(prop["type"].begin(), *iter);
                }
                else if (node->son[0]->raw ==
                    "var") {  // Component -> var VarDeclaration ;
                              // Component
                    prop = cast<Component>(node->son[3]->prop);
                    for (auto i : cast<VarDeclaration>(node->son[1]->prop))
                        prop["var"].push_back(i);
                }
                else if (node->son[0]->type ==
                    "Subprogram") {  // Component -> Subprogram ;
                                     // Component
                    prop = cast<Component>(node->son[2]->prop);
                    prop["function"].insert(prop["function"].begin(),
                        node->son[0]->prop);
                }
                node->prop = prop;
            }
            else if (node->type == "IDList") {
                IDList prop;
                if (node->son.size() == 1) {  // IDList -> ID
                    prop = IDList{ node->son[0]->raw };
                }
                else {  // IDList -> IDList , ID
                    prop = cast<IDList>(node->son[0]->prop);
                    prop.push_back(node->son[2]->raw);
                }
                node->prop = prop;
            }
            else if (node->type == "ConstDeclaration") {
                ConstDeclaration prop;
                if (node->son.size() ==
                    3) {  // ConstDeclaration -> ID = ConstValue
                    prop.push_back(new VariableDeclAST(
                        new VariableExprAST(node->son[0]->raw), nullptr, false,
                        true,
                        eval_const(cast<ConstValue>(node->son[2]->prop))));
                }
                else {  // ConstDeclaration -> ConstDeclaration ; ID =
                       // ConstValue
                    prop = cast<ConstDeclaration>(node->son[0]->prop);
                    prop.push_back(new VariableDeclAST(
                        new VariableExprAST(node->son[2]->raw), nullptr, false,
                        true,
                        eval_const(cast<ConstValue>(node->son[4]->prop))));
                }
                node->prop = prop;
            }
            else if (node->type == "ConstValue") {
                ConstValue prop;
                if (node->son[0]->raw == "+") {  // ConstValue -> + Num
                    prop = new UnaryExprAST("+",
                        cast<ExprAST*>(node->son[1]->prop));
                    prop->set_row(node->son[0]->row);
                }
                else if (node->son[0]->raw == "-") {  // ConstValue -> - Num
                    prop = new UnaryExprAST("-",
                        cast<ExprAST*>(node->son[1]->prop));
                    prop->set_row(node->son[0]->row);
                }
                else if (node->son.size() == 1) {  // ConstValue -> Value
                    prop = cast<Value>(node->son[0]->prop);
                }
                node->prop = prop;
            }
            else if (node->type == "TypeDeclaration") {  // new
                TypeDeclaration prop;
                if (node->son.size() ==
                    3) {  // TypeDeclaration -> ID = ActualType
                    prop.push_back(
                        new TypeDefAST(new BasicTypeAST(node->son[0]->raw),
                            cast<ActualType>(node->son[2]->prop)));
                }
                else {  // TypeDeclaration -> TypeDeclaration ; ID =
                       // ActualType
                    prop = cast<TypeDeclaration>(node->son[0]->prop);
                    prop.push_back(
                        new TypeDefAST(new BasicTypeAST(node->son[2]->raw),
                            cast<ActualType>(node->son[4]->prop)));
                }
                node->prop = prop;
            }
            else if (node->type == "ActualType") {
                ActualType prop;
                if (node->son.size() == 1) {  // ActualType -> Type
                    prop = cast<Type>(node->son[0]->prop);
                }
                else {  // ActualType -> record VarDeclaration ; end
                    prop = new StructDeclAST(
                        rand_name(), cast<VarDeclaration>(node->son[1]->prop));
                    prop->set_row(node->son[0]->row);
                }
                node->prop = prop;
            }
            else if (node->type == "VarDeclaration") {
                VarDeclaration prop;
                if (node->son.size() == 3) {  // VarDeclaration -> IDList : Type
                    for (std::string id : cast<IDList>(node->son[0]->prop)) {
                        prop.push_back(new VariableDeclAST(
                            new VariableExprAST(id),
                            cast<Type>(node->son[2]->prop), false));
                    }
                }
                else {  // VarDeclaration -> VarDeclaration ; IDList :
                       // ActualType
                    prop = cast<VarDeclaration>(node->son[0]->prop);
                    for (std::string id : cast<IDList>(node->son[2]->prop)) {
                        prop.push_back(new VariableDeclAST(
                            new VariableExprAST(id),
                            cast<ActualType>(node->son[4]->prop), false));
                    }
                }
                node->prop = prop;
            }
            else if (node->type == "Type") {
                Type prop;
                if (node->son[0]->raw ==
                    "array") {  // Type -> array [ Period ] of BasicType
                    prop = cast<BasicType>(node->son[5]->prop);
                    auto range = cast<Period>(node->son[2]->prop);
                    std::reverse(range.begin(), range.end());
                    for (auto [L, R] : range) {
                        prop = new ArrayTypeDeclAST(prop, L, R);
                    }
                    prop->set_row(node->son[0]->row);
                }
                else if (node->son[0]->type ==
                    "BasicType") {  // Type -> BasicType
                    prop = cast<BasicType>(node->son[0]->prop);
                }
                else {  // Type -> ^ BasicType
                    prop = new PointerTypeDeclAST(
                        cast<BasicType>(node->son[1]->prop));
                    prop->set_row(node->son[0]->row);
                }
                node->prop = prop;
            }
            else if (node->type == "BasicType") {
                BasicType prop = new BasicTypeAST(node->son[0]->raw);
                prop->set_row(node->son[0]->row);
                node->prop = prop;
            }
            else if (node->type == "Period") {
                Period prop;
                if (node->son.size() == 3) {  // Period -> Digits .. Digits
                    prop = Period{
                        std::make_pair(cast<Digits>(node->son[0]->prop),
                                       cast<Digits>(node->son[2]->prop)) };
                }
                else {  // Period -> Period , Digits .. Digits
                    prop = cast<Period>(node->son[0]->prop);
                    prop.emplace_back(cast<Digits>(node->son[2]->prop),
                        cast<Digits>(node->son[4]->prop));
                }
                node->prop = prop;
            }
            else if (node->type ==
                "Subprogram") {  // Subprogram -> SubprogramHead ;
                                 // SubprogramBody
                auto [parameter, body] =
                    cast<SubprogramBody>(node->son[2]->prop);
                Subprogram prop = new FunctionAST(
                    cast<SubprogramHead>(node->son[0]->prop), parameter, body);
                node->prop = prop;
            }
            else if (node->type == "SubprogramHead") {
                SubprogramHead prop;
                if (node->son.size() ==
                    3) {  // SubprogramHead -> procedure ID FormalParameter
                    prop = new FunctionSignatureAST(
                        node->son[1]->raw,
                        cast<FormalParameter>(node->son[2]->prop),
                        new BasicTypeAST("void"));
                }
                else {  // SubprogramHead -> function ID FormalParameter :
                       // BasicType
                    prop = new FunctionSignatureAST(
                        node->son[1]->raw,
                        cast<FormalParameter>(node->son[2]->prop),
                        cast<BasicType>(node->son[4]->prop));
                }
                prop->set_row(node->son[0]->row);
                node->prop = prop;
            }
            else if (node->type == "FormalParameter") {
                FormalParameter prop;
                if (node->son.size() <= 2) {
                    prop = FormalParameter();
                }
                else {  // FormalParameter -> ( ParameterList )
                    prop = cast<ParameterList>(node->son[1]->prop);
                }
                node->prop = prop;
            }
            else if (node->type == "ParameterList") {
                ParameterList prop;
                if (node->son.size() == 1) {  // ParameterList -> Parameter
                    prop = cast<ParameterList>(node->son[0]->prop);
                }
                else {  // ParameterList -> ParameterList ; Parameter
                    prop = cast<ParameterList>(node->son[0]->prop);
                    for (auto i : cast<Parameter>(node->son[2]->prop))
                        prop.push_back(i);
                }
                node->prop = prop;
            }
            else if (node->type == "Parameter") {
                // Parameter -> VarParameter
                // Parameter -> ValueParameter
                node->prop = cast<Parameter>(node->son[0]->prop);
            }
            else if (node->type == "VarParameter") {
                // VarParameter -> var ValueParameter
                VarParameter prop = cast<ValueParameter>(node->son[1]->prop);
                for (auto& ptr : prop) ptr->isRef = true;
                node->prop = prop;
            }
            else if (node->type == "ValueParameter") {
                // ValueParameter -> IDList : BasicType
                ValueParameter prop;
                std::vector<VariableDeclAST*> list;
                for (std::string i : cast<IDList>(node->son[0]->prop)) {
                    prop.push_back(new VariableDeclAST(
                        new VariableExprAST(i),
                        cast<BasicType>(node->son[2]->prop), false));
                }
                node->prop = prop;
            }
            else if (node->type == "SubprogramBody") {
                // SubprogramBody -> SubComponent CompoundStatement
                node->prop =
                    SubprogramBody(cast<SubComponent>(node->son[0]->prop),
                        cast<CompoundStatement>(node->son[1]->prop));
            }
            else if (node->type == "SubComponent") {
                SubComponent prop;
                if (node->son.size() == 0)
                    node->prop = {};
                else {
                    // SubComponent -> const ConstDeclaration ; SubComponent
                    // SubComponent -> var VarDeclaration ; SubComponent
                    prop = cast<SubComponent>(node->son[3]->prop);
                    if (node->son[0]->raw == "const") {
                        for (auto i :
                            cast<ConstDeclaration>(node->son[1]->prop))
                            prop.insert(prop.begin(), i);
                    }
                    else {
                        for (auto i : cast<VarDeclaration>(node->son[1]->prop))
                            prop.insert(prop.begin(), i);
                    }
                }
                node->prop = prop;
            }
            else if (node->type == "CompoundStatement") {
                // CompoundStatement -> begin StatementList end
                CompoundStatement prop;
                prop = new BlockAST(cast<StatementList>(node->son[1]->prop));
                prop->set_row(node->son[0]->row);
                node->prop = prop;
            }
            else if (node->type == "StatementList") {
                StatementList prop;
                if (node->son.size() == 1) {  // StatementList -> Statement
                    auto statement = cast<Statement>(node->son[0]->prop);
                    if (!statement)
                        prop = StatementList{};
                    else
                        prop = StatementList{ statement };
                }
                else {  // StatementList -> StatementList ; Statement
                    prop = cast<StatementList>(node->son[0]->prop);
                    auto statement = cast<Statement>(node->son[2]->prop);
                    if (statement) prop.push_back(statement);
                }
                node->prop = prop;
            }
            else if (node->type == "Statement") {
                Statement prop;
                if (node->son.empty()) {
                    prop = nullptr;
                }
                else if (node->son[0]->type == "ComposedVariable") {
                    // Statement -> ComposedVariable assignOP Expression
                    prop = new BinaryExprAST(
                        node->son[1]->raw, cast<ExprAST*>(node->son[0]->prop),
                        cast<ExprAST*>(node->son[2]->prop));
                    prop->set_row(node->son[1]->row);
                }
                else if (node->son[0]->type == "ProcedureCall(ARG)") {
                    prop = cast<Statement>(node->son[0]->prop);
                }
                else if (node->son[0]->raw == "if") {
                    if (node->son[3]->type == "CompoundStatement") {
                        // Statement -> if Expression then CompoundStatement
                        // ElsePart
                        prop = new IfStatementAST(
                            cast<Expression>(node->son[1]->prop),
                            cast<CompoundStatement>(node->son[3]->prop),
                            cast<ElsePart>(node->son[4]->prop));
                    }
                    else {
                        // Statement -> if Expression then Statement ElsePart
                        auto statement = cast<Statement>(node->son[3]->prop);
                        auto statementlist = StatementList{ statement };
                        prop = new IfStatementAST(
                            cast<Expression>(node->son[1]->prop),
                            new std::remove_pointer<CompoundStatement>::type(
                                statementlist),
                            cast<ElsePart>(node->son[4]->prop));
                    }
                    prop->set_row(node->son[0]->row);
                }
                else if (node->son[0]->raw == "for") {
                    // Statement -> for ID assignOP Expression to Expression do
                    // CompoundStatement
                    if (node->son[7]->type == "CompoundStatement") {
                        prop = new ForStatementAST(
                            new VariableExprAST(node->son[1]->raw),
                            cast<Expression>(node->son[3]->prop),
                            cast<Expression>(node->son[5]->prop),
                            cast<CompoundStatement>(node->son[7]->prop));
                    }
                    else {
                        // Statement -> for ID assignOP Expression to Expression
                        // do Statement
                        auto statement = cast<Statement>(node->son[7]->prop);
                        auto statementlist = StatementList{ statement };
                        prop = new ForStatementAST(
                            new VariableExprAST(node->son[1]->raw),
                            cast<Expression>(node->son[3]->prop),
                            cast<Expression>(node->son[5]->prop),
                            new std::remove_pointer<CompoundStatement>::type(
                                statementlist));
                    }
                    prop->set_row(node->son[0]->row);
                }
                else if (node->son[0]->raw == "while") {
                    if (node->son[3]->type == "CompoundStatement") {
                        // Statement -> while Expression do CompoundStatement
                        prop = new WhileStatementAST(
                            cast<ExprAST*>(node->son[1]->prop),
                            cast<CompoundStatement>(node->son[3]->prop));
                    }
                    else {
                        // Statement -> while Expression do Statement
                        auto statement = cast<Statement>(node->son[3]->prop);
                        auto statementlist = StatementList{ statement };
                        prop = new WhileStatementAST(
                            cast<ExprAST*>(node->son[1]->prop),
                            new std::remove_pointer<CompoundStatement>::type(
                                statementlist));
                    }
                    prop->set_row(node->son[0]->row);
                }
                node->prop = prop;
            }
            else if (node->type == "Variable") {  // Variable -> ID IDVarpart
                Variable prop = new VariableExprAST(node->son[0]->raw);
                auto part = cast<IDVarpart>(node->son[1]->prop);
                for (auto expr : part) {
                    prop = new BinaryExprAST("[]", prop, expr);
                }
                prop->set_row(node->son[0]->row);
                node->prop = prop;
            }
            else if (node->type == "IDVarpart") { //fixme
                IDVarpart prop;
                if (node->son.size() == 0) {
                    prop = IDVarpart();
                }
                else { // IDVarpart -> [ ExpressionList ]
                    prop = cast<IDVarpart>(node->son[1]->prop);
                }
                node->prop = prop;
            }
            else if (node->type == "ProcedureCall(ARG)") {
                ProcedureCall prop;
                if (node->son.size() ==
                    4) {  // ProcedureCall(ARG) -> ID ( ExpressionList )
                    prop = new CallExprAST(
                        node->son[0]->raw,
                        cast<ExpressionList>(node->son[2]->prop));
                }
                else {  // ProcedureCall(ARG) -> ID ( )
                    prop = new CallExprAST(node->son[0]->raw, {});
                }
                prop->set_row(node->son[0]->row);
                node->prop = prop;
            }
            else if (node->type == "ElsePart") {
                ElsePart prop;
                if (node->son.size() >
                    0) {  // ElsePart -> else CompoundStatement
                    if (node->son[1]->type == "CompoundStatement") {
                        prop = cast<ElsePart>(node->son[1]->prop);
                    }
                    else { //ElsePart -> else Statement
                        prop =
                            new BlockAST({ cast<Statement>(node->son[1]->prop) });
                    }
                    prop->set_row(node->son[0]->row);
                }
                else { //ElsePart -> 
                    prop = new BlockAST({});
                }
                node->prop = prop;
            }
            else if (node->type == "ExpressionList") { //fixme
                ExpressionList prop;
                if (node->son.size() == 1) {  // ExpressionList -> Expression
                    prop = ExpressionList{ cast<Expression>(node->son[0]->prop) };
                }
                else {  // ExpressionList -> ExpressionList , Expression
                    prop = cast<ExpressionList>(node->son[0]->prop);
                    prop.push_back(cast<Expression>(node->son[2]->prop));
                }
                node->prop = prop;
            }
            else if (node->type == "Expression") {
                /*
                    Expression -> SimpleExpression relOP SimpleExpression
                    Expression -> SimpleExpression = SimpleExpression
                    Expression -> stringVal
                    Expression -> SimpleExpression
                    Expression -> Expression ^
                */
                Expression prop = nullptr;
                if (node->son.size() == 3) {
                    prop = new BinaryExprAST(
                        node->son[1]->raw, cast<ExprAST*>(node->son[0]->prop),
                        cast<ExprAST*>(node->son[2]->prop));
                    prop->set_row(node->son[1]->row);
                }
                else if (node->son.size() == 2) {
                    prop = new UnaryExprAST("^",
                        cast<ExprAST*>(node->son[0]->prop));
                    prop->set_row(node->son[1]->row);
                }
                else {
                    if (node->son[0]->parserSymbol == "stringVal") {
                        prop = new StringExprAST(node->son[0]->raw);
                        prop->set_row(node->son[0]->row);
                    }
                    else {  // Expression -> SimpleExpression
                        prop = cast<Expression>(node->son[0]->prop);
                    }
                }
                node->prop = prop;
            }
            else if (node->type == "SimpleExpression") {
                SimpleExpression prop;
                if (node->son.size() == 1)  // SimpleExpression -> Term
                    prop = cast<SimpleExpression>(node->son[0]->prop);
                else {  // SimpleExpression -> SimpleExpression addOP Term
                    prop =
                        new BinaryExprAST(cast<std::string>(node->son[1]->prop),
                            cast<ExprAST*>(node->son[0]->prop),
                            cast<ExprAST*>(node->son[2]->prop));
                    prop->set_row(node->son[1]->row);
                }
                node->prop = prop;
            }
            else if (node->type == "Term") {
                Term prop = nullptr;
                if (node->son.size() == 1)  // Term -> Factor
                    prop = cast<Term>(node->son[0]->prop);
                else {  // Term -> Term mulOP Factor
                    prop = new BinaryExprAST(
                        node->son[1]->raw, cast<ExprAST*>(node->son[0]->prop),
                        cast<ExprAST*>(node->son[2]->prop));
                    prop->set_row(node->son[1]->row);
                }
                node->prop = prop;
            }
            else if (node->type == "Factor") {
                Factor prop = nullptr;
                if (node->son[0]->type == "Value") {  // Factor -> Value
                    prop = cast<Value>(node->son[0]->prop);
                }
                else if (node->son[0]->type == "Num") {  // Factor -> Num
                    prop = cast<Factor>(node->son[0]->prop);
                }
                else if (node->son[0]->type ==
                    "ComposedVariable") {  // Factor -> ComposedVariable
                    prop = cast<Factor>(node->son[0]->prop);
                }
                else if (node->son[1]->type == "Num") {
                    // Factor -> + Num
                    // Factor -> - Num
                    prop = new UnaryExprAST(node->son[0]->raw,
                        cast<ExprAST*>(node->son[1]->prop));
                    prop->assign(node->son[1]->prop);
                }
                else if (node->son[1]->type == "Factor") {
                    // Factor -> not Factor
                    // Factor -> uminus Factor
                    prop = new UnaryExprAST(node->son[0]->raw,
                        cast<ExprAST*>(node->son[1]->prop));
                    prop->assign(node->son[1]->prop);
                }
                else
                    abort();
                node->prop = prop;
            }
            else if (node->type == "ComposedVariable") {
                ComposedVariable prop = nullptr;
                if (node->son[0]->type == "Variable") {
                    prop = cast<ComposedVariable>(node->son[0]->prop);
                }
                else if (node->son[0]->type == "ProcedureCall(ARG)") {
                    prop = cast<ComposedVariable>(node->son[0]->prop);
                }
                else if (node->son[1]->type ==
                    "Expression") {  // ( Expression )
                    prop = cast<ComposedVariable>(node->son[1]->prop);
                    prop->set_row(node->son[0]->row);
                }
                else { //ComposedVariable -> ComposedVariable . ID
                    prop = new BinaryExprAST(
                        ".", cast<ExprAST*>(node->son[0]->prop),
                        new StringExprAST(node->son[2]->raw));
                    prop->set_row(node->son[2]->row);
                }
                node->prop = prop;
            }
            else if (node->type == "Num") {
                Num prop;
                if (node->son[0]->parserSymbol == "intVal") {  // Num -> intVal
                    prop = new NumberExprAST(std::stoi(node->son[0]->raw));
                }
                else if (node->son[0]->parserSymbol == "realVal") { //Num -> realVal
                    prop = new NumberExprAST(std::stod(node->son[0]->raw));
                }
                else
                    abort();
                prop->set_row(node->son[0]->row);
                node->prop = prop;
            }
            else if (node->type == "Digits") { //Digits -> intVal
                Digits prop = new NumberExprAST(std::stoi(node->son[0]->raw));
                prop->set_row(node->son[0]->row);
                node->prop = prop;
            }
            else if (node->type == "addOP") { //fixme
                node->prop = node->son[0]->raw;
                assert(node->son[0]->raw == "+" || node->son[0]->raw == "-" ||
                    node->son[0]->raw == "or");
                assert(node->prop.type() == typeid(addOP));
            }
            else if (node->type == "Value") {
                Value prop;
                if (node->son[0]->type == "Num") {  // Value -> Num
                    prop = cast<Value>(node->son[0]->prop);
                }
                else {  // Value -> stringVal
                    prop = new StringExprAST(node->son[0]->raw);
                    prop->set_row(node->son[0]->row);
                }
                node->prop = prop;
            }
            else
                abort();
            // cerr << node->type << endl;
            assert(node->prop.has_value());  //每个结点都必有一个属性
        };
        int pop_num = expr.second.size();
        std::vector<GrammarTreeNode*> reduced_node;
        for (int i = 0; i < pop_num; i++) {
            states.pop_back();
            symbols.pop_back();
            reduced_node.push_back(*unlinked_nodes.rbegin());
            unlinked_nodes.pop_back();
        }
        cur_state = *states.rbegin();
        states.push_back(goto_table[make_pair(cur_state, expr.first)]);
        symbols.push_back(expr.first);
        GrammarTreeNode* new_node =
            new GrammarTreeNode("", expr.first, expr.first, 0, 0);
        reverse(reduced_node.begin(), reduced_node.end());
        for (auto node : reduced_node) new_node->son.push_back(node);
        update_properties(new_node);
        unlinked_nodes.push_back(new_node);
        new_node->Report();
    };
    term_print.debug() << "start analyse" << std::endl;
    states.push_back(0);
    symbols.push_back("");
    TokenItem n;
    n.load(tq.front());
    tq.pop();
    for (;;) {
        cur_state = *states.rbegin();
        // std::cerr << curState << '\n';
        // for (auto x : symbols) std::cerr << x << " ";
        // std::cerr << endl;
        // std::cerr << " str: " << N.raw << " type: " << N.type << endl;
        // std::cerr << "row:" << N.row << " column: " << N.column << " type: "
        // << N.parserSymbol << endl;
        if (action_table.count(std::make_pair(cur_state, n.parser_symbol)) ==
            0) {
            if (parser_err_table.count(
                std::make_pair(cur_state, n.parser_symbol))) {
                std::cout << "Error: Expect "
                    << parser_err_table[std::make_pair(cur_state,
                        n.parser_symbol)]
                    << "  but " << n.parser_symbol << " found."
                    << std::endl;
            }
            throw;
        }
        std::pair<ACTION, int> act =
            action_table[make_pair(cur_state, n.parser_symbol)];
        if (act.first == SHIFT) {
            states.push_back(act.second);
            symbols.push_back(n.parser_symbol);
            unlinked_nodes.push_back(new GrammarTreeNode(
                n.raw, n.type, n.parser_symbol, n.row, n.column));
            (*unlinked_nodes.rbegin())->Report();
            assert(!tq.empty());
            n.load(tq.front());
            tq.pop();

        }
        else if (act.first == REDUCE) {
            Expr prod = production_content[act.second];
            do_reduce(prod);
        }
        else if (act.first == ACC) {
            do_reduce({ "S", {"ProgramStruct"} });
            term_print.debug() << "ACCEPT!" << std::endl;
            break;
        }
    }
    assert(unlinked_nodes.size() == 1);
    return *unlinked_nodes.begin();
}

void check_grammar_tree(uint64_t gid, int dep) {
    /*for (auto x : gramTree[gid]) {
        cout << dep << " " << x.word << " " << x.type << endl;
        check_grammar_tree(x.ID, dep + 1);
    }*/
}

GlobalAST* parser_work(TokenQueue tq) {
    init();
    get_first();
    generate_table();
    tq.push({ "", 0, 0, "" });
    auto root = analyse(tq);
    check_grammar_tree(0, 0);
    return NodeProperties::cast<GlobalAST*>(root->prop);
}
