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
std::string fjcS;
std::set<std::string> Terminal, Nonterminal;
typedef std::pair<std::string, std::vector<std::string>> Expr;
std::vector<Expr> Productions;
std::map<int, Expr> Production_content;
std::map<Expr, int> Production_ID;
int Production_num = 0;
std::vector<std::string> empty_vec;
std::map<std::string, std::set<std::string>> First;
std::map<std::string, std::set<std::vector<std::string>>> RHS_set;
std::set<std::string> Get_epsilon;
void init() {
    std::ifstream input("../files/grammar.txt");
    if (!input) {
        std::cerr << "找不到grammar.txt" << std::endl;
        abort();
    }
    int mode = 0;
    std::string LHS;
    std::vector<std::string> RHS;
    while (!input.eof()) {
        std::string str;
        input >> str;
        if (str.empty()) continue;
        if (str == "->")
            mode = 1;
        else if (str == "@" || str == "|") {
            RHS_set[LHS].insert(RHS);

            Expr prod = make_pair(LHS, RHS);
            Productions.push_back(prod);
            Production_ID[prod] = ++Production_num;
            Production_content[Production_num] = prod;

            if (RHS.empty()) Get_epsilon.insert(LHS);
            RHS.clear();
            if (str == "@") mode = 0;
        } else if (mode == 0) {
            LHS = str, Nonterminal.insert(str);
            if (fjcS.empty()) fjcS = LHS;
        } else
            RHS.push_back(str), Terminal.insert(str);
    }
    for (auto str : Nonterminal) Terminal.erase(str);

    //	for(auto str:Nonterminal){
    //		cout<<str<<"->";
    //		for(auto RHS:RHS_set[str]){
    //			for(auto x:RHS) cout<<x<<" ";
    //			cout<<" | ";
    //		}
    //		cout<<endl;
    //	}

    //	for(auto str:Nonterminal) cout<<str<<endl;
    //	cout<<"Terminal\n";
    //	for(auto str:Terminal) cout<<str<<endl;
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
        merge(First[Head], res);
        if (inset(Head, Terminal)) break;
        if (inset(Head, Get_epsilon)) {
            it++;
            if (it == strlist.end()) {
                merge(First[""], res);
                break;
            } else {
                Head = *it;
            }
        } else
            break;
    }
    return res;
}
void get_first() {
    for (auto x : Terminal) First[x].insert(x);
    First["$"].insert("$");
    First[""].insert("");
    bool changed = true;
    while (changed) {
        changed = false;
        for (auto LHS : Nonterminal) {
            for (auto RHS : RHS_set[LHS]) {
                if (RHS.empty()) continue;
                changed |= merge(get_first(RHS), First[LHS]);
            }
        }
    }

    //	for(auto str:Terminal){
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
    std::string LHS;
    std::string LookAhead;
    std::vector<std::string> previous, next;
    Item() {}
    Item(std::string LHS, std::vector<std::string> previous,
         std::vector<std::string> next, std::string LookAhead)
        : LHS(LHS), previous(previous), next(next), LookAhead(LookAhead) {}
    bool operator<(const Item other) const {
        if (LHS != other.LHS) {
            return LHS < other.LHS;
        } else if (!(previous == other.previous)) {
            return previous < other.previous;
        } else if (!(next == other.next)) {
            return next < other.next;
        } else
            return LookAhead < other.LookAhead;
    }
    bool operator==(const Item other) const {
        if (LHS != other.LHS) {
            return false;
        } else if (!(previous == other.previous)) {
            return false;
        } else if (!(next == other.next)) {
            return false;
        } else if (LookAhead != other.LookAhead)
            return false;
        return true;
    }
};
std::map<std::set<Item>, int> Item_set;
std::map<int, std::set<Item>> Item_content;
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
            temp.push_back(item.LookAhead);  // temp = bx

            std::set<std::string> bx_first = get_first(temp);

            for (auto RHS : RHS_set[B]) {
                for (auto str : bx_first) {
                    Item new_item = Item(B, empty_vec, RHS, str);  // B->.y,bx
                    J_add.insert(new_item);
                }
            }
        }
        changed |= merge(J_add, J);
    } while (changed);
    return J;
}
std::set<Item> GO(std::set<Item> I, std::string X) {
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
    Item_content[item_num] =
        get_closure(Item(fjcS, empty_vec, *RHS_set[fjcS].begin(), "$"));
    Item_set[get_closure(Item(fjcS, empty_vec, *RHS_set[fjcS].begin(), "$"))] =
        item_num++;
    std::set<std::string> Sign;
    merge(Terminal, Sign);
    merge(Nonterminal, Sign);
    // ofstream itemout("item.txt");
    bool changed = true;
    do {
        changed = false;
        for (auto I : Item_set) {
            for (auto X : Sign) {
                if (X.empty()) continue;
                std::set<Item> Goix = GO(I.first, X);
                if (!Goix.empty() && Item_set.count(Goix) == 0) {
                    Item_content[item_num] = Goix;
                    Item_set[Goix] = item_num++;
                    changed = true;
                    // itemout<<"================\n";
                    // for(auto x:Goix){
                    // 	itemout<<x.LHS<<"-> ";
                    // 	for(auto str:x.previous) itemout<<str<<" ";
                    // 	itemout<<". ";
                    // 	for(auto str:x.next) itemout<<str<<" ";
                    // 	itemout<<", ";
                    // 	itemout<<x.LookAhead<<endl;
                    // }
                }
            }
        }
    } while (changed);
    printf("%d\n", item_num);
}
enum ACTION { Shift = 1, Reduce, ACC };
std::map<std::pair<int, std::string>, std::pair<ACTION, int>> actionTable;
std::map<std::pair<int, std::string>, int> Goto;
void AddAction(std::pair<ACTION, int> act, std::pair<int, std::string> pos) {
    if (actionTable.count(pos)) {
        if (actionTable[pos] != act) {
            // if (actionTable[pos].first == Shift) return;
            /*std::cerr << "???" << endl;
            cerr << actionTable[pos].first << " " << actionTable[pos].second
                << endl;
            cerr << act.first << " " << act.second << endl;
            cerr << pos.first << " " << pos.second << endl;*/
        }
    }
    actionTable[pos] = act;
}
void LoadTable() {
    std::ifstream inf("../files/analyse_table.txt");
    if (!inf) {
        std::cerr << "找不到文件analyse_table.txt" << std::endl;
        abort();
    }
    int I, id, ACT_id;
    std::string a;
    while (!inf.eof()) {
        inf >> I >> a >> ACT_id >> id;
        if (id == -1) {
            Goto[std::make_pair(I, a)] = ACT_id;
        } else {
            enum ACTION ACT;
            if (ACT_id == 1)
                ACT = Shift;
            else if (ACT_id == 2)
                ACT = Reduce;
            else if (ACT_id == 3)
                ACT = ACC;
            else
                assert(0);
            actionTable[make_pair(I, a)] = std::make_pair(ACT, id);
        }
    }
}
void generate_table() {
    if (table_exist) {
        LoadTable();
        return;
    }
    std::cerr << "generating table\n";
    get_items();
    for (auto I : Item_set) {
        for (auto it : I.first) {
            std::vector<std::string> next = it.next;
            if (!next.empty()) {
                AddAction(
                    std::make_pair(Shift, Item_set[GO(I.first, *next.begin())]),
                    std::make_pair(I.second, *next.begin()));
            } else if (it.LHS != fjcS) {
                AddAction(
                    std::make_pair(
                        Reduce, Production_ID[make_pair(it.LHS, it.previous)]),
                    std::make_pair(I.second, it.LookAhead));
            } else {
                AddAction(std::make_pair(ACC, 0),
                          std::make_pair(I.second, it.LookAhead));
            }
        }
        for (auto A : Nonterminal) {
            std::set<Item> GoIA = GO(I.first, A);
            if (GoIA.empty()) continue;
            Goto[make_pair(I.second, A)] = Item_set[GoIA];
        }
    }
    std::ofstream of("analyse_table.txt");
    if (!of) {
        std::cerr << "找不到文件analyse_table.txt" << std::endl;
        abort();
    }
    for (auto I : Item_set) {
        for (auto a : Terminal) {
            if (actionTable.count(std::make_pair(I.second, a))) {
                of << I.second << " " << a << " "
                   << actionTable[std::make_pair(I.second, a)].first << " "
                   << actionTable[std::make_pair(I.second, a)].second
                   << std::endl;
            }
        }
        if (actionTable.count(std::make_pair(I.second, "$"))) {
            of << I.second << " "
               << "$"
               << " " << actionTable[std::make_pair(I.second, "$")].first << " "
               << actionTable[std::make_pair(I.second, "$")].second
               << std::endl;
        }
        for (auto A : Nonterminal) {
            if (Goto.count(std::make_pair(I.second, A))) {
                of << I.second << " " << A << " "
                   << Goto[std::make_pair(I.second, A)] << " -1" << std::endl;
            }
        }
    }
    std::cout << "table ready" << std::endl;
}

