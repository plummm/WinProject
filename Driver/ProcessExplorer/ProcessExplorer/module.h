#pragma once
#include "Driver.h"

char *block_list = NULL;

BOOLEAN VxkCopyMemory(PVOID pDestination, PVOID pSourceAddress, SIZE_T SizeOfCopy)
{
	RtlCopyMemory(pSourceAddress, pDestination, SizeOfCopy);
	return TRUE;

	/*user space address project to kernel space*/
	/*
	PMDL pMdl = NULL;
	PVOID pSafeAddress = NULL;
	pMdl = IoAllocateMdl(pSourceAddress, (ULONG)SizeOfCopy, FALSE, FALSE, NULL);
	if (!pMdl)
		return FALSE;

	__try
	{
		MmProbeAndLockPages(pMdl, KernelMode, IoReadAccess);
	}
	__except(EXCEPTION_EXECUTE_HANDLER)
	{
		IoFreeMdl(pMdl);
		return FALSE;
	}

	pSafeAddress = MmGetSystemAddressForMdlSafe(pMdl, NormalPagePriority);
	if (!pSafeAddress)
		return FALSE;
	RtlCopyMemory(pDestination, pSafeAddress, SizeOfCopy);
	MmUnlockPages(pMdl);
	IoFreeMdl(pMdl);
	return TRUE;
	*/
}

void DenyLoadDriver(PVOID DriverEntry)
{
	UCHAR fuck[] = "\xB8\x22\x00\x00\xc0\xc3";
	VxkCopyMemory(DriverEntry, fuck, sizeof(fuck));
}

VOID UnicodeToChar(PUNICODE_STRING dst, char *src)
{
	ANSI_STRING string;
	size_t size = 0;
	RtlUnicodeStringToAnsiString(&string, dst, TRUE);
	//size = strlen(string.Buffer) < buffer_size ? strlen(string.Buffer) : buffer_size;
	//DbgPrint("Length:%d", strlen(string.Buffer));
	if (strlen(string.Buffer)<=MAX_PATH)
		strcpy(src, string.Buffer);
	RtlFreeAnsiString(&string);
}

PVOID GetEntryByImageBase(PVOID ImageBase)
{
	PIMAGE_DOS_HEADER pDOSHeader;
	PIMAGE_NT_HEADERS64 pNTHeader;
	PVOID pEntryPoint;

	pDOSHeader = (PIMAGE_DOS_HEADER)ImageBase;
	pNTHeader = (PIMAGE_NT_HEADERS)((ULONG64)ImageBase + pDOSHeader->e_lfanew);
	pEntryPoint = (PVOID)((ULONG64)ImageBase + pNTHeader->OptionalHeader.AddressOfEntryPoint);
	return pEntryPoint;
}

void SetLoadImageNotifyRoutine(
	_In_opt_ PUNICODE_STRING FullImageName,
	_In_     HANDLE          ProcessId,
	_In_     PIMAGE_INFO     ImageInfo,
	_In_     BOOLEAN         Create
)
{
	PVOID pDrvEntry = NULL, pDllEntry = NULL;
	char szFullImageName[MAX_PATH] = { 0 };

	if (FullImageName != NULL && MmIsAddressValid(FullImageName))
	{
		if (ProcessId == 0)
		{
			pDrvEntry = GetEntryByImageBase(ImageInfo->ImageBase);
			DbgPrint("[LoadImageNotifyX64]%wZ  DriverEntry: %p\n", FullImageName, pDrvEntry);
			UnicodeToChar(FullImageName, szFullImageName);
			if (strstr(_strlwr(szFullImageName), block_list))
			{
				DbgPrint("Deny loading %s", block_list);
				DenyLoadDriver(pDrvEntry);
			}
		}
		else
		{
			pDllEntry = GetEntryByImageBase(ImageInfo->ImageBase);
			DbgPrint("[LoadImageNotifyX64]%wZ  DllEntry: %p\n", FullImageName, pDllEntry);
			UnicodeToChar(FullImageName, szFullImageName);
			if (strstr(_strlwr(szFullImageName), block_list))
			{
				DbgPrint("Deny loading %s", block_list);
				DenyLoadDriver(pDllEntry);
			}
		}
	}
}