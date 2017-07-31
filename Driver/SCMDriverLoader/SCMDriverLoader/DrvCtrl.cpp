#include "stdafx.h"
#include "DrvCtrl.h"

void DrvCtrl::PrintError()
{
	m_dwLastError = GetLastError();
	_tprintf(L"Error code:%x\n", m_dwLastError);
}

DrvCtrl::DrvCtrl()
{
	m_pDisplayName = NULL;
	m_pServiceName = NULL;
	m_pSysPath = NULL;
	m_hSCManager = NULL;
	m_hService = NULL;
	m_hDriver = NULL;
}


DrvCtrl::~DrvCtrl()
{
	if (m_hService != NULL)
		CloseServiceHandle(m_hService);
	if (m_hSCManager != NULL)
		CloseServiceHandle(m_hSCManager);
	if (m_hDriver != NULL)
		CloseHandle(m_hDriver);
}

DWORD DrvCtrl::CTL_CODE_GEN(DWORD lngFunction)
{
	return (FILE_DEVICE_UNKNOWN * 65536) |
		(FILE_ANY_ACCESS * 16384) |
		(lngFunction * 4) |
		METHOD_BUFFERED;

}

//Open service already existed 
BOOL DrvCtrl::GetSvcHandle(PWCHAR pServiceName)
{
	m_pServiceName = pServiceName;
	m_hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (m_hSCManager == NULL)
	{
		PrintError();
		return FALSE;
	}
	
	m_hService = OpenService(m_hSCManager, m_pServiceName, SERVICE_ALL_ACCESS);
	if (m_hService == NULL)
	{
		PrintError();
		CloseServiceHandle(m_hSCManager);
		return FALSE;
	}
	return TRUE;
}

DWORD DrvCtrl::Get_size(DWORD dwIoCode)
{
	DWORD dw = 0;
	if (!DeviceIoControl(m_hDriver, CTL_CODE_GEN(dwIoCode),
		0, 0, 0, 0, &dw, NULL))
	{
		PrintError();
		return 0;
	}
	_tprintf(L"Output Size:%d\n", dw);
	return dw;
}

BOOL DrvCtrl::Install(PWCHAR pSysPath, PWCHAR pServiceName, PWCHAR pDisplayName)
{
	m_pSysPath = pSysPath;
	m_pServiceName = pServiceName;
	m_pDisplayName = pDisplayName;
	m_hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (m_hSCManager == NULL)
	{
		PrintError();
		return FALSE;
	}
	m_hService = CreateService(m_hSCManager, m_pServiceName, m_pDisplayName,
		SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START,
		SERVICE_ERROR_NORMAL, m_pSysPath, NULL, NULL, NULL, NULL, NULL);

	if (m_hService == NULL)
	{
		m_dwLastError = GetLastError();
		//PrintError();
		if (ERROR_SERVICE_ALREADY_RUNNING == m_dwLastError)
		{
			_tprintf(L"Target driver is still running!\n");
			m_hService = OpenService(m_hSCManager, m_pServiceName, SERVICE_ALL_ACCESS);
			if (m_hService == NULL)
			{
				PrintError();
				CloseServiceHandle(m_hSCManager);
				return FALSE;
			}
			Stop();
		}
		else if (ERROR_SERVICE_EXISTS == m_dwLastError)
		{
			//if exist, open it
			_tprintf(L"Target driver exists!\n");
			m_hService = OpenService(m_hSCManager, m_pServiceName, SERVICE_ALL_ACCESS);
			if (m_hService == NULL)
			{
				PrintError();
				CloseServiceHandle(m_hSCManager);
				return FALSE;
			}
			Stop();
		}
		else
		{
			_tprintf(L"Meet some error.\n");
			CloseServiceHandle(m_hSCManager);
			return FALSE;
		}
	}
	return TRUE;
}

BOOL DrvCtrl::IoControl(DWORD dwIoCode, PVOID InBuff, DWORD InBufferLen,
	PVOID OutBuff, DWORD OutBuffLen)
{
	DWORD dw = 0;
	if (!DeviceIoControl(m_hDriver, CTL_CODE_GEN(dwIoCode),
		InBuff, InBufferLen, OutBuff, OutBuffLen, &dw, NULL))
	{
		PrintError();
		return 0;
	}
	_tprintf(L"Output Size:%d %p\n", dw, OutBuff);
	return 1;
}

BOOL DrvCtrl::Open(PWCHAR pLinkName)
{
	if (m_hDriver != NULL)
		return TRUE;

	m_hDriver = CreateFile(pLinkName, GENERIC_READ | GENERIC_WRITE,
		0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (m_hDriver != INVALID_HANDLE_VALUE)
		return TRUE;
	else
		return FALSE;
}

BOOL DrvCtrl::Remove()
{
	if (!DeleteService(m_hService))
	{
		PrintError();
		return FALSE;
	}
	return TRUE;
}

BOOL DrvCtrl::Start()
{
	if (!StartService(m_hService, NULL, NULL))
	{
		PrintError();
		return FALSE;
	}
	return TRUE;
}

BOOL DrvCtrl::Stop()
{
	SERVICE_STATUS ss;
	if (!ControlService(m_hService, SERVICE_CONTROL_STOP, &ss))
	{
		PrintError();
		return FALSE;
	}
	return TRUE;
}

BOOL DrvCtrl::Uninstall(PWCHAR pSysPath, PWCHAR pServiceName)
{
	m_pSysPath = pSysPath;
	m_pServiceName = pServiceName;
	m_hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	m_hService = OpenService(m_hSCManager, m_pServiceName, SERVICE_ALL_ACCESS);
	if (m_hService == NULL)
	{
		m_dwLastError = GetLastError();
		if (m_dwLastError == ERROR_SERVICE_DOES_NOT_EXIST)
		{
			_tprintf(L"Service does not exist.\n");
			CloseServiceHandle(m_hSCManager);
			return FALSE;
		}
		else
		{
			PrintError();
			CloseServiceHandle(m_hSCManager);
			return FALSE;
		}
	}
	return Stop();
}