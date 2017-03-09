#include "stdafx.h"
#include "access.h"

void DisplayErrorBox(LPTSTR lpszFunction);
void PrintLog(TCHAR log[1024], int dwError);
FILE *stream;

Access::Access()
{
	freopen_s(&stream, "log.txt", "a+", stdout);
	
}

int Access::CheckPass(TCHAR fileName[MAX_PATH], int flag)
{
	switch (flag)
	{
	case EXTENSION:
	{
		TCHAR *pos = NULL;

		pos = wcsrchr(fileName, '.');
		return (StrCmpW(pos + 1, L"doc") == 0 || StrCmpW(pos + 1, L"docx") == 0);
		break;
	}
	case DORMITORYNUM:
	{
		size_t length_of_name = 0;
		int dormitory_num = 0;
		StringCchLength(fileName, MAX_PATH, &length_of_name);
		if (length_of_name == 3)
		{
			dormitory_num = _wtoi(fileName);
			if (dormitory_num == 0) return 0;
			if (dormitory_num / 100 > 6) return 0;
			if (dormitory_num % 100 > 28) return 0;
			return 1;
		}
		break;
	}
	default:
		break;
	}
	return 0;
}

void Access::Clear()
{
	printer.Clear();
}

//做一个递归查询，复制所有文件到printed
int Access::ListAllFile(TCHAR szDirAdd[MAX_PATH])
{
	WIN32_FIND_DATA ffd;
	LARGE_INTEGER filesize;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	DWORD dwError = 0;
	TCHAR szDir[MAX_PATH] = { 0, }, szDirBack[MAX_PATH] = { 0, }, szPrintedDir[MAX_PATH] = { 0, };
	TCHAR szName[MAX_PATH] = { 0, }, szPrintedName[MAX_PATH] = { 0, };
	TCHAR szDirAddBak[MAX_PATH] = { 0, };
	size_t length_of_name;

	StringCchCopy(szDir, MAX_PATH, PRINTPATH);
	StringCchCopy(szPrintedDir, MAX_PATH, ALREADYPRINTPATH);

	if (!PathFileExists(szDir))
	{
		//_tprintf(L"File not exist\n");
		PrintLog(L"Path not exist", -1);
		return GetLastError();
	}

	StringCchCat(szDir, MAX_PATH, szDirAdd);
	StringCchCat(szPrintedDir, MAX_PATH, szDirAdd);

	StringCchCopy(szDirBack, MAX_PATH, szDir);
	StringCchCat(szDir, MAX_PATH, TEXT("\\*"));
	//_tprintf(L"szDir:%s szPrintedDir:%s szDirBack:%s\n", szDir, szPrintedDir, szDirBack);

	hFind = FindFirstFile(szDir, &ffd);

	if (INVALID_HANDLE_VALUE == hFind)
	{
		//DisplayErrorBox(TEXT("FindFirstFile"));
		PrintLog(L"Find file failed", -1);
		return GetLastError();
	}

	// List all the files in the directory with some info about them.

	do
	{
		TCHAR *cCmp = TEXT(".");
		if (ffd.cFileName[0] == *cCmp)
		{
			continue;
		}
		StringCchPrintf(szName, MAX_PATH, L"%s\\%s", szDirBack, ffd.cFileName);
		StringCchPrintf(szPrintedName, MAX_PATH, L"%s\\%s", szPrintedDir, ffd.cFileName);
		StringCchLength(szName, MAX_PATH, &length_of_name);

		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (!this->CheckPass(ffd.cFileName, DORMITORYNUM))
			{
				continue;
			}
			//_tprintf(TEXT("  %s   <DIR>\n"), ffd.cFileName);
			StringCchPrintf(szDirAdd, MAX_PATH, L"%s\\%s", szDirAdd, ffd.cFileName);
			//StringCchCat(szDirAdd, MAX_PATH, ffd.cFileName);

			StringCchPrintf(szDirAddBak, MAX_PATH, L"%s%s", ALREADYPRINTPATH, szDirAdd);
			CreateDirectory(szDirAddBak, NULL);
			
			this->ListAllFile(szDirAdd);
		}
		else
		{
			filesize.LowPart = ffd.nFileSizeLow;
			filesize.HighPart = ffd.nFileSizeHigh;
			//if (StrCmpW(ffd.cFileName, L"426") == 0 && length_of_name <= MAX_PATH - 4)
			if (!this->CheckPass(ffd.cFileName, EXTENSION) || filesize.QuadPart > 102400 || StrCmpW(szDirBack, PRINTPATH)==0 )
			{
				continue;
			}
			this->CopyFileFromNas(szName, szPrintedName);
			MessageBox(NULL, L"Printing...", L"Print", MB_OK);
			_tprintf(L"%s\n", szName);
			//this->printer.readDoc(szName);
			this->DeleteFileFromNas(szName);
			//_tprintf(TEXT("  %s   %ld bytes\n"), ffd.cFileName, filesize.QuadPart);
		}
	} while (FindNextFile(hFind, &ffd) != 0);

	dwError = GetLastError();
	if (dwError != ERROR_NO_MORE_FILES)
	{
		//DisplayErrorBox(TEXT("FindFirstFile"));
		PrintLog(L"Find more file error", -1);
		return dwError;
	}

	FindClose(hFind);

	this->DeleteDir(szDirBack);
	return dwError;
}

