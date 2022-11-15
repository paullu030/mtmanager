#include <crow/crow.h>
#include "sink.hpp"
#include "mt5_connector.hpp"
#include "managerpool.hpp"
#include <sstream>
#include <string>
#include <unordered_set>

ManagerPool* managerPool;

void createManagerDelar(crow::request req, crow::response* resp) {
    MTManager* manager = new MTManager();
    int res = manager->Dealar(req);
    if (res != 0) {
        resp->code = res;
    }
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
std::mutex mtx;;
std::unordered_set<crow::websocket::connection*> users;
int main()
{
    managerPool = new ManagerPool();
    CustomLogger logger;
    crow::logger::setHandler(&logger);
    crow::SimpleApp handler;

    struct SubscribeData {
        MTManager* manager;
        string server;
        int login;
        string password;
    };
    crow::logger::setLogLevel(crow::LogLevel::Info);

    CROW_ROUTE(handler, "/ws")
        .websocket()
        .onaccept([&](const crow::request& req, void** ud) {
        SubscribeData* data = new SubscribeData;
        MTManager* manager = new MTManager();
        data->manager = manager;
        data->login = stoi(req.url_params.get("login"));
        data->server = req.url_params.get("server");
        data->password = req.url_params.get("password");
        *ud = data;
        return true; })
        .onopen([&](crow::websocket::connection& conn) {
            std::lock_guard<std::mutex> _(mtx);
            users.insert(&conn);
            SubscribeData* sd = (SubscribeData*)conn.userdata();

            sd->manager->Subscribe(&conn, sd->server, sd->login, sd->password);
            })
            .onclose([&](crow::websocket::connection& conn, const std::string& reason) {
                SubscribeData* sd = (SubscribeData*)conn.userdata();
                sd->manager->Close();
                std::lock_guard<std::mutex> _(mtx);
                users.erase(&conn);
                cout << "close" << "reason:" << reason << endl;
                });
            /* example
    CROW_ROUTE(handler, "/")
                .methods("POST"_method)
                ([](const crow::request& req) {
                auto x = crow::json::load(req.body);
                if (!x)
                    return crow::response(crow::status::BAD_REQUEST);
                //int sum = x["a"].i() + x["b"].i();
                std::ostringstream os;
                os << sum;
                return crow::response{ os.str() };
                    });
             */

            CROW_ROUTE(handler, "/health").methods("GET"_method)(health);

            CROW_ROUTE(handler, "/DealerSend")
                .methods("POST"_method)
                ([](const crow::request& req) {
                crow::response resp;
                createManagerDelar(req, &resp);
                return resp;
                    });

            CROW_ROUTE(handler, "/UserGet/<int>")
                .methods("GET"_method)
                ([](int login) {
                MTManager* manager = new MTManager();
                crow::response resp;
                manager->GetUserData(login, &resp);
                return resp;
                    });

            handler.port(8080).multithreaded().run();

}
