// AsyncSockClient.h: interface for the AsyncSockClient class.
//
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// Copyright (c) 2003 by cdecl (byung-kyu kim)
// EMail : cdecl@interpark.com
//////////////////////////////////////////////////////////////////////
#ifndef __ASYNCSOCKCLIENT_H__BY_CDECL__
#define __ASYNCSOCKCLIENT_H__BY_CDECL__

#include "WinSockInit.h"

#include <cassert>
#include <string>
#include <algorithm>


namespace GLASS {


//////////////////////////////////////////////////////////////////
// �񵿱� Ŭ���̾�Ʈ ���� 
class AsyncSockClient  
{
public:
	enum { SZ_SIZE = 1024 };

public:
	AsyncSockClient();
	virtual ~AsyncSockClient();

public:
	//////////////////////////////////////////////////////////////////
	// Socket�� �ݴ´� 
	void Close();

	//////////////////////////////////////////////////////////////////
	// ������ �Ѵ� 
	bool Connect(const std::string &strIP, int nPort);

public:
	//////////////////////////////////////////////////////////////////
	// �޼����� ������ Callback
	virtual void OnRecv() {}

	//////////////////////////////////////////////////////////////////
	// ������ �������� 
	virtual void OnClose() {}

	//////////////////////////////////////////////////////////////////
	// �޼��� ������ 
	virtual int Send(const char *pBuff, size_t nLen, int nFlags = 0);

	//////////////////////////////////////////////////////////////////
	// �޼��� �ޱ�
	virtual int Recv(char *pBuff, int nBuffLen, int nFlags = 0);

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


	//////////////////////////////////////////////////////////////////
	// ������ recv ���۳����� ũ��
	ULONG GetSockBuffLen() const;

	//////////////////////////////////////////////////////////////////
	// ��ȿ�� �����̳�?
	bool IsValidSocket() const;

	
private:
	//////////////////////////////////////////////////////////////////
	// Main Run Thread 
	static UINT __stdcall MainRun(LPVOID lpvParam);

private:
	HandleManage hmThread_;
	UINT nThreadID_;

protected:
	SOCKET sock_;
};


}	// namespace GLASS 

#endif // __ASYNCSOCKCLIENT_H__BY_CDECL__

