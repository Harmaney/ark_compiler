#include "logger.h"

#include <fstream>

#include "data.h"

std::ofstream WALK_AST("walk_ast.log");

ParserOutputer parserOutputer;