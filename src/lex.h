#include <bits/stdc++.h>
using namespace std;

using TokenQueue = std::queue<std::tuple<std::string, int, int, std::string>>;

string load_file(string file_name);

void load_key_words();

TokenQueue lex_work(string file_name);