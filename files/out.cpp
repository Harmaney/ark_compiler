//prelude
#include <iostream>
#include <string>
using namespace std;
void write_int(int x){cout<<x;}
void write_str(string x){cout<<x;}
int read_int(){int x;cin>>x;return x;}
//global_define
int t0_0=0;
int t0_1=1;
int t0_2=1;
int t0_3=5;
string t0_4 = "\n";
int t0_5=0;
//0
int i;
int a;
int b;
int c;
void add(int a, int b, int* c)
{
int t2_0;
t2_0=a+b;
*c=t2_0;
}
int main()
{
a=t0_0;
b=t0_1;
i = t0_2;
if (i>t0_3)goto L0;
L1:
{
add(a,b,&c);
a=b;
b=c;
write_int(a);
write_str(t0_4);
}
i++;
if (i<=t0_3)goto L1;
L0:
return t0_5;
}
