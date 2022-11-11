#include <string>
#include <map>
using namespace std;


class ManagerPool {
private :
    string host;
    int maxOpenCount=10;
    map<string, MTManager*> connsMap;
    mutex mu;

    MTManager* get(string server, int login, string password,UINT64 mode)
    {
        lock_guard<mutex> mLock(mu);
        string key;
        key.append(server).append(to_string(login));
        int i;
        for (i = 0; i < maxOpenCount; i++)
        {
            string keyWithIndex;
            keyWithIndex.append(key).append(to_string(i));
            map<string, MTManager*>::iterator iter = connsMap.find(keyWithIndex);
            if (iter != connsMap.end())
            {
                if (!(*(iter->second)).IsIdle()) continue;
                iter->second->Open();
                return iter->second;
            }
            break;
        }

        if (i >= maxOpenCount){
            printf("%s %d: connection reached limit(%d).\n", server.c_str(), login, maxOpenCount);
            return NULL;
        }
        
        MTAPIRES res = MT_RET_ERROR;
        MTManager* manager = new MTManager;
        res = manager->Connect(server, login, password, mode);
        if (res != MT_RET_OK)
        {
            cout  << " Connection failed - " << server << ": " << login << " | " << password << " res: " << res << endl;
            delete manager;
            manager = NULL;
            return NULL;
        }
        for (int i = 0; i < maxOpenCount; i++) 
        {
            string keyWithIndex;
            keyWithIndex.append(key).append(to_string(i));
            map<string, MTManager*>::iterator iter = connsMap.find(keyWithIndex);
            if (iter == connsMap.end()) {
                connsMap[keyWithIndex] = manager;
                (*manager).Open();
                return manager;
            }
        }
        delete manager;
        manager = NULL; 
        return NULL;
    }

    void freeConn(string server, int login) {
        string key;
        key.append(server).append(to_string(login));
        int i;
        for (i = 0; i < maxOpenCount; i++){
            string keyWithIndex;
            keyWithIndex.append(key).append(to_string(i));
            map<string, MTManager*>::iterator iter = connsMap.find(keyWithIndex);
            if (iter != connsMap.end()){
                delete iter->second;
                connsMap.erase(iter);
            }
        }
    }
    void freeAll() {

    }


public :
    ManagerPool(){}

    ~ManagerPool()
    {
    }

    MTManager* Get(string server, int login, string password){
        return get(server, login, password,0);
    }

    void FreeConn(string server, int login){
        return freeConn(server, login);
    }
    void FreeAll() {
        return freeAll();
    }
};