struct TokenItem {
    std::string raw, type, parserSymbol;
    int row, column;
    TokenItem() {}
    TokenItem(std::string raw, std::string type, std::string parserSymbol,
              int row, int column)
        : raw(raw),
          type(type),
          parserSymbol(parserSymbol),
          row(row),
          column(column) {}
    void Load(std::tuple<std::string, int, int, std::string> i) {
        auto [_raw, _row, _column, _type] = i;
        if (_raw == "")
            *this = {"", "", "$", 0, 0};
        else {
            std::string _parserSymbol;
            if (_type == "keyword" || _type == "punc") {
                if (_raw == ":=")
                    _parserSymbol = "assignOP";
                else if (_raw == "<>" || _raw == ">" || _raw == "<" ||
                         _raw == ">=" || _raw == "<=")
                    _parserSymbol = "relOP";
                else if (_raw == "*" || _raw == "/" || _raw == "div" ||
                         _raw == "mod" || _raw == "and")
                    _parserSymbol = "mulOP";
                else
                    _parserSymbol = _raw;
            } else if (_type == "identify")
                _parserSymbol = "ID";
            else if (_type == "intVal" || _type == "realVal" ||
                     _type == "stringVal")
                _parserSymbol = _type;
            else
                _parserSymbol = _raw;
            *this = {_raw, _type, _parserSymbol, _row, _column};
        }
        std::cerr << raw << ' ' << type << ' ' << parserSymbol << ' ' << row
                  << ' ' << column << '\n';
    }
};

