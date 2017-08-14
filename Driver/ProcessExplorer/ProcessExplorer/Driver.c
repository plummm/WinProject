/*++

Module Name:

    driver.c

Abstract:

    This file contains the driver entry points and callbacks.

Environment:

    Kernel-mode Driver Framework

--*/

#include "driver.h"
#include "driver.tmh"
#include "source.cpp"
#include "SSDT.h"
#include "Hooklib.h"
#include "Inline.h"
//#include "hookssdt.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (INIT, DriverEntry)
#pragma alloc_text (PAGE, ProcessExplorerEvtDeviceAdd)
#pragma alloc_text (PAGE, ProcessExplorerEvtDriverContextCleanup)
#endif

printList *head = NULL, *tail = NULL;


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

	if (printSize!=0)
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
			RtlStringCchPrintfW(temp1, MAX_BUFFER/sizeof(WCHAR), L"EPROCESS=%p, PID=%d, PPID=%ld, Name=%s\n",
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

NTSTATUS DispatchCreate(PDEVICE_OBJECT DriverObject, PIRP pIrp)
{
	DbgPrint("[KrnlProcExplr]DispatchCreate\n");
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS DispatchClose(PDEVICE_OBJECT DriverObject, PIRP pIrp)
{
	DbgPrint("[KrnlProcExplr]DispatchClose\n");
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS DispatchIoct(PDEVICE_OBJECT DriverObject, PIRP pIrp)
{
	NTSTATUS status = STATUS_INVALID_DEVICE_REQUEST;
	PIO_STACK_LOCATION pIrpStack;
	ULONG uIoControlCode;
	PVOID pIoBuffer;
	ULONG uInSize;
	ULONG uOutSize;
	DWORD sum_buffer = 0;

	DbgPrint("[KrnlProcExplr]DispatchIoct\n");
	pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
	uIoControlCode = pIrpStack->Parameters.DeviceIoControl.IoControlCode;
	pIoBuffer = pIrp->AssociatedIrp.SystemBuffer;
	uInSize = pIrpStack->Parameters.DeviceIoControl.InputBufferLength;
	uOutSize = pIrpStack->Parameters.DeviceIoControl.OutputBufferLength;
	switch (uIoControlCode)
	{
	case IOCTL_GET_PROCESS:  //222000
	{
		uOutSize = EnumProcess((WCHAR*)pIoBuffer, uOutSize);
		/*
		while (i < 262144)
		{
			head = (struct OutLink*)ExAllocatePool(PagedPool, sizeof(printList));
			head->buffer = (WCHAR*)ExAllocatePool(PagedPool, MAX_BUFFER);
			sum_buffer += MAX_BUFFER;
			RtlFillMemory(head->buffer, MAX_BUFFER, 0);
			i = EnumProcess(i, head->buffer);
			//LinkList(head, pIoBuffer);
			head->next = tail;
			tail = head;
		}
		for (printList* t = tail; t; t = t->next)
		{
			RtlStringCchCatNW(pIoBuffer, sum_buffer, t->buffer, MAX_BUFFER);
		}*/
		//uOutSize = sum_buffer;
		status = STATUS_SUCCESS;
		break;
	}
	case IOCTL_SAY_HELLO:  //222004
	{
		DbgPrint("Hello");
		DbgPrint("[KrnlProcExplr]output size:%d output addr:%x\n", uOutSize, pIoBuffer);
		DWORD dw = 0;
		//输入
		memcpy(&dw, pIoBuffer, sizeof(DWORD));
		//使用
		dw++;
		//输出
		memcpy(pIoBuffer, &dw, sizeof(DWORD));
		//返回通信状态
		status = STATUS_SUCCESS;
		break;
	}
	case IOCTL_GET_SSDTADDR: //222008
	{
		HookSSDT(0x29);
		status = STATUS_SUCCESS;
		break;
	}
	case IOCTL_UNHOOK_SSDTADDR:  //22200C
	{
		UnhookSSDT(0x29);
		status = STATUS_SUCCESS;
		break;
	}
	case IOCTL_GET_SSSDTADDR:  //222010
	{
		HookSSSDT(0x100f, MyNtUserPostMessage, 4);
		status = STATUS_SUCCESS;
		break;
	}
	case IOCTL_UNHOOK_SSSDTADDR:  //222014
	{
		UNHOOK_SSSDT(0x100f);
		status = STATUS_SUCCESS;
		break;
	}
	case IOCTL_INLINE_HOOK:
	{
		my_eprocess = (ULONG64)PsGetCurrentProcess();
		restore_raw_code = HookKernelApi(PsLookupProcessByProcessId, Fake_PsLookupProcessByProcessId, &ori_pslp, &patch_size);
		status = STATUS_SUCCESS;
		break;
	}
	default:
		break;
	}

	if (status == STATUS_SUCCESS)
		pIrp->IoStatus.Information = uOutSize;
	else
		pIrp->IoStatus.Information = 0;

	DbgPrint("[KrnlProcExplr] Outsize:%u\n", pIrp->IoStatus.Information);
	DbgPrint("[KrnlProcExplr] Output:%p\n", pIoBuffer);
	pIrp->IoStatus.Status = status;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return status;
}

VOID DriverUnload(PDRIVER_OBJECT pDriverObj)
{
	UNICODE_STRING strLink;
	DbgPrint("[KrnlProcExplr]DriverUnload\n");
	RtlInitUnicodeString(&strLink, LINK_NAME);
	IoDeleteSymbolicLink(&strLink);
	IoDeleteDevice(pDriverObj->DeviceObject);
}

NTSTATUS
DriverEntry(
    _In_ PDRIVER_OBJECT  DriverObject,
    _In_ PUNICODE_STRING RegistryPath
    )
/*++

Routine Description:
    DriverEntry initializes the driver and is the first routine called by the
    system after the driver is loaded. DriverEntry specifies the other entry
    points in the function driver, such as EvtDevice and DriverUnload.

Parameters Description:

    DriverObject - represents the instance of the function driver that is loaded
    into memory. DriverEntry must initialize members of DriverObject before it
    returns to the caller. DriverObject is allocated by the system before the
    driver is loaded, and it is released by the system after the system unloads
    the function driver from memory.

    RegistryPath - represents the driver specific path in the Registry.
    The function driver can use the path to store driver related data between
    reboots. The path does not store hardware instance specific data.

Return Value:

    STATUS_SUCCESS if successful,
    STATUS_UNSUCCESSFUL otherwise.

--*/
{
	NTSTATUS status = STATUS_SUCCESS;
	UNICODE_STRING ustrLinkName;
	UNICODE_STRING ustrDevName;
	PDEVICE_OBJECT pDevObj;

	DriverObject->MajorFunction[IRP_MJ_CREATE] = DispatchCreate;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = DispatchClose;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchIoct;
	DriverObject->DriverUnload = DriverUnload;

	RtlInitUnicodeString(&ustrDevName, DEVICE_NAME);
	status = IoCreateDevice(DriverObject, 0, &ustrDevName, FILE_DEVICE_UNKNOWN, 0, FALSE, &pDevObj);
	if (!NT_SUCCESS(status))
		return status;
	if (IoIsWdmVersionAvailable(1, 0x10))
		RtlInitUnicodeString(&ustrLinkName, LINK_GLOBAL_NAME);
	else
		RtlInitUnicodeString(&ustrLinkName, LINK_NAME);

	status = IoCreateSymbolicLink(&ustrLinkName, &ustrDevName);
	if (!NT_SUCCESS(status))
	{
		IoDeleteDevice(pDevObj);
		return status;
	}
	DbgPrint("[KrnlProcExplr]DriverEntry\n");
	return STATUS_SUCCESS;
}

