#pragma once
#include "Driver.h"
#include <windef.h>

ULONG MyProcessId = 0;
ULONG64 my_eprocess = NULL;
PVOID ori_pslp = NULL;
ULONG patch_size = 0;
PVOID restore_raw_code = NULL;

typedef struct _LARGE_STRING
{
	ULONG Length;
	ULONG MaximumLength : 31;
	ULONG bAnsi : 1;
	PVOID Buffer;
} LARGE_STRING, *PLARGE_STRING;

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

typedef NTSTATUS(__fastcall *PFPSLOOKUPPROCESSBYPROCESSID)(HANDLE ProcessId, PEPROCESS *Process);


PFNTUSERPOSTMESSAGE NtUserPostMessage = NULL;
PFNTUSERQUERYWINDOW NtUserQueryWindow = NULL;
//NTKERNELAPI ULONG64 NtUserQueryWindow(HWND WindowHandle, ULONG64 TypeInformation);

ULONG64 MyNtUserPostMessage(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if (NtUserQueryWindow(hWnd, 0) == MyProcessId && PsGetCurrentProcessId() != (HANDLE)MyProcessId)
	{
		DbgPrint("Do not fuck with me!");
		return 0;
	}
	else
	{
		//DbgPrint("OriNtUserPostMessage called!");
		return NtUserPostMessage(hWnd, Msg, wParam, lParam);
	}
}

NTSTATUS Fake_PsLookupProcessByProcessId(HANDLE ProcessId, PEPROCESS *Process)
{

	if (*Process == (PEPROCESS)my_eprocess)
	{
		*Process = 0;
		return STATUS_ACCESS_DENIED;
	}
	return ((PFPSLOOKUPPROCESSBYPROCESSID)ori_pslp)(ProcessId, Process);
}