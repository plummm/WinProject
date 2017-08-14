#pragma once
#include "Driver.h"

ULONGLONG sssdt_old = 0, SSSDTBase = 0;
PSYSTEM_SERVICE_TABLE KeServiceDescriptorTableShadow;
typedef ULONG64(__fastcall *PFNTUSERQUERYWINDOW)
(
	IN HWND		WindowHandle,
	IN ULONG64	TypeInformation
	);

typedef ULONG64(__fastcall *PFNTUSERPOSTMESSAGE)
(
	HWND 	hWnd,
	UINT 	Msg,
	WPARAM 	wParam,
	LPARAM 	lParam
	);

VOID GetKernalModuleBase(char* lpModuleName, ULONG64 *ByRefBase, ULONG *ByRefSize)
{
	typedef struct _SYSTEM_MODULE_INFORMATION_ENTRY
	{
		ULONG Unknow1;
		ULONG Unknow2;
		ULONG Unknow3;
		ULONG Unknow4;
		PVOID Base;
		ULONG Size;
		ULONG Flags;
		USHORT Index;
		USHORT NameLength;
		USHORT LoadCount;
		USHORT ModuleNameOffset;
		char ImageName[256];
	} SYSTEM_MODULE_INFORMATION_ENTRY, *PSYSTEM_MODULE_INFORMATION_ENTRY;
	typedef struct _SYSTEM_MODULE_INFORMATION
	{
		ULONG Count;
		SYSTEM_MODULE_INFORMATION_ENTRY Module[1];
	} SYSTEM_MODULE_INFORMATION, *PSYSTEM_MODULE_INFORMATION;
	typedef struct _KLDR_DATA_TABLE_ENTRY
	{
		LIST_ENTRY64 InLoadOrderLinks;
		ULONG64 __Undefined1;
		ULONG64 __Undefined2;
		ULONG64 __Undefined3;
		ULONG64 NonPagedDebugInfo;
		ULONG64 DllBase;
		ULONG64 EntryPoint;
		ULONG SizeOfImage;
		UNICODE_STRING FullDllName;
		UNICODE_STRING BaseDllName;
		ULONG   Flags;
		USHORT  LoadCount;
		USHORT  __Undefined5;
		ULONG64 __Undefined6;
		ULONG   CheckSum;
		ULONG   __padding1;
		ULONG   TimeDateStamp;
		ULONG   __padding2;
	}KLDR_DATA_TABLE_ENTRY, *PKLDR_DATA_TABLE_ENTRY;

	ULONG NeedSize, i, ModuleCount, BufferSize = 0x5000;
	PVOID pBuffer = NULL;
	PCHAR pDrvName = NULL;
	NTSTATUS Result;
	PSYSTEM_MODULE_INFORMATION pSystemModuleInformation;

	do
	{
		pBuffer = ExAllocatePool(NonPagedPool, BufferSize);
		if (pBuffer == NULL)
			return;

		Result = ZwQuerySystemInformation(11, pBuffer, BufferSize, &NeedSize);
		if (Result == STATUS_INFO_LENGTH_MISMATCH)
		{
			ExFreePool(pBuffer);
			BufferSize *= 2;
		}
		else if (!NT_SUCCESS(Result))
		{
			ExFreePool(pBuffer);
			return;
		}
	} while (Result == STATUS_INFO_LENGTH_MISMATCH);

	pSystemModuleInformation = (PSYSTEM_MODULE_INFORMATION)pBuffer;
	ModuleCount = pSystemModuleInformation->Count;
	for (i = 0; i < ModuleCount; i++)
	{
		if ((ULONG64)(pSystemModuleInformation->Module[i].Base) >(ULONG64)0x8000000000000000)
		{
			pDrvName = pSystemModuleInformation->Module[i].ImageName + pSystemModuleInformation->Module[i].ModuleNameOffset;
			if (_stricmp(pDrvName, lpModuleName) == 0)
			{
				*ByRefBase = (ULONG64)pSystemModuleInformation->Module[i].Base;
				*ByRefSize = pSystemModuleInformation->Module[i].Size;
				goto exit_sub;
			}
		}
	}
exit_sub:
	ExFreePool(pBuffer);
}

void SafeMemcpy(PVOID dst, PVOID src, DWORD length)
{
	KIRQL irql;
	irql = WPOFFx64();
	//RtlMoveMemory(dst, src, length);
	memcpy(dst, src, length);
	WPONx64(irql);
}

