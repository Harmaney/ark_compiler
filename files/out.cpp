//prelude
#include <iostream>
#include <string>
using namespace std;
void write(int x){cout<<x;}
int read(){int x;cin>>x;return x;}
//global_define
int t0=1;
int t1=5;
int t2=0;
//0
int i;
int main()
{
i = t0;
if (i>t1)goto L0;
L1:
{
write(i);
}
i++;
if (i<=t1)goto L1;
L0:
return t2;
}
