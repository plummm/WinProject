// RemoteMain.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>
#include <tchar.h>


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

BOOL InjectDll(DWORD dwPID, LPCTSTR szDllPath)
{
	HANDLE hProcess = NULL, hThread = NULL;
	HMODULE hMod = NULL;
	LPVOID pRemoteBuf = NULL;
	DWORD dwBufSize = (DWORD)(_tcslen(szDllPath) + 1) * sizeof(TCHAR);
	LPTHREAD_START_ROUTINE pThreadProc;

	if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID)))
	{
		_tprintf(L"OpenProcess(%d) failed!!! [%d]\n", dwPID, GetLastError());
		return FALSE;
	}
	_tprintf(L"hProcess:%x\n", hProcess);

	pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE);
	_tprintf(L"pRemoteBuf:%x\n", pRemoteBuf);

	WriteProcessMemory(hProcess, pRemoteBuf, (LPVOID)szDllPath, dwBufSize, NULL);
	printf("%s\n", szDllPath);

	pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "LoadLibraryW");\
	_tprintf(L"pThreadProc:%x\n", pThreadProc);

	hThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, pRemoteBuf, 0, NULL);
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	CloseHandle(hProcess);

	return TRUE;

}
int _tmain(int argc, TCHAR *argv[])
{
	if (argc != 3)
	{
		//printf("argv[0]:%s argv[1]:%s argv[2]:%s\n", argv[0], argv[1], argv[2]);
		_tprintf(L"USAGE : %s pid dll_path\n", argv[0]);
		return 1;
	}

	_tprintf(L"[+] Setting Debug Privileges [%ld]\n", SetDebugPrivileges());
	//printf("argv[0]:%s argv[1]:%s argv[2]:%s\n", argv[0], _tstol(argv[1]), argv[2]);
	if (InjectDll((DWORD)_tstol(argv[1]), argv[2]))
		_tprintf(TEXT("InjectDll(\"%s\") success!!!\n"), argv[2]);
	else
		_tprintf(L"InjectDll(\"%s\") failed!!!\n", argv[2]);

	system("pause");
	return 0;
}

