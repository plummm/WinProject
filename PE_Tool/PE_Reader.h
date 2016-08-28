#pragma once
#include "stdafx.h"

class PE_Reader
{
public:
	PE_Reader();
	~PE_Reader();
	void PEchecker();
	void read(LPVOID buffer, DWORD length);
	void run();
	DWORD Rva2Offset(DWORD rva, PIMAGE_SECTION_HEADER psh, PIMAGE_NT_HEADERS pnt);
	void setPoint(HANDLE hFile, LONG lDistanceToMove);
	

private:
	HANDLE fileHandle, hMod;
	DWORD fileSize;
	PVOID virtualpointer;
	LPCTSTR fileName;
	PIMAGE_DOS_HEADER pfileDosHeader;
	PIMAGE_NT_HEADERS pfileNtHeaders;
	PIMAGE_IMPORT_DESCRIPTOR pfileImport;
	PIMAGE_SECTION_HEADER pfileSection;
	LPDWORD fileNum;

};

