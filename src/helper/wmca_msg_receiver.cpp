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
            resJson[L"data"][L"accounts"][i] = web::json::value::object();
            ACCOUNTINFO* pAccountInfo = &(pLogin->pLoginInfo->accountlist[i]);
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
        //주식 현재가/종목정보	
        if (strcmp(pOutData->pData->szBlockName, "c1101OutBlock") == 0) {
            resJson[L"data"][L"information"] = web::json::value::object();
            Tc1101OutBlock* pc1101Outblock = (Tc1101OutBlock*)pOutData->pData->szData;
            // data.information.code: 종목코드
            CStringW code(SCOPY_A(pc1101Outblock->code));
            resJson[L"data"][L"information"][L"code"] = web::json::value::string(code.GetBuffer());
            // data.information.name: 종목명
            CStringW name(SCOPY_A(pc1101Outblock->hname).TrimRight());
            resJson[L"data"][L"information"][L"name"] = web::json::value::string(name.GetBuffer());
            // data.information.price: 현재가
            uint32_t price = strtoul(SCOPY_A(pc1101Outblock->price), nullptr, 10);
            resJson[L"data"][L"information"][L"price"] = web::json::value::number(price);
            // data.information.sign: 등락부호
            CStringW sign(SCOPY_A(pc1101Outblock->sign));
            resJson[L"data"][L"information"][L"sign"] = web::json::value::string(sign.GetBuffer());
            // data.information.change: 등락폭
            uint32_t change = strtoul(SCOPY_A(pc1101Outblock->change), nullptr, 10);
            resJson[L"data"][L"information"][L"change"] = web::json::value::number(change);
            // data.information.chrate: 등락률
            float chrate = strtof(SCOPY_A(pc1101Outblock->chrate), nullptr);
            resJson[L"data"][L"information"][L"chrate"] = web::json::value::number(chrate);
            // data.information.offer: 매도호가
            uint32_t offer = strtoul(SCOPY_A(pc1101Outblock->offer), nullptr, 10);
            resJson[L"data"][L"information"][L"offer"] = web::json::value::number(offer);
            // data.information.bid: 매수호가
            uint32_t bid = strtoul(SCOPY_A(pc1101Outblock->bid), nullptr, 10);
            resJson[L"data"][L"information"][L"bid"] = web::json::value::number(bid);
            // data.information.volume: 거래량
            uint32_t volume = strtoul(SCOPY_A(pc1101Outblock->volume), nullptr, 10);
            resJson[L"data"][L"information"][L"volume"] = web::json::value::number(volume);
            // data.information.volrate: 거래비율
            float volrate = strtof(SCOPY_A(pc1101Outblock->volrate), nullptr);
            resJson[L"data"][L"information"][L"volrate"] = web::json::value::number(volrate);
            // data.information.yurate: 유동주회전율
            float yurate = strtof(SCOPY_A(pc1101Outblock->yurate), nullptr);
            resJson[L"data"][L"information"][L"yurate"] = web::json::value::number(yurate);
            //////......
            // data.information.sosokz6: 코스피코스닥구분
            CStringW sosokz6(SCOPY_A(pc1101Outblock->sosokz6));
            resJson[L"data"][L"information"][L"sosokz6"] = web::json::value::string(sosokz6.GetBuffer());
            // data.information.jisunamez18: 업종명
            CStringW jisunamez18(SCOPY_A(pc1101Outblock->jisunamez18).TrimRight());
            resJson[L"data"][L"information"][L"jisunamez18"] = web::json::value::string(jisunamez18.GetBuffer());
        }

        //주식 현재가/변동거래량
        if (strcmp(pOutData->pData->szBlockName, "c1101OutBlock2") == 0) {
            resJson[L"data"][L"variableVolume"] = web::json::value::array();
            Tc1101OutBlock2* pc1101Outblock2 = (Tc1101OutBlock2*)pOutData->pData->szData;

            //실제 데이터에 따라 수신 데이터 행의 수가 가변적이므로
            //수신자료 크기를 구조체 크기로 나누어 몇 번 반복되는지 계산함
            int nOccursCount = pOutData->pData->nLen / sizeof(Tc1101OutBlock2);    //반복 회수를 구함

            for (int i = 0; i < nOccursCount; i++) { //회수만큼 반복하면서 데이터 구함
                // data.variableVolume.[].time: 시간
                CStringW time(SCOPY_A(pc1101Outblock2->time));
                resJson[L"data"][L"variableVolume"][i][L"time"] = web::json::value::string(time.GetBuffer());
                // data.variableVolume.[].price: 현재가
                uint32_t price = strtoul(SCOPY_A(pc1101Outblock2->price), nullptr, 10);
                resJson[L"data"][L"variableVolume"][i][L"price"] = web::json::value::number(price);
                // data.variableVolume.[].sign: 등락부호
                CStringW sign(SCOPY_A(pc1101Outblock2->sign));
                resJson[L"data"][L"variableVolume"][i][L"sign"] = web::json::value::string(sign.GetBuffer());
                // data.variableVolume.[].change: 등락폭
                uint32_t change = strtoul(SCOPY_A(pc1101Outblock2->change), nullptr, 10);
                resJson[L"data"][L"variableVolume"][i][L"change"] = web::json::value::number(change);
                // data.variableVolume.[].offer: 매도호가
                uint32_t offer = strtoul(SCOPY_A(pc1101Outblock2->offer), nullptr, 10);
                resJson[L"data"][L"variableVolume"][i][L"offer"] = web::json::value::number(offer);
                // data.variableVolume.[].bid: 매수호가
                uint32_t bid = strtoul(SCOPY_A(pc1101Outblock2->bid), nullptr, 10);
                resJson[L"data"][L"variableVolume"][i][L"bid"] = web::json::value::number(bid);
                // data.variableVolume.[].movolume: 변동거래량
                uint32_t movolume = strtoul(SCOPY_A(pc1101Outblock2->movolume), nullptr, 10);
                resJson[L"data"][L"variableVolume"][i][L"movolume"] = web::json::value::number(movolume);
                // data.variableVolume.[].volume: 거래량
                uint32_t volume = strtoul(SCOPY_A(pc1101Outblock2->volume), nullptr, 10);
                resJson[L"data"][L"variableVolume"][i][L"volume"] = web::json::value::number(volume);
     
                pc1101Outblock2++;    //다음행으로 포인터 이동
            }
        }

        //주식 현재가/동시호가	
        if (strcmp(pOutData->pData->szBlockName, "c1101OutBlock3") == 0) {
            resJson[L"data"][L"simultaneousQuote"] = web::json::value::object();
            Tc1101OutBlock3* pc1101Outblock3 = (Tc1101OutBlock3*)pOutData->pData->szData;
            // data.simultaneousQuote.dongsi: 동시호가구분
            CStringW dongsi(SCOPY_A(pc1101Outblock3->dongsi));
            resJson[L"data"][L"simultaneousQuote"][L"dongsi"] = web::json::value::string(dongsi.GetBuffer());
            // data.simultaneousQuote.jeqprice: 예상체결가
            uint32_t jeqprice = strtoul(SCOPY_A(pc1101Outblock3->jeqprice), nullptr, 10);
            resJson[L"data"][L"simultaneousQuote"][L"jeqprice"] = web::json::value::number(jeqprice);
            // data.simultaneousQuote.jeqsign: 예상체결부호
            CStringW jeqsign(SCOPY_A(pc1101Outblock3->jeqsign));
            resJson[L"data"][L"simultaneousQuote"][L"jeqsign"] = web::json::value::string(jeqsign.GetBuffer());
            // data.simultaneousQuote.jeqchange: 예상체결등락폭
            uint32_t jeqchange = strtoul(SCOPY_A(pc1101Outblock3->jeqchange), nullptr, 10);
            resJson[L"data"][L"simultaneousQuote"][L"jeqchange"] = web::json::value::number(jeqchange);
            // data.simultaneousQuote.jeqchrate: 예상체결등락률
            float jeqchrate = strtof(SCOPY_A(pc1101Outblock3->jeqchrate), nullptr);
            resJson[L"data"][L"simultaneousQuote"][L"jeqchrate"] = web::json::value::number(jeqchrate);
            // data.simultaneousQuote.jeqvol: 예상체결수량
            uint32_t jeqvol = strtoul(SCOPY_A(pc1101Outblock3->jeqvol), nullptr, 10);
            resJson[L"data"][L"simultaneousQuote"][L"jeqvol"] = web::json::value::number(jeqvol);
            // data.simultaneousQuote.chkdataz1: ECN정보유무구분
            CStringW chkdataz1(SCOPY_A(pc1101Outblock3->chkdataz1));
            resJson[L"data"][L"simultaneousQuote"][L"chkdataz1"] = web::json::value::string(chkdataz1.GetBuffer());
            // data.simultaneousQuote.ecnPrice: ECN전일종가
            uint32_t ecnPrice = strtoul(SCOPY_A(pc1101Outblock3->ecn_price), nullptr, 10);
            resJson[L"data"][L"simultaneousQuote"][L"ecnPrice"] = web::json::value::number(ecnPrice);
            // data.simultaneousQuote.ecnSign: ECN부호
            CStringW ecnSign(SCOPY_A(pc1101Outblock3->ecn_sign));
            resJson[L"data"][L"simultaneousQuote"][L"ecnSign"] = web::json::value::string(ecnSign.GetBuffer());
            // data.simultaneousQuote.ecnChange: ECN등락폭
            uint32_t ecnChange = strtoul(SCOPY_A(pc1101Outblock3->ecn_change), nullptr, 10);
            resJson[L"data"][L"simultaneousQuote"][L"ecnChange"] = web::json::value::number(ecnChange);
            // data.simultaneousQuote.ecnChrate: ECN등락률
            float ecnChrate = strtof(SCOPY_A(pc1101Outblock3->ecn_chrate), nullptr);
            resJson[L"data"][L"simultaneousQuote"][L"ecnChrate"] = web::json::value::number(ecnChrate);
            // data.simultaneousQuote.ecnVolume: ECN체결수량
            double ecnVolume = strtod(SCOPY_A(pc1101Outblock3->ecn_volume), nullptr);
            resJson[L"data"][L"simultaneousQuote"][L"ecnVolume"] = web::json::value::number(ecnVolume);
            // data.simultaneousQuote.ecnJeqsign: ECN대비예상체결부호
            CStringW ecnJeqsign(SCOPY_A(pc1101Outblock3->ecn_jeqsign));
            resJson[L"data"][L"simultaneousQuote"][L"ecnJeqsign"] = web::json::value::string(ecnJeqsign.GetBuffer());
            // data.simultaneousQuote.ecnJeqchange: ECN대비예상체결등락폭
            uint32_t ecnJeqchange = strtoul(SCOPY_A(pc1101Outblock3->ecn_jeqchange), nullptr, 10);
            resJson[L"data"][L"simultaneousQuote"][L"ecnJeqchange"] = web::json::value::number(ecnJeqchange);
            // data.simultaneousQuote.ecnJeqchrate: ECN대비예상체결등락률
            float ecnJeqchrate = strtof(SCOPY_A(pc1101Outblock3->ecn_jeqchrate), nullptr);
            resJson[L"data"][L"simultaneousQuote"][L"ecnJeqchrate"] = web::json::value::number(ecnJeqchrate);
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
    // TR 호출 완료시점에 처리하는 부분
    auto generateMessage = [this, pOutData]() {
        resJson[L"code"] = web::json::value::string(L"00000");

        switch (pOutData->TrIndex)
        {
        case TRID_c1101:
            // 주식 현재가 조회 완료
            resJson[L"message"] = web::json::value::string(L"An inquiry of current price completed.");
            break;
        case TRID_c8201:
            // 계좌 잔고 조회 완료
            resJson[L"message"] = web::json::value::string(L"An inquiry of Account balance completed.");
            break;
        }
    };

    processMessage(generateMessage);
}

void CWmcaMsgReceiver::ReceiveError(OUTDATABLOCK* pError)
{

}

void CWmcaMsgReceiver::ConnectedStatus(BOOL isConnected)
{
    auto generateMessage = [this, isConnected]() {
        resJson[L"code"] = web::json::value::string(L"00000");
        resJson[L"message"] = web::json::value::string(L"It checked the status.");
        resJson[L"data"] = web::json::value::object();
        resJson[L"data"][L"status"] = web::json::value::boolean(isConnected != FALSE);
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
}