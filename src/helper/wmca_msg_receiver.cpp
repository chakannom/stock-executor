#define UNICODE
#include "core/framework.h"
#include "wmca_intf.h"
#include "wmca_msg_receiver.h"

#pragma warning(disable: 4996)

//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//	구조체 필드값을 문자열 형태로 변환하는 유틸리티 함수입니다.
//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
CStringA _scopy_a(const char* szData, int nSize)
{
    char szBuf[512]; //필드의 최대 크기는 상황에 따라 조절할 필요가 있음
    memset(szBuf, 0, sizeof szBuf);
    strncpy(szBuf, szData, nSize);

    return szBuf;
}

#define SCOPY_A(x) _scopy_a(x, sizeof x)


void CWmcaMsgReceiver::Connected(LOGINBLOCK* pLogin) {
    auto generateMessage = [this, pLogin]() {
        resJson[L"code"] = web::json::value::string(L"00000");
        resJson[L"message"] = web::json::value::string(L"Connected.");
        resJson[L"data"] = web::json::value::object();

        // data.connectedDate: 접속시간
        uint64_t connectedDate = strtoull(SCOPY_A(pLogin->pLoginInfo->szDate), nullptr, 10);
        resJson[L"data"][L"connectedDate"] = web::json::value::number(connectedDate);

        // data.username: 접속자ID
        CStringW username(SCOPY_A(pLogin->pLoginInfo->szUserID).TrimRight());
        resJson[L"data"][L"username"] = web::json::value::string(username.GetBuffer());

        // data.accounts: 계좌목록
        resJson[L"data"][L"accounts"] = web::json::value::array();
        int	nAccountCount = atoi(SCOPY_A(pLogin->pLoginInfo->szAccountCount));
        for (int i = 0; i < nAccountCount; i++) {
            ACCOUNTINFO* pAccountInfo = &(pLogin->pLoginInfo->accountlist[i]);
            resJson[L"data"][L"accounts"][i] = web::json::value::object();
            CStringW accountName(SCOPY_A(pAccountInfo->szAccountName).TrimRight());
            resJson[L"data"][L"accounts"][i][L"name"] = web::json::value::string(accountName.GetBuffer());
            CStringW accountNo(SCOPY_A(pAccountInfo->szAccountNo).TrimRight());
            resJson[L"data"][L"accounts"][i][L"number"] = web::json::value::string(accountNo.GetBuffer());
        }
    };

    processMessage(generateMessage);
}

void CWmcaMsgReceiver::Disconnected() {
    auto generateMessage = [this]() {
        if (onlyDisconnect) {
            resJson[L"code"] = web::json::value::string(L"00000");
            resJson[L"message"] = web::json::value::string(L"Disconnected.");
        }
    };

    processMessage(generateMessage);
    onlyDisconnect = true;
}

void CWmcaMsgReceiver::SocketError(int socketErrorCode) {

}

void CWmcaMsgReceiver::ReceiveData(OUTDATABLOCK* pOutData) {

}

void CWmcaMsgReceiver::ReceiveSise(OUTDATABLOCK* pSiseData) {

}

void CWmcaMsgReceiver::ReceiveMessage(OUTDATABLOCK* pMessage) {
    // 현재 상태를 문자열 형태로 출력함
    MSGHEADER* pMsgHeader = (MSGHEADER*)pMessage->pData->szData;
    CStringA code = SCOPY_A(pMsgHeader->msg_cd);
    if (code.Compare("00001") == 0) {
        resJson[L"code"] = web::json::value::string(L"00001");
        resJson[L"message"] = web::json::value::string(L"The id and password are incorrect.");
        onlyDisconnect = false;
    }
    else if (code.Compare("90002") == 0) {
        resJson[L"code"] = web::json::value::string(L"90002");
        resJson[L"message"] = web::json::value::string(L"The certificate password is incorrect.");
        onlyDisconnect = false;
    }
}

void CWmcaMsgReceiver::ReceiveComplete(OUTDATABLOCK* pOutData) {

}

void CWmcaMsgReceiver::ReceiveError(OUTDATABLOCK* pError) {

}

void CWmcaMsgReceiver::processMessage(std::function<void()> generateMessage)
{

    //HWND hWnd = FindWindow(0, L"STOCK-AGENT_STOCK-SERVICE-HELPER");
    //if (hWnd == NULL) return;

    generateMessage();

    std::wstring jsonString = resJson.serialize();
    //COPYDATASTRUCT cds;
    //cds.dwData = WM_USER + 2001;
    //cds.cbData = jsonString.size() * sizeof(wchar_t);
    //cds.lpData = (PVOID)jsonString.c_str();
    //SendMessage(hWnd, WM_COPYDATA, 0, (LPARAM)&cds);

    clearResponseJson();
}

void CWmcaMsgReceiver::clearResponseJson() {
    if (resJson.has_field(L"code")) {
        resJson.erase(L"code");
    }
    if (resJson.has_field(L"message")) {
        resJson.erase(L"message");
    }
    if (resJson.has_field(L"data")) {
        resJson.erase(L"data");
    }
}