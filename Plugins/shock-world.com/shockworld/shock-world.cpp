#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static CHAR szPluginName[] = "shock-world.com plugin";
TCHAR szBuffer[MAX_PATH];


typedef struct _BOX
{
	HWND hWnd;
	int PID;
} *LPBOX, BOX;

int nCount;

DWORD Buffer = 0;
LPVOID Address;

DWORD GetFireDllAddress()
{
	return 0x13140000;
}

bool ClearFireDllCounter(HWND hWnd)
{
	DWORD dwPID;
	DWORD dwTiny;
	bool bRet = FALSE;;
	GetWindowThreadProcessId(hWnd, &dwPID);
	Address = (LPVOID)(GetFireDllAddress() + 0x5B8FC);
	HANDLE hProcess = OpenProcess(GENERIC_WRITE, FALSE, dwPID);
	if (hProcess!=INVALID_HANDLE_VALUE)
	{
		bRet = WriteProcessMemory(hProcess, Address, &Buffer, sizeof(DWORD), &dwTiny);
	}
	CloseHandle(hProcess);
	return bRet;
}

BOOL __stdcall DllMain(HANDLE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
    {
		case DLL_PROCESS_ATTACH:
			DisableThreadLibraryCalls((HINSTANCE)hModule);
            break;
        case DLL_THREAD_ATTACH:  
			break;
        case DLL_THREAD_DETACH:  
			break;
        case DLL_PROCESS_DETACH: 
			break;
    }
    return TRUE;
}





/*

	���������� ��� �������

*/

CHAR* __stdcall GetPluginName()
{
	return szPluginName;
}

/*
	���������:
	hWnd - ����� ����, ���� ����� �������� ��������� � �������
	wParam - R�� ���������� �������

*/
int __stdcall SendPressKey(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	if (ClearFireDllCounter(hWnd))
	{
		//SendNotifyMessage(hWnd, WM_KEYUP, wParam, lParam);
		//SendNotifyMessage(hWnd, WM_KEYDOWN, wParam, lParam);
		PostMessage(hWnd, WM_KEYDOWN, wParam, lParam);
	}
	return 1;
}

/*int __stdcall SendPressKey(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	// ���������� ������� �������� ����
	HWND hTiny = GetForegroundWindow();

	// ������������� �������� ���� �����
	SetForegroundWindow(hWnd);

	// �������� ��������� � ������� ������� l2 elephant
	keybd_event(wParam, 0x45, 1, 0);
	keybd_event(wParam, 0x45, 3, 0);

	// ������������� �������� ������� ���� �������
	SetForegroundWindow(hTiny);

	// ���������� �� 0, ��� ��� ������ ������
	return (int)hTiny;
}*/