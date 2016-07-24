// PE_Tool.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "PE_Tool.h"
#include "PE_Reader.h"

int main()
{
	PE_Reader pe_reader;
	pe_reader.run();
	system("pause");
	return 0;
}