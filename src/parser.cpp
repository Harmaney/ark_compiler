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
using Json = nlohmann::json;
#define inset(y, x) x.find(y) != x.end()
#define table_exist 1
using namespace std;
string S;
set<string> Terminal, Nonterminal;
typedef pair<string, vector<string>> Expr;
vector<Expr> Productions;
map<int, Expr> Production_content;
map<Expr, int> Production_ID;
int Production_num = 0;
vector<string> empty_vec;
map<string, set<string>> First;
map<string, set<vector<string>>> RHS_set;
set<string> Get_epsilon;
std::default_random_engine rng(2018210789);
void init() {
    ifstream input("../files/grammar.txt");
    if (!input) {
        cerr << "找不到grammar.txt" << endl;
        abort();
    }
    int mode = 0;
    string LHS;
    vector<string> RHS;
    while (!input.eof()) {
        string str;
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
            if (S.empty()) S = LHS;
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
bool merge(const set<T>& A, set<T>& B) {
    bool flg = false;
    for (auto x : A) {
        if (B.find(x) == B.end()) {
            flg = true;
            B.insert(x);
        }
    }
    return flg;
}
set<string> get_first(vector<string> strlist) {
    set<string> res;
    vector<string>::iterator it = strlist.begin();
    string Head = *it;
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
bool operator<(vector<string> A, vector<string> B) {
    for (int i = 0; i < A.size() && i < B.size(); i++) {
        if (A[i] != B[i]) return A[i] < B[i];
    }
    if (A.size() != B.size()) return A.size() < B.size();
}
bool operator==(vector<string> A, vector<string> B) {
    if (A.size() != B.size()) return false;
    for (int i = 0; i < A.size() && i < B.size(); i++) {
        if (A[i] != B[i]) return false;
    }
    return true;
}
struct Item {
    string LHS;
    string LookAhead;
    vector<string> previous, next;
    Item() {}
    Item(string LHS, vector<string> previous, vector<string> next, string LookAhead) : LHS(LHS), previous(previous), next(next), LookAhead(LookAhead) {}
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
map<set<Item>, int> Item_set;
map<int, set<Item>> Item_content;
int item_num = 0;
set<Item> get_closure(Item I) {
    set<Item> J;
    J.insert(I);
    bool changed = true;
    do {
        changed = false;
        set<Item> J_new;
        set<Item> J_add;
        merge(J, J_new);
        for (auto item : J_new) {
            vector<string> next = item.next;
            if (next.empty()) continue;
            string B = *next.begin();  // B
            vector<string> temp = next;
            temp.erase(temp.begin());
            temp.push_back(item.LookAhead);  // temp = bx

            set<string> bx_first = get_first(temp);

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
set<Item> GO(set<Item> I, string X) {
    set<Item> J;
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
    Item_content[item_num] = get_closure(Item(S, empty_vec, *RHS_set[S].begin(), "$"));
    Item_set[get_closure(Item(S, empty_vec, *RHS_set[S].begin(), "$"))] = item_num++;
    set<string> Sign;
    merge(Terminal, Sign);
    merge(Nonterminal, Sign);
    // ofstream itemout("item.txt");
    bool changed = true;
    do {
        changed = false;
        for (auto I : Item_set) {
            for (auto X : Sign) {
                if (X.empty()) continue;
                set<Item> Goix = GO(I.first, X);
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
map<pair<int, string>, pair<ACTION, int>> actionTable;
map<pair<int, string>, int> Goto;
void AddAction(pair<ACTION, int> act, pair<int, string> pos) {
    if (actionTable.count(pos)) {
        if (actionTable[pos] != act) {
            // if (actionTable[pos].first == Shift) return;
            std::cerr << "???" << endl;
            cerr << actionTable[pos].first << " " << actionTable[pos].second << endl;
            cerr << act.first << " " << act.second << endl;
            cerr << pos.first << " " << pos.second << endl;
        }
    }
    actionTable[pos] = act;
}
void LoadTable() {
    ifstream inf("../files/analyse_table.txt");
    if (!inf) {
        cerr << "找不到文件analyse_table.txt" << endl;
        abort();
    }
    int I, id, ACT_id;
    string a;
    while (!inf.eof()) {
        inf >> I >> a >> ACT_id >> id;
        if (id == -1) {
            Goto[make_pair(I, a)] = ACT_id;
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
            actionTable[make_pair(I, a)] = make_pair(ACT, id);
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
            vector<string> next = it.next;
            if (!next.empty()) {
                AddAction(make_pair(Shift, Item_set[GO(I.first, *next.begin())]), make_pair(I.second, *next.begin()));
            } else if (it.LHS != S) {
                AddAction(make_pair(Reduce, Production_ID[make_pair(it.LHS, it.previous)]), make_pair(I.second, it.LookAhead));
            } else {
                AddAction(make_pair(ACC, 0), make_pair(I.second, it.LookAhead));
            }
        }
        for (auto A : Nonterminal) {
            set<Item> GoIA = GO(I.first, A);
            if (GoIA.empty()) continue;
            Goto[make_pair(I.second, A)] = Item_set[GoIA];
        }
    }
    ofstream of("analyse_table.txt");
    if (!of) {
        cerr << "找不到文件analyse_table.txt" << endl;
        abort();
    }
    for (auto I : Item_set) {
        for (auto a : Terminal) {
            if (actionTable.count(make_pair(I.second, a))) {
                of << I.second << " " << a << " " << actionTable[make_pair(I.second, a)].first << " " << actionTable[make_pair(I.second, a)].second << endl;
            }
        }
        if (actionTable.count(make_pair(I.second, "$"))) {
            of << I.second << " "
               << "$"
               << " " << actionTable[make_pair(I.second, "$")].first << " " << actionTable[make_pair(I.second, "$")].second << endl;
        }
        for (auto A : Nonterminal) {
            if (Goto.count(make_pair(I.second, A))) {
                of << I.second << " " << A << " " << Goto[make_pair(I.second, A)] << " -1" << endl;
            }
        }
    }
    cout << "table ready" << endl;
}
struct TokenItem {
    string raw, type, parserSymbol;
    int row, column;
    TokenItem() {}
    TokenItem(string raw, string type, string parserSymbol, int row, int column) : raw(raw), type(type), parserSymbol(parserSymbol), row(row), column(column) {}
};
std::ifstream& operator>>(std::ifstream& ifs, TokenItem& item) {
    std::string raw, type;
    int row, column;
    ifs >> raw >> row >> column >> type;
    if (raw == "")
        item = {"", "", "$", 0, 0};
    else {
        string parserSymbol;
        if (type == "keyword" || type == "punc") {
            if (raw == ":=")
                parserSymbol = "assignOP";
            else if (raw == "=" || raw == "<>" || raw == ">" || raw == "<" || raw == ">=" || raw == "<=")
                parserSymbol = "relOP";
            else if (raw == "*" || raw == "/" || raw == "div" || raw == "mod" || raw == "and")
                parserSymbol = "mulOP";
            else
                parserSymbol = raw;
        } else if (type == "identify")
            parserSymbol = "ID";
        else if (type == "intVal" || type == "realVal" || type == "stringVal")
            parserSymbol = type;
        else
            parserSymbol = raw;
        item = {raw, type, parserSymbol, row, column};
    }
    return ifs;
}
struct GrammarTreeNode {
    string raw, type, parserSymbol;
    int row, column;
    uint64_t ID;
    std::any prop;

    vector<GrammarTreeNode*> son;
    GrammarTreeNode(string raw, string type, string parserSymbol, int row, int column, uint64_t ID) : raw(raw), type(type), parserSymbol(parserSymbol), row(row), column(column), ID(ID), son{} {}
};
template <typename A>
inline A cast(std::any arg) {
    if (typeid(A) == typeid(AST*)) {
        if (arg.type() == typeid(BlockAST*))
            return A(std::any_cast<BlockAST*>(arg));
        else if (arg.type() == typeid(ExprAST*))
            return A(std::any_cast<ExprAST*>(arg));
        else if (arg.type() == typeid(TypeDeclAST*))
            return A(std::any_cast<TypeDeclAST*>(arg));
        else if (arg.type() == typeid(BasicTypeAST*))
            return A(std::any_cast<BasicTypeAST*>(arg));
        else if (arg.type() == typeid(PointerTypeDeclAST*))
            return A(std::any_cast<PointerTypeDeclAST*>(arg));
        else if (arg.type() == typeid(NumberExprAST*))
            return A(std::any_cast<NumberExprAST*>(arg));
        else if (arg.type() == typeid(ArrayTypeDeclAST*))
            return A(std::any_cast<ArrayTypeDeclAST*>(arg));
        else if (arg.type() == typeid(StringExprAST*))
            return A(std::any_cast<StringExprAST*>(arg));
        else if (arg.type() == typeid(VariableExprAST*))
            return A(std::any_cast<VariableExprAST*>(arg));
        else if (arg.type() == typeid(UnaryExprAST*))
            return A(std::any_cast<UnaryExprAST*>(arg));
        else if (arg.type() == typeid(BinaryExprAST*))
            return A(std::any_cast<BinaryExprAST*>(arg));
        else if (arg.type() == typeid(CallExprAST*))
            return A(std::any_cast<CallExprAST*>(arg));
        else if (arg.type() == typeid(VariableDeclAST*))
            return A(std::any_cast<VariableDeclAST*>(arg));
        else if (arg.type() == typeid(ForStatementAST*))
            return A(std::any_cast<ForStatementAST*>(arg));
        else if (arg.type() == typeid(WhileStatementAST*))
            return A(std::any_cast<WhileStatementAST*>(arg));
        else if (arg.type() == typeid(IfStatementAST*))
            return A(std::any_cast<IfStatementAST*>(arg));
        else if (arg.type() == typeid(FunctionSignatureAST*))
            return A(std::any_cast<FunctionSignatureAST*>(arg));
        else if (arg.type() == typeid(FunctionAST*))
            return A(std::any_cast<FunctionAST*>(arg));
        else if (arg.type() == typeid(StructDeclAST*))
            return A(std::any_cast<StructDeclAST*>(arg));
        else if (arg.type() == typeid(GlobalAST*))
            return A(std::any_cast<GlobalAST*>(arg));
    } else if (typeid(A) == typeid(TypeDeclAST*)) {
        if (arg.type() == typeid(BasicTypeAST*))
            return A(std::any_cast<BasicTypeAST*>(arg));
        else if (arg.type() == typeid(PointerTypeDeclAST*))
            return A(std::any_cast<PointerTypeDeclAST*>(arg));
        else if (arg.type() == typeid(ArrayTypeDeclAST*))
            return A(std::any_cast<ArrayTypeDeclAST*>(arg));
    } else if (typeid(A) == typeid(ExprAST*)) {
        if (arg.type() == typeid(NumberExprAST*))
            return A(std::any_cast<NumberExprAST*>(arg));
        else if (arg.type() == typeid(StringExprAST*))
            return A(std::any_cast<StringExprAST*>(arg));
        else if (arg.type() == typeid(VariableExprAST*))
            return A(std::any_cast<VariableExprAST*>(arg));
        else if (arg.type() == typeid(UnaryExprAST*))
            return A(std::any_cast<UnaryExprAST*>(arg));
        else if (arg.type() == typeid(BinaryExprAST*))
            return A(std::any_cast<BinaryExprAST*>(arg));
        else if (arg.type() == typeid(CallExprAST*))
            return A(std::any_cast<CallExprAST*>(arg));
    }
    // std::cerr << "TYPE: " << typeid(A).name() << std::endl;
    // std::cerr << "TYPE: " << arg.type().name() << std::endl;
    return std::any_cast<A>(arg);
}
template <>
inline map<std::string, std::vector<std::any>> cast(std::any arg) {
    return std::any_cast<map<std::string, std::vector<std::any>>>(arg);
}
template <>
inline std::vector<ExprAST*> cast(std::any arg) {
    return std::any_cast<std::vector<ExprAST*>>(arg);
}
template <>
inline std::vector<AST*> cast(std::any arg) {
    return std::any_cast<std::vector<AST*>>(arg);
}
template <>
inline std::vector<std::string> cast(std::any arg) {
    return std::any_cast<std::vector<std::string>>(arg);
}
template <>
inline std::string cast(std::any arg) {
    return std::any_cast<std::string>(arg);
}
template <>
inline std::vector<VariableDeclAST*> cast(std::any arg) {
    return std::any_cast<std::vector<VariableDeclAST*>>(arg);
}
template <>
inline std::pair<std::vector<VariableDeclAST*>, BlockAST*> cast(std::any arg) {
    return std::any_cast<std::pair<std::vector<VariableDeclAST*>, BlockAST*>>(arg);
}
template <>
inline std::vector<std::pair<NumberExprAST*, NumberExprAST*>> cast(std::any arg) {
    return std::any_cast<std::vector<std::pair<NumberExprAST*, NumberExprAST*>>>(arg);
}
GrammarTreeNode* Analyse(string file_name) {
    //记录下来每个node->prop的类型
    vector<GrammarTreeNode*> unlinkedNodes;
    int curState;
    vector<int> states;
    vector<string> symbols;
    auto DoReduce = [&](Expr expr) {
        auto UpdateProperties = [&](GrammarTreeNode* node) {
            auto CreateBinaryAST = [&]() -> ExprAST* { return new BinaryExprAST(node->son[1]->raw, cast<ExprAST*>(node->son[0]->prop), cast<ExprAST*>(node->son[2]->prop)); };
            using S = GlobalAST*;
            using ProgramStruct = GlobalAST*;
            using ProgramBody = map<std::string, std::vector<std::any>>;
            using Component = map<std::string, std::vector<std::any>>;
            using IDList = std::vector<std::string>;
            using ConstDeclaration = VariableDeclAST*;
            using ConstValue = ExprAST*;
            using ActualType = TypeDeclAST*;
            using VarDeclaration = std::vector<VariableDeclAST*>;
            using Type = TypeDeclAST*;
            using BasicType = BasicTypeAST*;
            using Period = std::vector<std::pair<NumberExprAST*, NumberExprAST*>>;
            using Subprogram = FunctionAST*;
            using SubprogramHead = FunctionSignatureAST*;
            using FormalParameter = std::vector<VariableDeclAST*>;
            using ParameterList = std::vector<VariableDeclAST*>;
            using Parameter = std::vector<VariableDeclAST*>;
            using VarParameter = std::vector<VariableDeclAST*>;
            using ValueParameter = std::vector<VariableDeclAST*>;
            using SubprogramBody = std::pair<std::vector<VariableDeclAST*>, BlockAST*>;
            using SubComponent = std::vector<VariableDeclAST*>;
            using CompoundStatement = BlockAST*;
            using StatementList = std::vector<AST*>;
            using Statement = AST*;
            using Variable = ExprAST*;
            using IDVarpart = std::vector<ExprAST*>;
            using ProcedureCall = CallExprAST*;
            using ElsePart = BlockAST*;
            using ExpressionList = std::vector<ExprAST*>;
            using Expression = ExprAST*;
            using SimpleExpression = ExprAST*;
            using Term = ExprAST*;
            using Factor = ExprAST*;
            using ComposedVariable = ExprAST*;
            using Num = NumberExprAST*;
            using Digits = NumberExprAST*;
            using addOP = std::string;
            if (node->type == "S") {  // S -> ProgramStruct
                node->prop = cast<S>(node->son[0]->prop);
            } else if (node->type == "ProgramStruct") {
                auto M = cast<ProgramBody>(node->son[2]->prop);
                std::vector<VariableDeclAST*> vars;
                for (auto i : M["var"]) vars.push_back(cast<VariableDeclAST*>(i));
                std::vector<FunctionAST*> functions;
                for (auto i : M["function"]) functions.push_back(cast<FunctionAST*>(i));
                assert(M["body"].size() == 1);
                node->prop = new std::remove_pointer<ProgramStruct>::type(vars, functions, cast<BlockAST*>((M["body"]).front()));
            } else if (node->type == "ProgramHead") {
                node->prop = nullptr;
            } else if (node->type == "ProgramBody") {  // ProgramBody -> Component CompoundStatement @
                auto M = cast<ProgramBody>(node->son[0]->prop);
                M["body"].push_back(cast<CompoundStatement>(node->son[1]->prop));
                node->prop = M;
            } else if (node->type == "Component") {
                Component prop;
                if (node->son.empty()) {
                    // Do nothing.
                } else if (node->son[0]->raw == "const") {  // Component -> const ConstDeclaration ; Component
                    abort();
                } else if (node->son[0]->raw == "type") {  // Component -> type TypeDeclaration ; Component
                    prop = cast<Component>(node->son[3]->prop);
                    prop["type"].push_back(node->son[1]->prop);  // fixme
                } else if (node->son[0]->raw == "var") {         // Component -> var VarDeclaration ; Component
                    prop = cast<Component>(node->son[3]->prop);
                    for (auto i : cast<VarDeclaration>(node->son[1]->prop)) prop["var"].push_back(i);
                } else if (node->son[0]->type == "Subprogram") {  // Component -> Subprogram ; Component
                    prop = cast<Component>(node->son[2]->prop);
                    prop["function"].push_back(node->son[0]->prop);
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
            } else if (node->type == "ConstDeclaration") {  // fixme

            } else if (node->type == "ConstValue") {
                ConstValue prop;
                if (node->son[0]->raw == "+") {  // ConstValue -> + Num
                    prop = new UnaryExprAST("+", cast<ExprAST*>(node->son[1]->prop));
                } else if (node->son[0]->raw == "-") {  // ConstValue -> - Num
                    prop = new UnaryExprAST("-", cast<ExprAST*>(node->son[1]->prop));
                } else if (node->son.size() == 1) {  // ConstValue -> Num
                    prop = cast<ConstValue>(node->son[0]->prop);
                }
                node->prop = prop;
            } else if (node->type == "TypeDeclaration") {  // fixme

            } else if (node->type == "ActualType") {  // fixme
                ActualType prop;
                if (node->son.size() == 1) {  // ActualType -> Type
                    prop = cast<Type>(node->son[0]->prop);
                } else
                    abort();
                node->prop = prop;
            } else if (node->type == "VarDeclaration") {
                VarDeclaration prop;
                if (node->son.size() == 3) {  // VarDeclaration -> IDList : Type
                    for (std::string id : cast<IDList>(node->son[0]->prop)) {
                        prop.push_back(new VariableDeclAST(new VariableExprAST(id), cast<Type>(node->son[2]->prop), false));
                    }
                } else {  // VarDeclaration -> VarDeclaration ; IDList : ActualType
                    prop = cast<VarDeclaration>(node->son[0]->prop);
                    for (std::string id : cast<IDList>(node->son[2]->prop)) {
                        prop.push_back(new VariableDeclAST(new VariableExprAST(id), cast<ActualType>(node->son[4]->prop), false));
                    }
                }
                node->prop = prop;
            } else if (node->type == "Type") {
                Type prop;
                if (node->son[0]->raw == "array") {  // Type -> array [ Period ] of BasicType
                    prop = cast<BasicType>(node->son[5]->prop);
                    auto range = cast<Period>(node->son[2]->prop);
                    reverse(range.begin(), range.end());
                    for (auto [L, R] : range) {
                        prop = new ArrayTypeDeclAST(prop, L, R);
                    }
                } else if (node->son[0]->type == "BasicType") {  // Type -> BasicType
                    prop = cast<BasicType>(node->son[0]->prop);
                } else {  // Type -> ^ BasicType
                    prop = new PointerTypeDeclAST(cast<BasicType>(node->son[1]->prop));
                }
                node->prop = prop;
            } else if (node->type == "BasicType") {
                BasicType prop = new BasicTypeAST(node->son[0]->raw);
                node->prop = prop;
            } else if (node->type == "Period") {
                Period prop;
                if (node->son.size() == 3) {  // Period -> Digits .. Digits
                    prop = Period{make_pair(cast<Digits>(node->son[0]->prop), cast<Digits>(node->son[2]->prop))};
                } else {  // Period -> Period , Digits .. Digits
                    prop = cast<Period>(node->son[0]->prop);
                    prop.emplace_back(cast<Digits>(node->son[2]->prop), cast<Digits>(node->son[4]->prop));
                }
                node->prop = prop;
            } else if (node->type == "Subprogram") {  // Subprogram -> SubprogramHead ; SubprogramBody
                auto [parameter, body] = cast<SubprogramBody>(node->son[2]->prop);
                Subprogram prop = new FunctionAST(cast<SubprogramHead>(node->son[0]->prop), parameter, body);
                node->prop = prop;
            } else if (node->type == "SubprogramHead") {
                SubprogramHead prop;
                if (node->son.size() == 3) {  // SubprogramHead -> procedure ID FormalParameter
                    prop = new FunctionSignatureAST(node->son[1]->raw, cast<FormalParameter>(node->son[2]->prop), new BasicTypeAST("void"));
                } else {  // SubprogramHead -> function ID FormalParameter : BasicType
                    prop = new FunctionSignatureAST(node->son[1]->raw, cast<FormalParameter>(node->son[2]->prop), new BasicTypeAST(node->son[4]->raw));
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
                    for (auto i : cast<Parameter>(node->son[2]->prop)) prop.push_back(i);
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
                    prop.push_back(new VariableDeclAST(new VariableExprAST(i), cast<BasicType>(node->son[2]->prop), false));
                }
                node->prop = prop;
            } else if (node->type == "SubprogramBody") {
                // SubprogramBody -> SubComponent CompoundStatement
                node->prop = SubprogramBody(cast<SubComponent>(node->son[0]->prop), cast<CompoundStatement>(node->son[1]->prop));
            } else if (node->type == "SubComponent") {
                SubComponent prop;
                if (node->son.size() == 0)
                    node->prop = {};
                else {
                    // SubComponent -> const ConstDeclaration ; SubComponent
                    // SubComponent -> var VarDeclaration ; SubComponent
                    prop = cast<SubComponent>(node->son[3]->prop);
                    if (node->son[0]->raw == "const") {
                        abort();
                    } else {  // fixme
                        for (auto i : cast<VarDeclaration>(node->son[1]->prop)) prop.insert(prop.begin(), i);
                    }
                }
                node->prop = prop;
            } else if (node->type == "CompoundStatement") {
                // CompoundStatement -> begin StatementList end
                node->prop = new BlockAST(cast<StatementList>(node->son[1]->prop));
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
                    prop = new BinaryExprAST(node->son[1]->raw, cast<ExprAST*>(node->son[0]->prop), cast<ExprAST*>(node->son[2]->prop));
                } else if (node->son[0]->type == "ProcedureCall(ARG)") {
                    prop = cast<Statement>(node->son[0]->prop);
                } else if (node->son[0]->raw == "if") {
                    // Statement -> if Expression then CompoundStatement ElsePart @
                    prop = new IfStatementAST(cast<Expression>(node->son[1]->prop), cast<CompoundStatement>(node->son[3]->prop), cast<ElsePart>(node->son[4]->prop));
                } else if (node->son[0]->raw == "for") {
                    // Statement -> for ID assignOP Expression to Expression do CompoundStatement
                    prop = new ForStatementAST(new VariableExprAST(node->son[1]->raw), cast<Expression>(node->son[3]->prop), cast<Expression>(node->son[5]->prop), cast<CompoundStatement>(node->son[7]->prop));
                } else if (node->son[0]->raw == "while") {
                    // Statement -> while Expression do CompoundStatement
                    prop = new WhileStatementAST(cast<ExprAST*>(node->son[1]->prop), cast<CompoundStatement>(node->son[3]->prop));
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
                if (node->son.size() == 4) {  // ProcedureCall(ARG) -> ID ( ExpressionList )
                    prop = new CallExprAST(node->son[0]->raw, cast<ExpressionList>(node->son[2]->prop));
                } else {  // ProcedureCall(ARG) -> ID ( )
                    prop = new CallExprAST(node->son[0]->raw, {});
                }
                node->prop = prop;
            } else if (node->type == "ElsePart") {
                ElsePart prop;
                if (node->son.size() > 0) {  // ElsePart -> else CompoundStatement
                    prop = cast<ElsePart>(node->son[1]->prop);
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
                    prop = CreateBinaryAST();
                } else if (node->son.size() == 2) {
                    prop = new UnaryExprAST("^", cast<ExprAST*>(node->son[0]->prop));
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
                else  // SimpleExpression -> SimpleExpression addOP Term
                    prop = CreateBinaryAST();
                node->prop = prop;
            } else if (node->type == "Term") {
                Term prop = nullptr;
                if (node->son.size() == 1)  // Term -> Factor
                    prop = cast<Term>(node->son[0]->prop);
                else  // Term -> Term mulOP Factor
                    prop = CreateBinaryAST();
                node->prop = prop;
            } else if (node->type == "Factor") {
                Factor prop = nullptr;
                if (node->son[0]->type == "Num") {  // Factor -> Num
                    prop = cast<Factor>(node->son[0]->prop);
                } else if (node->son[0]->type == "ComposedVariable") {  // Factor -> ComposedVariable
                    prop = cast<Factor>(node->son[0]->prop);
                } else if (node->son[1]->type == "Num") {
                    // Factor -> + Num
                    // Factor -> - Num
                    prop = new UnaryExprAST(node->son[0]->raw, cast<ExprAST*>(node->son[1]->prop));
                } else if (node->son[1]->type == "Factor") {
                    // Factor -> not Factor
                    // Factor -> uminus Factor
                    prop = new UnaryExprAST(node->son[0]->raw, cast<ExprAST*>(node->son[1]->prop));
                } else
                    abort();
                node->prop = prop;
            } else if (node->type == "ComposedVariable") {
                ComposedVariable prop = nullptr;
                if (node->son[0]->type == "Variable") {
                    prop = cast<ComposedVariable>(node->son[0]->prop);
                } else if (node->son[0]->type == "ProcedureCall(ARG)") {
                    prop = cast<ComposedVariable>(node->son[0]->prop);
                } else if (node->son[1]->type == "Expression") {  // ( Expression )
                    prop = cast<ComposedVariable>(node->son[1]->prop);
                } else {
                    prop = new BinaryExprAST(".", cast<ExprAST*>(node->son[0]->prop), new StringExprAST(node->son[2]->raw));
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
                assert(node->son[0]->raw == "+" || node->son[0]->raw == "-" || node->son[0]->raw == "or");
                assert(node->prop.type() == typeid(addOP));
            }
            cerr << node->type << endl;
            assert(node->prop.has_value());  //每个结点都必有一个属性
        };
        int popNum = expr.second.size();
        vector<GrammarTreeNode*> reducedNode;
        for (int i = 0; i < popNum; i++) {
            states.pop_back();
            symbols.pop_back();
            reducedNode.push_back(*unlinkedNodes.rbegin());
            unlinkedNodes.pop_back();
        }
        curState = *states.rbegin();
        states.push_back(Goto[make_pair(curState, expr.first)]);
        symbols.push_back(expr.first);
        uint64_t gid = rng();
        GrammarTreeNode* newNode = new GrammarTreeNode("", expr.first, expr.first, 0, 0, gid);
        reverse(reducedNode.begin(), reducedNode.end());
        for (auto node : reducedNode) newNode->son.push_back(node);
        UpdateProperties(newNode);
        unlinkedNodes.push_back(newNode);
    };
    std::cerr << "start analyse" << endl;
    ifstream lexOut(file_name);
    states.push_back(0);
    symbols.push_back("");
    TokenItem N;
    lexOut >> N;
    for (;;) {
        curState = *states.rbegin();
        if (actionTable.count(make_pair(curState, N.parserSymbol)) == 0) assert(0);
        pair<ACTION, int> act = actionTable[make_pair(curState, N.parserSymbol)];
        if (act.first == Shift) {
            states.push_back(act.second);
            symbols.push_back(N.parserSymbol);
            unlinkedNodes.push_back(new GrammarTreeNode(N.raw, N.type, N.parserSymbol, N.row, N.column, rng()));
            lexOut >> N;
        } else if (act.first == Reduce) {
            Expr prod = Production_content[act.second];
            DoReduce(prod);
        } else if (act.first == ACC) {
            DoReduce({"S", {"ProgramStruct"}});
            cout << "ACCEPT!" << endl;
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
GlobalAST* parser_work(string file_name) {
    init();
    get_first();
    generate_table();
    Analyse(file_name);
    auto root = Analyse(file_name);
    check_grammar_tree(0, 0);
    return cast<GlobalAST*>(root->prop);
}
