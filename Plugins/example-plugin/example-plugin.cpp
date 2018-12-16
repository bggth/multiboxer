#define WIN32_LEAN_AND_MEAN
#include <windows.h>

CHAR szPluginName[] = "c example plugin";
TCHAR szBuffer[MAX_PATH];

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

CHAR* __stdcall GetPluginName()
{
	return szPluginName;
}

int __stdcall SendPressKey(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	wsprintf(szBuffer, TEXT("Hello world!\nhWnd=0x%.8X, wParam0x%.8x, lParam0x%.8x"), hWnd, wParam, lParam);
	MessageBox(hWnd, szBuffer, TEXT("example"), MB_ICONINFORMATION);
	return 1;
}