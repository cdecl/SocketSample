// IOCPSockServer.h: interface for the IOCPSockServer class.
//
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// Copyright (c) 2003 by cdecl (byung-kyu kim)
// EMail : cdecl@interpark.com
//////////////////////////////////////////////////////////////////////
#ifndef __IOCPSERVER_H__BY_CDECL__
#define __IOCPSERVER_H__BY_CDECL__

#include "WinSockInit.h"

#include <cassert>
#include <queue>
#include <vector>
#include <list>
#include <algorithm>
#include <iterator>

#include "MemPool.h"


namespace GLASS {


//////////////////////////////////////////////////////////////////////
// WSABUF Ȯ�� 
// No virtual 
//////////////////////////////////////////////////////////////////////
struct WSAOverlappedExt : public WSAOVERLAPPED
{
	int flag_;
};

//////////////////////////////////////////////////////////////////////
// ���� ���� Ŭ����
//////////////////////////////////////////////////////////////////////
class PerSockBuff : public WSAOverlappedExt
{
public:
	enum { SOCK_MAX_BUFF_SIZE = 8192 };
	enum SOCKBUFF_FLAG { SOCK_RECV, SOCK_SEND };

public:
	PerSockBuff() : nSentSize_(0)
	{
		Initialize();
	}

	template <class BeginT, class EndT> 
	PerSockBuff(BeginT begin_, EndT end_) : nTotalSize_(0), nSentSize_(0) {
		Initialize();
		assign(begin_, end_);
	}

	// no virtual
	~PerSockBuff() {}

public:
	//////////////////////////////////////////////////////////////////
	// Overlapped �ʱ�ȭ 
	void Initialize() 
	{
		wsabuf.buf = szBuff;
		wsabuf.len = 0;
		memset(static_cast<WSAOverlappedExt*>(this), 0, sizeof(WSAOverlappedExt));
	}

	//////////////////////////////////////////////////////////////////
	// �÷��� ���� 
	void SetStatus(SOCKBUFF_FLAG flag) 
	{
		flag_ = flag;
	}

	int GetStatus() const
	{
		return flag_;
	}

	//////////////////////////////////////////////////////////////////
	// ������ �����͸� �����´� 
	char* GetBuff() const 
	{
		return wsabuf.buf;
	}

	size_t GetTotalSize() const 
	{
		return wsabuf.len;
	}

	void SetTotalSize(size_t si)
	{
		wsabuf.len = si;
	}

	size_t GetSentSize() const 
	{
		return nSentSize_;
	}

	void SetSentSize(size_t si)
	{
		nSentSize_ = si;
	}

	size_t GetRemainSize() const 
	{
		return wsabuf.len - nSentSize_;
	}

	size_t GetBuffSize() const 
	{
		return SOCK_MAX_BUFF_SIZE;
	}

	WSABUF* GetWsaBuf()  
	{
		return &wsabuf;
	}

	//////////////////////////////////////////////////////////////////
	// ���ۿ� ���� �Ҵ��Ѵ� 
	// STL �������̽��� ���� 
	template <class IterT>
	void Assign(IterT begin_, IterT end_) 
	{
		char* pEnd= std::copy(begin_, end_, szBuff);
		wsabuf.len = std::distance((char*)szBuff, pEnd);
		nSentSize_ = 0;
	}

	//////////////////////////////////////////////////////////////////
	// ���ۿ� ���� �Ҵ��Ѵ� 
	// STL �������̽��� ���� 
	template <class ContainerT>
	void Assign(ContainerT con_) 
	{
		char* pEnd = std::copy(con_.begin(), con_.end(), szBuff);
		wsabuf.len = std::distance((char*)szBuff, pEnd);
		nSentSize_ = 0;
	}

private:
	WSABUF	wsabuf;
	size_t	nSentSize_;
	char	szBuff[SOCK_MAX_BUFF_SIZE];
};


//////////////////////////////////////////////////////////////////////
// ���� Context Ŭ����
//////////////////////////////////////////////////////////////////////
class PerSockContext
{
public:
	//////////////////////////////////////////////////////////////////
	// Contructor 
	PerSockContext();	// ��������

	explicit PerSockContext(SOCKET s) : sock_(s) {}
	virtual ~PerSockContext() {}

public:
	//////////////////////////////////////////////////////////////////
	// GetPeerName
	int GetPeerName(std::string &strPeerAddr, int &nPort) const;
	int GetPeerName(sockaddr *pName, int *pNameLen) const;

	//////////////////////////////////////////////////////////////////
	// GetSockName
	int GetSockName(std::string &strSockAddr, int &nPort) const;
	int GetSockName(sockaddr *pName, int *pNameLen) const;

	//////////////////////////////////////////////////////////////////
	// Socket Option
	int GetSockOpt(int nOptionName, char *lpOptionValue, int *lpOptionLen, int nLevel = SOL_SOCKET) const;
	int SetSockOpt(int nOptionName, const char *lpOptionValue, int nOptionLen, int nLevel = SOL_SOCKET);

public:
	//////////////////////////////////////////////////////////////////
	// closesocket ȣ�� 
	void CloseSocket(bool bLinger = false);

	bool SendPostRemain(PerSockBuff* psb);

	//////////////////////////////////////////////////////////////////
	// �񵿱� Send Method
	bool SendPost(PerSockBuff* psb);

	template <class BeginT, class EndT>
	bool SendPost(BeginT begin, EndT end) 
	{
		PerSockBuff *pSendBuff = AllocPerSockBuff();
		pSendBuff->Assign(begin, end);
		return SendPost(pSendBuff);
	}

	//////////////////////////////////////////////////////////////////
	// �޼��� Recv ��⸦ ��û
	bool RecvWait();

	virtual PerSockBuff* AllocPerSockBuff()
	{
		PerSockBuff* psb = NULL;
		{
			CriticalSectionLock Lock(&CS_);
			psb = static_cast<PerSockBuff*>(pool.malloc());
		}
		new (psb) PerSockBuff();
		return psb;
	}

	virtual void FreePerSockBuff(PerSockBuff* psb) 
	{
		CriticalSectionLock Lock(&CS_);
		pool.free(psb);
	}

private:
	PerSockContext(const PerSockContext &psc);
	PerSockContext& operator=(const PerSockContext &pcs);

public:
	//////////////////////////////////////////////////////////////////
	// Get SOCKET Handle
	SOCKET GetSocket() const;
	bool IsValidSocket() const;

protected:
	SOCKET sock_;

public:
	PerSockBuff recvBuff;

private:
	CriticalSection CS_;
	static MemPool pool;
};




//////////////////////////////////////////////////////////////////////
// IOCP Ŭ����
//////////////////////////////////////////////////////////////////////
template <class PerSockContextT = PerSockContext>
class IOCPSockServer  
{
public:
	enum { ACCEPT_TIMEOUT = 100 };
	typedef std::vector<HANDLE> WorkThreadHandleArray;
	typedef WorkThreadHandleArray::iterator WorkThreadHandleArrayIter;
	typedef std::list<PerSockContextT *> SockContextPtrList;
	typedef typename SockContextPtrList::iterator SockContextPtrListIter;

public:
	IOCPSockServer() : nThreadID_(0), nListenPort_(0)
	{
		WinSock::Init();
	}

	virtual ~IOCPSockServer()
	{
		Release();
	}


public:
	//////////////////////////////////////////////////////////////////
	// �̺�Ʈ �Լ���  

	//////////////////////////////////////////////////////////////////
	// SendPost �Ŀ� �����Ⱑ �Ϸ�Ǹ� ȣ�� ��
	virtual void OnSend(PerSockContextT *pscontext, PerSockBuff* psb) {} 

	//////////////////////////////////////////////////////////////////
	// Recv�� �Ϸ�Ǹ� ȣ��� 
	virtual void OnRecv(PerSockContextT *pscontext, PerSockBuff* psb) {} 

	//////////////////////////////////////////////////////////////////
	// Accept��û�� ������ ȣ��� 
	virtual void OnAccept(PerSockContextT *pscontext) {}

