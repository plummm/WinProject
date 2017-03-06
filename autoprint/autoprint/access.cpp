#include "stdafx.h"
#include "access.h"

void DisplayErrorBox(LPTSTR lpszFunction);

access::access()
{
}


access::~access()
{
}


//做一个递归查询，复制所有文件到printed
BOOL access::ListAllFile(TCHAR szDirAdd[MAX_PATH])
{
	WIN32_FIND_DATA ffd;
	LARGE_INTEGER filesize;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError = 0;
	TCHAR szDir[MAX_PATH], szDirBack[MAX_PATH], szPrintedDir[MAX_PATH];
	TCHAR szName[MAX_PATH], szPrintedName[MAX_PATH];
	size_t length_of_name;

	StringCchCopy(szDir, MAX_PATH, PRINTPATH);
	StringCchCopy(szPrintedDir, MAX_PATH, ALREADYPRINTPATH);
	
	if (!PathFileExists(szDir))
	{
		_tprintf(L"File not exist\n");
		return FALSE;
	}

	StringCchCat(szDir, MAX_PATH, szDirAdd);
	StringCchCat(szPrintedDir, MAX_PATH, szDirAdd);

	StringCchCopy(szDirBack, MAX_PATH, szDir);
	StringCchCat(szDir, MAX_PATH, TEXT("\\*"));

	hFind = FindFirstFile(szDir, &ffd);

	if (INVALID_HANDLE_VALUE == hFind)
	{
		DisplayErrorBox(TEXT("FindFirstFile"));
		return dwError;
	}

	// List all the files in the directory with some info about them.

	do
	{
		StringCchPrintf(szName, MAX_PATH, L"%s\\%s", szDirBack, ffd.cFileName);
		StringCchPrintf(szPrintedName, MAX_PATH, L"%s\\%s", szPrintedDir, ffd.cFileName);
		StringCchLength(szName, MAX_PATH, &length_of_name);

		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			_tprintf(TEXT("  %s   <DIR>\n"), ffd.cFileName);
			if (StrCmpW(ffd.cFileName, L"426") == 0 && length_of_name <= MAX_PATH - 4)
				access::CopyFileFromNas(szName, szPrintedName);
		}
		else
		{
			filesize.LowPart = ffd.nFileSizeLow;
			filesize.HighPart = ffd.nFileSizeHigh;
			if (StrCmpW(ffd.cFileName, L"426") == 0 && length_of_name <= MAX_PATH - 4)
				access::CopyFileFromNas(szName, szPrintedName);
			_tprintf(TEXT("  %s   %ld bytes\n"), ffd.cFileName, filesize.QuadPart);
		}
	} while (FindNextFile(hFind, &ffd) != 0);

	dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES)
	{
		DisplayErrorBox(TEXT("FindFirstFile"));
	}

	FindClose(hFind);
	return dwError;
}

int access::DeleteFileFromNas(TCHAR szName[100])
{
	DWORD dwError = 0;

	if (DeleteFile(szName) == 0)
	{
		dwError = GetLastError();
		_tprintf(L"Delete %s failed!Error:%d\n", szName,dwError);
		return 1;
	}
	return 0;
}

int access::CopyFileFromNas(TCHAR szSrcName[MAX_PATH], TCHAR szDestName[MAX_PATH])
{
	DWORD dwError = 0, dwFlag = 0;
	int count = 0;

	do
	{
		StringCchPrintf(szDestName, MAX_PATH, L"%s(%d)", szDestName, count++);
		dwFlag = CopyFile(szSrcName, szDestName, TRUE);
	} while (dwFlag == 0);

	dwError = GetLastError();
	return dwError;
}


void DisplayErrorBox(LPTSTR lpszFunction)
{
	// Retrieve the system error message for the last-error code

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	// Display the error message and clean up

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
}
