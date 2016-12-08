// GameSafeDemo.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include <string.h>
#include <tchar.h>
#include <TlHelp32.h>
#include <Psapi.h>
#include <Shlwapi.h>
#include "GameSafeDemo.h"

#pragma comment(lib,"Gdi32.lib")
#pragma comment(lib,"Psapi.lib")
#pragma comment(lib,"Shlwapi.lib")

#define DEFGDI32 "gdi32.dll"
#define DEFKERNEL32 "kernel32.dll"

#define DEFCCD "CreateCompatibleDC"
#define DEFCPW "CreateProcessW"
#define DEFCPA "CreateProcessA"

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

//const TCHAR ProtectProcess[10] = { L"explorer.exe", L"svchost.exe", L"" };



typedef BOOL(WINAPI *PFCREATEPROCESS)(
	LPCTSTR               lpApplicationName,
	LPTSTR                lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL                  bInheritHandles,
	DWORD                 dwCreationFlags,
	LPVOID                lpEnvironment,
	LPCTSTR               lpCurrentDirectory,
	LPSTARTUPINFO         lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation
	);

typedef HDC(WINAPI *PFCREATECOMPATIBLEDC)(
	HDC hdc
	);

struct CopyScreenData
{
	DWORD g_dwCopyScreenFlag = 0;
	TCHAR *FileName = NULL;
};

CopyScreenData CopyScreenIntercapt;
FARPROC g_function = NULL;
DWORD g_tmpFlag = 0;
TCHAR dll_path[MAX_PATH];
BYTE g_pOrgBytes[5] = { 0, };



//当检测到截屏行为时调用该函数设置检测结果dwCopyScreenFlag，非0则标识检测到截图，0表示没检测到截图
VOID OnCheckCopyScreenCallback(TCHAR *FileName, DWORD dwCopyScreenFlag)
{
	if (CopyScreenIntercapt.g_dwCopyScreenFlag == 0)
	{
		CopyScreenIntercapt.g_dwCopyScreenFlag = dwCopyScreenFlag;
		CopyScreenIntercapt.FileName = FileName;
		g_tmpFlag = 0;
	}
}


int SetDebugPrivileges(void) {
	TOKEN_PRIVILEGES priv = { 0 };
	HANDLE hToken = NULL;
	TCHAR szOutputText[1024] = { 0 };

	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
		priv.PrivilegeCount = 1;
		priv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

		if (LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &priv.Privileges[0].Luid)) {
			if (AdjustTokenPrivileges(hToken, FALSE, &priv, 0, NULL, NULL) == 0) {
				wsprintfW(szOutputText, L"AdjustTokenPrivilege Error! [%u]\n", GetLastError());
				OutputDebugStringW(szOutputText);
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
	TCHAR szOutputText[1024] = { 0 };
	int pBit = 0;

	if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID)))
	{
		wsprintfW(szOutputText, L"OpenProcess(%d) failed!!! [%d]\n", dwPID, GetLastError());
		OutputDebugStringW(szOutputText);
		return FALSE;
	}
	//_tprintf(L"hProcess:%x\n", hProcess);

	pRemoteBuf = VirtualAllocEx(hProcess, NULL, dwBufSize, MEM_COMMIT, PAGE_READWRITE);
	//_tprintf(L"pRemoteBuf:%x\n", pRemoteBuf);

	WriteProcessMemory(hProcess, pRemoteBuf, (LPVOID)szDllPath, dwBufSize, NULL);

	pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "LoadLibraryW");
	//_tprintf(L"pThreadProc:%x\n", pThreadProc);

	hThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, pRemoteBuf, 0, NULL);
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	CloseHandle(hProcess);

	return TRUE;
}