ULONGLONG GetKeServiceDescriptorTableShadow64()
{
	PUCHAR StartSearchAddress = (PUCHAR)__readmsr(0xC0000082);
	PUCHAR EndSearchAddress = StartSearchAddress + 0x500;
	PUCHAR i = NULL;
	UCHAR b1 = 0, b2 = 0, b3 = 0;
	ULONG templong = 0;
	ULONGLONG addr = 0;


	for (i = StartSearchAddress; i < EndSearchAddress; i++)
	{
		if (MmIsAddressValid(i) && MmIsAddressValid(i + 1) && MmIsAddressValid(i + 2))
		{
			b1 = *i;
			b2 = *(i + 1);
			b3 = *(i + 2);
			if (b1 == 0x4c && b2 == 0x8d && b3 == 0x1d)
			{
				memcpy(&templong, i + 3, 4);
				addr = (ULONGLONG)templong + (ULONGLONG)i + 7;
				return addr;
			}
		}
	}
}

ULONGLONG GetSSSDTFunctionAddress64(ULONG Index)
{
	ULONGLONG W32pServiceTable = 0, qwTemp = 0;
	LONG dwTemp = 0;
	PSYSTEM_SERVICE_TABLE pWin32k;
	ULONG ul64W32pServiceTable = 0;

	pWin32k = (PSYSTEM_SERVICE_TABLE)((ULONGLONG)KeServiceDescriptorTableShadow + sizeof(SYSTEM_SERVICE_TABLE));
	W32pServiceTable = (ULONGLONG)(pWin32k->ServiceTableBase);
	ul64W32pServiceTable = W32pServiceTable;
	SSSDTBase = W32pServiceTable;
	qwTemp = W32pServiceTable + 4 * (Index - 0x1000);
	dwTemp = *(PLONG)qwTemp;
	dwTemp = dwTemp >> 4;
	qwTemp = W32pServiceTable + (LONG64)dwTemp;
	return qwTemp;
}

ULONG64 FindFreeSpace(ULONG64 StartAddress, ULONG64 Length)
{
	UCHAR c = 0;
	ULONG64 i = 0, qw = 0;
	unsigned char* Code = (unsigned char*)StartAddress;

	for (i = StartAddress; i < StartAddress + Length; i++)
	{
		for (unsigned int i = 0, j = 0; i < Length; i++)
		{
			if (Code[i] == 0x90 || Code[i] == 0xCC)  //NOP or INT3
				j++;
			else
				j = 0;
			if (j == 12)
				return (PVOID)((ULONG_PTR)StartAddress + i - 12 + 1);
		}

			/*
			RtlMoveMemory(&qw, (PVOID)(i + 1), 8);
			if (qw == 0x9090909090909090)
				return i + 1;
				*/
	}
	return 0;
}

VOID ModifySSSDT(ULONG64 Index, ULONG64 Address, CHAR ParamCount)
{
	CHAR b = 0, bits[4] = { 0 };
	LONG i;

	ULONGLONG W32pServiceTable = 0, qwTemp = 0;
	LONG dwTemp = 0;
	PSYSTEM_SERVICE_TABLE pWin32k;
	KIRQL irql;
	pWin32k = (PSYSTEM_SERVICE_TABLE)((ULONG64)KeServiceDescriptorTableShadow
		+ sizeof(SYSTEM_SERVICE_TABLE));
	W32pServiceTable = (ULONG64)(pWin32k->ServiceTableBase);
	qwTemp = W32pServiceTable + 4 * (Index - 0x1000);
	dwTemp = (LONG)(Address - W32pServiceTable);
	dwTemp = dwTemp << 4;

	/*
	if (ParamCount > 4)
		ParamCount -= 4;
	else
		ParamCount = 0;

	memcpy(&b, &dwTemp, 1);
	for (i = 0; i < 4; i++)
	{
		bits[i] = GETBIT(ParamCount, i);
		if (bits[i])
			SETBIT(b, i);
		else
			CLRBIT(b, i);
	}

	memcpy(&dwTemp, &b, i);
	*/

	irql = WPOFFx64();
	*(PLONG)qwTemp = dwTemp;
	WPONx64(irql);
}

