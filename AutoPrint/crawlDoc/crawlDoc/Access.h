#pragma once
#include "Printer.h"
ref class Access
{
public:
	Access();
	int ListAllFile(TCHAR szDir[MAX_PATH]);
	int DeleteFileFromNas(TCHAR szName[MAX_PATH]);
	int CopyFileFromNas(TCHAR szSrcName[MAX_PATH], TCHAR szDestName[MAX_PATH]);
	int DeleteDir(TCHAR szName[MAX_PATH]);
	int CheckExtension(TCHAR fileName[MAX_PATH]);
	void Clear();

private:
	Printer printer;
};

