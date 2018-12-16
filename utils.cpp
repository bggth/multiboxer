#include "utils.h"

VOID MakeCoolWindow(HWND hWnd)
{
	//RECT Rect;
	//GetClientRect(hWnd, &Rect);
	MARGINS Margins;
	BOOL bEnable = TRUE;
	//Margins.cxLeftWidth = 0;
	//Margins.cxRightWidth = 0;
	//Margins.cyBottomHeight = 25;
	//Margins.cyTopHeight = 0;
	Margins.cxLeftWidth = -1;
	Margins.cxRightWidth = -1;
	Margins.cyBottomHeight = -1;
	Margins.cyTopHeight = -1;
	//DwmIsCompositionEnabled(&bEnable);
	//SetLayeredWindowAttributes(hWnd, GetSysColor(COLOR_3DFACE), 1, LWA_COLORKEY);
	DwmExtendFrameIntoClientArea(hWnd, &Margins);
}

BOOL MyGetShortFileName(TCHAR* lpFileName, TCHAR* lpShortName)
{
	TCHAR* lpSh = lpFileName+lstrlen(lpFileName);
	return FALSE;
}

BOOL SaveFile(TCHAR* lpFileName, LPVOID lpBuffer, DWORD nSize)
{
	BOOL bRet = FALSE;
	DWORD nTiny;
	HANDLE hFile = CreateFile(lpFileName, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);
	if (INVALID_HANDLE_VALUE!=hFile)
	{
		bRet = WriteFile(hFile, lpBuffer, nSize, &nTiny, NULL);
		SetEndOfFile(hFile);
		CloseHandle(hFile);
	}
	return bRet;
}

BOOL LoadFile(TCHAR* lpFileName, LPVOID lpBuffer, DWORD nSize)
{
	BOOL bRet = FALSE;
	DWORD nTiny;
	HANDLE hFile = CreateFile(lpFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (INVALID_HANDLE_VALUE!=hFile)
	{
		bRet = ReadFile(hFile, lpBuffer, nSize, &nTiny, NULL);
		CloseHandle(hFile);
	}
	return bRet;
}

INT MyGetFileSize(TCHAR* lpFileName)
{
	INT iRet = -1;
	HANDLE hFile = CreateFile(lpFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (INVALID_HANDLE_VALUE!=hFile)
	{
		iRet = GetFileSize(hFile, NULL);
		CloseHandle(hFile);
	}

	return iRet;
}

BOOL ShowOpenDialog(HWND hWnd, TCHAR* lpFileName, TCHAR* lpFilter)
{
	/*OPENFILENAME Ofn;
	ZeroMemory(&Ofn, sizeof(OPENFILENAME));
	Ofn.lStructSize=sizeof(OPENFILENAME);
	Ofn.hInstance=GetModuleHandle(NULL);
	Ofn.hwndOwner=hWnd;
	Ofn.lpstrFilter=lpFilter;
	Ofn.lpstrFile=lpFileName;
	Ofn.nMaxFile=MAX_PATH;
	Ofn.Flags=OFN_HIDEREADONLY | OFN_FILEMUSTEXIST;
	//lstrcpy(lpFileName, NULL);
	return GetOpenFileName(&Ofn);*/
	return 0;
}

HICON GetShellIcon(LPTSTR lpFileName)
{
	/*SHFILEINFO FileInfo;
	ZeroMemory(&FileInfo, sizeof(SHFILEINFO));
	DWORD dwFlags = SHGFI_SYSICONINDEX | SHGFI_ICON;// SMALLICON;
	SHGetFileInfo(lpFileName, 0, &FileInfo, sizeof(SHFILEINFO), dwFlags);
	return FileInfo.hIcon;*/
	return NULL;
}

VOID GetAppPath(LPTSTR lpAppPath)
{
	static TCHAR szBuffer[MAX_PATH];
	GetModuleFileName(GetModuleHandle(NULL), szBuffer, 1024);
	for (int i=lstrlen(szBuffer); i>=0; i--)
	{
		if (szBuffer[i]=='\\')
		{
			szBuffer[i]='\0';
			lstrcpy(lpAppPath, szBuffer);
			return;
		}
	}
	return;
}

DWORD myatoi(char* text)
{
	DWORD ret = 0;
	char* next = text;
	char* end = text+lstrlen(text);
	while(next<end)
	{
		if ((*next>='0') && (*next<='9'))
			ret = ret*10+((DWORD)(*next)-0x30);
		next++;
	}
	return ret;
}

VOID MySleep(DWORD dwTime)
{
	//dwTime = dwTime*1000;
	HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	WaitForSingleObject(hEvent, dwTime);
}
