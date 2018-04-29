#pragma once
#include "stdafx.h"

typedef struct dllData {
	LPWSTR szLibName;
	wchar_t** funcNames;
	int count;
} DllData, *pDllData;

class PE_Reader
{
public:
	PE_Reader(wchar_t* path);
	~PE_Reader();
	void PEchecker();
	void Read(LPVOID buffer, DWORD length);
	void Run();
	DWORD Rva2Offset(DWORD rva, PIMAGE_SECTION_HEADER psh, PIMAGE_NT_HEADERS pnt);
	void SetPoint(HANDLE hFile, LONG lDistanceToMove);
	pDllData RetriveDll(int index);
	void Print(pDllData dll);
	PVOID MultiByte2WideChar(LPSTR multiByte);

private:
	HANDLE fileHandle, hMod;
	DWORD fileSize;
	PVOID virtualpointer;
	LPCTSTR fileName;
	PIMAGE_DOS_HEADER pfileDosHeader;
	PIMAGE_NT_HEADERS pfileNtHeaders;
	IMAGE_IMPORT_DESCRIPTOR *pfileImport;
	PIMAGE_SECTION_HEADER pfileSection;
	PIMAGE_THUNK_DATA pfileThunk;
	PIMAGE_IMPORT_BY_NAME pImportName;
	LPDWORD fileNum;

};

