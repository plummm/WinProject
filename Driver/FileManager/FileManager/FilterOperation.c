#include "FilterOperation.h"

FLT_PREOP_CALLBACK_STATUS
FileManagerPreCreate(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Flt_CompletionContext_Outptr_ PVOID *CompletionContext
)
{
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(CompletionContext);
	PAGED_CODE();
	{
		UCHAR MajorFunction = 0;
		ULONG Options = 0;
		PFLT_FILE_NAME_INFORMATION nameInfo;
		PEPROCESS eprocess;
		WCHAR pTempBuf[512] = { 0 };
		WCHAR *pNonPageBuf = NULL, *pTemp = pTempBuf;
		UCHAR cProcNameBuf[512] = { 0 };
		UCHAR *cProcName = cProcNameBuf;

		MajorFunction = Data->Iopb->MajorFunction;
		Options = Data->Iopb->Parameters.Create.Options;
		eprocess = PsGetCurrentProcess();
		cProcName = PsGetProcessImageFileName(eprocess);
		_strupr((CHAR*)cProcName);

		if (NT_SUCCESS(FltGetFileNameInformation(Data, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT, &nameInfo)))
		{
			if (NT_SUCCESS(FltParseFileNameInformation(nameInfo)))
			{
				if (nameInfo->Name.MaximumLength > 512)
				{
					pNonPageBuf = (WCHAR *)ExAllocatePool(NonPagedPool, nameInfo->Name.MaximumLength);
					pTemp = pNonPageBuf;
				}
				RtlCopyMemory(pTemp, nameInfo->Name.Buffer, nameInfo->Name.MaximumLength);
				_wcsupr(pTemp);
			}
		}
		else
		{
			return FLT_PREOP_SUCCESS_NO_CALLBACK;
		}

		if (strcmp("ETENAL.EXE", (CHAR*)cProcName) != 0 && NULL != wcsstr(pTemp, L"FLAG.TXT"))
		{
			if (NULL != pNonPageBuf)
				ExFreePool(pNonPageBuf);
			DbgPrint("[FileFilter]%s", cProcName);
			FltReleaseFileNameInformation(nameInfo);
			return FLT_PREOP_COMPLETE;
		}


		if (IRP_MJ_CREATE == MajorFunction && FILE_DELETE_ON_CLOSE == Options)
		{
			//RtlCopyMemory(pTemp, nameInfo->Name.Buffer, nameInfo->Name.MaximumLength);
			//DbgPrint("[FileManager][IRP_MJ_CREATE]%wZ", &nameInfo->Name);
			//_wcsupr(pTemp);
			if (NULL != wcsstr(pTemp, L"FLAG.TXT"))
			{
				if (NULL != pNonPageBuf)
					ExFreePool(pNonPageBuf);
				FltReleaseFileNameInformation(nameInfo);
				return FLT_PREOP_COMPLETE;
			}
			if (NULL != pNonPageBuf)
				ExFreePool(pNonPageBuf);
			FltReleaseFileNameInformation(nameInfo);
		}
	}
	return FLT_PREOP_SUCCESS_NO_CALLBACK;
}

