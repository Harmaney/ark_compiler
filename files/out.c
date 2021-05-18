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
string t0_1 = NULL;
string t0_2 = NULL;
int t0_3=0;
//pre_struct
//pre_array
//struct
//init_string
void init_string_() {
t0_0= new_string_("Hello");
t0_1= new_string_(" ");
t0_2= new_string_("World");
}
//main
int main()
{
init_string_();
int __ret;
string t2_0=NULL;
assign_string_(&(t2_0),add_string_((t0_0),(t0_1)));
string t2_1=NULL;
assign_string_(&(t2_1),add_string_((t2_0),(t0_2)));
write_string((t2_1));
return t0_3;
return __ret;
}
