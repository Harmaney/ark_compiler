#ifndef LOGGER_HPP
#define LOGGER_HPP
#include <iostream>

#include "include/rang.hpp"
static int LOG_LEVEL = 4;

#include "include/json.hpp"

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

extern Json lex_info, parser_info, gen_info;

struct DevNull : std::ostream {
    template <class T>
    std::ostream& operator<<(T const& x) {
        return *this;
    }
};

extern DevNull dev_null;

struct TerminalPrinter {
    std::ostream &ofs, &dev_null;
    TerminalPrinter(std::ostream& ofs, std::ostream& dev_null)
        : ofs(ofs), dev_null(dev_null) {}
    std::ostream& fatal() {
        if (0 <= LOG_LEVEL)
            return ofs << rang::style::bold << rang::fg::red << "FATAL "
                       << rang::style::reset << rang::fg::reset;
        else
            return dev_null;
    }

    std::ostream& warn() {
        if (1 <= LOG_LEVEL)
            return ofs << rang::style::bold << rang::fg::magenta << "WARN "
                       << rang::style::reset << rang::fg::reset;
        else
            return dev_null;
    }

    std::ostream& info() {
        if (3 <= LOG_LEVEL)
            return ofs << rang::style::bold << rang::fg::green << "INFO "
                       << rang::style::reset << rang::fg::reset;
        else
            return dev_null;
    }

    std::ostream& debug() {
        if (4 <= LOG_LEVEL)
            return ofs << rang::style::bold << rang::fg::blue << "DEBUG "
                       << rang::style::reset << rang::fg::reset;
        else
            return dev_null;
    }

    std::ostream& detail() {
        if (4 <= LOG_LEVEL)
            return ofs << rang::style::bold << rang::fg::gray << "DETAIL "
                       << rang::style::reset << rang::fg::reset;
        else
            return dev_null;
    }
};

extern TerminalPrinter term_print;
#endif