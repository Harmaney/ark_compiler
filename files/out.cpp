//prelude
#include <iostream>
#include <string>
using namespace std;
void write_int(int x){cout<<x;}
void write_str(string x){cout<<x;}
int read_int(){int x;cin>>x;return x;}
//global_define
int t0=1;
int t1=5;
string t2 = "hello, world\n";
int t3=0;
//0
int i;
int main()
{
i = t0;
if (i>t1)goto L0;
L1:
{
write_int(i);
write_str(t2);
}
i++;
if (i<=t1)goto L1;
L0:
return t3;
}
