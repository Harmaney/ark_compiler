#include <queue>
#include <string>
using TokenQueue = std::queue<std::tuple<std::string, int, int, std::string>>;

std::string load_file(std::string file_name);

void load_key_words();

TokenQueue lex_work(std::string file_name);