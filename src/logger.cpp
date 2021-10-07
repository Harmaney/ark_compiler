#include "logger.h"

#include "data.h"

using Json = nlohmann::json;

DevNull dev_null;
TerminalPrinter term_print(std::cerr, dev_null);

Json lex_info, parser_info, gen_info, parser_action;