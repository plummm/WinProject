// autoprint.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "access.h"
#include "printer.h"


int main()
{
	access Access;
	printer Printer;
	Printer.readDoc(L"C:\\Users\\ETenal\\Documents\\Hello World.docx");
	//Access.ListAllFile();
	system("pause");
}

