#pragma once
ref class Printer
{
public:
	Printer();
	int addPrinter();
	int enumPrinters();
	int print(LPBYTE lpData, DWORD dwCount, DWORD dwWritten);
	int readDoc(const TCHAR * filename);
	void Clear();

private:
	PRINTER_INFO_2 *pPrinterEnum = NULL;
	BOOL           bStatus = FALSE;
	HANDLE         *hPrinter = NULL;
};

