//prelude
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
typedef char* string;
int ____nouse = 1;

void assign(string* a, string b) {
    free(*a);
    char* p = malloc(strlen(b) + 1);
    strcpy(p, b);
    *a = p;
}

string add(string a, string b) {
    char* c = malloc(strlen(a) + strlen(b) + 1);
    strcpy(c, a);
    strcpy(c + strlen(a), b);
    return c;
}

string new_string(const char* s) {
    string p = malloc(strlen(s) + 1);
    strcpy(p, s);
    return p;
}

void write_string(string a1) { printf("%s", a1); }
//global_define
string t0_0 = new_string("Hello World");
int t0_1=0;
//main
int main()
{
int __ret;
write_string((t0_0));
return t0_1;
return __ret;
}
