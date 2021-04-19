#pragma once

#include <string>
#include <vector>

enum TokenKind {
    uncertain = 0,
    id,
    opt,
    comment,
    literal,
    number,
};

struct Token {
    TokenKind kind;
    int start, end;
    std::string str;
};

class LexerScanner {
   private:
    std::vector<std::string> lines;
    int line,column;

    

   public:
    LexerScanner(std::string src);
    char peek();
    char nextChar();
    void rollback();

    bool hasNextToken();
    Token nextToken();
};
