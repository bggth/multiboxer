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

	Возвращает имя плагина

*/

CHAR* __stdcall GetPluginName()
{
	return szPluginName;
}

/*
	Параметры:
	hWnd - хэндл окна, куда будем посылать сообщение о нажатии
	wParam - Rод нажимаемой клавиши

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
	// Запоминаем текущее активное окно
	HWND hTiny = GetForegroundWindow();

	// Устанавливаем активным окно бокса
	SetForegroundWindow(hWnd);

	// Посылаем сообщения о нажатии методом l2 elephant
	keybd_event(wParam, 0x45, 1, 0);
	keybd_event(wParam, 0x45, 3, 0);

	// Устанавливаем активным главное окно обратно
	SetForegroundWindow(hTiny);

	// Возвращаем не 0, что все прошло удачно
	return (int)hTiny;
}*/