// AsyncSockClientImpl.cpp: implementation of the AsyncSockClientImpl class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ClientSocket.h"
#include "AsyncSockClientImpl.h"

#include "ClientSocketDlg.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AsyncSockClientImpl::AsyncSockClientImpl(CClientSocketDlg *pWnd) : pParent_(pWnd)
{

}

AsyncSockClientImpl::~AsyncSockClientImpl()
{
	pParent_ = NULL;
}


void AsyncSockClientImpl::OnRecv()
{
	char szBuff[SZ_SIZE + 1];

	ULONG uSize = GetSockBuffLen();
	ULONG nRecvLen = 0;

	std::string strBuff;
	strBuff.reserve(SZ_SIZE);

	CString strLog = "[recv]";

	while (nRecvLen < uSize) {
		int nLen = Recv(szBuff, SZ_SIZE);
		nRecvLen += nLen;
		TRACE("%lu, %d\n", uSize, nLen);

		strLog += CString(szBuff, nLen);
		pParent_->WriteLogLine(strLog);
	}

	
}

void AsyncSockClientImpl::OnClose()
{
	Close();
	pParent_->SetDlgItemText(IDC_BTN_CONNECT, "Connect");
}
