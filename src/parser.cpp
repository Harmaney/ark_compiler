#include <cstdio>
#include <cstdlib>
#include <set>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <random>
#include <any>
#include <vector>
#include "parser.h"
#include "data.h"
#include "parser.h"
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
    ifstream input("../files/./grammar.txt");
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
        }
        else if (mode == 0) {
            LHS = str, Nonterminal.insert(str);
            if (S.empty()) S = LHS;
        }
        else
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
template <typename T> bool merge(const set<T>& A, set<T>& B) {
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
    Item(string LHS, vector<string> previous, vector<string> next,
        string LookAhead)
        : LHS(LHS), previous(previous), next(next), LookAhead(LookAhead) {}
    bool operator<(const Item other) const {
        if (LHS != other.LHS) {
            return LHS < other.LHS;
        }
        else if (!(previous == other.previous)) {
            return previous < other.previous;
        }
        else if (!(next == other.next)) {
            return next < other.next;
        }
        else
            return LookAhead < other.LookAhead;
    }
    bool operator==(const Item other) const {
        if (LHS != other.LHS) {
            return false;
        }
        else if (!(previous == other.previous)) {
            return false;
        }
        else if (!(next == other.next)) {
            return false;
        }
        else if (LookAhead != other.LookAhead)
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
    Item_content[item_num] =
        get_closure(Item(S, empty_vec, *RHS_set[S].begin(), "$"));
    Item_set[get_closure(Item(S, empty_vec, *RHS_set[S].begin(), "$"))] =
        item_num++;
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
            cerr << actionTable[pos].first << " " << actionTable[pos].second
                << endl;
            cerr << act.first << " " << act.second << endl;
            cerr << pos.first << " " << pos.second << endl;
        }
    }
    actionTable[pos] = act;
}
void LoadTable() {
    ifstream inf("../files/analyse_table.txt");
    int I, id, ACT_id;
    string a;
    while (!inf.eof()) {
        inf >> I >> a >> ACT_id >> id;
        if (id == -1) {
            Goto[make_pair(I, a)] = ACT_id;
        }
        else {
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
                AddAction(
                    make_pair(Shift, Item_set[GO(I.first, *next.begin())]),
                    make_pair(I.second, *next.begin()));
            }
            else if (it.LHS != S) {
                AddAction(
                    make_pair(Reduce,
                        Production_ID[make_pair(it.LHS, it.previous)]),
                    make_pair(I.second, it.LookAhead));
            }
            else {
                AddAction(make_pair(ACC, 0), make_pair(I.second, it.LookAhead));
            }
        }
        for (auto A : Nonterminal) {
            set<Item> GoIA = GO(I.first, A);
            if (GoIA.empty()) continue;
            Goto[make_pair(I.second, A)] = Item_set[GoIA];
        }
    }
    ofstream of("../files/analyse_table.txt");
    for (auto I : Item_set) {
        for (auto a : Terminal) {
            if (actionTable.count(make_pair(I.second, a))) {
                of << I.second << " " << a << " "
                    << actionTable[make_pair(I.second, a)].first << " "
                    << actionTable[make_pair(I.second, a)].second << endl;
            }
        }
        if (actionTable.count(make_pair(I.second, "$"))) {
            of << I.second << " "
                << "$"
                << " " << actionTable[make_pair(I.second, "$")].first << " "
                << actionTable[make_pair(I.second, "$")].second << endl;
        }
        for (auto A : Nonterminal) {
            if (Goto.count(make_pair(I.second, A))) {
                of << I.second << " " << A << " "
                    << Goto[make_pair(I.second, A)] << " -1" << endl;
            }
        }
    }
    cout << "table ready" << endl;
}
struct TokenItem {
    string raw, type, parserSymbol;
    int row, column;
    TokenItem() {}
    TokenItem(string raw, string type, string parserSymbol, int row, int column)
        : raw(raw),
        type(type),
        parserSymbol(parserSymbol),
        row(row),
        column(column) {}
};
std::ifstream& operator>>(std::ifstream& ifs, TokenItem& item) {
    std::string raw, type;
    int row, column;
    ifs >> raw >> row >> column >> type;
    if (raw == "")
        item = { "", "", "$", 0, 0 };
    else {
        string parserSymbol;
        if (type == "keyword" || type == "punc") {
            if (raw == ":=")
                parserSymbol = "assignOP";
            else if (raw == "=" || raw == "<>" || raw == ">" || raw == "<" ||
                raw == ">=" || raw == "<=")
                parserSymbol = "relOP";
            else if (raw == "*" || raw == "/" || raw == "div" || raw == "mod" ||
                raw == "and")
                parserSymbol = "mulOP";
            else
                parserSymbol = raw;
        }
        else if (type == "identify")
            parserSymbol = "ID";
        else if (type == "int" || type == "float" || type == "string")
            parserSymbol = type;
        else
            parserSymbol = raw;
        item = { raw, type, parserSymbol, row, column };
    }
    return ifs;
}
struct GrammarTreeNode {
    string raw, type, parserSymbol;
    int row, column;
    uint64_t ID;
    std::any prop;

