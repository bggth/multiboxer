#include <windows.h>
#include <shellapi.h>

#define WM_SLIP WM_USER+666

typedef struct
{
	HWND hWnd;
	NOTIFYICONDATA Nid;
} *PTRAYICON, TRAYICON;

void CreateTrayIcon(PTRAYICON pTi, HWND hWnd, HICON hIcon, LPTSTR lpTip);
void UpdateTrayIcon(PTRAYICON pTi, HICON hIcon);
void DeleteTrayIcon(PTRAYICON pTi);
void ShowBallon(PTRAYICON pTi, LPTSTR lpTitle, LPTSTR lpInfo);
void TrayIconSetIcon(PTRAYICON pTi, HICON hIcon);