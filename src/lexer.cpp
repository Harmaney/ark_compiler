#include "lexer.h"

#include <cassert>
#include <iostream>

class AbstractVisitor {
   public:
    virtual bool fit(char chr) = 0;
    virtual Token scan(LexerScanner &scanner) = 0;
};

class NumberVisitor : public AbstractVisitor {
   public:
    virtual bool fit(char chr) { return '0' <= chr && chr <= '9'; }
    virtual Token scan(LexerScanner &scanner) {
        int res = 0;

        while ('0' <= scanner.peek() && scanner.peek() <= '9') {
            res = res * 10 + scanner.nextChar() - '0';
        }
        return Token{number, 0, 0, std::to_string(res)};
    }
};

class OperatorVisitor : public AbstractVisitor {
   public:
    virtual bool fit(char chr) { return chr == '+'; }
    virtual Token scan(LexerScanner &scanner) {
        scanner.nextChar();
        return Token{opt, 0, 0, "+"};
    }
};

LexerScanner::LexerScanner(std::string src) {
    int pos = 0, nxt;
    while (pos < src.size()) {
        nxt = src.find('\n', pos);
        if (nxt == src.npos) nxt = src.size() - 1;
        lines.push_back(src.substr(pos, nxt - pos + 1));
        pos = nxt + 1;
    }
    this->line = 0;
    this->column = 0;
}

bool LexerScanner::hasNextToken() {
    return line != lines.size() && column != lines[line].size();
}

Token LexerScanner::nextToken() {
    {
        auto visitor = new NumberVisitor();
        if (visitor->fit(peek())) {
            return visitor->scan(*this);
        }
    }
    {
        auto visitor = new OperatorVisitor();
        if (visitor->fit(peek())) {
            return visitor->scan(*this);
        }
    }
    // TODO: unreachable
}

void LexerScanner::rollback() {
    if (this->line == 0 && this->column == 0) {
        // TODO: cannot rollback
    }

    column--;
    if (column < 0) {
        line--;
        column = lines[line].length() - 1;
    }
}

char LexerScanner::nextChar() {
    if (line >= lines.size()) {
        return 0;
    }
    if (column >= lines[line].size()) {
        column = 0;
        line++;
    }
    if (line >= lines.size()) {
        return 0;
    }
    column++;
    return lines[line][column - 1];
}

char LexerScanner::peek() {
    char res = nextChar();
    if (res == 0) return 0;
    rollback();
    return res;
}
