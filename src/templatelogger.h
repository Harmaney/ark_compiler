#include <iostream>
#include <string>

template <typename P, typename... T>
void logTemplate(std::ostream &out, std::string tplt, const P &x) {
    for (auto chr : tplt) {
        if (chr == '#')
            std::cout << x;
        else
            std::cout << chr;
    }
    std::cout << std::endl;
}

template <typename P, typename... T>
void logTemplate(std::ostream &out, std::string tplt, const P &x,
                 const T &...y) {
    if (tplt.empty()) std::cout << std::endl;
    if (tplt.front() == '#') {
        cout << x;
        logTemplate(out, tplt.substr(1), x, y...);
    } else {
        cout << tplt.front();
        logTemplate(out, tplt.substr(1), y...);
    }
}

void printHead(std::ostream &out, std::string level) {
    out << "[" << level << "] ";
}

template <typename... T>
void logInfo(std::string tplt, const T &...x) {
    printHead(std::cerr, "info");
    logTemplate(std::cerr, tplt, x...);
}

template <typename... T>
void logWarn(std::string tplt, const T &...x) {
    printHead(std::cerr, "warn");
    logTemplate(std::cerr, tplt, x...);
}

template <typename... T>
void logError(std::string tplt, const T &...x) {
    printHead(std::cerr, "error");
    logTemplate(std::cerr, tplt, x...);
}

template <typename... T>
void logDebug(std::string tplt, const T &...x) {
    printHead(std::cerr, "debug");
    logTemplate(std::cerr, tplt, x...);
}