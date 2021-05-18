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

string new_string(const char* s) {
    string p = malloc(strlen(s) + 1);
    strcpy(p, s);
    return p;
}

struct a {};

typedef struct a A;

int main() {
    string a = new_string("abcdef"), b = new_string("def");
    assign(&a, add(a, b));
    printf("%s", a);
    struct a xx;
    return 0;
}