    vector<GrammarTreeNode*> son;
    GrammarTreeNode(string raw, string type, string parserSymbol, int row,
        int column, uint64_t ID)
        : raw(raw),
        type(type),
        parserSymbol(parserSymbol),
        row(row),
        column(column),
        ID(ID),
        son{} {}
};
vector<string> IDlist, Varlist, Paralist;
void Analyse(string file_name) {
    //记录下来每个node->prop的类型
    vector<GrammarTreeNode*> unlinkedNodes;
    int curState;
    vector<int> states;
    vector<string> symbols;
    auto DoReduce = [&](Expr expr) {
        auto UpdateProperties = [&](GrammarTreeNode* node) {
            auto CreateBinaryAST = [&]() -> ExprAST* {
                return new BinaryExprAST(
                    node->son[1]->raw,
                    std::any_cast<ExprAST*>(
                        node->son[0]->prop),
                    std::any_cast<ExprAST*>(
                        node->son[2]->prop));
            };
            auto GetArgList = [&]() {
                std::vector<ExprAST*> arg_list;
                for (auto expr : std::any_cast<std::vector<std::any>&>(node->son[2]->prop)) {
                    arg_list.push_back(std::any_cast<ExprAST*>(expr));
                }
                return arg_list;
            };
            using ProgramStruct = GlobalAST*;
            using Component = map<std::string, std::vector<std::any>>;
            using ProgramBody = map<std::string, std::vector<std::any>>;
            using CompoundStatement = std::vector<AST*>;
            if (node->type == "S") {
                node->prop = node->son[0]->prop;
            }
            else if (node->type == "ProgramStruct") {
                auto M = std::any_cast<ProgramBody>(node->son[2]);
                std::vector<VariableDeclAST*> vars;
                for (auto i : std::any_cast<std::vector<std::any>>(M["var"]))
                    vars.push_back(std::any_cast<VariableDeclAST*>(i));
                std::vector<FunctionAST*> functions;
                for (auto i : std::any_cast<std::vector<std::any>>(M["function"]))
                    functions.push_back(std::any_cast<FunctionAST*>(i));
                node->prop = new GlobalAST(
                    vars,
                    functions,
                    std::any_cast<BlockAST*>(
                        std::any_cast<std::vector<std::any>>(M["body"]).front()
                    )
                );
                assert(node->prop.type() == typeid(ProgramStruct));
            }
            else if (node->type == "ProgramHead") {
                //Do nothing!
            }
            else if (node->type == "ProgramBody") { //ProgramBody -> Component CompoundStatement @
                ProgramBody M = std::any_cast<Component>(node->son[0]->prop);
                M["body"].push_back(std::any_cast<CompoundStatement>(node->son[1]->prop));
                node->prop = M;
            }
            else if (node->type == "Component") {
                if (node->son[0]->raw == "const") {
                    auto M = std::any_cast<Component>(node->son[3]->prop);
                    M["const"].push_back(node->son[1]->prop);
                    node->prop = M;
                }
                else if (node->son[0]->raw == "type") {
                    auto M = std::any_cast<Component>(node->son[3]->prop);
                    M["type"].push_back(node->son[1]->prop);
                    node->prop = M;
                }
                else if (node->son[0]->raw == "var") { //Component -> var VarDeclaration ; Component
                    auto M = std::any_cast<Component>(node->son[3]->prop);
                    M["var"].push_back(node->son[1]->prop);
                    node->prop = M;
                }
                else if (node->son[0]->type == "Subprogram") {
                    auto M = std::any_cast<Component>(node->son[2]->prop);
                    M["function"].push_back(node->son[0]->prop);
                    node->prop = M;
                }
                else {
                    node->prop = Component();
                }
            }
            else if (node->type == "IDList") {
                if (node->son.size() == 1) {  // IDList -> ID
                    node->prop = std::vector<std::string>(1, node->son[0]->raw);
                }
                else {
                    auto list = std::any_cast<std::vector<std::string>>(node->son[0]->prop);
                    list.push_back(node->son[2]->raw);
                    node->prop = list;
                }
            }
            else if (node->type == "ConstDeclaration") {

            }
            else if (node->type == "ConstValue") {
                if (node->son[0]->raw == "+") { //ConstValue -> + Num
                    node->prop = new UnaryExprAST("+",
                        std::any_cast<ExprAST*>(node->son[1]->prop)
                    );
                }
                else if (node->son[0]->raw == "-") { //ConstValue -> - Num
                    node->prop = new UnaryExprAST("-",
                        std::any_cast<ExprAST*>(node->son[1]->prop)
                    );
                }
                else if (node->son.size() == 1) { //ConstValue -> Num
                    node->prop = node->son[0]->prop;
                }
            }
            else if (node->type == "TypeDeclaration") {

            }
            //TypeDeclAST*
            else if (node->type == "ActualType") {
                if (node->son.size() == 1)
                    node->prop = node->son[0]->prop;
                else
                    abort();
            }
            //std::vector<VariableDeclAST*>
            else if (node->type == "VarDeclaration") {
                if (node->son.size() == 3) { //VarDeclaration -> IDList : Type
                    auto list = std::any_cast<std::vector<std::string>>(node->son[0]->prop);
                    std::vector<VariableDeclAST*> now;
                    for (auto id : list) {
                        now.push_back(new VariableDeclAST(
                            new VariableExprAST(id),
                            std::any_cast<TypeDeclAST*>(node->son[2]->prop),
                            true
                        ));
                    }
                    node->prop = now;
                }
                else {
                    auto now = std::any_cast<std::vector<VariableDeclAST*>>(node->son[0]->prop);
                    auto list = std::any_cast<std::vector<std::string>>(node->son[2]->prop);
                    for (auto id : list) {
                        now.push_back(new VariableDeclAST(
                            new VariableExprAST(id),
                            std::any_cast<TypeDeclAST*>(node->son[4]->prop),
                            true
                        ));
                    }
                    node->prop = now;
                }
            }
            //TypeDeclAST*
            else if (node->type == "Type") {
                if (node->son[0]->type == "array") {
                    auto pr = std::any_cast<std::pair<NumberExprAST*, NumberExprAST*>>(node->son[2]->prop);
                    node->prop = new ArrayTypeDeclAST(
                        std::any_cast<BasicTypeAST*>(node->son[5]->prop),
                        pr.first,
                        pr.second
                    );
                }
                else if (node->type == "BasicType") {
                    node->prop = node->son[0]->prop;
                }
                else {
                    node->prop = new PointerTypeDeclAST(
                        std::any_cast<BasicTypeAST*>(node->son[1]->prop)
                    );
                }
            }
            else if (node->type == "BasicType") {
                node->prop = new BasicTypeAST(node->son[0]->raw);
            }
            else if (node->type == "Period") {
                if (node->son.size() == 3) {
                    node->prop = make_pair(
                        std::any_cast<NumberExprAST*>(node->son[0]->prop),
                        std::any_cast<NumberExprAST*>(node->son[2]->prop)
                    );
                }
                else
                    abort();
            }
            else if (node->type == "Subprogram") {
                auto pr = std::any_cast<std::pair<std::any, std::any>>(node->son[2]);
                node->prop = new FunctionAST(
                    std::any_cast<FunctionSignatureAST*>(node->son[0]->prop),
                    std::any_cast<std::vector<VariableDeclAST*>>(pr.first),
                    std::any_cast<BlockAST*>(pr.second)
                );
            }
            else if (node->type == "SubprogramHead") {
                if (node->son.size() == 3) {
                    node->prop = new FunctionSignatureAST(
                        node->son[1]->raw,
                        std::any_cast<std::vector<VariableDeclAST*>>(node->son[2]->prop),
                        new BasicTypeAST("void")
                    );
                }
                else {
                    node->prop = new FunctionSignatureAST(
                        node->son[1]->raw,
                        std::any_cast<std::vector<VariableDeclAST*>>(node->son[2]->prop),
                        new BasicTypeAST(node->son[4]->raw)
                    );
                }
            }
            else if (node->type == "FormalParameter") {
                if (node->son.size() <= 2)
                    node->prop = std::vector<VariableDeclAST*>();
                else {
                    node->prop = node->son[1]->prop;
                }
            }
            else if (node->type == "ParameterList") {
                if (node->son.size() == 1)
                    node->prop = node->son[0]->prop;
                else {
                    auto list1 = std::any_cast<std::vector<VariableDeclAST*>>(node->son[0]);
                    auto list2 = std::any_cast<std::vector<VariableDeclAST*>>(node->son[2]);
                    for (auto i : list2)
                        list1.push_back(i);
                    node->prop = list1;
                }
            }
            else if (node->type == "Parameter") {
                node->prop = node->son[0]->prop;
            }
            else if (node->type == "VarParameter") {
                auto list = std::any_cast<std::vector<VariableDeclAST*>>(node->son[1]->prop);
                for (auto& ptr : list)
                    ptr->isRef = true;
                node->prop = list;
            }
            else if (node->type == "ValueParameter") {
                std::vector<VariableDeclAST*> list;
                for (auto i : std::any_cast<std::vector<std::string>>(node->son[0]->prop)) {
                    list.push_back(new VariableDeclAST(std::any_cast<VariableExprAST*>(i),
                        std::any_cast<BasicTypeAST*>(node->son[2]->prop),
                        false
                    ));
                }
                node->prop = list;
            }
            else if (node->type == "SubprogramBody") {
                node->prop = std::make_pair(node->son[0]->prop, node->son[1]->prop);
            }
            else if (node->type == "SubComponent") {
                if (node->son.size() == 0)
                    node->prop = nullptr;
                else {
                    auto list = std::any_cast<BlockAST*>(node->son[3]->prop);
                    if (node->son[0]->type == "var") {
                        list->exprs.insert(list->exprs.begin(), 
                            std::any_cast<AST*>(node->son[1]->prop));
                    }
                    else {
                        list->exprs.insert(list->exprs.begin(),
                            std::any_cast<AST*>(node->son[1]->prop));
                    }
                    node->prop = std::move(list);
                }
            }
            else if (node->type == "CompoundStatement") {
                node->prop = node->son[1]->prop;
            }
            else if (node->type == "StatementList") {
                if (node->son.size() == 1) {
                    node->prop = new BlockAST(
                        std::vector<AST*>(1, std::any_cast<AST*>(node->son[0]->prop))
                    );
                }
                else {
                    auto list = std::any_cast<BlockAST*>(node->son[0]->prop);
                    list->exprs.push_back(std::any_cast<AST* >(node->son[1]->prop));
                    node->prop = std::move(list);
                }
            }
            else if (node->type == "Statement") {
                if (node->son.empty()) {
                    node->prop = new BlockAST({});
                }
                else if (node->son[0]->type == "CompoundStatement") {
                    node->prop = node->son[0]->prop;
                }
                else if (node->son[0]->type == "ProcedureCall(ARG)") {
                    node->prop = node->son[0]->prop;
                }
                else if (node->son[0]->type == "Variable") {
                    node->prop = new BinaryExprAST(
                        node->son[1]->raw,
                        std::any_cast<ExprAST* >(node->son[0]->prop),
                        std::any_cast<ExprAST* >(node->son[2]->prop)
                    );
                }
                else if (node->son[0]->type == "if") {
                    BlockAST* body_true = nullptr;
                    BlockAST* body_false = nullptr;
                    if (node->son[3]->prop.type() != typeid(BlockAST*))
                        body_true = new BlockAST(std::vector<AST*>(1, 
                            std::any_cast<AST*>(node->son[3]->prop)));
                    else
                        body_true = std::any_cast<BlockAST*>(node->son[3]->prop);
                    if (node->son[4]->prop.type() != typeid(BlockAST*))
                        body_true = new BlockAST(std::vector<AST*>(1,
                            std::any_cast<AST*>(node->son[4]->prop)));
                    else
                        body_true = std::any_cast<BlockAST*>(node->son[4]->prop);
                    node->prop = new IfStatementAST(
                        std::any_cast<ExprAST*>(node->son[1]->prop),
                        body_true,
                        body_false
                    );
                }
                else if (node->son[0]->type == "for") {
                    
                }
                else if (node->son[0]->type == "while") {
                    BlockAST* body;
                    if (node->son[3]->prop.type() != typeid(BlockAST*))
                        body = new BlockAST(std::vector<AST*>(1,
                            std::any_cast<AST*>(node->son[3]->prop)));
                    else
                        body = std::any_cast<BlockAST*>(node->son[3]->prop);
                    node->prop = new WhileStatementAST(
                        std::any_cast<ExprAST*>(node->son[1]->prop),
                        body
                    );
                }
                else if (node->son[0]->type == "read") {
                    node->prop = new CallExprAST(
                        "read",
                        GetArgList()
                    );
                }
                else if (node->son[0]->type == "write") {
                    node->prop = new CallExprAST(
                        "write",
                        GetArgList()
                    );
                }
                else if (node->son[0]->type == "writeln") {
                    if (node->son.size() == 4) {
                        node->prop = new CallExprAST(
                            "writeln",
                            GetArgList()
                        );
                    }
                    else {
                        node->prop = new CallExprAST(
                            "writeln",
                            { }
                        );
                    }
                }
            }
            else if (node->type == "Variable") {
            auto var = new VariableExprAST(node->son[0]->raw);
                if (std::any_cast<nullptr_t>(node->son[1]->prop)==nullptr)
                    node->prop = var;
                else
                    node->prop = new BinaryExprAST("[]",
                        std::any_cast<ExprAST*>(var),
                        std::any_cast<ExprAST*>(node->son[1]->prop));
            }
            else if (node->type == "IDVarpart") {
                if (node->son.size() == 0)
                    node->prop = nullptr;
                else
                    node->prop = node->son[1]->prop;
            }
            else if (node->type == "ProcedureCall(ARG)") {
                if (node->son.size() == 4) { //ProcedureCall(ARG) -> ID ( ExpressionList )
                    node->prop = new CallExprAST(
                        node->son[0]->raw,
                        GetArgList()
                    );
                }
                else { //ProcedureCall(ARG) -> ID ( )
                    node->prop = new CallExprAST(
                        node->son[0]->raw,
                        { }
                    );
                }
            }
            else if (node->type == "ElsePart") {
                if (node->son.size() > 0) {
                    node->prop = node->son[1]->prop;
                }
                else {
                    node->prop = nullptr;
                }
            }
            else if (node->type == "ExpressionList") {
                if (node->son.size() == 1) {  // ExpressionList -> Expression
                    node->prop = std::vector<std::any>{
                        node->son[0]->prop };
                }
                else {  // ExpressionList -> ExpressionList , Expression
                    std::any_cast<std::vector<std::any>&>(
                        node->prop =
                        node->son[0]->prop)
                        .push_back(node->son[2]->prop);
                }
            }
            else if (node->type == "Expression") {
                /*
                    Expression -> SimpleExpression relOP SimpleExpression @
                    Expression -> string @
                    Expression -> SimpleExpression @
                    Expression -> Expression ^ @
                */
                if (node->son.size() == 3) {
                    node->prop = CreateBinaryAST();
                }
                else if (node->son.size() == 2) {
                    node->prop = new UnaryExprAST(
                        "^", std::any_cast<ExprAST*>(
                            node->son[0]->prop));
                }
                else {
                    if (node->son[0]->type == "string") {
                        node->prop =
                            new StringExprAST(node->raw);
                    }
                    else {
                        node->prop =
                            node->son[0]->prop;
                    }
                }
            }
            else if (node->type == "SimpleExpression") {
                if (node->son.size() == 1) //SimpleExpression -> Term
                    node->prop = node->son[0]->prop;
                else //SimpleExpression -> SimpleExpression addOP Term
                    node->prop = CreateBinaryAST();
            }
            else if (node->type == "Term") {
                if (node->son.size() == 1) //Term -> Factor
                    node->prop = node->son[0]->prop;
                else //Term -> Term mulOP Factor
                    node->prop = CreateBinaryAST();
            }
            else if (node->type == "Factor") {
                if (node->son[0]->type == "Num") {
                    node->prop = node->son[0]->prop;
                }
                else if (node->son[0]->type == "Variable") {  // FIXME
                    node->prop =
                        new VariableExprAST(node->son[0]->raw);
                }
                else if (node->son[0]->type == "ProcedureCall(ARG)") {
                    node->prop = node->son[0]->prop;
                }
                else if (node->son[1]->type == "Expression") {  // ( Expression )
                    node->prop =
                        node->son[1]->prop;
                }
                else { //Factor -> not Factor, Factor -> uminus Factor
                    assert(node->son[1]->type == "Factor");
                    node->prop = new UnaryExprAST(
                        node->son[0]->raw,
                        std::any_cast<ExprAST*>(
                            node->son[1]->prop));
                }
            }
            //NumberExprAST*
            else if (node->type == "Num") {
                if (node->son[0]->type == "int") {
                    node->prop = new NumberExprAST(std::stoi(node->son[0]->raw));
                }
                else if (node->son[0]->type == "float") {
                    node->prop = new NumberExprAST(std::stod(node->son[0]->raw));
                }
            }
            else if (node->type == "Digits") {
                node->prop =
                    new NumberExprAST(std::stoi(node->son[0]->raw));
            }
            else if (node->type == "addOP") {
            
            }
            else if (node->type == "relOP") {

            }
            else
                abort();
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
        GrammarTreeNode* newNode =
            new GrammarTreeNode("", expr.first, expr.first, 0, 0, gid);
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
        if (actionTable.count(make_pair(curState, N.parserSymbol)) == 0)
            assert(0);
        pair<ACTION, int> act =
            actionTable[make_pair(curState, N.parserSymbol)];
        if (act.first == Shift) {
            states.push_back(act.second);
            symbols.push_back(N.parserSymbol);
            unlinkedNodes.push_back(new GrammarTreeNode(
                N.raw, N.type, N.parserSymbol, N.row, N.column, rng()));
            lexOut >> N;
        }
        else if (act.first == Reduce) {
            Expr prod = Production_content[act.second];
            DoReduce(prod);
        }
        else if (act.first == ACC) {
            uint64_t gid = 0;
            GrammarTreeNode* newNode =
                new GrammarTreeNode("", "S", "S", 0, 0, gid);
            newNode->son = { *unlinkedNodes.rbegin() };
            cout << "ACCEPT!" << endl;
            break;
        }
    }
}
void check_grammar_tree(uint64_t gid, int dep) {
    /*for (auto x : gramTree[gid]) {
        cout << dep << " " << x.word << " " << x.type << endl;
        check_grammar_tree(x.ID, dep + 1);
    }*/
}
void parser_work(string file_name) {
    init();
    get_first();
    generate_table();
    cout << "?" << endl;
    Analyse(file_name);
    check_grammar_tree(0, 0);
}
