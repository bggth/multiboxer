#include <windows.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")
BOOL SaveFile(LPTSTR lpFileName, LPVOID lpBuffer, DWORD nSize);
BOOL LoadFile(LPTSTR lpFileName, LPVOID lpBuffer, DWORD nSize);
INT MyGetFileSize(LPTSTR lpFileName);
BOOL ShowOpenDialog(HWND hWnd, TCHAR* lpFileName, TCHAR* lpFilter);
HICON GetShellIcon(LPTSTR lpFileName);
BOOL MyGetShortFileName(TCHAR* lpFileName, TCHAR* lpShortName);
VOID MakeCoolWindow(HWND hWnd);
VOID GetAppPath(LPTSTR lpAppPath);
DWORD myatoi(char* text);
VOID MySleep(DWORD dwTime);