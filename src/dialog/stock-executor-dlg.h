// stock-executor-dlg.h: 헤더 파일
//
#pragma once

#include <string>

#include "helper/wmca_intf.h"
#include "helper/wmca_msg_event.h"
#include "helper/stock_msg_event.h"

// CStockExecutorDlg 대화 상자
class CStockExecutorDlg : public CDHtmlDialog
{
// 생성입니다.
public:
    CStockExecutorDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_STOCKEXECUTOR_DIALOG, IDH = IDR_HTML_STOCKEXECUTOR_DIALOG };
#endif

    protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

    HRESULT OnButtonConnect(IHTMLElement* pElement);
    HRESULT OnButtonOK(IHTMLElement *pElement);
    HRESULT OnButtonCancel(IHTMLElement *pElement);

// 구현입니다.
protected:
    HICON m_hIcon;
    CWmcaIntf m_wmca;
    std::wstring m_strData;

    // 생성된 메시지 맵 함수
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg BOOL OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();

    // Executor functions
    afx_msg void OnConnect();

    // Wmca functions
    afx_msg LRESULT OnWmcaEvent(WPARAM wParam, LPARAM lParam);

    DECLARE_MESSAGE_MAP()
    DECLARE_DHTML_EVENT_MAP()

private:
    CWmcaMsgEvent m_wmcaMsgEvent;
};