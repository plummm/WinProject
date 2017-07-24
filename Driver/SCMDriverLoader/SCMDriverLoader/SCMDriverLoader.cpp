// SCMDriverLoader.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "DrvCtrl.h"
#include <string.h>

#define DRIVERPATH 2
#define OPERATOR 1
#define DEFINE_CTL_CODE 3
#define INPUT 4

int _tmain(int argc, TCHAR *argv[])
{
	BOOL b;
	DWORD ctl = 0;
	DWORD x = 0, y = 0;
	DrvCtrl dc;
	WCHAR szSvcLnkName[MAX_PATH];
	WCHAR p[MAX_PATH];
	WCHAR *szSvcRawName;
	WCHAR szSysFile[MAX_PATH] = { 0, };

	if (argc < 3)
	{
		_tprintf(L"USAGE: SCMDriverLoader.exe [operator] example.sys [CTL_CODE] [Input]\n");
		_tprintf(L"       -start      load a driver\n");
		_tprintf(L"       -stop       unload a driver\n");
		return 0;
	}

	wcsncpy_s(szSysFile, MAX_PATH, argv[DRIVERPATH], wcslen(argv[DRIVERPATH]));
	szSvcRawName = wcsrchr(szSysFile, '\\') + 1;
	wcsncpy_s(p, MAX_PATH, szSvcRawName, wcsrchr(szSvcRawName, '.') - szSvcRawName);
	wsprintf(szSvcLnkName, L"\\\\.\\%s", p);

	if (lstrcmpW(L"-start", argv[OPERATOR]) == 0)
	{
		b = dc.Install(szSysFile, p, p);
		_tprintf(L"%d=LoadDriver\n", b);
		b = dc.Start();
		_tprintf(L"%d=StartDriver\n", b);
		b = dc.Open(szSvcLnkName);
		_tprintf(L"%d=OpenDriver\n", b);
		if (argc > 3)
		{
			if (argc >= 4)
			{
				ctl=(DWORD)_tstol(argv[DEFINE_CTL_CODE]);
				if (argc >= 5)
				{
					x = (DWORD)_tstol(argv[INPUT]);
					b = dc.IoControl(0x800, &x, sizeof(x), &y, sizeof(y));
					_tprintf(L"%d INPUT=%ld  OUTPUT=%ld\n", b, x, y);
				}
				else
				{
					b = dc.IoControl(0x800, 0, 0, 0, 0);
					_tprintf(L"%d INPUT=%ld  OUTPUT=%ld\n", b, x, y);
				}
			}
		}
	}
	
	if (lstrcmpW(L"-stop", argv[OPERATOR]) == 0)
	{
		b = dc.Open(szSvcLnkName);
		_tprintf(L"%d=CloseDriver\n", b);
		CloseHandle(dc.m_hDriver);
		b = dc.Uninstall(szSysFile, p);
		_tprintf(L"%d=StopDriver\n", b);
		b = dc.Remove();
		_tprintf(L"%d=UnloadDriver\n", b);
	}
    return 0;
}