std::string rand_name() {
    static int index = 0;
    std::string ret = "anonymous_" + std::to_string(index);
    index += 1;
    return ret;
}
GrammarTreeNode* Analyse(TokenQueue& tq) {
    using namespace NodeProperties;

    //记录下来每个node->prop的类型
    std::vector<GrammarTreeNode*> unlinkedNodes;
    int curState;
    std::vector<int> states;
    std::vector<std::string> symbols;
    auto DoReduce = [&](Expr expr) {
        auto evalConst = [&](AST* node) {
            int flag = 1;
            NumberExprAST* number;
            if (typeid(node) == typeid(UnaryExprAST*)) {
                auto ptr = dynamic_cast<UnaryExprAST*>(node);
                if (ptr->op == "-") flag = -1;
                number = dynamic_cast<NumberExprAST*>(ptr->expr);
            } else
                number = dynamic_cast<NumberExprAST*>(node);
            assert(number);
            if (number->const_type == CONSTANT_INT)
                return new NumberExprAST(number->val_int * flag);
            else
                return new NumberExprAST(number->val_float * flag);
        };
        auto UpdateProperties = [&](GrammarTreeNode* node) {
            if (node->type == "S") {  // S -> ProgramStruct
                node->prop = cast<S>(node->son[0]->prop);
            } else if (node->type == "ProgramStruct") {
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
            } else if (node->type == "ProgramHead") {
                node->prop = nullptr;
            } else if (node->type ==
                       "ProgramBody") {  // ProgramBody -> Component
                                         // CompoundStatement @
                auto M = cast<ProgramBody>(node->son[0]->prop);
                M["body"].push_back(
                    cast<CompoundStatement>(node->son[1]->prop));
                node->prop = M;
            } else if (node->type == "Component") {
                Component prop;
                if (node->son.empty()) {
                    // Do nothing.
                } else if (node->son[0]->raw ==
                           "const") {  // Component -> const ConstDeclaration ;
                                       // Component
                    prop = cast<Component>(node->son[3]->prop);
                    for (auto i : cast<ConstDeclaration>(node->son[1]->prop))
                        prop["var"].push_back(i);
                } else if (node->son[0]->raw ==
                           "type") {  // Component -> type TypeDeclaration ;
                                      // Component
                    prop = cast<Component>(node->son[3]->prop);
                    auto list = cast<TypeDeclaration>(node->son[1]->prop);
                    for (auto iter = list.rbegin(); iter != list.rend(); ++iter)
                        prop["type"].insert(prop["type"].begin(), *iter);
                } else if (node->son[0]->raw ==
                           "var") {  // Component -> var VarDeclaration ;
                                     // Component
                    prop = cast<Component>(node->son[3]->prop);
                    for (auto i : cast<VarDeclaration>(node->son[1]->prop))
                        prop["var"].push_back(i);
                } else if (node->son[0]->type ==
                           "Subprogram") {  // Component -> Subprogram ;
                                            // Component
                    prop = cast<Component>(node->son[2]->prop);
                    prop["function"].insert(prop["function"].begin(),
                                            node->son[0]->prop);
                }
                node->prop = prop;
            } else if (node->type == "IDList") {
                IDList prop;
                if (node->son.size() == 1) {  // IDList -> ID
                    prop = IDList{node->son[0]->raw};
                } else {  // IDList -> IDList , ID
                    prop = cast<IDList>(node->son[0]->prop);
                    prop.push_back(node->son[2]->raw);
                }
                node->prop = prop;
            } else if (node->type == "ConstDeclaration") {
                ConstDeclaration prop;
                if (node->son.size() ==
                    3) {  // ConstDeclaration -> ID = ConstValue
                    prop.push_back(new VariableDeclAST(
                        new VariableExprAST(node->son[0]->raw), nullptr, false,
                        true, evalConst(cast<ConstValue>(node->son[2]->prop))));
                } else {  // ConstDeclaration -> ConstDeclaration ; ID =
                          // ConstValue
                    prop = cast<ConstDeclaration>(node->son[0]->prop);
                    prop.push_back(new VariableDeclAST(
                        new VariableExprAST(node->son[2]->raw), nullptr, false,
                        true, evalConst(cast<ConstValue>(node->son[4]->prop))));
                }
                node->prop = prop;
            } else if (node->type == "ConstValue") {
                ConstValue prop;
                if (node->son[0]->raw == "+") {  // ConstValue -> + Num
                    prop = new UnaryExprAST("+",
                                            cast<ExprAST*>(node->son[1]->prop));
                } else if (node->son[0]->raw == "-") {  // ConstValue -> - Num
                    prop = new UnaryExprAST("-",
                                            cast<ExprAST*>(node->son[1]->prop));
                } else if (node->son.size() == 1) {  // ConstValue -> Value
                    prop = cast<Value>(node->son[0]->prop);
                }
                node->prop = prop;
            } else if (node->type == "TypeDeclaration") {  // new
                TypeDeclaration prop;
                if (node->son.size() ==
                    3) {  // TypeDeclaration -> ID = ActualType
                    prop.push_back(
                        new TypeDefAST(new BasicTypeAST(node->son[0]->raw),
                                       cast<ActualType>(node->son[2]->prop)));
                } else {  // TypeDeclaration -> TypeDeclaration ; ID =
                          // ActualType
                    prop = cast<TypeDeclaration>(node->son[0]->prop);
                    prop.push_back(
                        new TypeDefAST(new BasicTypeAST(node->son[2]->raw),
                                       cast<ActualType>(node->son[4]->prop)));
                }
                node->prop = prop;
            } else if (node->type == "ActualType") {
                ActualType prop;
                if (node->son.size() == 1) {  // ActualType -> Type
                    prop = cast<Type>(node->son[0]->prop);
                } else {  // ActualType -> record VarDeclaration ; end
                    prop = new StructDeclAST(
                        rand_name(), cast<VarDeclaration>(node->son[1]->prop));
                }
                node->prop = prop;
            } else if (node->type == "VarDeclaration") {
                VarDeclaration prop;
                if (node->son.size() == 3) {  // VarDeclaration -> IDList : Type
                    for (std::string id : cast<IDList>(node->son[0]->prop)) {
                        prop.push_back(new VariableDeclAST(
                            new VariableExprAST(id),
                            cast<Type>(node->son[2]->prop), false));
                    }
                } else {  // VarDeclaration -> VarDeclaration ; IDList :
                          // ActualType
                    prop = cast<VarDeclaration>(node->son[0]->prop);
                    for (std::string id : cast<IDList>(node->son[2]->prop)) {
                        prop.push_back(new VariableDeclAST(
                            new VariableExprAST(id),
                            cast<ActualType>(node->son[4]->prop), false));
                    }
                }
                node->prop = prop;
            } else if (node->type == "Type") {
                Type prop;
                if (node->son[0]->raw ==
                    "array") {  // Type -> array [ Period ] of BasicType
                    prop = cast<BasicType>(node->son[5]->prop);
                    auto range = cast<Period>(node->son[2]->prop);
                    std::reverse(range.begin(), range.end());
                    for (auto [L, R] : range) {
                        prop = new ArrayTypeDeclAST(prop, L, R);
                    }
                } else if (node->son[0]->type ==
                           "BasicType") {  // Type -> BasicType
                    prop = cast<BasicType>(node->son[0]->prop);
                } else {  // Type -> ^ BasicType
                    prop = new PointerTypeDeclAST(
                        cast<BasicType>(node->son[1]->prop));
                }
                node->prop = prop;
            } else if (node->type == "BasicType") {
                BasicType prop = new BasicTypeAST(node->son[0]->raw);
                node->prop = prop;
            } else if (node->type == "Period") {
                Period prop;
                if (node->son.size() == 3) {  // Period -> Digits .. Digits
                    prop = Period{
                        std::make_pair(cast<Digits>(node->son[0]->prop),
                                       cast<Digits>(node->son[2]->prop))};
                } else {  // Period -> Period , Digits .. Digits
                    prop = cast<Period>(node->son[0]->prop);
                    prop.emplace_back(cast<Digits>(node->son[2]->prop),
                                      cast<Digits>(node->son[4]->prop));
                }
                node->prop = prop;
            } else if (node->type ==
                       "Subprogram") {  // Subprogram -> SubprogramHead ;
                                        // SubprogramBody
                auto [parameter, body] =
                    cast<SubprogramBody>(node->son[2]->prop);
                Subprogram prop = new FunctionAST(
                    cast<SubprogramHead>(node->son[0]->prop), parameter, body);
                node->prop = prop;
            } else if (node->type == "SubprogramHead") {
                SubprogramHead prop;
                if (node->son.size() ==
                    3) {  // SubprogramHead -> procedure ID FormalParameter
                    prop = new FunctionSignatureAST(
                        node->son[1]->raw,
                        cast<FormalParameter>(node->son[2]->prop),
                        new BasicTypeAST("void"));
                } else {  // SubprogramHead -> function ID FormalParameter :
                          // BasicType
                    prop = new FunctionSignatureAST(
                        node->son[1]->raw,
                        cast<FormalParameter>(node->son[2]->prop),
                        cast<BasicType>(node->son[4]->prop));
                }
                node->prop = prop;
            } else if (node->type == "FormalParameter") {
                FormalParameter prop;
                if (node->son.size() <= 2) {
                    prop = FormalParameter();
                } else {  // FormalParameter -> ( ParameterList )
                    prop = cast<ParameterList>(node->son[1]->prop);
                }
                node->prop = prop;
            } else if (node->type == "ParameterList") {
                ParameterList prop;
                if (node->son.size() == 1) {  // ParameterList -> Parameter
                    prop = cast<ParameterList>(node->son[0]->prop);
                } else {  // ParameterList -> ParameterList ; Parameter
                    prop = cast<ParameterList>(node->son[0]->prop);
                    for (auto i : cast<Parameter>(node->son[2]->prop))
                        prop.push_back(i);
                }
                node->prop = prop;
            } else if (node->type == "Parameter") {
                // Parameter -> VarParameter
                // Parameter -> ValueParameter
                node->prop = cast<Parameter>(node->son[0]->prop);
            } else if (node->type == "VarParameter") {
                // VarParameter -> var ValueParameter
                VarParameter prop = cast<ValueParameter>(node->son[1]->prop);
                for (auto& ptr : prop) ptr->isRef = true;
                node->prop = prop;
            } else if (node->type == "ValueParameter") {
                // ValueParameter -> IDList : BasicType
                ValueParameter prop;
                std::vector<VariableDeclAST*> list;
                for (std::string i : cast<IDList>(node->son[0]->prop)) {
                    prop.push_back(new VariableDeclAST(
                        new VariableExprAST(i),
                        cast<BasicType>(node->son[2]->prop), false));
                }
                node->prop = prop;
            } else if (node->type == "SubprogramBody") {
                // SubprogramBody -> SubComponent CompoundStatement
                node->prop =
                    SubprogramBody(cast<SubComponent>(node->son[0]->prop),
                                   cast<CompoundStatement>(node->son[1]->prop));
            } else if (node->type == "SubComponent") {
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
                    } else {
                        for (auto i : cast<VarDeclaration>(node->son[1]->prop))
                            prop.insert(prop.begin(), i);
                    }
                }
                node->prop = prop;
            } else if (node->type == "CompoundStatement") {
                // CompoundStatement -> begin StatementList end
                node->prop =
                    new BlockAST(cast<StatementList>(node->son[1]->prop));
            } else if (node->type == "StatementList") {
                StatementList prop;
                if (node->son.size() == 1) {  // StatementList -> Statement
                    auto statement = cast<Statement>(node->son[0]->prop);
                    if (!statement)
                        prop = StatementList{};
                    else
                        prop = StatementList{statement};
                } else {  // StatementList -> StatementList ; Statement
                    prop = cast<StatementList>(node->son[0]->prop);
                    auto statement = cast<Statement>(node->son[2]->prop);
                    if (statement) prop.push_back(statement);
                }
                node->prop = prop;
            } else if (node->type == "Statement") {
                Statement prop;
                if (node->son.empty()) {
                    prop = nullptr;
                } else if (node->son[0]->type == "ComposedVariable") {
                    // Statement -> ComposedVariable assignOP Expression
                    prop = new BinaryExprAST(
                        node->son[1]->raw, cast<ExprAST*>(node->son[0]->prop),
                        cast<ExprAST*>(node->son[2]->prop));
                } else if (node->son[0]->type == "ProcedureCall(ARG)") {
                    prop = cast<Statement>(node->son[0]->prop);
                } else if (node->son[0]->raw == "if") {
                    if (node->son[3]->type == "CompoundStatement") {
                        // Statement -> if Expression then CompoundStatement
                        // ElsePart
                        prop = new IfStatementAST(
                            cast<Expression>(node->son[1]->prop),
                            cast<CompoundStatement>(node->son[3]->prop),
                            cast<ElsePart>(node->son[4]->prop));
                    } else {
                        // Statement -> if Expression then Statement ElsePart
                        auto statement = cast<Statement>(node->son[3]->prop);
                        auto statementlist = StatementList{statement};
                        prop = new IfStatementAST(
                            cast<Expression>(node->son[1]->prop),
                            new std::remove_pointer<CompoundStatement>::type(
                                statementlist),
                            cast<ElsePart>(node->son[4]->prop));
                    }
                } else if (node->son[0]->raw == "for") {
                    // Statement -> for ID assignOP Expression to Expression do
                    // CompoundStatement
                    if (node->son[7]->type == "CompoundStatement") {
                        prop = new ForStatementAST(
                            new VariableExprAST(node->son[1]->raw),
                            cast<Expression>(node->son[3]->prop),
                            cast<Expression>(node->son[5]->prop),
                            cast<CompoundStatement>(node->son[7]->prop));
                    } else {
                        // Statement -> for ID assignOP Expression to Expression
                        // do Statement
                        auto statement = cast<Statement>(node->son[7]->prop);
                        auto statementlist = StatementList{statement};
                        prop = new ForStatementAST(
                            new VariableExprAST(node->son[1]->raw),
                            cast<Expression>(node->son[3]->prop),
                            cast<Expression>(node->son[5]->prop),
                            new std::remove_pointer<CompoundStatement>::type(
                                statementlist));
                    }
                } else if (node->son[0]->raw == "while") {
                    if (node->son[3]->type == "CompoundStatement") {
                        // Statement -> while Expression do CompoundStatement
                        prop = new WhileStatementAST(
                            cast<ExprAST*>(node->son[1]->prop),
                            cast<CompoundStatement>(node->son[3]->prop));
                    } else {
                        // Statement -> while Expression do Statement
                        auto statement = cast<Statement>(node->son[3]->prop);
                        auto statementlist = StatementList{statement};
                        prop = new WhileStatementAST(
                            cast<ExprAST*>(node->son[1]->prop),
                            new std::remove_pointer<CompoundStatement>::type(
                                statementlist));
                    }
                }
                node->prop = prop;
            } else if (node->type == "Variable") {  // Variable -> ID IDVarpart
                Variable prop = new VariableExprAST(node->son[0]->raw);
                auto part = cast<IDVarpart>(node->son[1]->prop);
                for (auto expr : part) {
                    prop = new BinaryExprAST("[]", prop, expr);
                }
                node->prop = prop;
            } else if (node->type == "IDVarpart") {
                IDVarpart prop;
                if (node->son.size() == 0)
                    prop = IDVarpart();
                else  // IDVarpart -> [ ExpressionList ]
                    prop = cast<IDVarpart>(node->son[1]->prop);
                node->prop = prop;
            } else if (node->type == "ProcedureCall(ARG)") {
                ProcedureCall prop;
                if (node->son.size() ==
                    4) {  // ProcedureCall(ARG) -> ID ( ExpressionList )
                    prop = new CallExprAST(
                        node->son[0]->raw,
                        cast<ExpressionList>(node->son[2]->prop));
                } else {  // ProcedureCall(ARG) -> ID ( )
                    prop = new CallExprAST(node->son[0]->raw, {});
                }
                node->prop = prop;
            } else if (node->type == "ElsePart") {
                ElsePart prop;
                if (node->son.size() >
                    0) {  // ElsePart -> else CompoundStatement
                    if (node->son[1]->type == "CompoundStatement")
                        prop = cast<ElsePart>(node->son[1]->prop);
                    else
                        prop =
                            new BlockAST({cast<Statement>(node->son[1]->prop)});
                } else {
                    prop = new BlockAST({});
                }
                node->prop = prop;
            } else if (node->type == "ExpressionList") {
                ExpressionList prop;
                if (node->son.size() == 1) {  // ExpressionList -> Expression
                    prop = ExpressionList{cast<Expression>(node->son[0]->prop)};
                } else {  // ExpressionList -> ExpressionList , Expression
                    prop = cast<ExpressionList>(node->son[0]->prop);
                    prop.push_back(cast<Expression>(node->son[2]->prop));
                }
                node->prop = prop;
            } else if (node->type == "Expression") {
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
                } else if (node->son.size() == 2) {
                    prop = new UnaryExprAST("^",
                                            cast<ExprAST*>(node->son[0]->prop));
                } else {
                    if (node->son[0]->parserSymbol == "stringVal") {
                        prop = new StringExprAST(node->son[0]->raw);
                    } else {  // Expression -> SimpleExpression
                        prop = cast<Expression>(node->son[0]->prop);
                    }
                }
                node->prop = prop;
            } else if (node->type == "SimpleExpression") {
                SimpleExpression prop;
                if (node->son.size() == 1)  // SimpleExpression -> Term
                    prop = cast<SimpleExpression>(node->son[0]->prop);
                else {  // SimpleExpression -> SimpleExpression addOP Term
                    prop =
                        new BinaryExprAST(cast<std::string>(node->son[1]->prop),
                                          cast<ExprAST*>(node->son[0]->prop),
                                          cast<ExprAST*>(node->son[2]->prop));
                }
                node->prop = prop;
            } else if (node->type == "Term") {
                Term prop = nullptr;
                if (node->son.size() == 1)  // Term -> Factor
                    prop = cast<Term>(node->son[0]->prop);
                else {  // Term -> Term mulOP Factor
                    prop = new BinaryExprAST(
                        node->son[1]->raw, cast<ExprAST*>(node->son[0]->prop),
                        cast<ExprAST*>(node->son[2]->prop));
                }
                node->prop = prop;
            } else if (node->type == "Factor") {
                Factor prop = nullptr;
                if (node->son[0]->type == "Value") {  // Factor -> Value
                    prop = cast<Value>(node->son[0]->prop);
                } else if (node->son[0]->type == "Num") {  // Factor -> Num
                    prop = cast<Factor>(node->son[0]->prop);
                } else if (node->son[0]->type ==
                           "ComposedVariable") {  // Factor -> ComposedVariable
                    prop = cast<Factor>(node->son[0]->prop);
                } else if (node->son[1]->type == "Num") {
                    // Factor -> + Num
                    // Factor -> - Num
                    prop = new UnaryExprAST(node->son[0]->raw,
                                            cast<ExprAST*>(node->son[1]->prop));
                } else if (node->son[1]->type == "Factor") {
                    // Factor -> not Factor
                    // Factor -> uminus Factor
                    prop = new UnaryExprAST(node->son[0]->raw,
                                            cast<ExprAST*>(node->son[1]->prop));
                } else
                    abort();
                node->prop = prop;
            } else if (node->type == "ComposedVariable") {
                ComposedVariable prop = nullptr;
                if (node->son[0]->type == "Variable") {
                    prop = cast<ComposedVariable>(node->son[0]->prop);
                } else if (node->son[0]->type == "ProcedureCall(ARG)") {
                    prop = cast<ComposedVariable>(node->son[0]->prop);
                } else if (node->son[1]->type ==
                           "Expression") {  // ( Expression )
                    prop = cast<ComposedVariable>(node->son[1]->prop);
                } else {
                    prop = new BinaryExprAST(
                        ".", cast<ExprAST*>(node->son[0]->prop),
                        new StringExprAST(node->son[2]->raw));
                }
                node->prop = prop;
            } else if (node->type == "Num") {
                Num prop;
                if (node->son[0]->parserSymbol == "intVal") {  // Num -> intVal
                    prop = new NumberExprAST(std::stoi(node->son[0]->raw));
                } else if (node->son[0]->parserSymbol == "realVal") {
                    prop = new NumberExprAST(std::stod(node->son[0]->raw));
                } else
                    abort();
                node->prop = prop;
            } else if (node->type == "Digits") {
                node->prop = new NumberExprAST(std::stoi(node->son[0]->raw));
                assert(node->prop.type() == typeid(Digits));
            } else if (node->type == "addOP") {
                node->prop = node->son[0]->raw;
                assert(node->son[0]->raw == "+" || node->son[0]->raw == "-" ||
                       node->son[0]->raw == "or");
                assert(node->prop.type() == typeid(addOP));
            } else if (node->type == "Value") {
                Value prop;
                if (node->son[0]->type == "Num") {  // Value -> Num
                    prop = cast<Value>(node->son[0]->prop);
                } else {  // Value -> stringVal
                    prop = new StringExprAST(node->son[0]->raw);
                }
                node->prop = prop;
            } else
                abort();
            // cerr << node->type << endl;
            assert(node->prop.has_value());  //每个结点都必有一个属性
        };
        int popNum = expr.second.size();
        std::vector<GrammarTreeNode*> reducedNode;
        for (int i = 0; i < popNum; i++) {
            states.pop_back();
            symbols.pop_back();
            reducedNode.push_back(*unlinkedNodes.rbegin());
            unlinkedNodes.pop_back();
        }
        curState = *states.rbegin();
        states.push_back(Goto[make_pair(curState, expr.first)]);
        symbols.push_back(expr.first);
        GrammarTreeNode* newNode =
            new GrammarTreeNode("", expr.first, expr.first, 0, 0);
        reverse(reducedNode.begin(), reducedNode.end());
        for (auto node : reducedNode) newNode->son.push_back(node);
        UpdateProperties(newNode);
        unlinkedNodes.push_back(newNode);
        newNode->Report();
        if (newNode->type == "Statement")
            std::cerr << "report = " << newNode->type << '\n';
    };
    std::cerr << "start analyse" << std::endl;
    states.push_back(0);
    symbols.push_back("");
    TokenItem N;
    N.Load(tq.front());
    tq.pop();
    for (;;) {
        curState = *states.rbegin();
        // std::cerr << curState << '\n';
        // for (auto x : symbols) std::cerr << x << " ";
        // std::cerr << endl;
        // std::cerr << " str: " << N.raw << " type: " << N.type << endl;
        // std::cerr << "row:" << N.row << " column: " << N.column << " type: "
        // << N.parserSymbol << endl;
        if (actionTable.count(make_pair(curState, N.parserSymbol)) == 0) throw;
        std::pair<ACTION, int> act =
            actionTable[make_pair(curState, N.parserSymbol)];
        if (act.first == Shift) {
            states.push_back(act.second);
            symbols.push_back(N.parserSymbol);
            unlinkedNodes.push_back(new GrammarTreeNode(
                N.raw, N.type, N.parserSymbol, N.row, N.column));
            (*unlinkedNodes.rbegin())->Report();
            assert(!tq.empty());
            N.Load(tq.front());
            tq.pop();

        } else if (act.first == Reduce) {
            Expr prod = Production_content[act.second];
            DoReduce(prod);
        } else if (act.first == ACC) {
            DoReduce({"S", {"ProgramStruct"}});
            std::cout << "ACCEPT!" << std::endl;
            break;
        }
    }
    assert(unlinkedNodes.size() == 1);
    return *unlinkedNodes.begin();
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
    tq.push({"", 0, 0, ""});
    auto root = Analyse(tq);
    check_grammar_tree(0, 0);
    return NodeProperties::cast<GlobalAST*>(root->prop);
}