BOOL Hook(LPCSTR szDllName, LPCSTR szFuncName, PROC pfnNew, PBYTE pOrgBytes)
{
	FARPROC pfnOrg;
	DWORD dwOldProtect;
	DWORDLONG dwAddress;
	BYTE pBuf[5] = { 0xE9,0, };
	TCHAR szOutputText[1024] = { 0, };
	PBYTE pByte;
	
	pfnOrg = (FARPROC)GetProcAddress(GetModuleHandleA(szDllName), szFuncName);
	pByte = (PBYTE)pfnOrg;
	
	if (pByte[0] == 0xE9)
		return FALSE;

	OutputDebugStringW(L"Start Hook");

	//将内存改为可读写
	VirtualProtect((LPVOID)pfnOrg, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect);

	//备份原有数据
	memcpy(pOrgBytes, pfnOrg, 5);

	//新的函数地址-HOOK函数地址-5(JMP XXXXXXXX的字节)=相对偏移量
	dwAddress = (DWORDLONG)pfnNew - (DWORDLONG)pfnOrg - 5;

	//将相对偏移量存入pBuf
	memcpy(&pBuf[1], &dwAddress, 4);

	//将pBuf(被修改后的代码)写入内存，现在代码已经修改成功了
	memcpy(pfnOrg, pBuf, 5);

	//恢复内存初始权限
	VirtualProtect((LPVOID)pfnOrg, 5, dwOldProtect, &dwOldProtect);

	return TRUE;
	
}

BOOL UnHook(LPCSTR szDllName, LPCSTR szFuncName, PBYTE pOrgByte)
{
	FARPROC pFunc;
	DWORD dwOldProtect;
	PBYTE pByte;

	pFunc = GetProcAddress(GetModuleHandleA(szDllName), szFuncName);
	pByte = (PBYTE)pFunc;

	if (pByte[0] != 0xE9)
		return FALSE;

	OutputDebugStringW(L"Start Unhook!");

	VirtualProtect((LPVOID)pFunc, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect);

	//将备份的数据恢复回内存
	memcpy(pFunc, pOrgByte, 5);

	VirtualProtect((LPVOID)pFunc, 5, dwOldProtect, &dwOldProtect);

	return TRUE;
}

HDC MyCCD(
	HDC hdc
)
{
	FARPROC pFunc;
	HDC status;

	//MessageBox(NULL, "Hook Successfully!", "Succeed", MB_OK);
	UnHook(DEFGDI32, DEFCCD, g_pOrgBytes);

	pFunc = GetProcAddress(GetModuleHandleA(DEFGDI32), DEFCCD);
	OutputDebugStringW(L"Hook CD Succeed!");
	g_tmpFlag = 1;
	status = ((PFCREATECOMPATIBLEDC)pFunc)((HDC)1);

	Hook(DEFGDI32, DEFCCD, (PROC)MyCCD, g_pOrgBytes);
	return status;
}



BOOL MyCPW(
	LPCTSTR               lpApplicationName,
	LPTSTR                lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL                  bInheritHandles,
	DWORD                 dwCreationFlags,
	LPVOID                lpEnvironment,
	LPCTSTR               lpCurrentDirectory,
	LPSTARTUPINFO         lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation)
{
	FARPROC pFunc;
	BOOL status;
//	LPPROCESS_INFORMATION lpProcessInformation;
	TCHAR szOutputText[1024] = { 0 };

	UnHook(DEFKERNEL32, DEFCPW, g_pOrgBytes);

	pFunc = GetProcAddress(GetModuleHandleA(DEFKERNEL32), DEFCPW);
	OutputDebugStringW(L"Hook CPW Succeed!");
	status = ((PFCREATEPROCESS)pFunc)(
		lpApplicationName,
		lpCommandLine,
		lpProcessAttributes,
		lpThreadAttributes,
		bInheritHandles,
		dwCreationFlags,
		lpEnvironment,
		lpCurrentDirectory,
		lpStartupInfo,
		lpProcessInformation);

	wsprintfW(szOutputText, L"CPW child pid:%d\n", lpProcessInformation->dwProcessId);
	OutputDebugStringW(szOutputText);

	if (InjectDll(lpProcessInformation->dwProcessId, dll_path))
	{
		wsprintfW(szOutputText, L"Thread %d inject successfully!", lpProcessInformation->dwProcessId);
		OutputDebugStringW(szOutputText);
	}
	else
	{
		wsprintfW(szOutputText, L"Thread %d inject failed!", lpProcessInformation->dwProcessId);
		OutputDebugStringW(szOutputText);
	}
	

	Hook(DEFKERNEL32, DEFCPW, (PROC)MyCPW, g_pOrgBytes);
	return status;
}

