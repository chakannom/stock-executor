#include <cpprest/json.h>

#include "wmca_intf.h"
#include "wmca_msg_event.h"

void CWmcaMsgEvent::OnWmConnected(LOGINBLOCK* pLogin) {
	HWND hWnd = FindWindow(0, L"STOCK-AGENT_STOCK-SERVICE-HELPER");
	if (hWnd == NULL) return;

	web::json::value cResponseJson = web::json::value::object();
	cResponseJson[L"message"] = web::json::value::string(L"OnWmConnected아이디");
	cResponseJson[L"code"] = web::json::value::string(L"OnWmConnected");
	std::wstring jsonString = cResponseJson.serialize();

	COPYDATASTRUCT cds;
	cds.dwData = WM_USER + 2001;
	cds.cbData = jsonString.size() * sizeof(wchar_t);
	cds.lpData = (PVOID)jsonString.c_str();
	SendMessage(hWnd, WM_COPYDATA, 0, (LPARAM)&cds);
}

void CWmcaMsgEvent::OnWmDisconnected() {
	HWND hWnd = FindWindow(0, L"STOCK-AGENT_STOCK-SERVICE-HELPER");
	if (hWnd == NULL) return;

	web::json::value cResponseJson = web::json::value::object();
	cResponseJson[L"message"] = web::json::value::string(L"OnWmDisconnected");
	cResponseJson[L"code"] = web::json::value::string(L"OnWmDisconnected");
	std::wstring jsonString = cResponseJson.serialize();

	COPYDATASTRUCT cds;
	cds.dwData = WM_USER + 2001;
	cds.cbData = jsonString.size() * sizeof(wchar_t);
	cds.lpData = (PVOID)jsonString.c_str();
	SendMessage(hWnd, WM_COPYDATA, 0, (LPARAM)&cds);
}

void CWmcaMsgEvent::OnWmSocketError(int socketErrorCode) {

}

void CWmcaMsgEvent::OnWmReceiveData(OUTDATABLOCK* pOutData) {

}

void CWmcaMsgEvent::OnWmReceiveSise(OUTDATABLOCK* pSiseData) {

}

void CWmcaMsgEvent::OnWmReceiveMessage(OUTDATABLOCK* pMessage) {

}

void CWmcaMsgEvent::OnWmReceiveComplete(OUTDATABLOCK* pOutData) {

}

void CWmcaMsgEvent::OnWmReceiveError(OUTDATABLOCK* pError) {

}