//prelude
#include <iostream>
#include <string>
using namespace std;
int ____nouse=1;
void write_integer(int x){cout<<x;}
void write_int64(long long x){cout<<x;}
void write_integer_string(int a1,string a2){cout<<a1<<" "<<a2;}
void read_integer(int *x){scanf("%d",x);}
void read_int64(long long *x){scanf("%lld",x);}
//pre_array
typedef long long t0_0[400019];
typedef long long t0_1[400019];
typedef long long t0_2[400019];
//global_define
long long t0_3=1;
long long t0_4=0;
long long t0_5=2;
long long t0_6=2;
long long t0_7=1;
long long t0_8=2;
long long t0_9=1;
long long t0_10=0;
long long t0_11=2;
long long t0_12=2;
long long t0_13=1;
long long t0_14=2;
long long t0_15=1;
long long t0_16=2;
long long t0_17=2;
long long t0_18=1;
long long t0_19=2;
long long t0_20=0;
long long t0_21=2;
long long t0_22=1;
long long t0_23=2;
long long t0_24=1;
long long t0_25=0;
long long t0_26=2;
long long t0_27=2;
long long t0_28=1;
long long t0_29=2;
long long t0_30=1;
long long t0_31=2;
long long t0_32=2;
long long t0_33=1;
long long t0_34=1;
long long t0_35=1;
long long t0_36=1;
long long t0_37=1;
long long t0_38=1;
long long t0_39=1;
long long t0_40=1;
long long t0_41=2;
long long t0_42=1;
long long t0_43=1;
long long t0_44=0;
//main
int i;
long long n;
long long m;
long long x;
long long y;
long long z;
long long opt;
t0_0 a;
t0_1 sum;
t0_2 add;
void color_int64_int64_int64_int64(long long l, long long r, long long rt, long long z)
{
long long* t2_0;
t2_0=&(add)[(rt)];
long long* t2_1;
t2_1=&(add)[(rt)];
long long t2_2;
(t2_2)=(*t2_1)+(z);
(*t2_0)=(t2_2);
long long* t2_3;
t2_3=&(sum)[(rt)];
long long* t2_4;
t2_4=&(sum)[(rt)];
long long t2_5;
(t2_5)=(r)-(l);
long long t2_6;
(t2_6)=(t2_5)+(t0_3);
long long t2_7;
(t2_7)=(z)*(t2_6);
long long t2_8;
(t2_8)=(*t2_4)+(t2_7);
(*t2_3)=(t2_8);
int __ret;
}
void push_col_int64_int64_int64(long long l, long long r, long long rt)
{
long long m;
long long* t2_0;
t2_0=&(add)[(rt)];
long long t2_1;
(t2_1)=(*t2_0)!=(t0_4);
if (t2_1)goto L0;
{
}
goto L1;
L0: ____nouse=1;;
{
long long t3_0;
(t3_0)=(l)+(r);
long long t3_1;
(t3_1)=(t3_0)/(t0_5);
(m)=(t3_1);
long long t3_2;
(t3_2)=(rt)*(t0_6);
long long* t3_3;
t3_3=&(add)[(rt)];
color_int64_int64_int64_int64((l),(m),(t3_2),(*t3_3));
long long t3_4;
(t3_4)=(m)+(t0_7);
long long t3_5;
(t3_5)=(rt)*(t0_8);
long long t3_6;
(t3_6)=(t3_5)+(t0_9);
long long* t3_7;
t3_7=&(add)[(rt)];
color_int64_int64_int64_int64((t3_4),(r),(t3_6),(*t3_7));
long long* t3_8;
t3_8=&(add)[(rt)];
(*t3_8)=(t0_10);
}
L1: ____nouse=1;;
int __ret;
}
void modify_int64_int64_int64_int64_int64_int64(long long l, long long r, long long rt, long long nowl, long long nowr, long long z)
{
long long m;
long long t2_0;
(t2_0)=(nowl)<=(l);
long long t2_1;
(t2_1)=(nowr)>=(r);
long long t2_2;
(t2_2)=(t2_0)&&(t2_1);
if (t2_2)goto L2;
{
}
goto L3;
L2: ____nouse=1;;
{
color_int64_int64_int64_int64((l),(r),(rt),(z));
return ;
}
L3: ____nouse=1;;
push_col_int64_int64_int64((l),(r),(rt));
long long t2_3;
(t2_3)=(l)+(r);
long long t2_4;
(t2_4)=(t2_3)/(t0_11);
(m)=(t2_4);
long long t2_5;
(t2_5)=(nowl)<=(m);
if (t2_5)goto L4;
{
}
goto L5;
L4: ____nouse=1;;
{
long long t3_0;
(t3_0)=(rt)*(t0_12);
modify_int64_int64_int64_int64_int64_int64((l),(m),(t3_0),(nowl),(nowr),(z));
}
L5: ____nouse=1;;
long long t2_6;
(t2_6)=(nowr)>(m);
if (t2_6)goto L6;
{
}
goto L7;
L6: ____nouse=1;;
{
long long t3_0;
(t3_0)=(m)+(t0_13);
long long t3_1;
(t3_1)=(rt)*(t0_14);
long long t3_2;
(t3_2)=(t3_1)+(t0_15);
modify_int64_int64_int64_int64_int64_int64((t3_0),(r),(t3_2),(nowl),(nowr),(z));
}
L7: ____nouse=1;;
long long* t2_7;
t2_7=&(sum)[(rt)];
long long t2_8;
(t2_8)=(rt)*(t0_16);
long long* t2_9;
t2_9=&(sum)[(t2_8)];
long long t2_10;
(t2_10)=(rt)*(t0_17);
long long t2_11;
(t2_11)=(t2_10)+(t0_18);
long long* t2_12;
t2_12=&(sum)[(t2_11)];
long long t2_13;
(t2_13)=(*t2_9)+(*t2_12);
(*t2_7)=(t2_13);
return ;
int __ret;
}
long long query_int64_int64_int64_int64_int64(long long l, long long r, long long rt, long long nowl, long long nowr)
{
long long __ret;
long long ans;
long long m;
long long t2_0;
(t2_0)=(nowl)<=(l);
long long t2_1;
(t2_1)=(nowr)>=(r);
long long t2_2;
(t2_2)=(t2_0)&&(t2_1);
if (t2_2)goto L8;
{
}
goto L9;
L8: ____nouse=1;;
{
long long* t3_0;
t3_0=&(sum)[(rt)];
(__ret)=(*t3_0);
return __ret;
}
L9: ____nouse=1;;
push_col_int64_int64_int64((l),(r),(rt));
long long t2_3;
(t2_3)=(l)+(r);
long long t2_4;
(t2_4)=(t2_3)/(t0_19);
(m)=(t2_4);
(ans)=(t0_20);
long long t2_5;
(t2_5)=(nowl)<=(m);
if (t2_5)goto L10;
{
}
goto L11;
L10: ____nouse=1;;
{
long long t3_0;
(t3_0)=(rt)*(t0_21);
long long t3_1;
(t3_1)=query_int64_int64_int64_int64_int64((l),(m),(t3_0),(nowl),(nowr));
long long t3_2;
(t3_2)=(ans)+(t3_1);
(ans)=(t3_2);
}
L11: ____nouse=1;;
long long t2_6;
(t2_6)=(nowr)>(m);
if (t2_6)goto L12;
{
}
goto L13;
L12: ____nouse=1;;
{
long long t3_0;
(t3_0)=(m)+(t0_22);
long long t3_1;
(t3_1)=(rt)*(t0_23);
long long t3_2;
(t3_2)=(t3_1)+(t0_24);
long long t3_3;
(t3_3)=query_int64_int64_int64_int64_int64((t3_0),(r),(t3_2),(nowl),(nowr));
long long t3_4;
(t3_4)=(ans)+(t3_3);
(ans)=(t3_4);
}
L13: ____nouse=1;;
(__ret)=(ans);
return __ret;
}
void build_int64_int64_int64(long long l, long long r, long long rt)
{
long long m;
long long* t2_0;
t2_0=&(add)[(rt)];
(*t2_0)=(t0_25);
long long t2_1;
(t2_1)=(l)==(r);
if (t2_1)goto L14;
{
}
goto L15;
L14: ____nouse=1;;
{
long long* t3_0;
t3_0=&(sum)[(rt)];
long long* t3_1;
t3_1=&(a)[(l)];
(*t3_0)=(*t3_1);
return ;
}
L15: ____nouse=1;;
long long t2_2;
(t2_2)=(l)+(r);
long long t2_3;
(t2_3)=(t2_2)/(t0_26);
(m)=(t2_3);
long long t2_4;
(t2_4)=(rt)*(t0_27);
build_int64_int64_int64((l),(m),(t2_4));
long long t2_5;
(t2_5)=(m)+(t0_28);
long long t2_6;
(t2_6)=(rt)*(t0_29);
long long t2_7;
(t2_7)=(t2_6)+(t0_30);
build_int64_int64_int64((t2_5),(r),(t2_7));
long long* t2_8;
t2_8=&(sum)[(rt)];
long long t2_9;
(t2_9)=(rt)*(t0_31);
long long* t2_10;
t2_10=&(sum)[(t2_9)];
long long t2_11;
(t2_11)=(rt)*(t0_32);
long long t2_12;
(t2_12)=(t2_11)+(t0_33);
long long* t2_13;
t2_13=&(sum)[(t2_12)];
long long t2_14;
(t2_14)=(*t2_10)+(*t2_13);
(*t2_8)=(t2_14);
int __ret;
}
int main()
{
int __ret;
read_int64(&(n));
read_int64(&(m));
i = t0_34;
if (i>n)goto L16;
L17: ____nouse=1;;
{
long long* t3_0;
t3_0=&(a)[(i)];
read_int64(&(*t3_0));
}
i++;
if (i<=n)goto L17;
L16: ____nouse=1;;
build_int64_int64_int64((t0_35),(n),(t0_36));
i = t0_37;
if (i>m)goto L18;
L19: ____nouse=1;;
{
read_int64(&(opt));
long long t3_0;
(t3_0)=(opt)==(t0_38);
if (t3_0)goto L20;
{
}
goto L21;
L20: ____nouse=1;;
{
read_int64(&(x));
read_int64(&(y));
read_int64(&(z));
modify_int64_int64_int64_int64_int64_int64((t0_39),(n),(t0_40),(x),(y),(z));
}
L21: ____nouse=1;;
long long t3_1;
(t3_1)=(opt)==(t0_41);
if (t3_1)goto L22;
{
}
goto L23;
L22: ____nouse=1;;
{
read_int64(&(x));
read_int64(&(y));
long long t4_0;
(t4_0)=query_int64_int64_int64_int64_int64((t0_42),(n),(t0_43),(x),(y));
write_int64((t4_0));
}
L23: ____nouse=1;;
}
i++;
if (i<=m)goto L19;
L18: ____nouse=1;;
return t0_44;
return __ret;
}
