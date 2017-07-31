#pragma once

#include "Driver.h"
#include <intrin.h>

typedef struct _SYSTEM_SERVICE_TABLE {
	PVOID  		ServiceTableBase;
	PVOID  		ServiceCounterTableBase;
	ULONGLONG  	NumberOfServices;
	PVOID  		ParamTableBase;
} SYSTEM_SERVICE_TABLE, *PSYSTEM_SERVICE_TABLE;

PSYSTEM_SERVICE_TABLE KeServiceDescriptorTable;

typedef struct _Hook {
	ULONGLONG SSDTindex;
	ULONGLONG SSDTold;
	ULONGLONG SSDTnew;
	ULONGLONG SSDTaddress;
	PVOID SSDTbase;
} Hook_table, *PHook_Table;

typedef NTSTATUS (__fastcall *PFTERMINATEPROCESS)(
	_In_ HANDLE hProcess,
	_In_ NTSTATUS uExitCode
);

PFTERMINATEPROCESS NtTerminateProcess = NULL;

KIRQL WPOFFx64()
{
	KIRQL irql = KeRaiseIrqlToDpcLevel();
	UINT64 cr0 = __readcr0();
	cr0 &= 0xfffffffffffeffff;
	__writecr0(cr0);
	_disable();
	return irql;
}

void WPONx64(KIRQL irql)
{
	UINT64 cr0 = __readcr0();
	cr0 |= 0x10000;
	_enable();
	__writecr0(cr0);
	KeLowerIrql(irql);
}

void ShowStuff0(LONG int32num)
{
	CHAR b[4] = { 0 };
	memcpy(&b[0], (PUCHAR)(&int32num) + 0, 1); DbgPrint("b[0] & 0xF=%ld\n", b[0] & 0xF);
	memcpy(&b[1], (PUCHAR)(&int32num) + 1, 1); DbgPrint("b[1] & 0xF=%ld\n", b[1] & 0xF);
	memcpy(&b[2], (PUCHAR)(&int32num) + 2, 1); DbgPrint("b[2] & 0xF=%ld\n", b[2] & 0xF);
	memcpy(&b[3], (PUCHAR)(&int32num) + 3, 1); DbgPrint("b[3] & 0xF=%ld\n", b[3] & 0xF);
}

PHook_Table hHook;


/*
ULONG GetOffsetAddress(ULONGLONG FuncAddr, CHAR ParamCount)
{
	LONG dwtmp = 0, i;
	CHAR b = 0, bits[4] = { 0 };
	PULONG ServiceTableBase = NULL;
	ServiceTableBase = KeServiceDescriptorTable->ServiceTableBase;
	dwtmp = (LONG)FuncAddr - (ULONGLONG)ServiceTableBase;
	dwtmp = dwtmp << 4;
	if (ParamCount > 4)
		ParamCount = ParamCount - 4;
	else
		ParamCount = 0;

	memcpy(&b, &dwtmp, 1);
	for (i = 0; i < 4; i++)
	{
		bits[i] = GETBIT(ParamCount, i);
		if (bits[i])
			SETBIT(b, i);
		else
			CLRBIT(b, i);
	}
	memcpy(&dwtmp, &b, 1);
	return dwtmp;
}*/

ULONG GetOffsetAddress(ULONGLONG FuncAddr)
{
	ULONG dwtmp = 0;
	PULONG ServiceTableBase = NULL;
	ServiceTableBase = (PULONG)KeServiceDescriptorTable->ServiceTableBase;
	dwtmp = (ULONG)(FuncAddr - (ULONGLONG)ServiceTableBase);
	return dwtmp << 4;
}

NTSTATUS __fastcall MyNtTerminateProcess(IN HANDLE ProcessHandle, IN NTSTATUS ExitStatus)
{
	PEPROCESS Process;
	NTSTATUS st = ObReferenceObjectByHandle(
		ProcessHandle, 
		0, 
		*PsProcessType, 
		KernelMode, 
		&Process, 
		NULL);
	if (NT_SUCCESS(st))
	{
		DbgPrint("FileName:%s\n", PsGetProcessImageFileName(Process));
		if (!_stricmp(PsGetProcessImageFileName(Process), "calc.exe"))
			return STATUS_ACCESS_DENIED;
		else
			return NtTerminateProcess(ProcessHandle, ExitStatus);
	}
	else
		return NtTerminateProcess(ProcessHandle, ExitStatus);
}