BOOL MyCPA(
	LPCTSTR               lpApplicationName,
	LPTSTR                lpCommandLine,
	LPSECURITY_ATTRIBUTES lpProcessAttributes,
	LPSECURITY_ATTRIBUTES lpThreadAttributes,
	BOOL                  bInheritHandles,
	DWORD                 dwCreationFlags,
	LPVOID                lpEnvironment,
	LPCTSTR               lpCurrentDirectory,
	LPSTARTUPINFO         lpStartupInfo,
	LPPROCESS_INFORMATION lpProcessInformation)
{
	FARPROC pFunc;
	BOOL status;
	//	LPPROCESS_INFORMATION lpProcessInformation;
	TCHAR szOutputText[1024] = { 0 };

	UnHook(DEFKERNEL32, DEFCPA, g_pOrgBytes);

	pFunc = GetProcAddress(GetModuleHandleA(DEFKERNEL32), DEFCPA);
	OutputDebugStringW(L"Hook CPA Succeed!");
	status = ((PFCREATEPROCESS)pFunc)(
		lpApplicationName,
		lpCommandLine,
		lpProcessAttributes,
		lpThreadAttributes,
		bInheritHandles,
		dwCreationFlags,
		lpEnvironment,
		lpCurrentDirectory,
		lpStartupInfo,
		lpProcessInformation);

	wsprintfW(szOutputText, L"CPA child pid:%d\n", lpProcessInformation->dwProcessId);
	OutputDebugStringW(szOutputText);

	if (InjectDll(lpProcessInformation->dwProcessId, dll_path))
	{
		wsprintfW(szOutputText, L"Thread %d inject successfully!", lpProcessInformation->dwProcessId);
		OutputDebugStringW(szOutputText);
	}
	else
	{
		wsprintfW(szOutputText, L"Thread %d inject failed!", lpProcessInformation->dwProcessId);
		OutputDebugStringW(szOutputText);
	}

	Hook(DEFKERNEL32, DEFCPA, (PROC)MyCPA, g_pOrgBytes);
	return status;
}

void EnumProcess()
{
	HANDLE hProcess, hProcessSnap;
	PROCESSENTRY32 pe32;
	DWORD dwPriorityClass;
	DWORD explrPid = 0;
	PVOID targetProcess;
	TCHAR szOutputText[1024] = { 0 };
	long long flag = 0;


	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		OutputDebugStringW(L"CreateToolhelp32Snapshot failed!\n");
	}
	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hProcessSnap, &pe32))
	{
		OutputDebugStringW(L"Process32First failed!\n");
	}
	do
	{
		if (wcscmp(pe32.szExeFile, L"explorer.exe")==0)
		{
			explrPid = pe32.th32ProcessID;
			if (InjectDll(pe32.th32ProcessID, dll_path))
			{
				OutputDebugStringW(L"explorer.exe injected successfully!\n");
			}
			else
			{
				OutputDebugStringW(L"explorer.exe injected failed!\n");
			}
			break;
		}

	} while (Process32Next(hProcessSnap, &pe32));


	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		OutputDebugStringW(L"CreateToolhelp32Snapshot failed!\n");
	}
	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hProcessSnap, &pe32))
	{
		OutputDebugStringW(L"Process32First failed!\n");
	}
	do
	{
		if (wcscmp(pe32.szExeFile, L"explorer.exe") != 0 && pe32.th32ProcessID!=GetCurrentThreadId() && pe32.th32ParentProcessID == explrPid)
		{
			//_tprintf(L"This pid is %d and the name is %s , My parent is %d\n", pe32.th32ProcessID, pe32.szExeFile, pe32.th32ParentProcessID);
			if (InjectDll(pe32.th32ProcessID, dll_path))
			{
				wsprintfW(szOutputText, L"Thread %d inject successfully!", pe32.th32ProcessID);
				OutputDebugStringW(szOutputText);
			}
			else
			{
				wsprintfW(szOutputText, L"%s failed!", pe32.szExeFile);
				OutputDebugStringW(szOutputText);
			}
		}

		if (g_tmpFlag == 1) 
			OnCheckCopyScreenCallback(pe32.szExeFile, g_tmpFlag);

		

	} while (Process32Next(hProcessSnap, &pe32));
	
	return;
}

