#include "lex.h"

#include <bits/stdc++.h>

#include "logger.h"
#define letter \
    case '_':  \
    case 'a':  \
    case 'A':  \
    case 'b':  \
    case 'B':  \
    case 'c':  \
    case 'C':  \
    case 'd':  \
    case 'D':  \
    case 'e':  \
    case 'E':  \
    case 'f':  \
    case 'F':  \
    case 'g':  \
    case 'G':  \
    case 'h':  \
    case 'H':  \
    case 'i':  \
    case 'I':  \
    case 'j':  \
    case 'J':  \
    case 'k':  \
    case 'K':  \
    case 'l':  \
    case 'L':  \
    case 'm':  \
    case 'M':  \
    case 'n':  \
    case 'N':  \
    case 'o':  \
    case 'O':  \
    case 'p':  \
    case 'P':  \
    case 'q':  \
    case 'Q':  \
    case 'r':  \
    case 'R':  \
    case 's':  \
    case 'S':  \
    case 't':  \
    case 'T':  \
    case 'u':  \
    case 'U':  \
    case 'v':  \
    case 'V':  \
    case 'w':  \
    case 'W':  \
    case 'x':  \
    case 'X':  \
    case 'y':  \
    case 'Y':  \
    case 'z':  \
    case 'Z':
#define number \
    case '0':  \
    case '1':  \
    case '2':  \
    case '3':  \
    case '4':  \
    case '5':  \
    case '6':  \
    case '7':  \
    case '8':  \
    case '9':
#define null_char \
    case ' ':     \
    case '\t':
#define return_char \
    case '\n':      \
    case '\r':
#define inset(y, x) x.find(y) != x.end()
#define add_end remain_token += now_char;
#define add_and_reset                                                     \
    columns = i - n_columns - remain_token.length() + 1;                  \
    if(remain_token.length() == 3){present_state = 101;}                  \
    token_stream.push_back({remain_token, rows, columns, present_state}); \
    remain_token.clear();                                                 \
    present_state = 0;
using namespace std;

string load_file(string file_name) {
    ifstream input(file_name);
    string result_string;
    string each_line;
    while (true) {
        if (!getline(input, each_line)) break;
        result_string += each_line + '\n';
        each_line.clear();
    }
    return result_string;
}

set<string> key_words;

void load_key_words() {
    ifstream input("../files/keywords.txt");
    string word;
    while (input >> word) key_words.insert(word);
}

struct Mark {
    string word;
    int row;
    int column;
    int type;
};

