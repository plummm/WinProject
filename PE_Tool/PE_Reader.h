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
	void setPoint(HANDLE hFile, LONG lDistanceToMove);
	

private:
	HANDLE fileHandle;
	LPCTSTR fileName;
	IMAGE_DOS_HEADER fileDosHeader;
	IMAGE_NT_HEADERS fileNtHeaders;
	PIMAGE_IMPORT_DESCRIPTOR fileImport;
	LPDWORD fileNum;

};

