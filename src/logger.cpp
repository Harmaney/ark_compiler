#include "logger.h"

#include <fstream>

#include "data.h"

using Json = nlohmann::json;
std::ofstream WALK_AST("walk_ast.log");

Json parserOutputer;