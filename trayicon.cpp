#include "trayicon.h"


void ShowBallon(PTRAYICON pTi, LPTSTR lpTitle, LPTSTR lpInfo)
{
	lstrcpy(pTi->Nid.szInfoTitle, lpTitle);
	lstrcpy(pTi->Nid.szInfo, lpInfo);
	pTi->Nid.uTimeout = 1000;
	pTi->Nid.dwInfoFlags = NIIF_USER;
	Shell_NotifyIcon(NIM_MODIFY, &pTi->Nid);
}

void CreateTrayIcon(PTRAYICON pTi, HWND hWnd, HICON hIcon, LPTSTR lpTip)
{
	pTi->Nid.cbSize = sizeof(NOTIFYICONDATA);
	pTi->Nid.hIcon = hIcon;
	lstrcpy(pTi->Nid.szTip, lpTip);
	pTi->Nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_INFO;
	pTi->Nid.uID = 123;
	pTi->Nid.hWnd = hWnd;
	pTi->Nid.uCallbackMessage = WM_SLIP;

	Shell_NotifyIcon(NIM_ADD, &pTi->Nid);
}

void UpdateTrayIcon(PTRAYICON pTi, HICON hIcon)
{

}

void DeleteTrayIcon(PTRAYICON pTi)
{
	Shell_NotifyIcon(NIM_DELETE, &pTi->Nid);
}

void TrayIconSetIcon(PTRAYICON pTi, HICON hIcon)
{
	pTi->Nid.hIcon = hIcon;
	Shell_NotifyIcon(NIM_MODIFY, &pTi->Nid);
}