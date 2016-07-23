#include "stdafx.h"
#include "PE_Reader.h"


PE_Reader::PE_Reader():
fileName(L"demo")
{
	if ((fileHandle = CreateFile(
		fileName,
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL)) == INVALID_HANDLE_VALUE)
	{
		_tprintf(L"File open error!The error code is %x", GetLastError());
	}
}


PE_Reader::~PE_Reader()
{
}


void PE_Reader::PEchecker()
{
	if (fileDosHeader.e_magic != IMAGE_DOS_SIGNATURE)
	{
		_tprintf(L"Invalid DOS header!");
	}
}

void PE_Reader::read(LPVOID buffer, DWORD length)
{
	if (!ReadFile(fileHandle, buffer, length, fileNum, NULL))
	{
		_tprintf(L"File read error!The error code is %x", GetLastError());
	}
}

void PE_Reader::run()
{
	read((void *)&fileDosHeader, sizeof(fileDosHeader));
	PEchecker();
}
