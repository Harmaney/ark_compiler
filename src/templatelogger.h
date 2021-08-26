#include <iostream>
#include <string>

#ifndef _CPP_LOG
#define _CPP_LOG

void logTemplate(std::ostream &out, std::string tplt);
template <typename P, typename... T>
void logTemplate(std::ostream &out, std::string tplt, const P &x);

template <typename P, typename... T>
void logTemplate(std::ostream &out, std::string tplt, const P &x,
                 const T &...y);


void printHead(std::ostream &out, std::string level);


template <typename... T>
void logInfo(std::string tplt, const T &...x);
template <typename... T>
void logWarn(std::string tplt, const T &...x);
template <typename... T>
void logError(std::string tplt, const T &...x);
template <typename... T>
void logDebug(std::string tplt, const T &...x);

class CustomLogger {
   private:
    std::ostream &out;

   public:
    CustomLogger(std::ostream &out) : out(out) {}
    template <typename... T>
    void info(std::string tplt, const T &...x);
    template <typename... T>
    void warn(std::string tplt, const T &...x);
    template <typename... T>
    void error(std::string tplt, const T &...x);
    template <typename... T>
    void debug(std::string tplt, const T &...x);
    template <typename... T>
    void raw(std::string tplt, const T &...x);
};

#endif