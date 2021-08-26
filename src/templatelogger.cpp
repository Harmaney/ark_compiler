#include "templatelogger.h"

void logTemplate(std::ostream &out, std::string tplt) {
    out << tplt << std::endl;
}

template <typename P, typename... T>
void logTemplate(std::ostream &out, std::string tplt, const P &x) {
    for (auto chr : tplt) {
        if (chr == '#')
            out << x;
        else
            out << chr;
    }
    std::cout << std::endl;
}

template <typename P, typename... T>
void logTemplate(std::ostream &out, std::string tplt, const P &x,
                 const T &...y) {
    if (tplt.empty()) std::cout << std::endl;
    if (tplt.front() == '#') {
        out << x;
        logTemplate(out, tplt.substr(1), x, y...);
    } else {
        out << tplt.front();
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

template <typename... T>
void CustomLogger::info(std::string tplt, const T &...x) {
    printHead(out, "info");
    logTemplate(out, tplt, x...);
}
template <typename... T>
void CustomLogger::warn(std::string tplt, const T &...x) {
    printHead(out, "warn");
    logTemplate(out, tplt, x...);
}
template <typename... T>
void CustomLogger::error(std::string tplt, const T &...x) {
    printHead(out, "error");
    logTemplate(out, tplt, x...);
}
template <typename... T>
void CustomLogger::debug(std::string tplt, const T &...x) {
    printHead(out, "debug");
    logTemplate(out, tplt, x...);
}

template <typename... T>
void CustomLogger::raw(std::string tplt, const T &...x) {
    logTemplate(out, tplt, x...);
}