/*++

Module Name:

    driver.h

Abstract:

    This file contains the driver definitions.

Environment:

    Kernel-mode Driver Framework

--*/
#pragma once
#define INITGUID

#include <ntddk.h>
#include <wdf.h>
#include <stdio.h>
#include <windef.h>
#include <Ntstrsafe.h>

#include "device.h"
#include "queue.h"
#include "trace.h"

#define DEVICE_NAME			L"\\Device\\KrnlProcExplr"
#define LINK_NAME			L"\\DosDevices\\KrnlProcExplr"
#define LINK_GLOBAL_NAME	L"\\DosDevices\\Global\\KrnlProcExplr"

#define IOCTL_GET_PROCESS	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SAY_HELLO		CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_GET_SSDTADDR  CTL_CODE(FILE_DEVICE_UNKNOWN, 0x802, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define MAX_BUFFER 4096

#define SETBIT(x,y) x|=(1<<y)
#define CLRBIT(x,y) x&=~(1<<y)
#define GETBIT(x,y) (x & (1<<y))

NTKERNELAPI UCHAR* PsGetProcessImageFileName(IN PEPROCESS Process);
NTKERNELAPI HANDLE PsGetProcessInheritedFromUniqueProcessId(IN PEPROCESS Process);
NTKERNELAPI NTSTATUS PsLookupProcessByProcessId(HANDLE Id, PEPROCESS *Process);

struct OutList
{
	WCHAR* buffer;
	struct OutList* next;
};

typedef struct OutList printList;

EXTERN_C_START

//
// WDFDRIVER Events
//

DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD ProcessExplorerEvtDeviceAdd;
EVT_WDF_OBJECT_CONTEXT_CLEANUP ProcessExplorerEvtDriverContextCleanup;

EXTERN_C_END
