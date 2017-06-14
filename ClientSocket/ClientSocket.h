// ClientSocket.h : main header file for the CLIENTSOCKET application
//

#if !defined(AFX_CLIENTSOCKET_H__ADADFA7A_1EE6_4425_BABF_FC7332241231__INCLUDED_)
#define AFX_CLIENTSOCKET_H__ADADFA7A_1EE6_4425_BABF_FC7332241231__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CClientSocketApp:
// See ClientSocket.cpp for the implementation of this class
//

class CClientSocketApp : public CWinApp
{
public:
	CClientSocketApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClientSocketApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CClientSocketApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLIENTSOCKET_H__ADADFA7A_1EE6_4425_BABF_FC7332241231__INCLUDED_)
