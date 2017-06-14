// AsyncSockClientImpl.h: interface for the AsyncSockClientImpl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ASYNCSOCKCLIENTIMPL_H__D37561BA_DC7E_4A24_A6B7_D7AA1B34D681__INCLUDED_)
#define AFX_ASYNCSOCKCLIENTIMPL_H__D37561BA_DC7E_4A24_A6B7_D7AA1B34D681__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "../AsyncSockClient.h"

class CClientSocketDlg;

class AsyncSockClientImpl : public GLASS::AsyncSockClient
{
public:
	AsyncSockClientImpl(CClientSocketDlg *pWnd);
	virtual ~AsyncSockClientImpl();

public:
	virtual void OnRecv();
	virtual void OnClose();

public:
	CClientSocketDlg *pParent_;
};

#endif // !defined(AFX_ASYNCSOCKCLIENTIMPL_H__D37561BA_DC7E_4A24_A6B7_D7AA1B34D681__INCLUDED_)
