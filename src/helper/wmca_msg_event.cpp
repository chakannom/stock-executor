#include "core/framework.h"
#include "util/string_util.h"
#include "wmca_intf.h"
#include "wmca_msg_event.h"

//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//	구조체 필드값을 문자열 형태로 변환하는 유틸리티 함수입니다.
//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
std::string _scopy(const char* szData, int nSize)
{
    char szBuf[512]; //필드의 최대 크기는 상황에 따라 조절할 필요가 있음
    memset(szBuf, 0, sizeof szBuf);
    strncpy_s(szBuf, szData, nSize);

    return std::string(szBuf);
}

#define SCOPY(x) _scopy(x,sizeof x)

void CWmcaMsgEvent::OnWmConnected(LOGINBLOCK* pLogin) {
    HWND hWnd = FindWindow(0, L"STOCK-AGENT_STOCK-SERVICE-HELPER");
    if (hWnd == NULL) return;

    responseJson[L"code"] = web::json::value::string(L"00000");
    responseJson[L"message"] = web::json::value::string(L"Connected.");
    std::wstring jsonString = responseJson.serialize();

    COPYDATASTRUCT cds;
    cds.dwData = WM_USER + 2001;
    cds.cbData = jsonString.size() * sizeof(wchar_t);
    cds.lpData = (PVOID)jsonString.c_str();
    ::SendMessage(hWnd, WM_COPYDATA, 0, (LPARAM)&cds);

    clearResponseJson();
}

void CWmcaMsgEvent::OnWmDisconnected() {
     HWND hWnd = FindWindow(0, L"STOCK-AGENT_STOCK-SERVICE-HELPER");
     if (hWnd == NULL) return;

    if (onlyDisconnect) {
        responseJson[L"code"] = web::json::value::string(L"00000");
        responseJson[L"message"] = web::json::value::string(L"Disconnected.");
    }
    std::wstring jsonString = responseJson.serialize();

    COPYDATASTRUCT cds;
    cds.dwData = WM_USER + 2001;
    cds.cbData = jsonString.size() * sizeof(wchar_t);
    cds.lpData = (PVOID)jsonString.c_str();
    ::SendMessage(hWnd, WM_COPYDATA, 0, (LPARAM)&cds);

    clearResponseJson();
    onlyDisconnect = true;
}

void CWmcaMsgEvent::OnWmSocketError(int socketErrorCode) {

}

void CWmcaMsgEvent::OnWmReceiveData(OUTDATABLOCK* pOutData) {

}

void CWmcaMsgEvent::OnWmReceiveSise(OUTDATABLOCK* pSiseData) {

}

void CWmcaMsgEvent::OnWmReceiveMessage(OUTDATABLOCK* pMessage) {
    //현재 상태를 문자열 형태로 출력함
    MSGHEADER* pMsgHeader = (MSGHEADER*)pMessage->pData->szData;
    std::wstring code = cks::StringUtil::toWString(SCOPY(pMsgHeader->msg_cd));
    if (code._Equal(L"00001")) {
        responseJson[L"code"] = web::json::value::string(L"00001");
        responseJson[L"message"] = web::json::value::string(L"The id and password are incorrect.");
        onlyDisconnect = false;
    }
    else if (code._Equal(L"90002")) {
        responseJson[L"code"] = web::json::value::string(L"90002");
        responseJson[L"message"] = web::json::value::string(L"The certificate password is incorrect.");
        onlyDisconnect = false;
    }
    else {
        responseJson[L"code"] = web::json::value::string(code);
        std::wstring message = cks::StringUtil::toWString(std::string(pMsgHeader->user_msg));
        TRACE(L"U have to work!! [%10d]  %5s : %s", pMessage->TrIndex, code.c_str(), message.c_str());
    }
}

void CWmcaMsgEvent::OnWmReceiveComplete(OUTDATABLOCK* pOutData) {

}

void CWmcaMsgEvent::OnWmReceiveError(OUTDATABLOCK* pError) {

}

void CWmcaMsgEvent::clearResponseJson() {
    responseJson.erase(L"code");
    responseJson.erase(L"message");
}