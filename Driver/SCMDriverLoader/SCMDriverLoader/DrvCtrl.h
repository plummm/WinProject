#pragma once
#include <Windows.h>

class DrvCtrl
{
public:
	DWORD m_dwLastError;
	PWCHAR m_pSysPath;
	PWCHAR m_pServiceName;
	PWCHAR m_pDisplayName;
	HANDLE m_hDriver;
	SC_HANDLE m_hSCManager;
	SC_HANDLE m_hService;
	DWORD CTL_CODE_GEN(DWORD lngFunction);
	BOOL GetSvcHandle(PWCHAR pServiceName);
	BOOL Install(PWCHAR pSysPath, PWCHAR pServiceName, PWCHAR pDisplayName);
	BOOL Uninstall(PWCHAR pSysPath, PWCHAR pServiceName);
	void PrintError();
	BOOL Start();
	BOOL Stop();
	BOOL Remove();
	BOOL Open(PWCHAR pLinkName);
	BOOL IoControl(DWORD dwIoCode, PVOID InBuff, DWORD InBufferLen,
		PVOID OutBuff, DWORD OutBuffLen);
	DrvCtrl();
	~DrvCtrl();
};

