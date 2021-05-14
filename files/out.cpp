//prelude
#include <iostream>
#include <string>
using namespace std;
void write_int(int x){cout<<x;}
void write_str(string x){cout<<x;}
int read_int(){int x;cin>>x;return x;}
//global_define
string t0_0 = "a";
string t0_1 = "a";
int t0_2=0;
//0
int main()
{
struct Test{
int a;
};
Test test;
int p;
int* t2_0;
t2_0=&(test.a);
int t2_1;
t2_1=read_int();
*t2_0=t2_1;
int* t2_2;
t2_2=&(test.a);
p=*t2_2;
write_int(p);
return t0_2;
}
