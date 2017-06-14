// ClientSocketDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ClientSocket.h"
#include "ClientSocketDlg.h"

#include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClientSocketDlg dialog

CClientSocketDlg::CClientSocketDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CClientSocketDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CClientSocketDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CClientSocketDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CClientSocketDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CClientSocketDlg, CDialog)
	//{{AFX_MSG_MAP(CClientSocketDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_CONNECT, OnBtnConnect)
	ON_BN_CLICKED(IDC_BTN_SEND, OnBtnSend)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClientSocketDlg message handlers

BOOL CClientSocketDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	

	spSock_.reset(new AsyncSockClientImpl(this));

	SetDlgItemText(IDC_EDIT_IP, "localhost");
	SetDlgItemText(IDC_EDIT_PORT, "4000");
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CClientSocketDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		if (SC_CLOSE == nID) {
			EndDialog(IDOK); 
		}

		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CClientSocketDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CClientSocketDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CClientSocketDlg::OnCancel() 
{
	//CDialog::OnCancel();
}

void CClientSocketDlg::OnOK() 
{
	//CDialog::OnOK();
}

void CClientSocketDlg::OnBtnConnect() 
{
	if (!spSock_->IsValidSocket()) {
		CString strIP, strPort;
		GetDlgItemText(IDC_EDIT_IP, strIP);
		GetDlgItemText(IDC_EDIT_PORT, strPort);

		if (!spSock_->Connect((LPCSTR)strIP, atoi((LPCSTR)strPort))) {
			AfxMessageBox("Connect Error !!");
			return;
		}
		SetDlgItemText(IDC_BTN_CONNECT, "Close");
		
		CString strLog;
		std::string str; 
		int nPort;

		int n = spSock_->GetSockName(str, nPort);

		//strLog.Format("GetSockName=%s:%d", str.c_str(), nPort);
		//WriteLogLine(strLog);
		
	}
	else {
		spSock_->Close();
		SetDlgItemText(IDC_BTN_CONNECT, "Connect");
	}
}

void CClientSocketDlg::WriteLogLine(const CString &strLog)
{
	CEdit *pEdit = static_cast<CEdit *>(GetDlgItem(IDC_EDIT_LOG));

	int nLen = pEdit->GetWindowTextLength(); 

	if (pEdit->GetLimitText() <= nLen) {
		pEdit->SetWindowText(_T("")); 	
	}

	pEdit->SetSel(nLen, nLen);
	pEdit->ReplaceSel(strLog + "\r\n");
	//pEdit->SetWindowText(strLog);
}

void CClientSocketDlg::OnBtnSend() 
{
	if (spSock_->IsValidSocket()) {
		CString strSend;
		GetDlgItemText(IDC_EDIT_SEND, strSend);

		if (strSend.IsEmpty()) {
			return;
		}
 
		std::ostringstream sout;
//		for (int i = 0; i < 1; ++i) {
			sout << char(0x02) << (LPCSTR)strSend << char(0x03);	
			std::string str(sout.str());
			sout.str("");

			if (SOCKET_ERROR == spSock_->Send(str.c_str(), str.length())) {
				spSock_->Close();
				SetDlgItemText(IDC_BTN_CONNECT, "Connect");
				return;
			}

			str = "[send]" + str;
			WriteLogLine(str.c_str());
			Sleep(1);
//		}

		SetDlgItemText(IDC_EDIT_SEND, "");
	}
}

void CClientSocketDlg::OnDestroy() 
{
	spSock_->Close();
	CDialog::OnDestroy();
}
