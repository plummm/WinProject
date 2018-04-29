// PE_Tool.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "PE_Tool.h"
#include "PE_Reader.h"
#include <stdio.h>

int main()
{
	wchar_t path[MAX_PATH];
	wscanf_s(L"%260ls", path, MAX_PATH);
	PE_Reader* pe_reader = new PE_Reader(path);
	pe_reader->Run();
	system("pause");
	return 0;
}