// AsyncSockClient.cpp: implementation of the AsyncSockClient class.
//
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// Copyright (c) 2003 by cdecl (byung-kyu kim)
// EMail : cdecl@interpark.com
//////////////////////////////////////////////////////////////////////
#include "AsyncSockClient.h"
using namespace GLASS;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
AsyncSockClient::AsyncSockClient() : nThreadID_(0), sock_(INVALID_SOCKET)
{
	WinSock::Init();
}

AsyncSockClient::~AsyncSockClient()
{

}

bool AsyncSockClient::Connect(const std::string &strIP, int nPort)
{
	sock_ = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if(sock_ == INVALID_SOCKET) {
		throw WinSockException("Error: AsyncSockClient::Connect WSASocket()");
	}

	sockaddr_in addr = {0};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(static_cast<short>(nPort));

	hostent *pHost = gethostbyname(strIP.c_str());
	memcpy(&addr.sin_addr, pHost->h_addr_list[0], sizeof(addr.sin_addr));

	int nSuccess = connect(sock_, (sockaddr*)&addr, sizeof(sockaddr_in));
	if (nSuccess == SOCKET_ERROR || nSuccess) {
		Close();
		return false;
	}

	hmThread_ = reinterpret_cast<HANDLE>(_beginthreadex(NULL, 0, MainRun, this, 0, &nThreadID_));
	if (!(HANDLE)hmThread_) {
		throw WinSockException("Error: AsyncSockClient::Connect _beginthreadex()");
	}

	return true;
}

void AsyncSockClient::Close()
{
	if ((HANDLE)hmThread_) {
		PostThreadMessage(nThreadID_, WM_QUIT, 0, 0);
		::WaitForSingleObject(hmThread_, INFINITE);
	}
	hmThread_ = NULL;
	nThreadID_ = 0;

	if (sock_ != INVALID_SOCKET) {
		LINGER  lingerStruct;
		lingerStruct.l_onoff = 1;
		lingerStruct.l_linger = 0;

		SetSockOpt(SO_LINGER, (char *)&lingerStruct, sizeof(lingerStruct));

		shutdown(sock_, SD_SEND);
		closesocket(sock_);
	}
	sock_ = INVALID_SOCKET;
}


int AsyncSockClient::Send(const char *pBuff, size_t nLen, int nFlags)
{
	return send(sock_, pBuff, nLen, nFlags); 
}


int AsyncSockClient::Recv(char *pBuff, int nBuffLen, int nFlags)
{
	return recv(sock_, pBuff, nBuffLen, nFlags);
}

bool AsyncSockClient::IsValidSocket() const
{
	return !(sock_ == INVALID_SOCKET);
}

ULONG AsyncSockClient::GetSockBuffLen() const 
{
	ULONG ulBuffSize = 0;
	ioctlsocket(sock_, FIONREAD, &ulBuffSize);

	return ulBuffSize;
}


int AsyncSockClient::GetPeerName(sockaddr *pName, int *pNameLen) const
{
	return getpeername(sock_, pName, pNameLen);
}


int AsyncSockClient::GetPeerName(std::string &strPeerAddr, int &nPort) const
{
	// MFC CAsyncSocket 구현 
	sockaddr_in sockAddr = {0};

	int nSockAddrLen = sizeof(sockAddr);
	int nResult = GetPeerName((sockaddr*)&sockAddr, &nSockAddrLen);

	if (!nResult)
	{
		nPort = ntohs(sockAddr.sin_port);
		strPeerAddr = inet_ntoa(sockAddr.sin_addr);
	}

	return nResult;
}


int AsyncSockClient::GetSockName(sockaddr *pName, int *pNameLen) const
{
	return getsockname(sock_, pName, pNameLen);
}


int AsyncSockClient::GetSockName(std::string &strSockAddr, int &nPort) const
{
	// MFC CAsyncSocket 구현 
	sockaddr_in sockAddr = {0};

	int nSockAddrLen = sizeof(sockAddr);
	int nResult = GetSockName((sockaddr*)&sockAddr, &nSockAddrLen);

	if (!nResult)
	{
		nPort = ntohs(sockAddr.sin_port);
		strSockAddr = inet_ntoa(sockAddr.sin_addr);
	}

	return nResult;
}


int AsyncSockClient::GetSockOpt(int nOptionName, char *lpOptionValue, int *lpOptionLen, int nLevel) const
{
	return getsockopt(sock_, nLevel, nOptionName, lpOptionValue, lpOptionLen);
}

int AsyncSockClient::SetSockOpt(int nOptionName, const char *lpOptionValue, int nOptionLen, int nLevel)
{
	return setsockopt(sock_, nLevel, nOptionName, lpOptionValue, nOptionLen);
}


UINT __stdcall AsyncSockClient::MainRun(LPVOID lpvParam)
{
	AsyncSockClient *pThis = reinterpret_cast<AsyncSockClient *>(lpvParam);
	SOCKET &sockClient = pThis->sock_;

	HandleManage hmEvent(WSACreateEvent());

	int nRet = WSAEventSelect(sockClient, hmEvent, FD_READ | FD_CLOSE);
	if (SOCKET_ERROR == nRet) {
		return 1;
	}

	MSG msg;
	while (true) {
		if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				break;
			}
		}

		DWORD dwIndex = WSAWaitForMultipleEvents(1, &hmEvent, true, 100, FALSE);
		if (dwIndex == WSA_WAIT_FAILED || dwIndex == WSA_WAIT_TIMEOUT) {
			continue;
		}
		
		WSANETWORKEVENTS wsaEvent;
		WSAEnumNetworkEvents(sockClient, hmEvent, &wsaEvent);

		if (wsaEvent.lNetworkEvents & FD_READ) {
			if (wsaEvent.iErrorCode[FD_READ_BIT] != 0) {
				break;
			} 

			pThis->OnRecv();
		}
		else if (wsaEvent.lNetworkEvents & FD_CLOSE) {
			if (wsaEvent.iErrorCode[FD_CLOSE_BIT] != 0) {
				break;
			} 

			pThis->hmThread_ = NULL;
			pThis->OnClose();
			break;
		}
	}

	return 0;
}
