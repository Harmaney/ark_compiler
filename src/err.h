#pragma once
#include <stdexcept>

class CompilerErrorException : public std::exception {
   public:
    CompilerErrorException(std::string type, std::string msg, int row, int col)
        : e(type + "[" + std::to_string(row) + "," + std::to_string(col) + "]" +
            ": " + msg) {}

    virtual const char* what() const throw() { return e.c_str(); }

   private:
    std::string e;
};

class TypeErrorException : public CompilerErrorException {
   public:
    TypeErrorException(std::string msg, std::string cur, std::string expected,
                       int row, int col)
        : CompilerErrorException(
              "TypeError",
              msg + " (expect `" + expected + "` but `" + cur + "`)", row,
              col) {}
};

class InternalErrorException : public CompilerErrorException {
   public:
    InternalErrorException(std::string msg, int row, int col)
        : CompilerErrorException("InternalError", msg, row, col) {}
};

class SymbolUndefinedException : public CompilerErrorException {
   public:
    SymbolUndefinedException(std::string msg, std::string symbol, int row,
                             int col)
        : CompilerErrorException("SymbolUndefined", msg + "(expect `" + symbol + "`)",
                                 row, col) {}
};

class UndefinedBehaviorException: public CompilerErrorException {
   public:
    UndefinedBehaviorException(std::string msg, int row,
                             int col)
        : CompilerErrorException("UndefinedBehavior", msg,
                                 row, col) {}
};