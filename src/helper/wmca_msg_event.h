#pragma once

#include <cpprest/json.h>

class CWmcaMsgEvent
{
public:
    void OnWmConnected(LOGINBLOCK* pLogin);
    void OnWmDisconnected();
    void OnWmSocketError(int socketErrorCode);
    void OnWmReceiveData(OUTDATABLOCK* pOutData);
    void OnWmReceiveSise(OUTDATABLOCK* pSiseData);
    void OnWmReceiveMessage(OUTDATABLOCK* pMessage);
    void OnWmReceiveComplete(OUTDATABLOCK* pOutData);
    void OnWmReceiveError(OUTDATABLOCK* pError);
private:
    web::json::value resJson = web::json::value::object();
    bool onlyDisconnect = true;

    void processMessage(std::function<void()> generateMessage);
    void clearResponseJson();
};