//截屏检测初始化逻辑在该函数内实现
extern "C" GAMESAFEDEMO_API DWORD InitCheck()
{
	TCHAR output[MAX_PATH];
	TCHAR *p = NULL;

	OutputDebugStringW(L"InitCheck");

	SetDebugPrivileges();

	//GetCurrentDirectoryW(MAX_PATH, dll_path);
	GetModuleFileNameW((HINSTANCE)&__ImageBase,dll_path,MAX_PATH);
	//wcscat_s(dll_path, MAX_PATH, L"\\GameSafeDemo.dll");
	OutputDebugStringW(dll_path);
	if (PathFileExistsW(dll_path) == FALSE)
	{
		OutputDebugStringW(L"Can't find dll file,please put dll in the same folder as your exe!");
		return 0;
	}

	EnumProcess();
	//if (InjectDll(pid, dll_path))
		//_tprintf(L"Thread %d inject successfully!", pid);
	return 0;
}



//应该不用修改这个函数
extern "C" GAMESAFEDEMO_API DWORD GetCheckResult()
{
	TCHAR szOutputText[1024] = {0};
	DWORD dwCopyScreenFlag = CopyScreenIntercapt.g_dwCopyScreenFlag;
	CopyScreenIntercapt.g_dwCopyScreenFlag = 0;
	wsprintfW(szOutputText, L"%s GetCheckResult(%d)", CopyScreenIntercapt.FileName, dwCopyScreenFlag);
	OutputDebugStringW(szOutputText);
	return dwCopyScreenFlag;
}




BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	TCHAR exe_path[MAX_PATH] = { 0, };
	TCHAR *p = NULL;
	TCHAR szOutputText[1024] = { 0 };

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		SetDebugPrivileges();
		GetModuleFileNameW((HINSTANCE)&__ImageBase, dll_path, MAX_PATH);
		//wcscat_s(dll_path, MAX_PATH, L"\\GameSafeDemo.dll");
		OutputDebugStringW(dll_path);
		if (PathFileExistsW(dll_path) == FALSE)
		{
			OutputDebugStringW(L"Can't find dll file,please put dll in the same folder as your exe!");
			return 0;
		}

		HANDLE handle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
			FALSE, GetCurrentProcessId());
		GetModuleFileNameEx(handle, 0, exe_path, MAX_PATH);
		p = wcsrchr(exe_path, '\\');
		OutputDebugStringW(p + 1);
		wsprintfW(szOutputText, L"Begin Hook %s", p + 1);
		OutputDebugStringW(szOutputText);
		if (wcscmp(p + 1, L"explorer.exe") == 0)
		{
			Hook(DEFKERNEL32, DEFCPW, (PROC)MyCPW, g_pOrgBytes);
			Hook(DEFKERNEL32, DEFCPA, (PROC)MyCPA, g_pOrgBytes);
			
		}
		else
		{
			Hook(DEFGDI32, DEFCCD, (PROC)MyCCD, g_pOrgBytes);
		}
		break;
	}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		//OutputDebugStringW(L"Exit Hook");
		//UnHook_CCD(DEFDLL, DEFCCD, g_pOrgBytes);
		break;
	}
	return TRUE;
}