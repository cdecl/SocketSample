// IOCPSockServer.cpp: implementation of the IOCPServer class.
//
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// Copyright (c) 2003 by cdecl (byung-kyu kim)
// EMail : cdecl@interpark.com
//////////////////////////////////////////////////////////////////////
//#include "stdafx.h"
#include "IOCPSockServer.h"
using namespace GLASS;

//////////////////////////////////////////////////////////////////////
// PerSockContext class implememtation
//////////////////////////////////////////////////////////////////////

// static 변수 초기화 
MemPool PerSockContext::pool(sizeof(PerSockBuff));


int PerSockContext::GetPeerName(sockaddr *pName, int *pNameLen) const
{
	return getpeername(sock_, pName, pNameLen);
}


int PerSockContext::GetPeerName(std::string &strPeerAddr, int &nPort) const
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

int PerSockContext::GetSockName(sockaddr *pName, int *pNameLen) const
{
	return getsockname(sock_, pName, pNameLen);
}


int PerSockContext::GetSockName(std::string &strSockAddr, int &nPort) const
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


int PerSockContext::GetSockOpt(int nOptionName, char *lpOptionValue, int *lpOptionLen, int nLevel) const
{
	return getsockopt(sock_, nLevel, nOptionName, lpOptionValue, lpOptionLen);
}

int PerSockContext::SetSockOpt(int nOptionName, const char *lpOptionValue, int nOptionLen, int nLevel)
{
	return setsockopt(sock_, nLevel, nOptionName, lpOptionValue, nOptionLen);
}

SOCKET PerSockContext::GetSocket() const 
{
	return sock_;
}

bool PerSockContext::IsValidSocket() const 
{
	return sock_ != INVALID_SOCKET;
}



//////////////////////////////////////////////////////////////////
// closesocket 호출 
void PerSockContext::CloseSocket(bool bLinger)
{
	if (sock_ != INVALID_SOCKET) {
		if (bLinger) {
			LINGER  lingerStruct;
			lingerStruct.l_onoff = 1;
			lingerStruct.l_linger = 0;
			SetSockOpt(SO_LINGER, (char *)&lingerStruct, sizeof(lingerStruct));
		}

		shutdown(sock_, SD_SEND);	// 우아한 종료 
		closesocket(sock_);
		sock_ = INVALID_SOCKET;
	}
}


bool PerSockContext::SendPostRemain(PerSockBuff* psb)
{
	size_t nSendSize = psb->GetRemainSize();
	if (!nSendSize) { return true; }

	DWORD dwNoOfByteSend = 0;
	psb->SetStatus(PerSockBuff::SOCK_SEND);

	WSABUF wsabuf = {0};
	wsabuf.buf = psb->GetBuff() + psb->GetSentSize();
	wsabuf.len = nSendSize;

	int nRet = WSASend(
		sock_, &wsabuf, 1, &dwNoOfByteSend, 0, reinterpret_cast<LPWSAOVERLAPPED>(psb), NULL
	);
	
	if (SOCKET_ERROR == nRet && (ERROR_IO_PENDING != WSAGetLastError())) {
		return false;
	}

	return true;
}


//////////////////////////////////////////////////////////////////
// 비동기 Send Method
bool PerSockContext::SendPost(PerSockBuff* psb)
{
	DWORD dwNoOfByteSend = 0;
	psb->SetStatus(PerSockBuff::SOCK_SEND);

	int nRet = WSASend(
		sock_, psb->GetWsaBuf(), 1, &dwNoOfByteSend, 0, reinterpret_cast<LPWSAOVERLAPPED>(psb), NULL
	); 

	if (SOCKET_ERROR == nRet && (ERROR_IO_PENDING != WSAGetLastError())) {
		return false;
	}

	return true;
}


//////////////////////////////////////////////////////////////////
// 메세지 Recv 대기를 신청
bool PerSockContext::RecvWait()
{
	DWORD dwNoOfByteRecv = 0, dwFlag = 0;
	recvBuff.SetStatus(PerSockBuff::SOCK_RECV);

	recvBuff.SetTotalSize(recvBuff.GetBuffSize());

	int nRet = WSARecv(
		sock_, recvBuff.GetWsaBuf(), 1, &dwNoOfByteRecv, &dwFlag, 
		reinterpret_cast<LPWSAOVERLAPPED>(&recvBuff), NULL
	);

	if (SOCKET_ERROR == nRet) {
		if (WSA_IO_PENDING != WSAGetLastError()) {
			return false;
		}
	}
	return true;
}


