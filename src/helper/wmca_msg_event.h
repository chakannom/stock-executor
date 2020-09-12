#pragma once

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
};

