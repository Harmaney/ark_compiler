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

void write_string(string a1) { printf("%s", a1); }
//global_define
string t0_0 = NULL;
int t0_1=0;
//pre_struct
//pre_array
//struct
//init_global_var
void init_global_var_() {
t0_0= new_string_("Hello World");
}
//main
int main()
{
init_global_var_();
int __ret;
write_string((t0_0));
return t0_1;
return __ret;
}
