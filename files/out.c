//prelude
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
typedef char* string;
int ____nouse = 1;

void assign_string_(string* a, string b) {
    if (*a) free(*a);
    char* p = malloc(strlen(b) + 1);
    strcpy(p, b);
    *a = p;
}

string add_string_(string a, string b) {
    char* c = malloc(strlen(a) + strlen(b) + 1);
    strcpy(c, a);
    strcpy(c + strlen(a), b);
    return c;
}

string new_string_(const char* s) {
    string p = malloc(strlen(s) + 1);
    strcpy(p, s);
    return p;
}

void read_integer_integer(int*  a2,int*  a1) { scanf("%d%d", a2, a1); }
void write_integer(int a1) { printf("%d", a1); }
//global_define
int t0_0=0;
int t0_1=0;
//pre_struct
//pre_array
//struct
//init_global_var
void init_global_var_() {
}
//main
int x;
int y;
int gcd_integer_integer(int a, int b)
{
int __ret;
int t2_0;
(t2_0)=(b)==(t0_0);
if (t2_0)goto L0;
{
int t3_0;
(t3_0)=(a)%(b);
int t3_1;
(t3_1)=gcd_integer_integer((b),(t3_0));
(__ret)=(t3_1);
}
goto L1;
L0: ____nouse=1;;
{
(__ret)=(a);
}
L1: ____nouse=1;;
return __ret;
}
int main()
{
init_global_var_();
int __ret;
read_integer_integer(&(x),&(y));
int t2_0;
(t2_0)=gcd_integer_integer((x),(y));
write_integer((t2_0));
return t0_1;
return __ret;
}
