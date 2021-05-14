#include "parser.h"

#include <bits/stdc++.h>

#include "data.h"
using Json = nlohmann::json;

#define inset(y, x) x.find(y) != x.end()
#define table_exist 0
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
bool merge(const set<T> &A, set<T> &B) {
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
    Item(string LHS, vector<string> previous, vector<string> next,
         string LookAhead)
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
                AddAction(
                    make_pair(Shift, Item_set[GO(I.first, *next.begin())]),
                    make_pair(I.second, *next.begin()));
            } else if (it.LHS != S) {
                AddAction(
                    make_pair(Reduce,
                              Production_ID[make_pair(it.LHS, it.previous)]),
                    make_pair(I.second, it.LookAhead));
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

std::ifstream &operator>>(std::ifstream &ifs, TokenItem &item) {
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
            else if (raw == "=" || raw == "<>" || raw == ">" || raw == "<" ||
                     raw == ">=" || raw == "<=")
                parserSymbol = "relOP";
            else if (raw == "*" || raw == "/" || raw == "div" || raw == "mod" ||
                     raw == "and")
                parserSymbol = "mulOP";
            else
                parserSymbol = raw;
        } else if (type == "identify")
            parserSymbol = "ID";
        else if (type == "int" || type == "float" || type == "string")
            parserSymbol = type;
        else
            parserSymbol = raw;
        item = {raw, type, parserSymbol, row, column};
    }
    return ifs;
}

enum NODE_PROP {
    EXPRESSION_AST,
    EXPRESSION_AST_LIST,
    ID_LIST,
    PATCH_DECLS,
    VAR_TYPE
};
using PropAny = std::map<NODE_PROP, std::any>;

struct GrammarTreeNode {
    string raw, type, parserSymbol;
    int row, column;
    uint64_t ID;
    PropAny prop;

    vector<GrammarTreeNode *> son;
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
    vector<GrammarTreeNode *> unlinkedNodes;
    int curState;
    vector<int> states;
    vector<string> symbols;

    auto DoReduce = [&](Expr expr) {
        auto UpdateProperties = [&](GrammarTreeNode *node) {
            auto CreateBinaryAST = [&]() -> ExprAST * {
                return new BinaryExprAST(
                    node->son[1]->raw,
                    std::any_cast<ExprAST *>(
                        node->son[0]->prop[EXPRESSION_AST]),
                    std::any_cast<ExprAST *>(
                        node->son[1]->prop[EXPRESSION_AST]));
            };
            std::cerr << node->type << '\n';
            if (node->type == "IDList") {
                if (node->son.size() == 1) {  // IDList -> ID
                    node->prop[ID_LIST] =
                        std::vector<std::string>({node->son[0]->parserSymbol});
                } else {
                    assert(node->son.size() ==
                           3);  //下面的IDLIST没必要正确维护，可以直接覆盖
                    std::any_cast<std::vector<std::string> &>(
                        node->prop[ID_LIST] = node->son[0]->prop[ID_LIST])
                        .push_back(node->son[2]->parserSymbol);
                }
            } else if (node->type == "VarDeclaration") {
                if (node->son.size() == 3) {  // VarDeclaration -> IDList : Type
                    std::any_cast<std::vector<PropAny> &>(
                        node->prop[PATCH_DECLS]) = {
                        {{ID_LIST, node->son[0]->prop[ID_LIST]},
                         {VAR_TYPE, node->son[2]->parserSymbol}}};
                } else {  // VarDeclaration -> VarDeclaration ; IDList :
                          // Type
                    assert(node->son.size() == 5);
                    std::any_cast<std::vector<PropAny> &>(
                        node->prop[PATCH_DECLS] =
                            node->son[0]->prop[PATCH_DECLS])
                        .push_back({{ID_LIST, node->son[2]->prop[ID_LIST]},
                                    {VAR_TYPE, node->son[4]->parserSymbol}});
                }
            } else if (node->type == "ExpressionList") {
                if (node->son.size() == 1) {  // ExpressionList -> Expression
                    node->prop[EXPRESSION_AST_LIST] = std::vector<std::any>{
                        node->son[0]->prop[EXPRESSION_AST]};
                } else {  // ExpressionList -> ExpressionList ,
                          // Expression
                    std::any_cast<std::vector<std::any> &>(
                        node->prop[EXPRESSION_AST_LIST] =
                            node->son[0]->prop[EXPRESSION_AST_LIST])
                        .push_back(node->son[2]->prop[EXPRESSION_AST]);
                }
            } else if (node->type == "Expression") {
                /*
                  Expression -> SimpleExpression relOP SimpleExpression
                  @ Expression -> string @ Expression ->
                  SimpleExpression @ Expression -> Expression ^ @
                */
                if (node->son.size() == 3) {
                    node->prop[EXPRESSION_AST] = CreateBinaryAST();
                } else if (node->son.size() == 2) {
                    node->prop[EXPRESSION_AST] = new UnaryExprAST(
                        "^", std::any_cast<ExprAST *>(
                                 node->son[0]->prop[EXPRESSION_AST]));
                } else {
                    if (node->type == "string") {
                        node->prop[EXPRESSION_AST] =
                            new StringExprAST(node->raw);
                    } else {
                        node->prop[EXPRESSION_AST] =
                            node->son[0]->prop[EXPRESSION_AST];
                    }
                }
            } else if (node->type == "SimpleExpression") {
                /*
                SimpleExpression -> SimpleExpression addOP Term @
                SimpleExpression -> Term @
                */
                node->prop[EXPRESSION_AST] =
                    node->son.size() == 1 ? node->son[0]->prop[EXPRESSION_AST]
                                          : CreateBinaryAST();
            } else if (node->type == "Term") {
                node->prop[EXPRESSION_AST] =
                    node->son.size() == 1 ? node->son[0]->prop[EXPRESSION_AST]
                                          : CreateBinaryAST();
            } else if (node->type == "Factor") {
                if (node->type == "Num") {
                    node->prop[EXPRESSION_AST] =
                        node->son[0]->prop[EXPRESSION_AST];
                } else if (node->son[0]->type == "Variable") {  // FIXME
                    node->prop[EXPRESSION_AST] =
                        new VariableExprAST(node->son[0]->raw);
                } else if (node->son[0]->type == "ID") {
                } else if (node->son[1]->type ==
                           "Expression") {  // ( Expression )
                    node->prop[EXPRESSION_AST] =
                        node->son[1]->prop[EXPRESSION_AST];
                } else {
                    assert(node->son[1]->type == "Factor");
                    node->prop[EXPRESSION_AST] = new UnaryExprAST(
                        node->son[0]->raw,
                        std::any_cast<ExprAST *>(
                            node->son[1]->prop[EXPRESSION_AST]));
                }
            } else if (node->type == "Num") {
                if (node->son[0]->type == "int") {
                    node->prop[EXPRESSION_AST] =
                        new NumberExprAST(std::stoi(node->son[0]->raw));
                } else if (node->son[0]->type == "float") {
                    node->prop[EXPRESSION_AST] =
                        new NumberExprAST(std::stod(node->son[0]->raw));
                }
            }
        };
        int popNum = expr.second.size();
        vector<GrammarTreeNode *> reducedNode;
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
        GrammarTreeNode *newNode =
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
        } else if (act.first == Reduce) {
            Expr prod = Production_content[act.second];
            DoReduce(prod);
        } else if (act.first == ACC) {
            uint64_t gid = 0;
            GrammarTreeNode *newNode =
                new GrammarTreeNode("", "S", "S", 0, 0, gid);
            newNode->son = {*unlinkedNodes.rbegin()};
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