// SCMDriverLoader.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "DrvCtrl.h"
#include <WinBase.h>

#define DRIVERPATH 3
#define OPERATOR 2
#define DEFINE_CTL_CODE 4
#define INPUT 5
#define DRIVERTYPE 1

int _tmain(int argc, TCHAR *argv[])
{
	BOOL b;
	DWORD ctl = 0;
	DWORD x = 0, y = 0;
	WCHAR *output = NULL;
	DrvCtrl dc;
	WCHAR szSvcLnkName[MAX_PATH];
	WCHAR p[MAX_PATH];
	WCHAR *szSvcRawName;
	WCHAR szSysFile[MAX_PATH] = { 0, };
	WCHAR szInfName[MAX_PATH] = { 0, };

	if (argc < 4)
	{
		_tprintf(L"USAGE: SCMDriverLoader.exe driver_type [operator] c:/example.sys [CTL_CODE] [Input]\n");
		_tprintf(L"   NT    -start      load an nt driver\n");
		_tprintf(L"   WDM   -stop       unload an wdm driver\n");
		return 0;
	}

	wcsncpy_s(szSysFile, MAX_PATH, argv[DRIVERPATH], wcslen(argv[DRIVERPATH]));
	szSvcRawName = wcsrchr(szSysFile, '\\') + 1;
	wcsncpy_s(p, MAX_PATH, szSvcRawName, wcsrchr(szSvcRawName, '.') - szSvcRawName);
	wsprintf(szSvcLnkName, L"\\\\.\\%s", p);
	if (lstrcmpW(L"NT", argv[DRIVERTYPE]) == 0)
	{
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
					ctl = (DWORD)_tstol(argv[DEFINE_CTL_CODE]);
					if (argc >= 5)
					{
						x = (DWORD)_tstol(argv[INPUT]);
						b = dc.IoControl(ctl, &x, sizeof(x), &y, sizeof(y));
						_tprintf(L"%d INPUT=%ld  OUTPUT=%ld ctl=%d\n", b, x, y, ctl);
					}
					else
					{
						//output = (printList*)malloc(sizeof(printList));
						int size = dc.Get_size(ctl);
						//int size = 9000;
						output = (WCHAR*)malloc(size);
						memset(output, 0, size);
						b = dc.IoControl(ctl, NULL, 0, output, size);
						_tprintf(L"%d ctl=%d\n", b, ctl);
						//for (printList* t=(printList*)output;t;t=t->next)
						//std::wcout << (output+100) << std::endl;
						wprintf(L"%d OUTPUT=\n%s\n", b, (WCHAR*)output);
						//DWORD dw = 0;
						//WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), output, wcslen(output), &dw, NULL);
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
	}
	if (lstrcmpW(L"WDM", argv[DRIVERTYPE]) == 0)
	{
		if (lstrcmpW(L"-start", argv[OPERATOR]) == 0)
		{
			wcsncpy_s(szInfName, MAX_PATH, szSysFile, wcslen(szSysFile) - 3);
			wcscat_s(szInfName, L".inf");
			b = dc.LoadWdmInf(szInfName, szSvcRawName);
			_tprintf(L"%d=LoadWdmDrv\n", b);
			b = dc.Install(szSysFile, p, p);
			_tprintf(L"%d=Load\n", b);
			b = dc.Start();
			_tprintf(L"%d=Start\n", b);
		}

		if (lstrcmpW(L"-stop", argv[OPERATOR]) == 0)
		{
			b = dc.Open(szSvcLnkName);
			_tprintf(L"%d=CloseDriver\n", b);
			CloseHandle(dc.m_hDriver);
			b = dc.Uninstall(szSysFile, p);
			_tprintf(L"%d=StopDriver\n", b);
		}

	}
    return 0;
}

