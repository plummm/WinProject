#include "stdafx.h"
#include "PE_Reader.h"

using namespace std;


PE_Reader::PE_Reader(wchar_t* path):
fileName(path)
{
	if ((fileHandle = CreateFile(
		fileName,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL)) == INVALID_HANDLE_VALUE)
	{
		_tprintf(L"File open error!The error code is %x\n", GetLastError());
	}
	hMod = fileHandle;
	fileSize = GetFileSize(fileHandle, NULL);
	virtualpointer = VirtualAlloc(NULL, fileSize, MEM_COMMIT, PAGE_READWRITE);
	
}


PE_Reader::~PE_Reader()
{
}


void PE_Reader::PEchecker()
{
	if (pfileDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
	{
		_tprintf(L"Invalid DOS header!\n");
	}

	if (pfileNtHeaders->Signature != IMAGE_NT_SIGNATURE)
	{
		_tprintf(L"Invalid NT header!\n");
		
	}
}

void PE_Reader::Read(LPVOID buffer, DWORD length)
{
	if (!ReadFile(fileHandle, buffer, length, fileNum, NULL))
	{
		_tprintf(L"File read error!The error code is %x\n", GetLastError());
	}
}

pDllData PE_Reader::RetriveDll(int index)
{
	LPSTR szLibName;
	PVOID wszLibName = NULL;
	PVOID wc = NULL;
	int p = NULL;
	pDllData dll = NULL;
	wchar_t **funcNames = NULL;
	int* pRva;
	int count = 0;


	szLibName = (PCHAR)((DWORD_PTR)virtualpointer + Rva2Offset(pfileImport[index].Name, pfileSection, pfileNtHeaders));
	wszLibName = MultiByte2WideChar(szLibName);
	pRva = (int*)((DWORD_PTR)virtualpointer + Rva2Offset(pfileImport[index].OriginalFirstThunk, pfileSection, pfileNtHeaders));
	for (int* i = pRva; (*i) != 0; i++)
		count++;
	funcNames = (wchar_t **)malloc(count * 4);
	count = 0;
	for (int* i = pRva; *i != 0; i++)
	{
		size_t tmp;
		pImportName = PIMAGE_IMPORT_BY_NAME((DWORD_PTR)virtualpointer + Rva2Offset(*i, pfileSection, pfileNtHeaders));
		int len = MultiByteToWideChar(CP_UTF8, 0, pImportName->Name, -1, NULL, 0);
		//int len = strlen(pImportName->Name);
		funcNames[count] = (wchar_t *)malloc(len * sizeof(wchar_t));
		wc = MultiByte2WideChar(pImportName->Name);
		//mbstowcs_s(&tmp, wc, len, pImportName->Name, len);
		wcscpy_s(funcNames[count++], len, (LPWSTR)wc);
	}
	dll = (pDllData)malloc(sizeof(dllData));
	dll->szLibName = (LPWSTR)wszLibName;
	dll->funcNames = funcNames;
	dll->count = count;
	return dll;
}

void PE_Reader::Run()
{
	DWORD rva;

	Read(virtualpointer,fileSize);
	CloseHandle(fileHandle);
	pfileDosHeader = PIMAGE_DOS_HEADER(virtualpointer);
	pfileNtHeaders = PIMAGE_NT_HEADERS((DWORD_PTR)virtualpointer + pfileDosHeader->e_lfanew);
	pfileSection = IMAGE_FIRST_SECTION(pfileNtHeaders);
	rva = Rva2Offset(pfileNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress,
		pfileSection,
		pfileNtHeaders);
	pfileImport = (IMAGE_IMPORT_DESCRIPTOR *)((DWORD_PTR)virtualpointer + rva);
	
	int lenOfImport = sizeof(pfileImport);
	for (int i = 0; i >= 0; i++)
	{
		if (pfileImport[i].Characteristics == 0)
			break;
		Print(RetriveDll(i));
	}
	PEchecker();
}

PVOID PE_Reader::MultiByte2WideChar(LPSTR multiByte)
{
	int wideSize = 0;
	PVOID wideChar = NULL;
	wideSize = MultiByteToWideChar(CP_UTF8, 0, multiByte, -1, NULL, 0);
	wideChar = VirtualAlloc(NULL, wideSize * sizeof(wchar_t), MEM_COMMIT, PAGE_READWRITE);
	MultiByteToWideChar(CP_UTF8, 0, multiByte, -1, (LPWSTR)wideChar, wideSize * sizeof(wchar_t));
	return wideChar;
}

void PE_Reader::Print(pDllData dll)
{
	_tprintf(L"DLL ===> %s\n", dll->szLibName);

	for (int i = 0; i < dll->count; i++)
	{
		_tprintf(L"    %s\n", dll->funcNames[i]);
	}
}

DWORD PE_Reader::Rva2Offset(DWORD rva, PIMAGE_SECTION_HEADER psh, PIMAGE_NT_HEADERS pnt)
{
	size_t i = 0;
	PIMAGE_SECTION_HEADER pSeh;
	if (rva == 0)
	{
		return (rva);
	}
	pSeh = psh;
	for (; i < pnt->FileHeader.NumberOfSections; i++)
	{
		if (rva >= pSeh->VirtualAddress && rva < pSeh->VirtualAddress +
			pSeh->Misc.VirtualSize)
		{
			break;
		}
		pSeh++;
	}
	return (rva - pSeh->VirtualAddress + pSeh->PointerToRawData);
}

void PE_Reader::SetPoint(HANDLE hFile, LONG lDistanceToMove)
{
	SetFilePointer(hFile, lDistanceToMove, NULL, FILE_BEGIN);
}
