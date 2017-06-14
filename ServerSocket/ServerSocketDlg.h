// ServerSocketDlg.h : header file
//

#if !defined(AFX_SERVERSOCKETDLG_H__A4A39720_D1C3_4DE5_8889_558E0813D2B7__INCLUDED_)
#define AFX_SERVERSOCKETDLG_H__A4A39720_D1C3_4DE5_8889_558E0813D2B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <memory>
#include "IOCPSockServerImpl.h"

/////////////////////////////////////////////////////////////////////////////
// CServerSocketDlg dialog

class CServerSocketDlg : public CDialog
{
// Construction
public:
	CServerSocketDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CServerSocketDlg() {}

// Dialog Data
	//{{AFX_DATA(CServerSocketDlg)
	enum { IDD = IDD_SERVERSOCKET_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CServerSocketDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CServerSocketDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnBtnListen();
	afx_msg void OnDestroy();
	afx_msg void OnBtnNoop();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void SyncSessionView();
	void WriteLogLine(const CString &strLog);

private:
	std::auto_ptr<IOCPSockServerImpl<SocketNode> > spIOCP_; 
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERVERSOCKETDLG_H__A4A39720_D1C3_4DE5_8889_558E0813D2B7__INCLUDED_)
