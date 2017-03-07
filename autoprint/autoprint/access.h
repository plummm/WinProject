#pragma once

#include "printer.h"
class access
{
public:
	access();
	~access();
	int ListAllFile(TCHAR szDir[MAX_PATH]);
	int DeleteFileFromNas(TCHAR szName[MAX_PATH]);
	int CopyFileFromNas(TCHAR szSrcName[MAX_PATH], TCHAR szDestName[MAX_PATH]);

private:
	printer Printer;
};