VOID FuckKeBugCheckEx(ULONGLONG fake_func)
{
	KIRQL irql;
	ULONGLONG myfunc;
	UCHAR jmp_code[] = "\x48\xB8\xFF\xFF\xFF\xFF"\
		"\xFF\xFF\xFF\x00\xFF\xE0";

	DbgPrint("[KrnlProcExplr]FuckKeBugCheckEx\n");
	myfunc = (ULONGLONG)fake_func;
	memcpy(jmp_code + 2, &myfunc, 8);
	irql = WPOFFx64();
	memset(KeBugCheckEx, 0x90, 15);
	memcpy(KeBugCheckEx, jmp_code, 12);
	WPONx64(irql);
}

ULONGLONG GetKeServiceDescriptorTable64()
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
			if (b1 == 0x4c && b2 == 0x8d && b3 == 0x15)
			{
				memcpy(&templong, i + 3, 4);
				addr = (ULONGLONG)templong + (ULONGLONG)i + 7;
				return addr;
			}
		}
	}
	return 0;
}

ULONGLONG GetSSDTFunctionAddress64(ULONGLONG NtApiIndex)
{
	LONG dwtmp = 0;
	ULONGLONG add = 0;
	PULONG ServiceTableBase = NULL;

	ServiceTableBase = KeServiceDescriptorTable->ServiceTableBase;
	//hHook->SSDTbase = ssdt->ServiceTableBase;
	dwtmp = ServiceTableBase[NtApiIndex];
	DbgPrint("ServiceTableBase:%llx\n", ServiceTableBase);
	ShowStuff0(dwtmp);
	dwtmp = dwtmp >> 4;
	add = ((LONGLONG)dwtmp + (ULONGLONG)ServiceTableBase);//&0xFFFFFFF0;
	return add;
	/*__asm
	{
	mov rax, rcx
	lea r10, [rdx]
	mov edi, eax
	shr edi, 7
	and edi, 0x20
	mov r10, qword ptr[r10 + rdi]
	movsxd r11, dword prt[r10 + rax]
	mov rax, r11
	sar r11, 4
	add r10, r11
	mov rax, r10
	ret
	}*/
}

VOID HookSSDT(ULONGLONG index)
{
	KIRQL irql;
	ULONGLONG dwtmp = 0;
	PULONG ServiceTableBase = NULL;
	ULONGLONG ssdt = NULL;
	ULONGLONG ssdt_old = NULL;

	KeServiceDescriptorTable = GetKeServiceDescriptorTable64();
	NtTerminateProcess = (PFTERMINATEPROCESS)GetSSDTFunctionAddress64(index);
	FuckKeBugCheckEx(MyNtTerminateProcess);
	ServiceTableBase = KeServiceDescriptorTable->ServiceTableBase;
	ssdt_old = ServiceTableBase[index];
	irql = WPOFFx64();
	ServiceTableBase[index] = GetOffsetAddress((ULONGLONG)KeBugCheckEx);
	WPONx64(irql);
	DbgPrint("[KrnlProcExplr]HookSSDT{%d} to %llx\n",
		index,
		(ULONGLONG)KeBugCheckEx,
		GetSSDTFunctionAddress64(index, ssdt));
}

VOID UnhookSSDT(ULONGLONG index)
{
	KIRQL irql;
	PULONG ServiceTableBase = NULL;

	ServiceTableBase = KeServiceDescriptorTable->ServiceTableBase;
	irql = WPOFFx64();
	ServiceTableBase[index] = GetOffsetAddress((ULONGLONG)NtTerminateProcess);
	WPONx64(irql);
}


VOID TitanHook(const char* apiname, void* newfunc)
{
	PULONG SSDTBase = NULL;

	KeServiceDescriptorTable = GetKeServiceDescriptorTable64();
	SSDTBase = KeServiceDescriptorTable->ServiceTableBase;
#ifdef _WIN64
	static ULONG CodeSize = 0;
	static PVOID CodeStart = 0;
	if (!CodeStart)
	{
		ULONG_PTR Lowest = SSDTBase;
		ULONG_PTR Highest = Lowest + 0x0FFFFFFF;
		CodeSize = 0;
		//CodeStart = GetPageBase
	}
#else
#endif
}




