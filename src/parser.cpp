#include<bits/stdc++.h>
#include"parser.h"
#define inset(y,x) x.find(y)!=x.end()
#define table_exist 1
using namespace std;

string S;
set<string> Terminal,Nonterminal;
typedef pair<string,vector<string> > Production;
vector<Production> Productions;
map<int,Production> Production_content;
map<Production,int> Production_ID;
int Production_num = 0;
vector<string> empty_vec;
map<string,set<string> > First;	
map<string,set<vector<string> > > RHS_set;
set<string> Get_epsilon;

typedef unsigned long long ull;
ull ran(){
	static ull x=2018210789;
	x^=(x<<17);
	x^=(x>>13);
	x^=(x<<7);
	return x;
}

void init(){
	ifstream input("../files/./grammar.txt");
	int mode = 0;
	string LHS;
	vector<string> RHS;
	while(!input.eof()){
		string str;
		input>>str;
		if(str.empty()) continue;
		if(str == "->") mode = 1;
		else if(str == "@" || str == "|"){
			RHS_set[LHS].insert(RHS);
			
			Production prod = make_pair(LHS,RHS);
			Productions.push_back(prod);
			Production_ID[prod] = ++Production_num;
			Production_content[Production_num] = prod;
			
			if(RHS.empty()) Get_epsilon.insert(LHS);
			RHS.clear();
			if(str == "@") mode = 0;
		}else if(mode == 0){
			LHS = str,Nonterminal.insert(str);
			if(S.empty()) S=LHS;
		}else RHS.push_back(str),Terminal.insert(str);
		
	}
	for(auto str:Nonterminal) Terminal.erase(str);
	
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
bool merge(const set<T> &A,set<T> &B){
    bool flg=false;
    for(auto x:A){
        if(B.find(x)==B.end()){
            flg=true;
            B.insert(x);
        }
    }
    return flg;
}

set<string> get_first(vector<string> strlist){
	set<string> res;
	vector<string>::iterator it = strlist.begin();
	string Head = *it;
	while(1){
		merge(First[Head],res);
		if(inset(Head,Terminal)) break;
		if(inset(Head,Get_epsilon)){
			it++;
			if(it == strlist.end()){
				merge(First[""],res);
				break;
			}else{
				Head = *it;
			}
		}else break;
	}
	return res;
}

void get_first(){
	for(auto x:Terminal) First[x].insert(x);
	First["$"].insert("$");
	First[""].insert("");
	bool changed = true;
	while(changed){
		changed = false;
		for(auto LHS:Nonterminal){
			for(auto RHS:RHS_set[LHS]){
				if(RHS.empty()) continue;
				changed |= merge(get_first(RHS),First[LHS]);
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

//typedef tuple<string,vector<string>,vector<string>,string> Item;//0A->1a.2Bb,3x
bool operator < (vector<string> A, vector<string> B) {
	for(int i = 0; i < A.size() && i < B.size(); i++) {
		if(A[i] != B[i]) return A[i] < B[i];
	}
	if(A.size() != B.size()) return A.size() < B.size();
}
bool operator == (vector<string> A, vector<string> B) {
	if(A.size() != B.size()) return false;
	for(int i = 0; i < A.size() && i < B.size(); i++) {
		if(A[i] != B[i]) return false;
	}
	return true;
}
struct Item{
	string LHS;
	string LookAhead;
	vector<string> previous,next;
	Item(){
	}
	Item(string LHS,vector<string> previous,vector<string> next,string LookAhead) :
		LHS(LHS), previous(previous), next(next), LookAhead(LookAhead) {}
	bool operator < (const Item other) const {
		if(LHS != other.LHS) {
			return LHS < other.LHS;
		}else if(!(previous == other.previous)) {
			return previous < other.previous;
		}else if(!(next == other.next)) {
			return next < other.next;
		}else return LookAhead < other.LookAhead;
	}
	bool operator == (const Item other) const {
		if(LHS != other.LHS) {
			return false;
		}else if(!(previous == other.previous)) {
			return false;
		}else if(!(next == other.next)) {
			return false;
		}else if(LookAhead != other.LookAhead) return false;
		return true;
	}
};

map<set<Item>,int> Item_set;
map<int,set<Item>> Item_content;
int item_num = 0;

set<Item> get_closure(Item I){
	set<Item> J;
	J.insert(I);
	bool changed = true;
    do{
        changed = false;
        set<Item> J_new;
        set<Item> J_add;
        merge(J,J_new);
        for(auto item:J_new){
        	vector<string> next = item.next;
        	if(next.empty()) continue;
        	string B = *next.begin();//B
        	vector<string> temp = next;
        	temp.erase(temp.begin());
        	temp.push_back(item.LookAhead);//temp = bx
        	
        	set<string> bx_first = get_first(temp);
        	 
        	for(auto RHS:RHS_set[B]){
        		for(auto str:bx_first){
        			Item new_item=Item(B,empty_vec,RHS,str);//B->.y,bx
        			J_add.insert(new_item);
        		}
        	}
        }
        changed |= merge(J_add,J);
    }while(changed);
    return J;
}

set<Item> GO(set<Item> I,string X){
	set<Item> J;
	for(auto item:I){
		//A->a.Bb,x
		if(item.next.empty()) continue;
		if(*item.next.begin() == X){
			item.previous.push_back(*item.next.begin());
			item.next.erase(item.next.begin());
			merge(get_closure(item),J);
		}
	}
	return J;
}

void get_items(){
	Item_content[item_num] = get_closure(Item(S,empty_vec,*RHS_set[S].begin(),"$"));
	Item_set[get_closure(Item(S,empty_vec,*RHS_set[S].begin(),"$"))] = item_num++;
	set<string> Sign;
	merge(Terminal,Sign);
	merge(Nonterminal,Sign);
	bool changed = true;
	do{
		changed = false;
		for(auto I:Item_set){
			for(auto X:Sign){
				if(X.empty()) continue;
				set<Item> Goix = GO(I.first,X);
				if(!Goix.empty() && Item_set.count(Goix) == 0) {
					Item_content[item_num] = Goix;
					Item_set[Goix] = item_num++;
					changed=true;
					
//					cout<<"================\n";
//					for(auto x:Goix){
//						cout<<x.LHS<<"-> ";
//						for(auto str:x.previous) cout<<str<<" ";
//						cout<<". ";
//						for(auto str:x.next) cout<<str<<" ";
//						cout<<", ";
//						cout<<x.LookAhead<<endl;
//					}
					cout<<item_num<<endl;

				}
			}
		}
	}while(changed);
	printf("%d\n",item_num);
}

enum ACTION{
    Shift=1,
    Reduce,
    ACC
};
map<pair<int,string>,pair<ACTION,int> > Action;
map<pair<int,string>,int> Goto;
        
void add_Action(pair<ACTION,int> act,pair<int,string> pos){
	if(Action.count(pos)){
		if(Action[pos] != act){
			
			if(Action[pos].first == Shift) return;
//			cout << "???" << endl;
//			cout << Action[pos].first << " " << Action[pos].second << endl;
//			cout << act.first << " " << act.second << endl;
//			cout << pos.first << " " << pos.second << endl;
		}
	}
	Action[pos] = act;
}

void load_table(){
	ifstream inf("../files/analyse_table.txt");
	int I,id,ACT_id;
	string a;
	while(!inf.eof()){
		inf>>I>>a>>ACT_id>>id;
		if(id == -1) {
			Goto[make_pair(I,a)]=ACT_id;
		} else {
			enum ACTION ACT;
			if(ACT_id == 1) ACT = Shift;
			else if(ACT_id == 2) ACT = Reduce;
			else if(ACT_id == 3) ACT = ACC;
			else assert(0);
			Action[make_pair(I,a)]=make_pair(ACT,id);
		}
	}
}

void generate_table(){
	if(table_exist){
		load_table();
		return;
	}
	get_items();
	for(auto I:Item_set){
		for(auto it:I.first){
			vector<string> next = it.next;
			if(!next.empty()){
				add_Action(make_pair(Shift,Item_set[GO(I.first,*next.begin())]),make_pair(I.second,*next.begin()));
			}else if(it.LHS != S){
				add_Action(make_pair(Reduce,Production_ID[make_pair(it.LHS,it.previous)]),make_pair(I.second,it.LookAhead));
			}else{
				add_Action(make_pair(ACC,0),make_pair(I.second,it.LookAhead));
			}
		}
		for(auto A:Nonterminal){
			set<Item> GoIA = GO(I.first,A);
			if(GoIA.empty()) continue;
			Goto[make_pair(I.second,A)]=Item_set[GoIA];
		}
	}
	ofstream of("../files/analyse_table.txt");
	for(auto I:Item_set){
		for(auto a:Terminal){
			if(Action.count(make_pair(I.second,a))){
				of<<I.second<<" "<<a<<" "<<Action[make_pair(I.second,a)].first<<" "<<Action[make_pair(I.second,a)].second<<endl;
			}
		}
		if(Action.count(make_pair(I.second,"$"))){
			of<<I.second<<" "<<"$"<<" "<<Action[make_pair(I.second,"$")].first<<" "<<Action[make_pair(I.second,"$")].second<<endl;
		}
		for(auto A:Nonterminal){
			if(Goto.count(make_pair(I.second,A))){
				of<<I.second<<" "<<A<<" "<<Goto[make_pair(I.second,A)]<<" -1"<<endl;
			}
		}
	}
	cout<<"table ready"<<endl;
}

struct newinput{
	string word,type,now_str;
	int row,column;
	newinput(string word,string type,string now_str,int row,int column):
		word(word),type(type),now_str(now_str),row(row),column(column) {}
};
newinput get_newinput(ifstream &inf){
//	if(inf.eof()) return newinput("","","$",0,0);
	string word,type,now_str;
	int row,column;
	inf>>word>>row>>column>>type;
	if(word=="") return newinput("","","$",0,0);
	if(type == "keyword" || type == "punc"){
		if(word == ":=") now_str = "assignOP";
		else if(word == "=" || word == "<>" || word == ">" || word == "<" || word == ">=" || word == "<=") now_str = "relOP";
		else if(word == "*" || word == "/" || word == "div" || word == "mod" || word == "and") now_str = "mulOP";
		else now_str = word;
	}
	else if(type == "identify") now_str = "ID";
	else if(type == "int" || type == "float" || type == "string") now_str = type;
	else now_str = word;
	return newinput(word,type,now_str,row,column);
}

void do_reduce(Production prod,vector<int> &State,vector<string> &Symbol,vector<AST> &Ast_Node){
	int pop_num = prod.second.size();
	vector<AST> Reduced_AST;
	for(int i = 0; i < pop_num; i++){
		State.pop_back();
		Symbol.pop_back();
		Reduced_AST.push_back(*Ast_Node.rbegin());
		Ast_Node.pop_back();
	}
	now_state = *State.rbegin();
	State.push_back(Goto[make_pair(now_state,prod.first)]);
	Symbol.push_back(prod.first);

	

}

void analyse(string file_name){
	
	/*for(auto I:Item_content){
		cout<<I.first<<endl;
		for(auto it:I.second){
			cout<<it.LHS<<"-> ";
			for(auto str:it.previous) cout<<str<<" ";
			cout<<". ";
			for(auto str:it.next) cout<<str<<" ";
			cout<<","<<it.LookAhead<<endl;
		}
	}*/
	cout<<"start analyse"<<endl;
	
	ifstream inf(file_name);
	int now_state;
	string now_str;
	vector<int> State;
	vector<string> Symbol;
	vector<AST> Ast_Node;
	State.push_back(0);
	Symbol.push_back("");
	newinput N = get_newinput(inf);
	do{
		
		now_state = *State.rbegin();
//		cout<<"state:"<<now_state<<" str: "<<N.word<<" type: "<<N.now_str<<endl;
		
		if(Action.count(make_pair(now_state,N.now_str)) == 0) assert(0);
		pair<ACTION,int> act = Action[make_pair(now_state,N.now_str)];
		if(act.first == Shift) {
			State.push_back(act.second);
			Symbol.push_back(N.now_str);
			N = get_newinput(inf);
		}else if(act.first == Reduce) {
			Production prod = Production_content[act.second];
			// cout<<"Reduce by:"<<prod.first<<"->";
			// for(auto str:prod.second) cout<<" "<<str;
			// cout<<endl;

			do_reduce(prod,State,Symbol,Ast_Node);

			
		}else if(act.first == ACC) {
			cout<<"ACCEPT!"<<endl;
			break;
		}
		
	}while(1);
}

void parser_work(string file_name){
	init();
	get_first();
	generate_table();
	cout<<"?"<<endl;
	analyse(file_name);
}