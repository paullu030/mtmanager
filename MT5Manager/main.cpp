#include"crow/crow.h"
#include <iostream>
#include "sink.hpp"
#include "mt5_connector.hpp"
#include "managerpool.hpp"
#include <sstream>
#include <string>

ManagerPool *managerPool;

void createManagerDelar(crow::request req) {
    MTManager* manager = new MTManager();
    manager->Dealar(req);
}


auto health = []() {
    std::ostringstream os;
    os << "pong";
    return crow::response{ os.str() };
};

class CustomLogger : public crow::ILogHandler {
public:
    CustomLogger() {}
    void log(std::string message, crow::LogLevel /*level*/) {
        // "message" doesn't contain the timestamp and loglevel
        // prefix the default logger does and it doesn't end
        // in a newline.
        std::cerr << message << std::endl;
    }
};

int main()
{
    managerPool = new ManagerPool();
    CustomLogger logger;
    crow::logger::setHandler(&logger);
    crow::SimpleApp handler;
    
    CROW_ROUTE(handler, "/*")
        .websocket() 
        .onaccept([&](const crow::request& req) {
        return true; })
        .onopen([&](crow::websocket::connection& conn) {
            MTManager* manager = new MTManager();
            cout << conn.userdata();
            //manager->Subscribe(&conn, psd->server, psd->login, psd->password);
            })
        .onclose([&](crow::websocket::connection& conn, const std::string& reason) {
                conn.close();
                cout << "close"<<"reason"<<reason;
            });

    CROW_ROUTE(handler, "/")
                .methods("POST"_method)
                ([](const crow::request& req) {
                auto x = crow::json::load(req.body);
                if (!x)
                    return crow::response(crow::status::BAD_REQUEST);
                int sum = x["a"].i() + x["b"].i();
                std::ostringstream os;
                os << sum;
                return crow::response{ os.str() };
                    });

    CROW_ROUTE(handler, "/health").methods("GET"_method)(health);
    crow::logger::setLogLevel(crow::LogLevel::Info);

    CROW_ROUTE(handler, "/DealerSend")
        .methods("POST"_method)
        ([](const crow::request& req) {
        crow::response resp;
        createManagerDelar(req);
        return resp;
            });

    CROW_ROUTE(handler, "/UserGet")
        .methods("POST"_method)
        ([](const crow::request& req) {
        crow::response resp;
       
        return resp;
            });
            
    handler.port(8080).multithreaded().run();

}
