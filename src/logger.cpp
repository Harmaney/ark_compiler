#include "logger.h"

#include <fstream>

#include "data.h"

using Json = nlohmann::json;

Json lexOutputer, parserOutputer, genOutputer;