TokenQueue lex_work(string all_chars) {
    // init
    //    string file_name = argv[1];
    // load
    load_key_words();
    int present_state = 0;
    vector<Mark> token_stream;
    string remain_token;
    int rows = 1;
    int columns = 0;
    int n_columns = 0;
    int length = all_chars.size();
    for (auto i = 0; i < length; i++) {
        char now_char = all_chars[i];
        switch (present_state) {
            case 0:  // start
                switch (now_char) {
                    letter add_end present_state = 1;
                    break;
                    number add_end present_state = 2;
                    break;
                    null_char break;
                    return_char n_columns = i + 1;
                    rows++;
                    break;
                    case ':':
                        add_end present_state = 5;
                        break;
                    case '<':
                        add_end present_state = 6;
                        break;
                    case '>':
                        add_end present_state = 7;
                        break;
                    case '{':
                        add_end present_state = 8;
                        break;
                    case '\'':
                        add_end present_state = 11;
                        break;
                    case '"':
                        add_end present_state = 13;
                        break;
                    default:
                        add_end i++;
                        add_and_reset i--;
                        break;
                        // todo
                }
                break;
            case 1:  // id
                switch (now_char) {
                    letter number add_end break;
                    default:
                        add_and_reset i--;
                        break;
                }
                break;
            case 2:  // num
                switch (now_char) {
                    number add_end break;
                    case '.':
                        add_end present_state = 3;
                        break;
                    default:
                        add_and_reset i--;
                        break;
                }
                break;
            case 3:  // num.
                switch (now_char) {
                    number add_end present_state = 4;
                    break;
                    case '.':
                        add_end present_state = 12;
                        break;
                }
                break;
            case 4:  // num.num
                switch (now_char) {
                    number add_end break;
                    default:
                        add_and_reset i--;
                        break;
                }
                break;
            case 5:  //:
                switch (now_char) {
                    case '=':
                        add_end i++;
                        add_and_reset i--;
                        break;
                    default:
                        add_and_reset i--;
                        break;
                }
                break;
            case 6:  //<
                switch (now_char) {
                    case '=':
                        add_end i++;
                        add_and_reset i--;
                        break;
                    case '>':
                        add_end i++;
                        add_and_reset i--;
                        break;
                    default:
                        add_and_reset i--;
                        break;
                }
                break;
            case 7:  //>
                switch (now_char) {
                    case '=':
                        add_end i++;
                        add_and_reset i--;
                        break;
                    default:
                        add_and_reset i--;
                        break;
                }
                break;
            case 8:  // {
                switch (now_char) {
                    case '}':
                        remain_token.clear();
                        present_state = 0;
                        break;
                    case '*':
                        present_state = 9;
                        break;
                        null_char
                            // error
                            break;
                }
                break;
            case 9:  //{ *
                switch (now_char) {
                    case '*':
                        present_state = 10;
                        break;
                    default:
                        break;
                }
                break;
            case 10:  // {* *
                switch (now_char) {
                    case '}':
                        present_state = 0;
                        break;
                    default:
                        present_state = 9;
                        break;
                }
                break;
            case 11:  //'
                switch (now_char) {
                    case '\'':
                        add_end add_and_reset break;
                        return_char
                            // error
                            break;
                    default:
                        add_end break;
                }
                break;
            case 12:  // num..
                switch (now_char) {
                    number add_end break;
                    default:
                        string s1, s2, s3;
                        int st = 0;
                        for (auto c : remain_token) {
                            if (st == 0 && c == '.') {
                                st = 1;
                            }
                            if (st == 1 && (c >= '0' && c <= '9')) {
                                st = 2;
                            }
                            if (!st) {
                                s1 += c;
                            } else if (st == 1) {
                                s2 += c;
                            } else if (st == 2) {
                                s3 += c;
                            }
                        }
                        columns = i - n_columns - remain_token.length() + 1;
                        int c2 = columns + s1.length();
                        int c3 = c2 + s2.length();
                        token_stream.push_back({s1, rows, columns, 2});
                        token_stream.push_back({s2, rows, c2, 0});
                        token_stream.push_back({s3, rows, c3, 2});
                        remain_token.clear();
                        present_state = 0;
                        i--;
                        break;
                }
                break;
            case 13:  //""
                switch (now_char) {
                    case '\"':
                        add_end add_and_reset break;
                        return_char
                            // error
                            break;
                    default:
                        add_end break;
                }
                break;
        }
    }
    ofstream lout("../files/lex_out.txt");
    ofstream lerr("../files/lex_err.txt");

    TokenQueue result;
    for (auto s : token_stream) {
		if (s.type == 11) {
            s.word = s.word.substr(1);
            s.word.pop_back();
        }
        lout << s.word << " " << s.row << " " << s.column << " ";

        std::string type;
        if (inset(s.word, key_words)) {
            lerr << s.word << endl;
            s.type = 100;
        }

        switch (s.type) {
            case 2:
                type = "intVal";
                break;
            case 4:
                type = "realVal";
                break;
            case 11:
                type = "stringVal";
                break;
            case 1:
                type = "identify";
                break;
            case 100:
                type = "keyword";
                break;
            case 101:
                type = "charVal";
                break;
            default:
                type = "punc";
                break;
        }
        lout << type << endl;
        lexOutputer.push_back({{"word", s.word},
                               {"row", s.row},
                               {"column", s.column},
                               {"type", type}});
        result.push({s.word, s.row, s.column, type});
    }
    lout.close();
    lerr.close();
    return result;
}