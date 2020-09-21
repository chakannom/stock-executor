#define UNICODE
#include "core/framework.h"
#include "util/string_util.h"
#include "trio_def.h"
#include "trio_inv.h"
#include "wmca_intf.h"
#include "wmca_msg_receiver.h"

void CWmcaMsgReceiver::Connected(LOGINBLOCK* pLogin)
{
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
            // data.accounts.name: 계좌명
            CStringW accountName(SCOPY_A(pAccountInfo->szAccountName).TrimRight());
            resJson[L"data"][L"accounts"][i][L"name"] = web::json::value::string(accountName.GetBuffer());
            // data.accounts.name: 계좌번호
            CStringW accountNo(SCOPY_A(pAccountInfo->szAccountNo).TrimRight());
            resJson[L"data"][L"accounts"][i][L"number"] = web::json::value::string(accountNo.GetBuffer());
        }
    };

    processMessage(generateMessage);
}

void CWmcaMsgReceiver::Disconnected()
{
    auto generateMessage = [this]() {
        if (onlyDisconnect) {
            resJson[L"code"] = web::json::value::string(L"00000");
            resJson[L"message"] = web::json::value::string(L"Disconnected.");
        }
    };

    processMessage(generateMessage);
    onlyDisconnect = true;
}

void CWmcaMsgReceiver::SocketError(int socketErrorCode)
{

}

void CWmcaMsgReceiver::ReceiveData(OUTDATABLOCK* pOutData)
{
    switch (pOutData->TrIndex) {
    case TRID_c1101:
        //////////////////////////////////////////////////////////////////////////
        //반복되지 않는 단순출력 처리 방식
        //////////////////////////////////////////////////////////////////////////

        if (strcmp(pOutData->pData->szBlockName, "c1101OutBlock") == 0) {
            // Information // 주식현재가조회 - 현재가
            Tc1101OutBlock* pc1101Outblock = (Tc1101OutBlock*)pOutData->pData->szData;

            CStringW code(SCOPY_A(pc1101Outblock->code));
            CStringW hname(SCOPY_A(pc1101Outblock->hname));
            CStringW sosokz6(SCOPY_A(pc1101Outblock->sosokz6));
            CStringW jisunamez18(SCOPY_A(pc1101Outblock->jisunamez18));
            CStringW price(COMMA_A(pc1101Outblock->price));
        }
        //////////////////////////////////////////////////////////////////////////
        //반복가능한 출력에 대한 처리 방식
        //////////////////////////////////////////////////////////////////////////

        //주식 현재가/변동거래량자료	
        if (strcmp(pOutData->pData->szBlockName, "c1101OutBlock2") == 0) {
            // 주식현재가조회 - 변동거래량
            Tc1101OutBlock2* pc1101Outblock2 = (Tc1101OutBlock2*)pOutData->pData->szData;

            //실제 데이터에 따라 수신 데이터 행의 수가 가변적이므로
            //수신자료 크기를 구조체 크기로 나누어 몇 번 반복되는지 계산함
            int nOccursCount = pOutData->pData->nLen / sizeof(Tc1101OutBlock2);    //반복 회수를 구함

            for (int i = 0; i < nOccursCount; i++) { //회수만큼 반복하면서 데이터 구함
                CStringW time(SCOPY_A(pc1101Outblock2->time));
                CStringW price(COMMA_A(pc1101Outblock2->price));

                pc1101Outblock2++;    //다음행으로 포인터 이동
            }
        }
        break;
    }
}

void CWmcaMsgReceiver::ReceiveSise(OUTDATABLOCK* pSiseData)
{

}

void CWmcaMsgReceiver::ReceiveMessage(OUTDATABLOCK* pMessage)
{
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

void CWmcaMsgReceiver::ReceiveComplete(OUTDATABLOCK* pOutData) 
{
    auto generateMessage = [this]() {
    };

    processMessage(generateMessage);
}

void CWmcaMsgReceiver::ReceiveError(OUTDATABLOCK* pError)
{

}

void CWmcaMsgReceiver::ConnectedStatus(BOOL isConnected)
{
    auto generateMessage = [this, isConnected]() {
        resJson[L"status"] = web::json::value::boolean(isConnected != FALSE);
    };

    processMessage(generateMessage);
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

void CWmcaMsgReceiver::clearResponseJson()
{
    if (resJson.has_field(L"code")) {
        resJson.erase(L"code");
    }
    if (resJson.has_field(L"message")) {
        resJson.erase(L"message");
    }
    if (resJson.has_field(L"data")) {
        resJson.erase(L"data");
    }
    if (resJson.has_field(L"status")) {
        resJson.erase(L"status");
    }
}