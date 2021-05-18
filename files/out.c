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

void write_integer(int a1) { printf("%d", a1); }
//global_define
int t0_0=2;
int t0_1=0;
//pre_struct
//pre_array
//struct
//init_global_var
void init_global_var_() {
}
//main
int i=2;
int main()
{
init_global_var_();
int __ret;
write_integer((i));
return t0_1;
return __ret;
}
