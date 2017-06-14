//////////////////////////////////////////////////////////////////////
// Copyright (c) 2003 by cdecl (byung-kyu kim)
// EMail : cdecl@interpark.com
//////////////////////////////////////////////////////////////////////

#ifndef __WINSOCKINIT_H__BY_CDECL__
#define __WINSOCKINIT_H__BY_CDECL__

#pragma comment(lib, "ws2_32.lib")		// ws2_32.lib 모듈 추가 
#include <winsock2.h> 
#include <process.h>

#include <string>

namespace GLASS {

//////////////////////////////////////////////////////////////////////
// 윈속 초기화 클래스
//////////////////////////////////////////////////////////////////////
class WinSock
{
public:
	~WinSock() 
	{ 
		WSACleanup(); 
	}

public:
	static WinSock& Init()	// WinSock 정적 인스턴스 생성 
	{
		static WinSock _init;
		return _init;
	}

private:
	WinSock() 
	{
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);	// WINSOCK 2.2 로 초기화 	
	}

	// 인스턴스를 생성하지 못하게 하기위해 
	WinSock(const WinSock &);
	WinSock& operator=(const WinSock &);
};



//////////////////////////////////////////////////////////////////////
// Exception 클래스 
//////////////////////////////////////////////////////////////////////
class WinSockException 
{
public:
	explicit WinSockException(const std::string str = "Unknown Exception", int n = 0) : str_(str), n_(n) {}
	WinSockException(const WinSockException &wse) : str_(wse.str_), n_(wse.n_) {}
	virtual ~WinSockException() {}

public:
	WinSockException& operator=(const WinSockException &wse) 
	{
		if (this == &wse) {
			return *this;
		}

		str_ = wse.str_;
		n_ = wse.n_;
		return *this;
	}

	std::string GetMessage() const
	{
		return str_;
	}

	int GetErrCode() const
	{
		return n_;
	}

private:
	std::string str_;
	int n_;
};



//////////////////////////////////////////////////////////////////////
// CriticalSection 관리 클래스 
//////////////////////////////////////////////////////////////////////
class CriticalSection
{
public:
	CriticalSection() 
	{
		InitializeCriticalSection(&cs_);
	}

	~CriticalSection() 
	{
		DeleteCriticalSection(&cs_);
	}

	LPCRITICAL_SECTION operator&()
	{
		return &cs_;
	}

private:
	CriticalSection(const CriticalSection&);
	CriticalSection& operator=(const CriticalSection&);

private:
	CRITICAL_SECTION cs_;
};


//////////////////////////////////////////////////////////////////////
// CriticalSection 락 관리 클래스 
// 리소스의 획득은 초기화 이다 
//////////////////////////////////////////////////////////////////////
class CriticalSectionLock
{
public:
	//////////////////////////////////////////////////////////////////
	// EnterCriticalSection
	CriticalSectionLock(LPCRITICAL_SECTION pcs) : pcs_(pcs) 
	{ 
		EnterCriticalSection(pcs_); 
	}

	//////////////////////////////////////////////////////////////////
	// LeaveCriticalSection
	~CriticalSectionLock() 
	{ 
		LeaveCriticalSection(pcs_); 
	}

private:
	LPCRITICAL_SECTION pcs_;
};



//////////////////////////////////////////////////////////////////////
// HANDLE 관리 클래스 
// 소멸자에서 CloseHandle() 호출 
//////////////////////////////////////////////////////////////////////
class HandleManage
{
public:
	explicit HandleManage(HANDLE h = NULL) : handle_(h) {}
	~HandleManage() { Release(); }

	void Release() 
	{
		if (handle_) {
			::CloseHandle(handle_);
			handle_ = NULL;
		}
	}

	operator HANDLE() const
	{
		return handle_;
	}

	const HANDLE* operator&() const
	{
		return &handle_;
	}

	bool operator!() const 
	{
		return handle_ == NULL;
	}

	HandleManage& operator=(HANDLE h) 
	{
		if (handle_ == h) {
			return *this;
		}

		Release();
		handle_ = h;
		return *this;
	}

private:
	HandleManage(const HandleManage&);
	HandleManage& operator=(const HandleManage&);

private:
	HANDLE handle_;
};



}	// namespace GLASS

#endif	// __WINSOCKINIT_H__BY_CDECL__