	//////////////////////////////////////////////////////////////////
	// ������ �������� ȣ���
	virtual void OnClose(PerSockContextT *pscontext) {}


public:
	struct FindSocketClass : public std::binary_function<PerSockContextT *, PerSockContextT *, bool> 
	{
		bool operator()(PerSockContextT *lhs, PerSockContextT *rhs) const {
			return lhs->GetSocket() == rhs->GetSocket();
		}
	};

public:
	//////////////////////////////////////////////////////////////////
	// ����Ǿ� �ִ� Ŭ���̾�Ʈ�� �� 
	int GetClientCount() const
	{
		return listSockContextPtr_.size();
	}

	//////////////////////////////////////////////////////////////////
	// ���� WorkerThread ������ ���� 
	size_t GetWorkThreadCount() const
	{
		return vecWorkThread_.size();
	}

	//////////////////////////////////////////////////////////////////
	// ���������� ��� ��Ŵ 
	// �̶� MainThread �� WorkerThread�� ���� ��
	void Listen(int nListenPort, size_t nWorkThreadCount = 0);
	
	//////////////////////////////////////////////////////////////////
	// ��⸦ ����
	void Close();

	//////////////////////////////////////////////////////////////////
	// ��� ���ҽ� ����
	void Release();

	bool IsListen() const;


protected:
	//////////////////////////////////////////////////////////////////
	// ������ ������ �ڵ�� PerSockContext�� ������ (�޸� �Ҵ�)
	PerSockContextT* AllocPerSockContext(SOCKET s) throw()
	{
		return new PerSockContextT(s);
	}

	//////////////////////////////////////////////////////////////////
	// PerSockContextT �� �޸� ���� ��Ŵ 
	void FreePerSockContext(PerSockContextT *pscontext) throw() 
	{
		delete pscontext;
	}


	//////////////////////////////////////////////////////////////////
	// Listen Socket�� �Ҵ�, ��� ��Ű�� ���� 
	PerSockContextT* CreateListenSocket(int nPort, bool bReUseAddr = true);

	//////////////////////////////////////////////////////////////////
	// Listen Socket�� �Ҵ�, ��� ��Ű�� ���� 
	void DeleteListenSocket(PerSockContextT *pscxListen);

	bool FindSocket(PerSockContextT *pscontext);

	//////////////////////////////////////////////////////////////////
	// closesocket�� ȣ�� �ϰ� �޸� ���� 
	bool CloseSocket(PerSockContextT *pscontext);

	//////////////////////////////////////////////////////////////////
	// ��� Ŭ���̾�Ʈ�� ������ �ݰ� �޸� ���� 
	void CloseAllSocket();

	//////////////////////////////////////////////////////////////////
	// IOCP �ڵ��� ���� 
	void CreateIOCP();

	//////////////////////////////////////////////////////////////////
	// IOCP �ڵ鿡 ���� �ڵ� Mapping
	void UpdateIOCP(PerSockContextT *pscontext);

	//////////////////////////////////////////////////////////////////
	// OptimumWorkThreadCount �Լ��� ���ǵ� ����ŭ WorkerThread ���� 
	void CreateWorkerThread();

	//////////////////////////////////////////////////////////////////
	// accept �� �ϰ� PerSockContextT�� �Ҵ��� 
	PerSockContextT* Accept(PerSockContextT *pscxListen);

	//////////////////////////////////////////////////////////////////
	// WorkerThread�� ������Ŵ 
	void ReleaseWorkThread();
	
private:
	//////////////////////////////////////////////////////////////////
	// WorkerThread �Լ� 
	static UINT __stdcall WorkerThreadFunc(LPVOID lpParam);

	//////////////////////////////////////////////////////////////////
	// MainThread �Լ� 
	static UINT __stdcall MainRunFunc(LPVOID lpvParam); 

protected:
	WorkThreadHandleArray vecWorkThread_;
	SockContextPtrList listSockContextPtr_;
	HandleManage hmIOCP_;

	HandleManage hmMainThread_;
	UINT nThreadID_;

