#include "stdafx.h"
#include "printer.h"



printer::printer()
{
	bStatus = OpenPrinter(NAMEOFPRINTER, &hPrinter, NULL);
}


printer::~printer()
{
}

int printer::addPrinter()
{
	AddPrinter(NAMEOFPRINTER, 2, (LPBYTE)&pPrinter);
	return 0;
}

int printer::enumPrinters()
{
	DWORD pcbNeeded = 0, pcReturned = 0;
	int dwError = 0;
	EnumPrinters(PRINTER_ENUM_NAME, NULL, 2, NULL, 0, &pcbNeeded, &pcReturned);
	pPrinterEnum = (PRINTER_INFO_2 *)malloc(pcbNeeded);
	if (EnumPrinters(PRINTER_ENUM_NAME, NULL, 2, (LPBYTE)pPrinterEnum, pcbNeeded, &pcbNeeded, &pcReturned) == 0)
	{
		dwError = GetLastError();
	}
	for (DWORD i = 0; i < pcReturned; i++)
	{
		_tprintf(L"Printer Name:%s\n", pPrinterEnum[i].pPrinterName);
		if (StrCmpW(pPrinterEnum[i].pPrinterName, L"HP Deskjet F2100 series") == 0)
			pPrinter = pPrinterEnum[i];
	}
	return dwError;
}


int printer::print(LPBYTE lpData, DWORD dwCount, DWORD dwWritten)
{
	DOCINFO DocInfo;
	DOC_INFO_3 DocInfo3;
	DWORD   dwJob = 0L, dwError = 0;
	int dwId = 0;
	PRINTER_DEFAULTS pDefault;
	HDC hdcPrinter = NULL;
	HANDLE hSpool = NULL;

	pDefault.DesiredAccess = PRINTER_ACCESS_USE;
	pDefault.pDatatype = L"RAW";
	pDefault.pDevMode = (LPDEVMODE)NAMEOFPRINTER;

	hdcPrinter = CreateDC(NULL, NAMEOFPRINTER, NULL, NULL);

	//StringCchLength(lpData, 1024, &dwCount);
	bStatus = OpenPrinter(NAMEOFPRINTER, &hPrinter, &pDefault);
	if (bStatus) {
		// Fill in the structure with info about this "document." 
		DocInfo3.pDocName = (LPTSTR)_T("my.pdf");
		DocInfo3.pOutputFile = (LPTSTR)_T("USB001");
		DocInfo3.pDatatype = (LPTSTR)_T("NT EMF 1.008");
		DocInfo3.dwFlags = DI_MEMORYMAP_WRITE;
		
		DocInfo.lpszDocName = (LPTSTR)_T("my.pdf");
		DocInfo.lpszOutput = (LPTSTR)_T("USB001");
		DocInfo.fwType = 0;
		DocInfo.lpszDatatype = NULL;
		DocInfo.cbSize = sizeof(DocInfo);

		// Inform the spooler the document is beginning. 
		dwId = StartDoc(hdcPrinter, &DocInfo);
		if (dwId <= 0)
		{
			_tprintf(L"Failed to StartDoc!\n");
			dwError = GetLastError();
			return 0;
		}

		//dwJob = StartDocPrinter(hPrinter, 3, (LPBYTE)&DocInfo3);
		dwJob = 1;
		if (dwJob > 0) {

			hSpool = GetSpoolFileHandle(hPrinter);
			// Start a page. 
			bStatus = StartPage(hdcPrinter);
			//bStatus = StartPagePrinter(hPrinter);
			if (bStatus) {
				TextOut(hdcPrinter, 20, 20, L"Hello World!", 12);
				// Send the data to the printer. 
				//bStatus = WritePrinter(hPrinter, lpData, dwCount, &dwWritten);
				//CommitSpoolData(hPrinter, hSpool, dwWritten);
				//EndPagePrinter(hPrinter);
				EndPage(hdcPrinter);
			}
			// Inform the spooler that the document is ending. 
			//EndDocPrinter(hPrinter);
			EndDoc(hdcPrinter);
		}
		// Close the printer handle. 
		ClosePrinter(hPrinter);
	}
	// Check to see if correct number of bytes were written. 
	if (!bStatus || (dwWritten != dwCount)) {
		bStatus = FALSE;
	}
	else {
		bStatus = TRUE;
	}
	return bStatus;
}

// RawDataToPrinter - sends binary data directly to a printer
// 
// Params:
//   szPrinterName - NULL terminated string specifying printer name
//   lpData        - Pointer to raw data bytes
//   dwCount       - Length of lpData in bytes
// 
// Returns: TRUE for success, FALSE for failure.
// 
BOOL RawDataToPrinter(LPWSTR szPrinterName, LPBYTE lpData, DWORD dwCount)
{
	HANDLE     hPrinter;
	DOC_INFO_1 DocInfo;
	DWORD      dwJob;
	DWORD      dwBytesWritten;

	// Need a handle to the printer.
	if (!OpenPrinter(szPrinterName, &hPrinter, NULL))
		return FALSE;

	// Fill in the structure with info about this "document."
	DocInfo.pDocName = L"My Document";
	DocInfo.pOutputFile = NULL;
	DocInfo.pDatatype = L"RAW";
	// Inform the spooler the document is beginning.
	if ((dwJob = StartDocPrinter(hPrinter, 1, (LPBYTE)&DocInfo)) == 0)
	{
		ClosePrinter(hPrinter);
		return FALSE;
	}
	// Start a page.
	if (!StartPagePrinter(hPrinter))
	{
		EndDocPrinter(hPrinter);
		ClosePrinter(hPrinter);
		return FALSE;
	}
	// Send the data to the printer.
	if (!WritePrinter(hPrinter, lpData, dwCount, &dwBytesWritten))
	{
		EndPagePrinter(hPrinter);
		EndDocPrinter(hPrinter);
		ClosePrinter(hPrinter);
		return FALSE;
	}
	// End the page.
	if (!EndPagePrinter(hPrinter))
	{
		EndDocPrinter(hPrinter);
		ClosePrinter(hPrinter);
		return FALSE;
	}
	// Inform the spooler that the document is ending.
	if (!EndDocPrinter(hPrinter))
	{
		ClosePrinter(hPrinter);
		return FALSE;
	}
	// Tidy up the printer handle.
	ClosePrinter(hPrinter);
	// Check to see if correct number of bytes were written.
	if (dwBytesWritten != dwCount)
		return FALSE;
	return TRUE;
}


int printer::readDoc(const TCHAR * filename)
{
	HANDLE hFile = NULL;
	TCHAR *buffer;
	DWORD dwRead = 0, dwWritten = 0;

	hFile = CreateFile(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
		return 0;

	for (int size = GetFileSize(hFile, NULL); size; size -= dwRead)
	{
		buffer = (TCHAR*)malloc(size);
		if (!ReadFile(hFile, buffer, size, &dwRead, NULL))
		{
			break;
		}
		
		//printer::print((LPBYTE)cbuffer, sizeof(cbuffer), dwWritten);
		RawDataToPrinter(NAMEOFPRINTER, (LPBYTE)buffer, size);
		_tprintf(L"Written : %d", dwWritten);
	}


}
