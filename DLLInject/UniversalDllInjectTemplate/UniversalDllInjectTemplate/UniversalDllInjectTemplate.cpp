// UniversalDllInjectTemplate.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>
#include <tchar.h>
#include <Psapi.h>
#include <TlHelp32.h>

/*
记录导出表信息
*/
typedef struct _IAT_EAT_INFO
{
	TCHAR ModuleName[256];
	char FuncName[64];
	ULONG64 Address;
	ULONG64 RecordAddr;
	ULONG64 ModBase;
} IAT_EAT_INFO, *PIAT_EAT_INFO;

/*
取出所有模块，并且比较是否为kernel.dll
*/
HMODULE GetRemoteModuleHandleByProcessHandle(HANDLE hProcess, TCHAR *szModuleName)
{
	HMODULE hMods[1024] = { 0 };
	DWORD cbNeeded = 0, i = 0;
	TCHAR szModName[MAX_PATH];
	TCHAR *lastString;

	if (EnumProcessModulesEx(hProcess, hMods, sizeof(hMods), &cbNeeded, LIST_MODULES_ALL))
	{
		for (i = 0; i <= cbNeeded / sizeof(HMODULE); i++)
		{
			if (GetModuleFileNameEx(hProcess, hMods[i], szModName, sizeof(szModName)))
			{
				lastString = wcsrchr(szModName, '\\');
				if (!_wcsicmp(lastString + 1, szModuleName))
				{
					return hMods[i];
				}
			}
		}
	}
	return NULL;
}

/*
导出32位程序中的LoadLibraryW
*/
long GetProcessExportTable32(HANDLE hProcess, TCHAR *ModuleName, IAT_EAT_INFO tbinfo[], int tb_info_max)
{
	ULONG count = 0, mBase = 0;
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)new BYTE[sizeof(IMAGE_DOS_HEADER)];
	PIMAGE_NT_HEADERS32 pNtHeader = (PIMAGE_NT_HEADERS32)new BYTE[sizeof(IMAGE_NT_HEADERS32)];
	PIMAGE_EXPORT_DIRECTORY pExport = (PIMAGE_EXPORT_DIRECTORY)new BYTE[sizeof(IMAGE_EXPORT_DIRECTORY)];
	DWORD dwStup = 0, dwOffset = 0;
	char strName[130];

	mBase =(ULONG) GetRemoteModuleHandleByProcessHandle(hProcess, ModuleName);
	if (!mBase)
	{
		_tprintf(L"GetRemoteModuleHandleByProcessHandle Failed!\n");
		system("pause");
	}

	//读取DOS头和NT头
	ReadProcessMemory(hProcess, (PVOID)mBase, pDosHeader, sizeof(IMAGE_DOS_HEADER), NULL);
	ReadProcessMemory(hProcess, (PVOID)(mBase + pDosHeader->e_lfanew), pNtHeader, sizeof(IMAGE_NT_HEADERS32), NULL);
	if (pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress == 0)
	{
		_tprintf(L"Directory entry export do not exist!\n");
		system("pause");
	}

	//读取EXPORT表
	ReadProcessMemory(hProcess,
		(PVOID)(mBase + pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress),
		pExport,
		sizeof(IMAGE_EXPORT_DIRECTORY),
		NULL);
	ReadProcessMemory(hProcess, (PVOID)(mBase + pExport->Name), strName, sizeof(strName), NULL);

	if (pExport->NumberOfNames < 0 || pExport->NumberOfNames>8192)
	{
		return 0;
	}

	//遍历导出表，保存所有数据
	for (DWORD i = 0; i < pExport->NumberOfNames; i++)
	{
		char bFuncName[100];
		ULONG ulPointer;
		USHORT usFuncId;
		ULONG64 ulFuncAddr;
		ReadProcessMemory(hProcess, (PVOID)(mBase + pExport->AddressOfNames + i * 4), &ulPointer, 4, 0);
		RtlZeroMemory(bFuncName, sizeof(bFuncName));
		ReadProcessMemory(hProcess, (PVOID)(mBase + ulPointer), bFuncName, 100, 0);
		ReadProcessMemory(hProcess, (PVOID)(mBase + pExport->AddressOfNameOrdinals + i * 2), &usFuncId, 2, 0);
		ReadProcessMemory(hProcess, (PVOID)(mBase + pExport->AddressOfFunctions + 4 * usFuncId), &ulPointer, 4, 0);
		ulFuncAddr = mBase + ulPointer;
		//printf("\t%llx\t%s\n", ulFuncAddr, bFuncName);
		//MultiByteToWideChar(0, 0, bFuncName);
		wcscpy_s(tbinfo[count].ModuleName, sizeof(tbinfo[count].ModuleName), ModuleName);
		strcpy_s(tbinfo[count].FuncName, sizeof(tbinfo[count].FuncName), bFuncName);
		tbinfo[count].Address = ulFuncAddr;
		tbinfo[count].RecordAddr = (ULONG64)(mBase + pExport->AddressOfFunctions + 4 * usFuncId);
		tbinfo[count].ModBase = mBase;
		count++;
		if (count > (ULONG)tb_info_max)
			break;
	}
	delete[]pDosHeader;
	delete[]pExport;
	delete[]pNtHeader;
	return count;
}

/*
返回LoadLibraryW位置
*/
ULONG64 GetProcAddressIn32BitProcess(HANDLE hProcess, TCHAR *ModuleName, char *FuncName)
{
	ULONG64 RetAddr = 0;
	PIAT_EAT_INFO pInfo = (PIAT_EAT_INFO)malloc(4096 * sizeof(IAT_EAT_INFO));
	long count = GetProcessExportTable32(hProcess, ModuleName, pInfo, 2048);
	if (!count)
		return NULL;
	for (long i = 0; i < count; i++)
	{
		if (!_stricmp(pInfo[i].FuncName, FuncName))
		{
			RetAddr = pInfo[i].Address;
			break;
		}
	}
	free(pInfo);
	return RetAddr;
}

/*
检查目标程序是32位还是64位
*/
int ProcessBitCheck(HANDLE hProcess)
{
	BOOL Wow64Flag = NULL;
	IsWow64Process(hProcess, &Wow64Flag);
	return Wow64Flag;
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
BOOL InjectDll(DWORD dwPID, LPCTSTR szDllPath)
{
	HANDLE hProcess = NULL, hThread = NULL;
	HMODULE hMod = NULL;
	LPVOID pRemoteBuf = NULL;
	DWORD dwBufSize = (DWORD)(_tcslen(szDllPath) + 1) * sizeof(TCHAR);
	LPTHREAD_START_ROUTINE pThreadProc;
	int pBit = 0;

	if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID)))
	{
		_tprintf(L"OpenProcess(%d) failed!!! [%d]\n", dwPID, GetLastError());
		return FALSE;
	}
	_tprintf(L"hProcess:%x\n", hProcess);

	pBit = ProcessBitCheck(hProcess);
	
	pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE);
	_tprintf(L"pRemoteBuf:%x\n", pRemoteBuf);

	WriteProcessMemory(hProcess, pRemoteBuf, (LPVOID)szDllPath, dwBufSize, NULL);

	if (pBit==X64)
		pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "LoadLibraryW");
	else
		pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddressIn32BitProcess(hProcess, L"kernel32.dll", "LoadLibraryW");
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


