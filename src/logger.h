#ifndef LOGGER_HPP
#define LOGGER_HPP

static int LOG_LEVEL = 4;

// FATAL    ERROR   WARN    INFO    DEBUG   TRACE
// 0        1       2       3       4       5

#define TRACE(a)          \
    if (5 <= LOG_LEVEL) { \
        a                 \
    }
#define DEBUG(a)          \
    if (4 <= LOG_LEVEL) { \
        a                 \
    }
#define INFO(a)           \
    if (3 <= LOG_LEVEL) { \
        a                 \
    }
#define WARN(a)           \
    if (2 <= LOG_LEVEL) { \
        a                 \
    }
#define ERROR(a)          \
    if (1 <= LOG_LEVEL) { \
        a                 \
    }
#define FATAL(a)          \
    if (0 <= LOG_LEVEL) { \
        a                 \
    }

#include <fstream>

#include "json.hpp"
#include "parser.h"
using Json = nlohmann::json;

extern std::ofstream WALK_AST;
struct ParserOutputer {
    Json j;
    /*
      address :
      type: 'pst' / 'ast'

    */
    ParserOutputer() : j(Json::array()) {}
    void AddNode(GrammarTreeNode* node) {
        Json nj;
        std::map<std::string, std::function<void()>> deal = {
            {"S", [&]() -> void { nj['ast'] = (uint64_t)cast<S>(node->prop); }},
        };
        nj["address"] = (uint64_t)node;
        if (deal.count(node->type)) deal[node->type]();
        j.push_back(nj);
    }
};

extern ParserOutputer parserOutputer;
#endif