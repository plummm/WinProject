#pragma once
#include "Driver.h"
#include "LDE64.h"

VOID *GetFunctionAddr(PCWSTR FunctionName)
{
	UNICODE_STRING UnicodeFunctionName;
	RtlInitUnicodeString(&UnicodeFunctionName, FunctionName);
	return MmGetSystemRoutineAddress(&UnicodeFunctionName);
}

ULONG GetPatchSize(PUCHAR Address)
{
	ULONG LenCount = 0, Len = 0;
	while (LenCount<=12)
	{
		Len = LDE(Address, 64);
		Address = Address + Len;
		LenCount = LenCount + Len;
	}
	return LenCount;
}

PVOID  HookKernelApi(IN PVOID ApiAddress, IN PVOID Fake_ApiAddress,
	OUT PVOID *Original_ApiAddress, OUT ULONG *PatchSize)
{
	KIRQL irql;
	UINT64 tmpv;
	PVOID raw_code, ori_func;
	UCHAR jmp_code[] = "\x48\xB8\xFF\xFF\xFF\xFF"\
		"\xFF\xFF\xFF\x00\xFF\xE0";
	
	LDE_init();
	*PatchSize = GetPatchSize((PUCHAR)ApiAddress);
	raw_code = ExAllocatePoolWithTag(NonPagedPool, *PatchSize, 'lnTE');
	irql = WPOFFx64();
	memcpy(raw_code, ApiAddress, *PatchSize);
	WPONx64(irql);
	ori_func = ExAllocatePoolWithTag(NonPagedPool, *PatchSize + 12, 'lnTE');
	RtlFillMemory(ori_func, *PatchSize + 12, 0x90);
	tmpv = (ULONG64)ApiAddress + *PatchSize;
	memcpy(jmp_code + 2, &tmpv, 8);
	memcpy(ori_func, raw_code, *PatchSize);
	memcpy((PUCHAR)ori_func + *PatchSize, jmp_code, 12);
	*Original_ApiAddress = ori_func;
	//patch original func
	tmpv = (ULONG64)Fake_ApiAddress;
	memcpy(jmp_code + 2, &tmpv, 8);
	irql = WPOFFx64();
	RtlFillMemory(ApiAddress, *PatchSize, 0x90);
	memcpy(ApiAddress, jmp_code, 12);
	WPONx64(irql);
	return raw_code;
}

VOID UnhookKernelApi(IN PVOID ApiAddress,
	IN PVOID OriCode,
	IN ULONG *PatchSize)
{
	KIRQL irql;
	irql = WPOFFx64();
	memcpy(ApiAddress, OriCode, *PatchSize);
	WPONx64(irql);
}