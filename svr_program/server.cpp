#include "include/httplib.h"
#include "include/json.hpp"

int main() {
    using namespace httplib;
    using Json = nlohmann::json;
    Server svr;
    svr.Post("/compile", [&](const Request &req, Response &res) {
        auto val = req.get_param_value("code");
        std::cerr << val << '\n';
        std::ofstream user_code("in.pas");
        user_code << val;
        user_code.close();
        system("./p2c in.pas -o out.c");
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(1000ms);
        std::ifstream info("info.json");
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Cache-Control", "no-cache");
        res.set_content(std::string((std::istreambuf_iterator<char>(info)),
                                    (std::istreambuf_iterator<char>())),
                        "application/json");
    });
    svr.listen("0.0.0.0", 8848);
}