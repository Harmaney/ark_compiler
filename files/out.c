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

void read_string_string_string(string*  a3,string*  a2,string*  a1) { *a3=malloc(255);*a2=malloc(255);*a1=malloc(255);scanf("%s%s%s", *a3, *a2, *a1); }
void write_string(string a1) { printf("%s", a1); }
//global_define
int t0_0=0;
//pre_struct
//pre_array
//struct
//init_string
void init_string_() {
}
//main
string a;
string b;
string c;
int main()
{
init_string_();
int __ret;
read_string_string_string(&(a),&(b),&(c));
string t2_0=NULL;
assign_string_(&(t2_0),add_string_((a),(b)));
string t2_1=NULL;
assign_string_(&(t2_1),add_string_((t2_0),(c)));
write_string((t2_1));
return t0_0;
return __ret;
}
