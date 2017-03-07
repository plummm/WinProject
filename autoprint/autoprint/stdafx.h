// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <Windows.h>
#include <Shlwapi.h>
#include <strsafe.h>
#include <iostream>
#include <msclr/marshal_cppstd.h>

#using "..\Release\sendToPrinter.dll"

//#pragma comment( linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"" )
#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Shlwapi.lib")
#pragma comment(lib, "Winspool.lib")

#define PRINTPATH L"\\\\PERSONALCLOUD\\Public\\Print"
#define ALREADYPRINTPATH L"\\\\PERSONALCLOUD\\Public\\Printed"
#define CLOUDPATH L"\\\\PERSONALCLOUD\\Public"
#define NAMEOFPRINTER L"HP Deskjet F2100 series"


// TODO: 在此处引用程序需要的其他头文件
