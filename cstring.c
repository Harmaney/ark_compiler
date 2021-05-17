#include "stdio.h"
#include "stdlib.h"
#include "string.h"
typedef char* string;

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

string new (const char* s) {
    string p = malloc(strlen(s) + 1);
    strcpy(p, s);
    return p;
}

int main() {
    string a = new ("abcdef"), b = new ("def");
    assign(&a, add(a, b));
    printf("%s", a);
    return 0;
}