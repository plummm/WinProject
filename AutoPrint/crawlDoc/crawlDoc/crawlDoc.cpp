// crawlDoc.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "Access.h"

TCHAR path_to_cloud[MAX_PATH];

int main()
{
	Access access;
	while (1)
	{
		StringCchCopy(path_to_cloud, MAX_PATH, L"");
		access.ListAllFile(path_to_cloud);
		access.Clear();
		Sleep(10000);
	}
	return 0;
}

