// APIHook.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include <Windows.h>
#include <tchar.h>

FARPROC g_function = NULL;
typedef BOOL (WINAPI *PFWRITEFILE)(HANDLE hFile,
	LPCVOID lpBuffer,
	DWORD nNumberOfBytesToWrite,
	LPDWORD lpNumberOfBytesWritten,
	LPOVERLAPPED lpOverlapped);


BOOL Hook(LPCSTR szDllName, PROC pfnOrg, PROC pfnNew)
{
	HMODULE hMod;
	LPCSTR szLibName;
	PIMAGE_IMPORT_DESCRIPTOR pImportDesc;
	PIMAGE_THUNK_DATA pThunk;
	DWORD dwOldProtect;
	DWORD dwRVA;
	PBYTE pAddr;

	//找到IAT位置
	hMod = GetModuleHandle(NULL);
	pAddr = (PBYTE)hMod;

	pAddr += *((DWORD*)&pAddr[0x3C]); //NT头

	dwRVA = *((DWORD*)&pAddr[0x80]);  //IAT的偏移量

	pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)hMod + dwRVA);

	//遍历寻找Kernel32.dll
	for (; pImportDesc->Name; pImportDesc++)
	{
		szLibName = (LPCSTR)((DWORD)hMod + pImportDesc->Name);

		if (!_stricmp(szLibName, szDllName))
		{

			pThunk = (PIMAGE_THUNK_DATA)((DWORD)hMod + pImportDesc->FirstThunk);

			//遍历寻找WriteFile
			for (; pThunk->u1.Function; pThunk++)
			{
				if (pThunk->u1.Function == (DWORD)pfnOrg)
				{
					VirtualProtect((LPVOID)&pThunk->u1.Function, 4, PAGE_EXECUTE_READWRITE, &dwOldProtect);
					pThunk->u1.Function = (DWORD)pfnNew;
					VirtualProtect((LPVOID)&pThunk->u1.Function, 4, dwOldProtect, &dwOldProtect);
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

//MyWriteFile需与WriteFile的格式完全一样
BOOL WINAPI MyWriteFile(HANDLE hFile, 
	LPCVOID lpBuffer, //指向记事本数据的指针
	DWORD nNumberOfBytesToWrite, //数据的长度
	LPDWORD lpNumberOfBytesWritten, 
	LPOVERLAPPED lpOverlapped)
{
	MessageBox(NULL, L"Hook successfully!", L"Hello", MB_OK);
	for (DWORD i = 0; i < nNumberOfBytesToWrite; i++)
	{
		if (0x61 <= *((char *)lpBuffer+i) && *((char *)lpBuffer+i) <= 0x7A) //枚举每一个数据是否是小写字母
			*((char *)lpBuffer + i) -= 0x20;  //转大写
		
	}
	return ((PFWRITEFILE)g_function)(hFile, lpBuffer, nNumberOfBytesToWrite, lpNumberOfBytesWritten, lpOverlapped);
}
BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		//获取WriteFile原始地址
		g_function = GetProcAddress(GetModuleHandle(L"kernel32.dll"), "WriteFile"); 
		//Hook该函数
		Hook("kernel32.dll", g_function, (PROC)MyWriteFile);
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		//解Hook
		Hook("kernel32.dll", (PROC)MyWriteFile, g_function);
		break;
	}
	return TRUE;
}

