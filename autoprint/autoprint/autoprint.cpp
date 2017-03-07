// autoprint.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "access.h"
#include "printer.h"

int main()
{
	access Access;
	Access.ListAllFile(L"");
	//Access.ListAllFile();
	system("pause");
}

