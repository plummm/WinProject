// DllInject.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>

HANDLE hProcess = NULL, hThread = NULL;
HMODULE hMod = NULL;
LPVOID pRemoteBuf = NULL;
DWORD dwBufSize = 0;
LPTHREAD_START_ROUTINE pThreadProc;

bool _openProcess(DWORD dwPID, LPCTSTR szDllPath)
{
	dwBufSize = (DWORD)(_tcslen(szDllPath) + 1) * sizeof(TCHAR);
	int pBit = 0;

	if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID)))
	{
		_tprintf(L"OpenProcess(%d) failed!!! [%d]\n", dwPID, GetLastError());
		return FALSE;
	}
	_tprintf(L"hProcess:%x\n", hProcess);
	return TRUE;
}

void _virtualAlloc()
{
	pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE);
	_tprintf(L"pRemoteBuf:%x\n", pRemoteBuf);
}

void _writeProcessMemory(LPCTSTR szDllPath)
{
	WriteProcessMemory(hProcess, pRemoteBuf, (LPVOID)szDllPath, dwBufSize, NULL);
}

void _getLoadLibrary()
{
	pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "LoadLibraryW");
	_tprintf(L"pThreadProc:%x\n", pThreadProc);
}

void _injectDll()
{
	hThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, pRemoteBuf, 0, NULL);
	WaitForSingleObject(hThread, INFINITE);
}

void _closeHandles()
{
	CloseHandle(hThread);
	CloseHandle(hProcess);
}

BOOL InjectDll(DWORD dwPID, LPCTSTR szDllPath)
{
	if (_openProcess(dwPID, szDllPath))
	{
		_virtualAlloc();
		_writeProcessMemory(szDllPath);
		_getLoadLibrary();
		_injectDll();
		return TRUE;
	}

	return FALSE;

}

int SetDebugPrivileges(void) {
	TOKEN_PRIVILEGES priv = { 0 };
	HANDLE hToken = NULL;

	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
		priv.PrivilegeCount = 1;
		priv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		if (LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &priv.Privileges[0].Luid)) {
			if (AdjustTokenPrivileges(hToken, FALSE, &priv, 0, NULL, NULL) == 0) {
				printf("AdjustTokenPrivilege Error! [%u]\n", GetLastError());
			}
		}

		CloseHandle(hToken);
	}
	return GetLastError();
}


int _tmain(int argc, TCHAR *argv[])
{

	if (argc != 3)
	{
		//printf("argv[0]:%s argv[1]:%s argv[2]:%s\n", argv[0], argv[1], argv[2]);
		_tprintf(L"USAGE : %s dll_path pid\n", argv[0]);
		return 1;
	}

	_tprintf(L"[+] Setting Debug Privileges [%ld]\n", SetDebugPrivileges());
	//printf("argv[0]:%s argv[1]:%s argv[2]:%s\n", argv[0], _tstol(argv[1]), argv[2]);
	if (InjectDll((DWORD)_tstol(argv[2]), argv[1]))
		_tprintf(TEXT("InjectDll(\"%s\") success!!!\n"), argv[1]);
	else
		_tprintf(L"InjectDll(\"%s\") failed!!!\n", argv[1]);

	system("pause");
	return 0;
}