FLT_POSTOP_CALLBACK_STATUS
FileManagerPostCreate(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_opt_ PVOID CompletionContext,
	_In_ FLT_POST_OPERATION_FLAGS Flags
)
{
	UNREFERENCED_PARAMETER(Data);
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(CompletionContext);
	UNREFERENCED_PARAMETER(Flags);
	return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS
FileManagerPreRead(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Flt_CompletionContext_Outptr_ PVOID *CompletionContext
)
{
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(CompletionContext);
	PAGED_CODE();
	{
		PFLT_FILE_NAME_INFORMATION nameInfo;

		if (NT_SUCCESS(FltGetFileNameInformation(Data, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT, &nameInfo)))
		{
			if (NT_SUCCESS(FltParseFileNameInformation(nameInfo)))
			{
				WCHAR pTempBuf[512] = { 0 };
				WCHAR *pNonPageBuf = NULL, *pTemp = pTempBuf;
				if (nameInfo->Name.MaximumLength > 512)
				{
					pNonPageBuf = (WCHAR *)ExAllocatePool(NonPagedPool, nameInfo->Name.MaximumLength);
					pTemp = pNonPageBuf;
				}
				RtlCopyMemory(pTemp, nameInfo->Name.Buffer, nameInfo->Name.MaximumLength);
				//DbgPrint("[FileFilter][IRP_MJ_READ]%wZ", &nameInfo->Name);
				if (NULL != pNonPageBuf)
					ExFreePool(pNonPageBuf);
			}
			FltReleaseFileNameInformation(nameInfo);
		}
	}
	return FLT_PREOP_SUCCESS_NO_CALLBACK;
}

FLT_POSTOP_CALLBACK_STATUS
FileManagerPostRead(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_opt_ PVOID CompletionContext,
	_In_ FLT_POST_OPERATION_FLAGS Flags
)
{
	UNREFERENCED_PARAMETER(Data);
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(CompletionContext);
	UNREFERENCED_PARAMETER(Flags);
	return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS
FileManagerPreWrite(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Flt_CompletionContext_Outptr_ PVOID *CompletionContext
)
{
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(CompletionContext);
	PAGED_CODE();
	{
		PFLT_FILE_NAME_INFORMATION nameInfo;

		if (NT_SUCCESS(FltGetFileNameInformation(Data, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT, &nameInfo)))
		{
			if (NT_SUCCESS(FltParseFileNameInformation(nameInfo)))
			{
				WCHAR pTempBuf[512] = { 0 };
				WCHAR *pNonPageBuf = NULL, *pTemp = pTempBuf;
				if (nameInfo->Name.MaximumLength > 512)
				{
					pNonPageBuf = (WCHAR *)ExAllocatePool(NonPagedPool, nameInfo->Name.MaximumLength);
					pTemp = pNonPageBuf;
				}
				RtlCopyMemory(pTemp, nameInfo->Name.Buffer, nameInfo->Name.MaximumLength);
				//DbgPrint("[FileFilter][IRP_MJ_WRITE]%wZ", &nameInfo->Name);
				_wcsupr(pTemp);
				if (NULL != wcsstr(pTemp, L"FLAG.TXT"))
				{
					DbgPrint("[FileFilter][IRP_MJ_WRITE]%wZ", &nameInfo->Name);
					if (NULL != pNonPageBuf)
						ExFreePool(pNonPageBuf);
					FltReleaseFileNameInformation(nameInfo);
					return FLT_PREOP_DISALLOW_FASTIO;
				}
				if (NULL != pNonPageBuf)
					ExFreePool(pNonPageBuf);
			}
			FltReleaseFileNameInformation(nameInfo);
		}
	}
	return FLT_PREOP_SUCCESS_NO_CALLBACK;
}

FLT_POSTOP_CALLBACK_STATUS
FileManagerPostWrite(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_opt_ PVOID CompletionContext,
	_In_ FLT_POST_OPERATION_FLAGS Flags
)
{
	UNREFERENCED_PARAMETER(Data);
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(CompletionContext);
	UNREFERENCED_PARAMETER(Flags);
	return FLT_POSTOP_FINISHED_PROCESSING;
}

FLT_PREOP_CALLBACK_STATUS
FileManagerPreSetInformation(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_Flt_CompletionContext_Outptr_ PVOID *CompletionContext
)
{
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(CompletionContext);
	PAGED_CODE();
	{
		UCHAR MajorFunction = 0;
		PFLT_FILE_NAME_INFORMATION nameInfo;
		MajorFunction = Data->Iopb->MajorFunction;
		if (IRP_MJ_SET_INFORMATION == MajorFunction &&
			NT_SUCCESS(FltGetFileNameInformation(Data, FLT_FILE_NAME_NORMALIZED | FLT_FILE_NAME_QUERY_DEFAULT, &nameInfo)))
		{
			if (NT_SUCCESS(FltParseFileNameInformation(nameInfo)))
			{
				WCHAR pTempBuf[512] = { 0 };
				WCHAR *pNonPageBuf = NULL, *pTemp = pTempBuf;
				if (nameInfo->Name.MaximumLength > 512)
				{
					pNonPageBuf = (WCHAR *)ExAllocatePool(NonPagedPool, nameInfo->Name.MaximumLength);
					pTemp = pNonPageBuf;
				}
				RtlCopyMemory(pTemp, nameInfo->Name.Buffer, nameInfo->Name.MaximumLength);
				//DbgPrint("[FileFilter][IRP_MJ_SET_INFORMATION]%wZ", &nameInfo->Name);
				_wcsupr(pTemp);
				if (NULL != wcsstr(pTemp, L"FLAG.TXT"))
				{
					DbgPrint("[FileFilter][IRP_MJ_SET_INFORMATION]%wZ", &nameInfo->Name);
					if (NULL != pNonPageBuf)
						ExFreePool(pNonPageBuf);
					FltReleaseFileNameInformation(nameInfo);
					return FLT_PREOP_DISALLOW_FASTIO;
				}
				if (NULL != pNonPageBuf)
					ExFreePool(pNonPageBuf);
			}
			FltReleaseFileNameInformation(nameInfo);
		}
	}
	return FLT_PREOP_SUCCESS_NO_CALLBACK;
}

FLT_POSTOP_CALLBACK_STATUS
FileManagerPostSetInformation(
	_Inout_ PFLT_CALLBACK_DATA Data,
	_In_ PCFLT_RELATED_OBJECTS FltObjects,
	_In_opt_ PVOID CompletionContext,
	_In_ FLT_POST_OPERATION_FLAGS Flags
)
{
	UNREFERENCED_PARAMETER(Data);
	UNREFERENCED_PARAMETER(FltObjects);
	UNREFERENCED_PARAMETER(CompletionContext);
	UNREFERENCED_PARAMETER(Flags);
	return FLT_POSTOP_FINISHED_PROCESSING;
}