	int nListenPort_;
	size_t nWorkThreadCount_;
	CriticalSection CS_;
};



//////////////////////////////////////////////////////////////////
// ���� �ý����� CPU ������ ������ 
static size_t GetNumberOfProcessors()
{
	SYSTEM_INFO systeminfo;
	GetSystemInfo(&systeminfo);
	return systeminfo.dwNumberOfProcessors;
}



//////////////////////////////////////////////////////////////////////////
// Implememtation


//////////////////////////////////////////////////////////////////
// ���������� ��� ��Ŵ 
// �̶� MainThread �� WorkerThread�� ���� ��
template <class PerSockContextT>
void IOCPSockServer<PerSockContextT>::Listen(int nListenPort, size_t nWorkThreadCount)
{
	nListenPort_ = nListenPort;
	nWorkThreadCount_ = nWorkThreadCount;

	hmMainThread_ = (HANDLE)_beginthreadex(NULL, 0, MainRunFunc, this, 0, &nThreadID_);
}

//////////////////////////////////////////////////////////////////
// ��⸦ ����
template <class PerSockContextT>
void IOCPSockServer<PerSockContextT>::Close()
{
	::PostThreadMessage(nThreadID_, WM_QUIT, 0, 0);

	// MainThread�� Signal ���°� �ɶ����� ��� 
	WaitForSingleObject(hmMainThread_, INFINITE);	
	Release();
}


//////////////////////////////////////////////////////////////////
// ��� ���ҽ� ����
template <class PerSockContextT>
void IOCPSockServer<PerSockContextT>::Release()
{
	hmIOCP_.Release();
	hmMainThread_.Release();
	nThreadID_ = 0;
}

template <class PerSockContextT>
bool IOCPSockServer<PerSockContextT>::IsListen() const
{
	return !((HANDLE)hmMainThread_ == NULL);
}


//////////////////////////////////////////////////////////////////
// Listen Socket�� �Ҵ�, ��� ��Ű�� ���� 
template <class PerSockContextT>
PerSockContextT* IOCPSockServer<PerSockContextT>::CreateListenSocket(int nPort, bool bReUseAddr) 
{
	SOCKET sockListen = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if(sockListen == INVALID_SOCKET) {
		throw WinSockException("Error: CreateListenSocket WSASocket()");
	}

	if (bReUseAddr) {
		int option = 1;
		// Listen Port�� ���� 
		setsockopt(sockListen, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char *>(&option), sizeof(option));
	}

	sockaddr_in addr = {0};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(static_cast<short>(nPort));
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(sockListen, (sockaddr*)&addr, sizeof(sockaddr_in)) == SOCKET_ERROR) {
		throw WinSockException("Error: CreateListenSocket bind()");
	}

	if (listen(sockListen, 5) == SOCKET_ERROR) {
		throw WinSockException("Error: CreateListenSocket listen()");
	}		

	return AllocPerSockContext(sockListen);
}


//////////////////////////////////////////////////////////////////
// Listen Socket�� �Ҵ�, ��� ��Ű�� ���� 
template <class PerSockContextT>
void IOCPSockServer<PerSockContextT>::DeleteListenSocket(PerSockContextT *pscxListen) 
{
	pscxListen->CloseSocket();
	FreePerSockContext(pscxListen);
}


template <class PerSockContextT>
bool IOCPSockServer<PerSockContextT>::FindSocket(PerSockContextT *pscontext) 
{
	if (pscontext) {
		SockContextPtrListIter it = std::find_if(
										listSockContextPtr_.begin(), 
										listSockContextPtr_.end(), 
										std::bind2nd(FindSocketClass(), pscontext)
									);
		return it != listSockContextPtr_.end();
	}
	return false;
}

//////////////////////////////////////////////////////////////////
// closesocket�� ȣ�� �ϰ� �޸� ���� 
template <class PerSockContextT>
bool IOCPSockServer<PerSockContextT>::CloseSocket(PerSockContextT *pscontext) 
{
	if (pscontext) {
		CriticalSectionLock Lock(&CS_);	// LOCK
		SockContextPtrListIter it = std::find(listSockContextPtr_.begin(), listSockContextPtr_.end(), pscontext);

		if (it != listSockContextPtr_.end()) {
			pscontext->CloseSocket();
			listSockContextPtr_.erase(it);

			FreePerSockContext(pscontext);
			return true;
		}
	}

	return false;
}


