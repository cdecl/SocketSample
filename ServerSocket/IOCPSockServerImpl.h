// IOCPSockServerImpl.h: interface for the IOCPSockServerImpl class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IOCPSOCKSERVERIMPL_H__6FF02983_70B0_453A_8C8F_635D6FBA040D__INCLUDED_)
#define AFX_IOCPSOCKSERVERIMPL_H__6FF02983_70B0_453A_8C8F_635D6FBA040D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <sstream>
#include "../IOCPSockServer.h"
using GLASS::PerSockContext;
using GLASS::PerSockBuff;

class CServerSocketDlg;


class SocketNode : public PerSockContext
{
public:
	SocketNode() : PerSockContext() {}
	explicit SocketNode(SOCKET s) : PerSockContext(s) {}
	virtual ~SocketNode() {}
};




template <class NodeT>
class IOCPSockServerImpl : public GLASS::IOCPSockServer<NodeT>
{
public:
	IOCPSockServerImpl(CServerSocketDlg *pWnd) : pParent_(pWnd)
	{

	}

	~IOCPSockServerImpl()
	{
		pParent_ = NULL;
	}

	void OnSend(NodeT *pscontext, PerSockBuff* psb)
	{
		if (!pscontext->IsValidSocket()) {
			TRACE("OnSend: !pscontext->IsValidSocket() \n");
			return;
		}

		CString strSend(psb->GetBuff(), psb->GetTotalSize());  
		pParent_->WriteLogLine("[SEND]" + strSend);

	}

	//////////////////////////////////////////////////////////////////
	// Recv가 완료되면 호출됨 
	void OnRecv(NodeT *pscontext, PerSockBuff* psb)
	{
		OutputDebugString("START\n");

		bool bResult = pscontext->SendPost(psb->GetBuff(), psb->GetBuff() + psb->GetTotalSize());
		OutputDebugString(bResult ? "TRUE" : "FALSE");

		if (!pscontext->RecvWait()) {
			CloseSocket(pscontext);
		}

	}

	//////////////////////////////////////////////////////////////////
	// Accept가 완료되면 호출됨 
	void OnAccept(NodeT *pscontext)
	{
		NodeT *p = Accept(pscontext);

		assert(pParent_);

		CString str;
		str.Format("ACCEPT:%d", p->GetSocket());
		pParent_->WriteLogLine(str);
		pParent_->SyncSessionView();
	}

	//////////////////////////////////////////////////////////////////
	void OnClose(NodeT *pscontext)
	{
		//if (pscontext->SendBuffList.GetListCount()) {
//			TRACE("pscontext->CloseSocket()\n");
		//}
		CloseSocket(pscontext);

		assert(pParent_);
		pParent_->WriteLogLine("CLIENT CLOSE");
		pParent_->SyncSessionView();
	}


	void SendAll(const CString &strSendAll)
	{
		SockContextPtrListIter it = listSockContextPtr_.begin();
		SockContextPtrListIter itEnd = listSockContextPtr_.end();

		SockContextPtrList listSock;

		for (; it != itEnd; ++it) {
			if (!ForEachClient(*it)) {
				CloseSocket(*it);
			}
		}
	}

	bool ForEachClient(NodeT *psc)
	{
		for (int i = 0; i < 10; ++i) {
			std::ostringstream os;
			os << i << " ";

			std::string str(os.str());

			if (!psc->SendPost(str.begin(), str.end())) {
				return false;
			}
		}
		return true;
	}


protected:
	CServerSocketDlg *pParent_;
};







#endif // !defined(AFX_IOCPSOCKSERVERIMPL_H__6FF02983_70B0_453A_8C8F_635D6FBA040D__INCLUDED_)
