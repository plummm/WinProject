#pragma once
#include "Driver.h"

PEPROCESS LookupProcess(HANDLE Pid)
{
	PEPROCESS eprocess = NULL;
	if (NT_SUCCESS(PsLookupProcessByProcessId(Pid, &eprocess)))
		return eprocess;
	else
		return NULL;
}

DWORD EnumProcess(WCHAR* printBuffer, DWORD printSize)
{
	ULONG i = 0;
	PEPROCESS eproc = NULL;
	WCHAR *temp1, *temp2;
	DWORD sum_buffer = 0;
	int wcount = 0;

	if (printSize != 0)
		wcount = printSize / sizeof(WCHAR);

	temp1 = (WCHAR*)ExAllocatePool(PagedPool, MAX_BUFFER);
	temp2 = (WCHAR*)ExAllocatePool(PagedPool, printSize);
	RtlFillMemory(temp1, MAX_BUFFER, 0);
	RtlFillMemory(temp2, printSize, 0);
	for (i = 1; i < 262144; i += 4)
	{
		eproc = LookupProcess((HANDLE)i);
		if (eproc != NULL)
		{
			RtlStringCchPrintfW(temp1, MAX_BUFFER / sizeof(WCHAR), L"EPROCESS=%p, PID=%d, PPID=%ld, Name=%s\n",
				eproc,
				(DWORD)PsGetProcessId(eproc),
				(DWORD)PsGetProcessInheritedFromUniqueProcessId(eproc),
				PsGetProcessImageFileName(eproc));
			sum_buffer += wcslen(temp1) * sizeof(WCHAR);

			RtlStringCchPrintfW(printBuffer, wcount, L"%s%s",
				temp2,
				temp1);

			RtlMoveMemory(temp2, printBuffer, printSize);
			ObDereferenceObject(eproc);
		}
	}
	return sum_buffer;
}

PCHAR GetProcessNameByProcessId(HANDLE ProcessId)
{
	NTSTATUS st = STATUS_UNSUCCESSFUL;
	PEPROCESS ProcessObj = NULL;
	PCHAR string = NULL;
	st = PsLookupProcessByProcessId(ProcessId, &ProcessObj);
	if (NT_SUCCESS(st))
	{
		string = PsGetProcessImageFileName(ProcessObj);
		ObfDereferenceObject(ProcessObj);
	}
	return string;
}

void SetCreateProcessNotifyRoutineEx(
	_In_        HANDLE                 ParentId,
	_In_        HANDLE                 ProcessId,
	_Inout_opt_ PPS_CREATE_NOTIFY_INFO CreateInfo
)
{
	NTSTATUS st = 0;
	HANDLE hProcess = NULL;
	OBJECT_ATTRIBUTES oa = { 0 };
	CLIENT_ID ClientId = { 0 };
	char xxx[MAX_PATH] = { 0 };

	if (CreateInfo != NULL)
	{
		DbgPrint("[monitor_create_process_x64][%ld]%s CreateProcess: %wZ",
			CreateInfo->ParentProcessId,
			GetProcessNameByProcessId(CreateInfo->ParentProcessId),
			CreateInfo->ImageFileName);
		strcpy_s(xxx, MAX_PATH, PsGetProcessImageFileName(ParentId));
		if (!_stricmp(xxx, "calc.exe"))
		{
			DbgPrint("Create process denied!");
			CreateInfo->CreationStatus = STATUS_UNSUCCESSFUL;
		}
	}
	else
	{
		DbgPrint("[monitor_create_process_x64]进程退出: %s", PsGetProcessImageFileName(ParentId));
	}
}