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
	int CheckPass(TCHAR fileName[MAX_PATH], int flag);
	void Clear();

private:
	Printer printer;
};

