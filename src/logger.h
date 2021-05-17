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

#include <bits/stdc++.h>

#include "json.hpp"

using Json = nlohmann::json;

template <typename P, typename... T>
Json Serialize(P* ptr) {
    Json ret;
    ret.push_back((uint64_t)ptr);
    return ret;
}

template <typename P, typename... T>
Json Serialize(const std::vector<P*>& vec) {
    Json ret;
    for (auto ptr : vec) ret.push_back((uint64_t)ptr);
    return ret;
}

template <typename P, typename... T>
Json Serialize(P* ptr, const T&... arg) {
    Json ret;
    ret.push_back((uint64_t)ptr);
    for (auto i : Serialize(arg...)) ret.push_back(i);
    return ret;
}

template <typename P, typename... T>
Json Serialize(const std::vector<P*>& vec, const T&... arg) {
    Json ret;
    for (auto ptr : vec) ret.push_back((uint64_t)ptr);
    for (auto i : Serialize(arg...)) ret.push_back(i);
    return ret;
}

extern Json lexOutputer, parserOutputer, genOutputer;

#endif