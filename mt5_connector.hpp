#include <MT5APIManager.h>
#include "dealer_sink.hpp"
#include "util.hpp"


#define MT5_TIMEOUT_CONNECT   30000
#define MT5_TIMEOUT_DEALER    10000


class MTManager
{
private:
    bool isIdle;
    string id;
    IMTManagerAPI* manager = NULL;
    CMTManagerAPIFactory factory;
    MTAPIRES res = MT_RET_OK_NONE;
    UINT version = 0;
    string server;
    int login;
    string password;

    ManagerSink* managerSink;
    RequestSink* requestSink;
 public:
    MTManager()
    {
        if ((res = factory.Initialize(NULL)) != MT_RET_OK) {
            printf("Loading manager API failed (%u)\n", res);
            return;
        }
        if ((res = factory.Version(version)) != MT_RET_OK) {
            printf("Getting API version failed (%u)\n", res);
            return;
        }
        if (version < MTManagerAPIVersion) {
            printf("Wrong Manager API version %u, version %u required\n", version, MTManagerAPIVersion);
            return;
        }
        if ((res = factory.CreateManager(MTManagerAPIVersion, &manager)) != MT_RET_OK) {
            printf("Creating manager interface failed (%u)\n", res);
            return;
        }
    }
    ~MTManager()
    {
        if (manager != NULL)
        {
            manager->Disconnect();
            manager->Release();
            manager = NULL;
        }
        factory.Shutdown();
    }

    bool IsValid(){
        return(manager != NULL);
    }

    bool IsIdle(){
        return isIdle;
    }
    void Open() {
        isIdle = false;
    }
    void Close(){
        isIdle = true;
    }

    MTAPIRES Connect(string server, int login, string password , UINT64 pump_mode) {
        this->server = server;
        this->login = login;
        this->password = password;
        res= manager->Connect(stringToLPCWSTR(server), login, stringToLPCWSTR(password), L"", pump_mode, MT5_TIMEOUT_CONNECT);
        
        return res;
    }

    void DisConnect() {
        manager->Disconnect();
    }

    int Dealar(crow::request crowreq) {

        auto dealerRequest = crow::json::load(crowreq.body);
        if (!dealerRequest.has("server") || !dealerRequest.has("source_login") || !dealerRequest.has("password")) {
            cout << " | 400 | DealerSend - server login password should not be empty: " << endl;
            return -1;
        }
        string server = dealerRequest["server"].s();
        string password = dealerRequest["password"].s();
        int source_login = dealerRequest["source_login"].i();



        res = manager->Connect(stringToLPCWSTR(server), source_login, stringToLPCWSTR(password), L"", 0, MT5_TIMEOUT_CONNECT);
        //res = manager->Connect(L"20.48.6.157:443", 1006, L"Otso@1234", L"", 0, MT5_TIMEOUT_CONNECT);
        if (res != MT_RET_OK) {
            wprintf_s(L"Dealer Connection failed (%u)\n", res);
            manager = NULL;
            return -1;
        }
        IMTRequest* mtDealaeRequest = manager->RequestCreate();
        if (mtDealaeRequest == NULL) {
            cout <<"| 400 | DealerSend - requestArr is NULL" << endl;
            return -1;
        }

        if (!dealerRequest) {
            return -1;
        }

        if (dealerRequest.has("type")) {
            string orderType = dealerRequest["type"].s();
            if (orderType != "") {
                int iType = stoi(orderType);
                if (iType < IMTOrder::OP_FIRST || iType > IMTOrder::OP_LAST) {
                    cout << " | 400 | DealerSend - type error, type: " << iType << endl;
                    return -1;
                }
                mtDealaeRequest->Type(iType);
            }
        }
        if (dealerRequest.has("type")) {
            string orderType = dealerRequest["type"].s();
            if (orderType != "") {
                int iType = stoi(orderType);
                if (iType < IMTOrder::OP_FIRST || iType > IMTOrder::OP_LAST) {
                    cout << " | 400 | DealerSend - type error, type: " << iType << endl;
                    return -1;
                }
                mtDealaeRequest->Type(iType);
            }
        }
        if (dealerRequest.has("symbol")) {
            string symbol = dealerRequest["symbol"].s();
            if (symbol != "") { mtDealaeRequest->Symbol(stringToLPCWSTR(symbol)); }
        }

        if (dealerRequest.has("login")) {
            string login = dealerRequest["login"].s();
            if (login != "") { mtDealaeRequest->Login(stoi(login)); }
        }
        if (dealerRequest.has("position")) {
            string position = dealerRequest["position"].s();
            if (position != "") { mtDealaeRequest->Position(stoi(position)); }
        }

        if (dealerRequest.has("position_by")) {
            string positionBy = dealerRequest["position_by"].s();
            if (positionBy != "") { mtDealaeRequest->PositionBy(stoi(positionBy)); }
        }
        if (dealerRequest.has("source_login")) {
            string sourceLogin = dealerRequest["source_login"].s();
            if (sourceLogin != "") { mtDealaeRequest->SourceLogin(stoi(sourceLogin)); }
        }

        if (dealerRequest.has("action")) {
            string action = dealerRequest["action"].s();
            if (action != "") {
                int iAction = stoi(action);
                if (iAction < IMTRequest::TA_FIRST || iAction > IMTRequest::TA_LAST) {
                    cout << " | 400 | DealerSend - action error, action: " << iAction << endl;
                    return -1;
                }
                mtDealaeRequest->Action(iAction);
            }
        }
        if (dealerRequest.has("volume")) {
            string volume = dealerRequest["volume"].s();
            if (volume != "") {
                int iVolume = stoi(volume);
                if (iVolume < 100 || iVolume > MTAPI_VOLUME_MAX) {
                    cout << " | 400 | DealerSend - action error, action: " << iVolume << endl;
                    return -1;
                }
                mtDealaeRequest->Volume(iVolume);
            }
        }
        if (dealerRequest.has("price_order")) {
            string priceOrder = dealerRequest["price_order"].s();
            if (priceOrder != "") {
                mtDealaeRequest->PriceOrder(stod(priceOrder));
            }
        }
        else if (dealerRequest.has("price")) {
                string price = dealerRequest["price"].s();
                if (price != "") {
                    mtDealaeRequest->PriceOrder(stod(price));
                }
        }

        //res = manager->Connect(L"20.48.6.157:443", 1006, L"Otso@1234", L"", 0, MT5_TIMEOUT_CONNECT);

        res = manager->Connect(stringToLPCWSTR(server), source_login, stringToLPCWSTR(password), L"", 0, MT5_TIMEOUT_CONNECT);
        if (res != MT_RET_OK) {
            wprintf_s(L"Dealar Connection failed (%u)\n", res);
            manager = NULL;
            return -1;
        }

        UINT id = 0;
        IMTRequest* result = manager->RequestCreate();
        CDealerSink sink;
        
        if (mtDealaeRequest && result && sink.Initialize(result)) {
            res = manager->DealerSend(mtDealaeRequest, &sink, id);
            if (res == MT_RET_OK) {
                res = sink.Wait(MT5_TIMEOUT_DEALER);
            }

            if (res != MT_RET_REQUEST_DONE) {
                cout  << mtDealaeRequest->Login() << ": order login " << result->ResultOrder() << " failed. res: " << res << endl;
                string loginOrOrder = mtDealaeRequest->Action() == IMTRequest::TA_DEALER_POS_EXECUTE ? to_string(mtDealaeRequest->Login()) : to_string(result->Position());
                cout << loginOrOrder;
            }
        }
        else {
            if (mtDealaeRequest) {
                cout  << mtDealaeRequest->Login() << " init order login  failed." << endl;
            }
        }
        result->Release();
        result = NULL;
        return 0;
    }