int Access::DeleteDir(TCHAR szName[MAX_PATH])
{
	DWORD dwError = 0;
	TCHAR logOut[1024] = { 0, };

	if (!RemoveDirectory(szName))
	{
		dwError = GetLastError();
		StringCchPrintf(logOut, 1024, L"Delete dir (%s) failed!", szName);
		PrintLog(logOut, dwError);
	}
	return dwError;
}

int Access::DeleteFileFromNas(TCHAR szName[MAX_PATH])
{
	DWORD dwError = 0;
	TCHAR logOut[1024] = { 0, };

	if (DeleteFile(szName) == 0)
	{
		dwError = GetLastError();
		StringCchPrintf(logOut, 1024, L"Delete file (%s) failed!", szName);
		PrintLog(logOut, dwError);
		return 1;
	}
	return 0;
}

int Access::CopyFileFromNas(TCHAR szSrcName[MAX_PATH], TCHAR szDestName[MAX_PATH])
{
	DWORD dwError = 0, dwFlag = 0;
	int count = 0;
	TCHAR *pos = NULL;
	TCHAR extension[MAX_PATH] = { 0, }, pDir[MAX_PATH] = { 0, };
	TCHAR szDestNameBak[MAX_PATH] = { 0, }, logOut[1024] = { 0, };

	pos = wcsrchr(szDestName, '.');
	StringCchCopyN(pDir, MAX_PATH, szDestName, pos - szDestName);

	if (!CopyFile(szSrcName, szDestName, TRUE))
		do
		{
			StringCchPrintf(szDestNameBak, MAX_PATH, L"%s(%d)%s",pDir, count++, pos);
			dwFlag = CopyFile(szSrcName, szDestNameBak, TRUE);
			if (count > 100)
			{
				dwError = GetLastError();
				StringCchPrintf(logOut, 1024, L"Copy file (%s) failed!", szSrcName);
				PrintLog(logOut, dwError);
			}
		} while (dwFlag == 0);

	return dwError;
}

/*
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
*/

void PrintLog(TCHAR log[1024], int dwError)
{
	if (dwError == 0)
		return;
	SYSTEMTIME systemTime;
	GetSystemTime(&systemTime);
	if (&systemTime != NULL)
		fwprintf_s(stream, L"%d-%d-%d %d:%d:%d  %s Error %d\n", systemTime.wYear, systemTime.wMonth, systemTime.wDay,
			systemTime.wHour, systemTime.wMinute, systemTime.wSecond, log, dwError);
}