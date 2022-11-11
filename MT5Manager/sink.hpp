#include <mt5manapi/Include/MT5APIManager.h>

using namespace std;

class ManagerSink : public IMTManagerSink {
public:
    void OnConnect() {

    }
    void OnDisconnect() {

    }
};

class RequestSink : public IMTRequestSink {
private:
    crow::websocket::connection* conn;

public:
    RequestSink(crow::websocket::connection* conn) {
        this->conn = conn;
    }
    ~RequestSink() {
        this->conn->close();
    }

    void OnRequestAdd(const IMTRequest* request) {}
    void OnRequestUpdate(const IMTRequest* request) {
        if (!this->conn) { return; }
        stringstream ss;
       // ss << "{\"type\":\"request_update\",\"data\":" << RequestToJSON(request) << "}";
        ss << "{\"type\":\"request_update\",\"data\":" << "}";

        this->conn->send_text(ss.str());
    }
    void OnRequestDelete(const IMTRequest* request) {
        cout << "delete";
        if (!this->conn) { return; }
        int action = request->Action();
        // TA_ACTIVATE_SL, TA_ACTIVATE_TP
        if (action == 101 || action == 102) { return; }
        stringstream ss;
        //ss << "{\"type\":\"request_delete\",\"data\":" << RequestToJSON(request) << "}";
        ss << "{\"type\":\"request_delete\",\"data\":" << "}";

        this->conn->send_text(ss.str());
    }

    void SendMessage(string message) {
        this->conn->send_text(message);
    }

};