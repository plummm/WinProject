#pragma once
#include "Driver.h"
#include <windef.h>

ULONG MyProcessId = 0;

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