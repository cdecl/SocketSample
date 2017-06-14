// ClientSocketDlg.h : header file
//

#if !defined(AFX_CLIENTSOCKETDLG_H__52B773DD_EDD5_4383_AE73_D0D0CC64FEF7__INCLUDED_)
#define AFX_CLIENTSOCKETDLG_H__52B773DD_EDD5_4383_AE73_D0D0CC64FEF7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CClientSocketDlg dialog

#include "AsyncSockClientImpl.h"
#include <memory>


class CClientSocketDlg : public CDialog
{
// Construction
public:
	void WriteLogLine(const CString &strLog);
	CClientSocketDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CClientSocketDlg)
	enum { IDD = IDD_CLIENTSOCKET_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClientSocketDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CClientSocketDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnBtnConnect();
	afx_msg void OnBtnSend();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	std::auto_ptr<AsyncSockClientImpl> spSock_;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLIENTSOCKETDLG_H__52B773DD_EDD5_4383_AE73_D0D0CC64FEF7__INCLUDED_)
