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
}


PE_Reader::~PE_Reader()
{
}


void PE_Reader::PEchecker()
{
	if (fileDosHeader.e_magic != IMAGE_DOS_SIGNATURE)
	{
		_tprintf(L"Invalid DOS header!\n");
	}

	if (fileNtHeaders.Signature != IMAGE_NT_SIGNATURE)
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
	read((void *)&fileDosHeader, sizeof(fileDosHeader));
	setPoint(fileHandle, fileDosHeader.e_lfanew);
	read((void *)&fileNtHeaders, sizeof(fileNtHeaders));
	setPoint(fileHandle, fileNtHeaders.OptionalHeader.DataDirectory[1].VirtualAddress);
	read((void *)&fileImport, sizeof(fileImport));
	
	//_tprintf(L"Name is %s\n", (LPCSTR)((DWORD)&fileDosHeader+fileImport.Name));
	PEchecker();
}

void PE_Reader::setPoint(HANDLE hFile, LONG lDistanceToMove)
{
	SetFilePointer(hFile, lDistanceToMove, NULL, FILE_BEGIN);
}
