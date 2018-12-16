#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <psapi.h>

#pragma comment (lib, "psapi.lib")

static CHAR szPluginName[] = "shock-world.com plugin";
TCHAR szBuffer[MAX_PATH];


DWORD Buffer = 0;
LPVOID Address;
bool Lock;

typedef struct _LIST
{
	HWND hWnd;
	DWORD dwAddress;
}LIST;

LIST List[100];
int nCount;

DWORD FindFireDll(HWND hWnd)
{
	HMODULE hMods[1024];
	DWORD dwNeeded;
	DWORD dwPID;
	DWORD dwRet;

	for (int i=0; i<=nCount; i++)
		if (List[i].hWnd == hWnd)
			return List[i].dwAddress;

	GetWindowThreadProcessId(hWnd, &dwPID);
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ, FALSE, dwPID);
	if (0!=hProcess)
	{
		if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &dwNeeded))
			for (int i=0; i<=(dwNeeded/sizeof(HMODULE)); i++)
			{
				TCHAR szFullModule[1024];
				TCHAR szModule[1024];
				ZeroMemory(szFullModule, 1024*sizeof(TCHAR));
				DWORD dwTiny = 1024;
				GetModuleFileNameEx(hProcess, hMods[i], szFullModule, dwTiny);//(hMods[i], szFullModule, dwTiny);
				int len = lstrlen(szFullModule);
				// c prog files\fire.dll
				ZeroMemory(szModule, 1024*sizeof(TCHAR));
				lstrcpyn(szModule, &szFullModule[len-8], 9);
				if (lstrcmpi(szModule, TEXT("fire.dll"))==0)
				{
					List[nCount].hWnd = hWnd;
					List[nCount].dwAddress = (DWORD)hMods[i];
					nCount++;
					dwRet = (DWORD)hMods[i];
				}
			}

	}

	CloseHandle(hProcess);
	return dwRet;
}

DWORD GetFireDllAddress(HWND hWnd)
{
	return FindFireDll(hWnd);
}

bool ClearFireDllCounter(HWND hWnd)
{
	DWORD dwPID;
	DWORD dwTiny;
	bool bRet = FALSE;;
	GetWindowThreadProcessId(hWnd, &dwPID);
	Address = (LPVOID)(GetFireDllAddress(hWnd) + 0x5B8FC);
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
	if (Lock)
		return 1;
	Lock = 1;
	if (ClearFireDllCounter(hWnd))
	{
		SendMessage(hWnd, WM_KEYUP, wParam, lParam);
		SendMessage(hWnd, WM_KEYDOWN, wParam, lParam);
		//SendMessage(hWnd, WM_KEYDOWN, wParam, lParam);
	}
	Lock = 0;
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