//////////////////////////////////////////////////////////////////
// ��� Ŭ���̾�Ʈ�� ������ �ݰ� �޸� ���� 
template <class PerSockContextT>
void IOCPSockServer<PerSockContextT>::CloseAllSocket()
{
	CriticalSectionLock Lock(&CS_);

	SockContextPtrListIter it = listSockContextPtr_.begin();
	for (; it != listSockContextPtr_.end(); ++it) {
		PerSockContextT *pscontext = *it;
		if (pscontext) {
			pscontext->CloseSocket();
			FreePerSockContext(pscontext);
		}
	}

	listSockContextPtr_.erase(listSockContextPtr_.begin(), listSockContextPtr_.end());
}

//////////////////////////////////////////////////////////////////
// IOCP �ڵ��� ���� 
template <class PerSockContextT>
void IOCPSockServer<PerSockContextT>::CreateIOCP()
{
	hmIOCP_ = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, nWorkThreadCount_);
	if (!(HANDLE)hmIOCP_) {
		throw WinSockException("Error: CreateIOCP CreateIoCompletionPort()");
	}
}	

//////////////////////////////////////////////////////////////////
// IOCP �ڵ鿡 ���� �ڵ� Mapping
template <class PerSockContextT>
void IOCPSockServer<PerSockContextT>::UpdateIOCP(PerSockContextT *pscontext)
{
	HANDLE hIOCP = CreateIoCompletionPort((HANDLE)pscontext->GetSocket(), hmIOCP_, reinterpret_cast<DWORD>(pscontext), 0);
	if (!hIOCP) {
		throw WinSockException("Error: UpdateIOCP CreateIoCompletionPort()");
	}
}	


//////////////////////////////////////////////////////////////////
// OptimumWorkThreadCount �Լ��� ���ǵ� ����ŭ WorkerThread ���� 
template <class PerSockContextT>
void IOCPSockServer<PerSockContextT>::CreateWorkerThread()
{
	int nThreadCount = nWorkThreadCount_ == 0 ? GetNumberOfProcessors() : nWorkThreadCount_;

	for (int i = 0; i < nThreadCount; ++i) {
		vecWorkThread_.push_back((HANDLE)_beginthreadex(NULL, 0, WorkerThreadFunc, this, 0, NULL));
	}
}


//////////////////////////////////////////////////////////////////
// accept �� �ϰ� PerSockContextT�� �Ҵ��� 
template <class PerSockContextT>
PerSockContextT* IOCPSockServer<PerSockContextT>::Accept(PerSockContextT *pscxListen)
{
	sockaddr_in addr = {0};
	int nLen = sizeof(sockaddr_in);
	
	SOCKET s = accept(pscxListen->GetSocket(), (sockaddr*)&addr, &nLen);
	if (s == SOCKET_ERROR) {
		
	}

	PerSockContextT* pscontext = AllocPerSockContext(s);

	{
		CriticalSectionLock Lock(&CS_);		// LOCK
		listSockContextPtr_.push_back(pscontext);
	}

	UpdateIOCP(pscontext);
	pscontext->RecvWait();

	return pscontext;
}


//////////////////////////////////////////////////////////////////
// WorkerThread�� ������Ŵ 
template <class PerSockContextT>
void IOCPSockServer<PerSockContextT>::ReleaseWorkThread()
{
	WorkThreadHandleArrayIter it = vecWorkThread_.begin();
	for (; it != vecWorkThread_.end(); ++it) {
		CloseHandle(*it);
	}

	vecWorkThread_.clear(); 
}