    void Subscribe(crow::websocket::connection *conn,string server,int login,string password) {
        managerSink = new ManagerSink();
        requestSink = new RequestSink(conn);

        res = manager->Subscribe(managerSink);
        if (res != MT_RET_OK) {
            manager->Unsubscribe(managerSink);
            manager = NULL;
            conn->send_text("err");
            return;
        }
       cout<<stringToLPCWSTR(server)<<login<<stringToLPCWSTR(password)<<endl;

        res =manager->Connect(stringToLPCWSTR(server), login, stringToLPCWSTR(password), L"", IMTManagerAPI::PUMP_MODE_REQUESTS, MT5_TIMEOUT_CONNECT);
        //res = manager->Connect(L"20.48.6.157:443", 1006, L"Otso@1234", L"", IMTManagerAPI::PUMP_MODE_REQUESTS, MT5_TIMEOUT_CONNECT);
        if (res != MT_RET_OK) {
            wprintf_s(L"Connection failed (%u)\n", res);
            manager->Unsubscribe(managerSink);
            manager = NULL;
            conn->send_text("err");
            return;
        }
        cout << "connect success";

        res =manager->RequestSubscribe(requestSink);
        if (res != MT_RET_OK) {
           manager->RequestUnsubscribe(requestSink);
           manager->Unsubscribe(managerSink);
           
            manager = NULL;
            conn->send_text("err");
            conn->close();
            return;
        }

    }
    void WebSocketClose() {
        manager->RequestUnsubscribe(this->requestSink);
        manager->Unsubscribe(this->managerSink);
    }
    int GetUserData(int login,crow::response *resp) {

        IMTUser* user = manager->UserCreate();

        res = manager->Connect(L"20.48.6.157:443", 1006, L"Otso@1234", L"", 0, MT5_TIMEOUT_CONNECT);
        if (res != MT_RET_OK) {
            cout << "connect failed";
            return -1;
        }

        MTAPIRES mtRes = manager->UserRequest(login, user);
        if (mtRes != MT_RET_OK) {
            return -1;
        }
        stringstream ss;
        
        //ss << "{\"type\":\"request_delete\",\"data\":" << RequestToJSON(request) << "}";
       ss <<"{\"login\": "<< user->Login()<<","
           <<"\"balance\": " << user->Balance() 
           <<"}";


       // 
         //crow::json::wvalue respData;
        // respData["login"] = user->Login();
       //  respData["balance"] = user->Balance();
            
         
         resp->body.append(ss.str());

         user->Release();
         user = NULL;
         manager->Disconnect();
    }
};

struct PerSocketData {
    string path;
    string server;
    int login;
    string password;
    int minutes;
    MTManager* manager;
    ManagerSink* otsoManagerSink;
    RequestSink* otsoRequestSink;
};
