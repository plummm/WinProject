#pragma once
class printer
{
public:
	printer();
	~printer();
	int addPrinter();
	int enumPrinters();
	int print(LPBYTE lpData, DWORD dwCount, DWORD dwWritten);
	int readDoc(const TCHAR * filename);

private:
	PRINTER_INFO_2 pPrinter, *pPrinterEnum;
	BOOL           bStatus = FALSE;
	HANDLE         hPrinter = NULL;
};