//////////////////////////////////////////////////////////////////
// WorkerThread �Լ� 
template <class PerSockContextT>
UINT __stdcall IOCPSockServer<PerSockContextT>::WorkerThreadFunc(LPVOID lpParam)
{
	IOCPSockServer *pThis = reinterpret_cast<IOCPSockServer*>(lpParam);

	DWORD dwNoOfByteTrans = 0;
	LPWSAOVERLAPPED lpOverlapped = NULL;

	PerSockContextT *pscontext = NULL;
	
	while (true) {
		int nResult = GetQueuedCompletionStatus(
							pThis->hmIOCP_, 
							&dwNoOfByteTrans,
							reinterpret_cast<LPDWORD>(&pscontext), 
							&lpOverlapped,
							INFINITE
					);

		// pscontext �� 0�̸� WorkerThread ���� 
		// Main Thread���� ����� CompletionKey�� 0���� �ѱ�
		if (!pscontext) {
			break;	// Thread�� ������ 
		}

		// dwNoOfByteTrans 0�̸� ������ ���� �ߴٴ� ��ȣ 
		if (!nResult || (nResult && dwNoOfByteTrans == 0)) {
			pThis->OnClose(pscontext);
			continue;
		}
		
		PerSockBuff *psb = static_cast<PerSockBuff *>(lpOverlapped);

		switch (psb->GetStatus()) 
		{
			case PerSockBuff::SOCK_RECV :
				pscontext->recvBuff.SetTotalSize(dwNoOfByteTrans);
				pThis->OnRecv(pscontext, &pscontext->recvBuff);
				break;

			case PerSockBuff::SOCK_SEND :
				PerSockBuff *pSockBuff = psb;
				pSockBuff->SetSentSize(pSockBuff->GetSentSize() + dwNoOfByteTrans);

				if (pSockBuff->GetRemainSize()) {
					if (!pscontext->SendPostRemain(pSockBuff)) {
						pThis->OnClose(pscontext);
					}
				}
				else {
					pThis->OnSend(pscontext, pSockBuff);
					pscontext->FreePerSockBuff(psb);

					if (!pscontext->IsValidSocket()) {
						pThis->OnClose(pscontext);
					}
				}
				break;
		}
	}

	return 0;
}

//////////////////////////////////////////////////////////////////
// MainThread �Լ� 
template <class PerSockContextT>
UINT __stdcall IOCPSockServer<PerSockContextT>::MainRunFunc(LPVOID lpvParam)
{
	IOCPSockServer *pThis = reinterpret_cast<IOCPSockServer*>(lpvParam);
	
	PerSockContextT *pscxListen = pThis->CreateListenSocket(pThis->nListenPort_);
	SOCKET sockListen = pscxListen->GetSocket();

	pThis->CreateIOCP();
	pThis->UpdateIOCP(pscxListen);
	
	pThis->CreateWorkerThread();

	HandleManage hmEvent(WSACreateEvent());
	int nRet = WSAEventSelect(sockListen, hmEvent, FD_ACCEPT);
	if (SOCKET_ERROR == nRet) {
		return -1;
	}

	MSG msg;
	while (true) {
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				break;
			}
		}

		DWORD dwIndex = WSAWaitForMultipleEvents(1, &hmEvent, true, ACCEPT_TIMEOUT, FALSE);
		if (dwIndex == WSA_WAIT_FAILED || dwIndex == WSA_WAIT_TIMEOUT) {
			continue;
		}

		WSANETWORKEVENTS wsaEvent;
		WSAEnumNetworkEvents(sockListen, hmEvent, &wsaEvent);

		if (wsaEvent.lNetworkEvents & FD_ACCEPT) {
			if (wsaEvent.iErrorCode[FD_ACCEPT_BIT] != 0) {
				break;
			} 

			pThis->OnAccept(pscxListen);
		}
	}

	int nWorkThreadCount = (int)pThis->vecWorkThread_.size();
	for (int i = 0; i < nWorkThreadCount; ++i) {
		PostQueuedCompletionStatus(pThis->hmIOCP_, 0, 0, NULL); 
	}
	
	WaitForMultipleObjects(
		pThis->vecWorkThread_.size(),
		&pThis->vecWorkThread_[0],
		TRUE,
		INFINITE
	);

	pThis->CloseAllSocket();
	pThis->DeleteListenSocket(pscxListen);
	
	return 0;
} 








}	// namespace GLASS 


#endif 