ULONG RvaToSection(IMAGE_NT_HEADERS* pNtHdr, ULONG dwRVA)
{
	USHORT wSections;
	PIMAGE_SECTION_HEADER pSectionHdr;
	pSectionHdr = IMAGE_FIRST_SECTION(pNtHdr);
	wSections = pNtHdr->FileHeader.NumberOfSections;
	for (int i = 0; i < wSections; i++)
	{
		if (pSectionHdr[i].VirtualAddress <= dwRVA)
			if ((pSectionHdr[i].VirtualAddress + pSectionHdr[i].Misc.VirtualSize) > dwRVA)
			{
				return i;
			}
	}
	return (ULONG)-1;
}

ULONG64 GetPageBase(PVOID lpHeader, ULONG* CodeSize, PVOID ptr)
{
	if ((unsigned char*)ptr < (unsigned char*)lpHeader)
		return 0;
	ULONG dwRva = (ULONG)((unsigned char*)ptr - (unsigned char*)lpHeader);
	IMAGE_DOS_HEADER* pdh = (IMAGE_DOS_HEADER*)lpHeader;
	if (pdh->e_magic != IMAGE_DOS_SIGNATURE)
		return 0;
	IMAGE_NT_HEADERS* pnth = (IMAGE_NT_HEADERS*)((unsigned char*)lpHeader + pdh->e_lfanew);
	if (pnth->Signature != IMAGE_NT_SIGNATURE)
		return 0;
	IMAGE_SECTION_HEADER* psh = IMAGE_FIRST_SECTION(pnth);
	int section = RvaToSection(pnth, dwRva);
	if (section == -1)
		return 0;
	if (CodeSize)
		*CodeSize = psh[section].SizeOfRawData;
	return (PVOID)((unsigned char*)lpHeader + psh[section].VirtualAddress);
}

ULONG64 GetCaveHook(ULONG64 fake_func)
{
	UCHAR jmp_code[] = "\x48\xB8\xFF\xFF\xFF\xFF"\
		"\xFF\xFF\xFF\x00\xFF\xE0";
	ULONG64 FreeSpace = 0, OriFunctionAddress = 0;
	ULONG64	Win32kBase = 0, CodeStart = 0;
	ULONG	Win32kSize = 0, CodeSize = 0;
	LONG lng = 0;

	GetKernalModuleBase("win32k.sys", &Win32kBase, &Win32kSize);
	DbgPrint("Win32kBase: %p\n", Win32kBase);
	if (Win32kBase == 0 || Win32kSize == 0)
		return 0;
	CodeStart = GetPageBase(Win32kBase, &CodeSize, (PVOID)sssdt_old);
	FreeSpace = FindFreeSpace(CodeStart, CodeSize);
	DbgPrint("FreeSpace: %p", FreeSpace);
	if (FreeSpace == 0)
		return 0;
	//SafeMemcpy((PVOID)FreeSpace, &fake_func, 8);
	//lng = (LONG)(fake_func - (sssdt_old - 6) - 6); //第一个-6是为了写入jmp，第二个-6是
	memcpy(&jmp_code[2], &fake_func, 8);
	SafeMemcpy((PVOID)FreeSpace, jmp_code, 12);
	return FreeSpace;
}

VOID HookSSSDT(ULONGLONG index, ULONG64 fake_func, CHAR ParamCount)
{
	KIRQL irql;
	ULONGLONG dwtmp = 0;
	PULONG ServiceTableBase = NULL;
	ULONGLONG ssdt = NULL, FreeSpace = NULL;
	ULONG64 sssdt_userquerywindow = NULL;
	extern PFNTUSERQUERYWINDOW NtUserQueryWindow;
	extern PFNTUSERPOSTMESSAGE NtUserPostMessage;

	KeServiceDescriptorTableShadow = (PSYSTEM_SERVICE_TABLE)GetKeServiceDescriptorTableShadow64();
	//NtTerminateProcess = (PFTERMINATEPROCESS)GetSSSDTFunctionAddress64(index);
	sssdt_old = GetSSSDTFunctionAddress64(index);
	sssdt_userquerywindow = GetSSSDTFunctionAddress64(0x1010);
	NtUserQueryWindow = sssdt_userquerywindow;
	NtUserPostMessage = sssdt_old;
	FreeSpace = GetCaveHook(fake_func);
	ModifySSSDT(index, FreeSpace, ParamCount);
	DbgPrint("SSSDT Hook Successfully!");
}

VOID UNHOOK_SSSDT(ULONG64 index, ULONG OriFunctionAddress, CHAR ParamCount)
{
	ModifySSSDT(index, (ULONG64)OriFunctionAddress, ParamCount);
	DbgPrint("UNHOOK_SSSDT successfully");
}

