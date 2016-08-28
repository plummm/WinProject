#include "stdafx.h"
#include "PE_Reader.h"


PE_Reader::PE_Reader():
fileName(L"F:\\WinProject\\DLLinject\\NOTEPAD.EXE")
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
	
	
	/*
	pfileNtHeaders = (PIMAGE_NT_HEADERS)malloc(sizeof(PIMAGE_NT_HEADERS));
	pfileDosHeader = (PIMAGE_DOS_HEADER)malloc(sizeof(IMAGE_DOS_HEADER));
	pfileImport = (PIMAGE_IMPORT_DESCRIPTOR)malloc(sizeof(PIMAGE_IMPORT_DESCRIPTOR));
	*/
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

void PE_Reader::read(LPVOID buffer, DWORD length)
{
	if (!ReadFile(fileHandle, buffer, length, fileNum, NULL))
	{
		_tprintf(L"File read error!The error code is %x\n", GetLastError());
	}
}

void PE_Reader::run()
{
	LPCSTR szLibName;
	DWORD rva;

	read(virtualpointer,fileSize);
	CloseHandle(fileHandle);
	pfileDosHeader = PIMAGE_DOS_HEADER(virtualpointer);
	pfileNtHeaders = PIMAGE_NT_HEADERS((DWORD_PTR)virtualpointer + pfileDosHeader->e_lfanew);
	pfileSection = IMAGE_FIRST_SECTION(pfileNtHeaders);
	rva = Rva2Offset(pfileNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress,
		pfileSection,
		pfileNtHeaders);
	pfileImport = PIMAGE_IMPORT_DESCRIPTOR((DWORD_PTR)virtualpointer+rva);
	_tprintf(L"Library name is %s", (PCHAR)((DWORD_PTR)virtualpointer + Rva2Offset(pfileImport->Name, pfileSection, pfileNtHeaders)));

	//pfileSection = PIMAGE_SECTION_HEADER()
	/*
	read((void *)pfileDosHeader, sizeof(IMAGE_DOS_HEADER));
	setPoint(fileHandle, pfileDosHeader->e_lfanew);
	read((void *)pfileNtHeaders, sizeof(pfileNtHeaders));
	pfileSection = (PIMAGE_SECTION_HEADER)malloc(sizeof(PIMAGE_SECTION_HEADER)*pfileNtHeaders->FileHeader.NumberOfSections);
	pfileSection = IMAGE_FIRST_SECTION(pfileNtHeaders);
	rva=Rva2Offset(pfileNtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress,
			   pfileSection,
			   pfileNtHeaders);
	setPoint(fileHandle, rva);
	//read((void *)pfileImport, sizeof(pfileImport));
	pfileImport = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD_PTR)&pfileDosHeader + rva);
	_tprintf(L"Name is %x %x %x\n", pfileDosHeader, pfileDosHeader->e_lfanew, pfileNtHeaders);
	//for (; )
	*/
	PEchecker();
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

void PE_Reader::setPoint(HANDLE hFile, LONG lDistanceToMove)
{
	SetFilePointer(hFile, lDistanceToMove, NULL, FILE_BEGIN);
}
