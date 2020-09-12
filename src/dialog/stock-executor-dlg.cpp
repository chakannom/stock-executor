
// stock-executor-dlg.cpp: 구현 파일
//

#include "core/framework.h"
#include "stock-executor.h"
#include "about-dlg.h"
#include "stock-executor-dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CStockExecutorDlg 대화 상자

BEGIN_DHTML_EVENT_MAP(CStockExecutorDlg)
    DHTML_EVENT_ONCLICK(_T("ButtonOK"), OnButtonOK)
    DHTML_EVENT_ONCLICK(_T("ButtonCancel"), OnButtonCancel)
END_DHTML_EVENT_MAP()


CStockExecutorDlg::CStockExecutorDlg(CWnd* pParent /*=nullptr*/)
    : CDHtmlDialog(IDD_STOCKEXECUTOR_DIALOG, IDR_HTML_STOCKEXECUTOR_DIALOG, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CStockExecutorDlg::DoDataExchange(CDataExchange* pDX)
{
    CDHtmlDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CStockExecutorDlg, CDHtmlDialog)
    ON_WM_SYSCOMMAND()
    ON_MESSAGE(CA_WMCAEVENT, OnWmcaEvent)
END_MESSAGE_MAP()


// CStockExecutorDlg 메시지 처리기

BOOL CStockExecutorDlg::OnInitDialog()
{
    CDHtmlDialog::OnInitDialog();

    // 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

    // IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != nullptr)
    {
        BOOL bNameValid;
        CString strAboutMenu;
        bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
        ASSERT(bNameValid);
        if (!strAboutMenu.IsEmpty())
        {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }

    // 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
    //  프레임워크가 이 작업을 자동으로 수행합니다.
    SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
    SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

    ShowWindow(SW_MINIMIZE);

    // TODO: 여기에 추가 초기화 작업을 추가합니다.

    return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CStockExecutorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else
    {
        CDHtmlDialog::OnSysCommand(nID, lParam);
    }
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CStockExecutorDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // 아이콘을 그립니다.
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDHtmlDialog::OnPaint();
    }
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CStockExecutorDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

HRESULT CStockExecutorDlg::OnButtonOK(IHTMLElement* /*pElement*/)
{
    OnOK();
    return S_OK;
}

HRESULT CStockExecutorDlg::OnButtonCancel(IHTMLElement* /*pElement*/)
{
    OnCancel();
    return S_OK;
}

//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
//	wmca.dll로 부터 수신한 윈도우 메시지를 통해 각 이벤트 핸들러로 분기합니다
//━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
LRESULT CStockExecutorDlg::OnWmcaEvent(WPARAM dwMessageType, LPARAM lParam)
{
    switch (dwMessageType) {
    case CA_CONNECTED:          //로그인 성공
        m_wmcaMsgEvent.OnWmConnected((LOGINBLOCK*)lParam);
        break;
    case CA_DISCONNECTED:       //접속 끊김
        m_wmcaMsgEvent.OnWmDisconnected();
        break;
    case CA_SOCKETERROR:        //통신 오류 발생
        m_wmcaMsgEvent.OnWmSocketError((int)lParam);
        break;
    case CA_RECEIVEDATA:        //서비스 응답 수신(TR)
        m_wmcaMsgEvent.OnWmReceiveData((OUTDATABLOCK*)lParam);
        break;
    case CA_RECEIVESISE:        //실시간 데이터 수신(BC)
        m_wmcaMsgEvent.OnWmReceiveSise((OUTDATABLOCK*)lParam);
        break;
    case CA_RECEIVEMESSAGE:     //상태 메시지 수신 (입력값이 잘못되었을 경우 문자열형태로 설명이 수신됨)
        m_wmcaMsgEvent.OnWmReceiveMessage((OUTDATABLOCK*)lParam);
        break;
    case CA_RECEIVECOMPLETE:    //서비스 처리 완료
        m_wmcaMsgEvent.OnWmReceiveComplete((OUTDATABLOCK*)lParam);
        break;
    case CA_RECEIVEERROR:       //서비스 처리중 오류 발생 (입력값 오류등)
        m_wmcaMsgEvent.OnWmReceiveError((OUTDATABLOCK*)lParam);
        break;
    default:
        break;
    }

    return TRUE;
}