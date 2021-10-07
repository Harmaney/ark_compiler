#ifndef LOGGER_HPP
#define LOGGER_HPP
#include <iostream>
#include <sstream>

#include "include/rang.hpp"

// extern int term_print.log_level;

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

extern Json lex_info, parser_info, gen_info, parser_action;

using DevNull = std::ostringstream;

extern DevNull dev_null;

struct TerminalPrinter {
  int log_level;
  std::ostream &ofs, &dev_null;
  TerminalPrinter(std::ostream& ofs, std::ostream& dev_null)
      : log_level(2), ofs(ofs), dev_null(dev_null) {}
  std::ostream& fatal() {
    if (0 <= log_level)
      return ofs << rang::style::bold << rang::fg::red << "FATAL "
                 << rang::style::reset << rang::fg::reset;
    else
      return dev_null;
  }

  std::ostream& error() {
    if (1 <= log_level)
      return ofs << rang::style::bold << rang::fg::yellow << "ERROR "
                 << rang::style::reset << rang::fg::reset;
    else
      return dev_null;
  }

  std::ostream& warn() {
    if (2 <= log_level)
      return ofs << rang::style::bold << rang::fg::magenta << "WARN "
                 << rang::style::reset << rang::fg::reset;
    else
      return dev_null;
  }

  std::ostream& info() {
    if (3 <= log_level)
      return ofs << rang::style::bold << rang::fg::green << "INFO "
                 << rang::style::reset << rang::fg::reset;
    else
      return dev_null;
  }

  std::ostream& debug() {
    if (4 <= log_level)
      return ofs << rang::style::bold << rang::fg::blue << "DEBUG "
                 << rang::style::reset << rang::fg::reset;
    else
      return dev_null;
  }

  std::ostream& detail() {
    if (5 <= log_level)
      return ofs << rang::style::bold << rang::fg::gray << "DETAIL "
                 << rang::style::reset << rang::fg::reset;
    else
      return dev_null;
  }
};

extern TerminalPrinter term_print;
#endif
