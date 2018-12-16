#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
//#include <uxtheme.h>
#include "trayicon.h"
#include "utils.h"
#include "resource.h"
#include "help.h"
#include "strlist.h"
#include "plugin.h"
#include "clicker.h"

#pragma comment(lib, "UxTheme.lib")


TCHAR szBuffer[MAX_PATH];

#define ID_SELECTHOTKEY 0x0666
#define ID_ACTIVATEHOTKEY 0x0667
#define ID_SWITCHWINDOW 0x6668


#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "Comdlg32.lib")
#ifndef _DEBUG
#pragma comment(linker, "/entry:WinMain")
int main()
{
	return 0;
}
#endif

HWND hL2Win;

typedef struct _BOX
{
	TCHAR szName[15];
	HWND hWnd;
	BOOL Main;
} *LPBOX, BOX;

typedef struct _ACTION
{
	int nBox;
	TCHAR szName[30];
	DWORD dwHotKey;
	DWORD dwHotKeyMods;
	DWORD dwAction;
	DWORD dwActionMods;
	DWORD dwTimer;
	BOOL bTimerEnable;
	DWORD dwCounter;
	BOOL bMultiAction;
	BOOL bMultiActionEnable;
	BOOL bChatAction;
	DWORD dwMultiActionCount;
	STRLIST MultiAction;
	TCHAR szMultiActionStr[200];
	BOOL bClicker;
	TCHAR szClickerName;
	HANDLE hThread;
	DWORD dwThread;
} *LPACTION, ACTION;

int BoxCount;
BOX Boxes[20];
int ActionCount;
ACTION Actions[100];
BOOL bHotKeyActive;
TRAYICON Ti;
HICON hNormalCartman;
HICON hGrayedCartman;
int SendMessageMethod;
int PlaySounds;
int ResetActionsList;
HWND hLink;

TCHAR szSettings[] = TEXT("settings");
TCHAR szMethod[] = TEXT("method");
TCHAR szSounds[] = TEXT("sounds");
TCHAR szResetAL[] = TEXT("resetactions");
TCHAR szIniFile[] = TEXT("\\settings.ini");
TCHAR szIniFileName[MAX_PATH];

PLUGIN* Plugins;


WNDPROC OldGetKeyEditProc;
BOOL CALLBACK NewGetKeyEditProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_KEYDOWN:
			wsprintf(szBuffer, TEXT("_%d"), wParam);
			SetWindowText(hWnd, szBuffer);
			break;
	}
	
	return CallWindowProc(OldGetKeyEditProc, hWnd, uMsg, wParam, lParam);
}
#ifndef GWL_WNDPROC 
#define GWL_WNDPROC -4
#endif
BOOL CALLBACK GetKeyDialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INITDIALOG:
			OldGetKeyEditProc = (WNDPROC)SetWindowLongPtr(GetDlgItem(hDlg, IDC_KEYEDIT), GWL_WNDPROC, (LONG_PTR)NewGetKeyEditProc);
			SetFocus(GetDlgItem(hDlg, IDC_KEYEDIT));
			break;
		case WM_KEYDOWN:
			//wsprintf(szBuffer, TEXT("_%d"), wParam);
			//SetDlgItemText(hDlg, IDC_KEYEDIT, szBuffer);
			break;
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDOK:
					break;
				case IDCANCEL:
					EndDialog(hDlg, 0);
					break;
			}
			break;
		case WM_CLOSE:
			EndDialog(hDlg, 0);
			break;
		default: 
            return 0;//DefWindowProc(hDlg, uMsg, wParam, lParam); 
	}


	/*	while(GetMessage(&Msg, NULL, 0, 0)) 
	{ 
		TranslateMessage(&Msg); //Преобразует сообщения клавиш в символы
		DispatchMessage(&Msg); //Передаёт сообщение соответствующей функции окна
	}*/
	return FALSE;
}


VOID LoadSettings()
{
	if (!lstrlen(szIniFileName))
	{
		GetAppPath(szIniFileName);
		lstrcat(szIniFileName, szIniFile);
	}
	SendMessageMethod = GetPrivateProfileInt(szSettings, szMethod, 0, szIniFileName);
	PlaySounds = GetPrivateProfileInt(szSettings, szSounds, 0, szIniFileName);
	ResetActionsList = GetPrivateProfileInt(szSettings, szResetAL, 0, szIniFileName);
	return;
}

VOID SaveSettings()
{
	wsprintf(szBuffer, TEXT("%d"), SendMessageMethod);
	WritePrivateProfileString(szSettings, szMethod, szBuffer, szIniFileName);
	wsprintf(szBuffer, TEXT("%d"), PlaySounds);
	WritePrivateProfileString(szSettings, szSounds, szBuffer, szIniFileName);
	wsprintf(szBuffer, TEXT("%d"), ResetActionsList);
	WritePrivateProfileString(szSettings, szResetAL, szBuffer, szIniFileName);
	return;
}

BOOL CALLBACK SettingsDialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INITDIALOG:

			// init method combobox list
			SendDlgItemMessage(hDlg, IDC_METHODCB, CB_ADDSTRING, NULL, (LPARAM)TEXT("Метод 1 (старый, глючит на руофе l2.ru)"));
			SendDlgItemMessage(hDlg, IDC_METHODCB, CB_ADDSTRING, NULL, (LPARAM)TEXT("Метод 2 (новый, если не работает старый) :)"));
			SendDlgItemMessage(hDlg, IDC_METHODCB, CB_ADDSTRING, NULL, (LPARAM)TEXT("Метод 3 (если глючат два других)"));
			SendDlgItemMessage(hDlg, IDC_METHODCB, CB_ADDSTRING, NULL, (LPARAM)TEXT("Метод 4 (привет l2 elephant)"));

			for(int i=0; i<GetPluginsCount(); i++)
				SendDlgItemMessage(hDlg, IDC_METHODCB, CB_ADDSTRING, NULL, (LPARAM)Plugins[i+1].szName);


			SendDlgItemMessage(hDlg, IDC_METHODCB, CB_SETCURSEL, SendMessageMethod, 0);

			// init sound checkbox
			CheckDlgButton(hDlg, IDC_SOUNDCB, PlaySounds?BST_CHECKED:BST_UNCHECKED);
			CheckDlgButton(hDlg, IDC_RESETALCB, ResetActionsList?BST_CHECKED:BST_UNCHECKED);
			break;
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDOK:
					SendMessageMethod = SendDlgItemMessage(hDlg, IDC_METHODCB, CB_GETCURSEL, 0, 0);
					PlaySounds = (IsDlgButtonChecked(hDlg, IDC_SOUNDCB)==BST_CHECKED);
					ResetActionsList = (IsDlgButtonChecked(hDlg, IDC_RESETALCB)==BST_CHECKED);
					EndDialog(hDlg, 0);
					break;
				case IDCANCEL:
					EndDialog(hDlg, 0);
					break;
			}
			break;
		case WM_CLOSE:
			EndDialog(hDlg, 0);
			break;
	}
	return FALSE;
}

VOID CALLBACK ActionTimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
VOID DoAction(HWND hDlg, int nAction);

VOID AddBox(LPTSTR lpName, BOOL IsMain)
{
	Boxes[BoxCount].Main = IsMain;
	lstrcpy(Boxes[BoxCount].szName, lpName);
	BoxCount++;
}


HWND hActiveWin;

//DWORD TextToKey(LPTSTR lpText)
//{
//	/// 1 2 3 A B C D 
//	if (lstrlen(lpText)>0)
//		if (lstrlen(lpText)==1)
//		{
//			TCHAR t = lpText[0];
//			// 0..9
//			if (t>='0' && t<='9')
//				return t;
//			// A..Z
//			else if (t>='A' && t<='Z')
//				return t;
//		}
//		else if (lstrlen(lpText)==2)
//			if (lpText[0]=='F')
//				if (lpText[1]>='1' && lpText[1]<='9')
//					return VK_F1+lpText[1]-'1';
//	return 0;
//
//}



//BOOL KeyToText(DWORD dwKey, LPTSTR lpBuff)
//{
//	if ((dwKey>='0' && dwKey<='9') || (dwKey>='A') && (dwKey<='Z'))
//		wsprintf(lpBuff, "%c", dwKey);
//	else if (dwKey>=VK_F1 || dwKey<=VK_F9)
//		wsprintf(lpBuff, "F%d", dwKey-VK_F1+1);
//	return TRUE;
//}

DWORD StripMods(LPTSTR lpBuff)
{
	// ctrl shift win
	DWORD dwMods = 0;
	int nCount = 0;
	for (int i=0; i<=strlen(lpBuff); i++)
	{
		switch (lpBuff[i])
		{
			case 'c':
				dwMods = dwMods|MOD_CONTROL;
				nCount++;
				break;
			case 's':
				dwMods = dwMods|MOD_SHIFT;
				nCount++;
				break;
			case 'w':
				dwMods = dwMods|MOD_WIN;
				nCount++;
				break;
			default:
				i = strlen(lpBuff)+1;
		}
	}
	lstrcpy(lpBuff, &lpBuff[nCount]);
	return dwMods;
}

BOOL KeyToText(DWORD dwKey, DWORD dwMods, LPTSTR lpBuff)
{
	lpBuff[0] = '\0';
	if (dwMods&MOD_CONTROL)
		lstrcat(lpBuff, TEXT("Ctrl+"));
	if (dwMods&MOD_SHIFT)
		lstrcat(lpBuff, TEXT("Shift+"));
	if (dwMods&MOD_WIN)
		lstrcat(lpBuff, TEXT("Win+"));
	switch (dwKey)
	{
		case VK_INSERT:
			lstrcat(lpBuff, TEXT("Insert"));
			break;
		case VK_DELETE:
			lstrcat(lpBuff, TEXT("Delete"));
			break;
		case VK_HOME:
			lstrcat(lpBuff, TEXT("Home"));
			break;
		case VK_END:
			lstrcat(lpBuff, TEXT("End"));
			break;
		case VK_PRIOR:
			lstrcat(lpBuff, TEXT("PageUp"));
			break;
		case VK_NEXT:
			lstrcat(lpBuff, TEXT("PageDown"));
			break;
	}

	if ((dwKey>='0' && dwKey<='9') || (dwKey>='A') && (dwKey<='Z'))
		wsprintf(lpBuff, "%s%c", lpBuff, dwKey);
	else if ((dwKey>=VK_F1) && (dwKey<=VK_F12))
		wsprintf(lpBuff, "%sF%d", lpBuff, dwKey-VK_F1+1);
	else if ((dwKey>=VK_NUMPAD0) && (dwKey<=VK_NUMPAD9))
		wsprintf(lpBuff, "%sN%d", lpBuff, dwKey-VK_NUMPAD0);
	else
		wsprintf(lpBuff, "%s_%d", lpBuff, dwKey);
	return TRUE;

}

DWORD TextToKey(LPTSTR lpText)
{
	if (lpText[0]=='_')
	{
		return myatoi(&lpText[1]);
	}
	int nLen = lstrlen(lpText);
	TCHAR t = NULL;
	t = lpText[0];
	switch (nLen)
	{
	case 1:
		if (t>='0' && t<='9')
			return t;
		else if (t>='A' && t<='Z')
			return t;
		break;
	case 2:
		if (t=='F')
			return VK_F1+lpText[1]-'1';
				// F1 0x70 + 0x31 - 0x31
				// F2 0x70 + 0x32 - 0x31
				// F10 0x79 + 0x30 - 0x30
				// F11 0x79 + 0x31 - 0x30
		else if (t=='N')
			return VK_NUMPAD0+lpText[1]-'0';
		break;
	case 3:
		if (t=='F')
			return VK_F10+lpText[2]-'0';
		break;
	}
	if (lstrcmpi(lpText, TEXT("Insert"))==0)
		return VK_INSERT;
	if (lstrcmpi(lpText, TEXT("Delete"))==0)
		return VK_DELETE;
	if (lstrcmpi(lpText, TEXT("Home"))==0)
		return VK_HOME;
	if (lstrcmpi(lpText, TEXT("End"))==0)
		return VK_END;
	if (lstrcmpi(lpText, TEXT("PageUp"))==0)
		return VK_PRIOR;
	if (lstrcmpi(lpText, TEXT("PageDown"))==0)
		return VK_NEXT;

	return 0;
}

int ActionSearchHotKey(DWORD dwHotKey, DWORD dwMods)
{
	//int nret = -1;
	for (int i=0; i<ActionCount; i++)
		if ((LOWORD(Actions[i].dwHotKey)==dwHotKey) && (LOWORD(Actions[i].dwHotKeyMods)==dwMods))
			return i;
	return -1;
}

VOID AddAction(int nBox, LPTSTR lpActionName, DWORD dwHotKey, DWORD dwHotKeyMod, DWORD dwAction, DWORD dwTimer)
{
	Actions[ActionCount].nBox = nBox;
	lstrcpy(Actions[ActionCount].szName, lpActionName);
	int nHK = ActionSearchHotKey(dwHotKey, dwHotKeyMod);
	if (nHK!=-1)
	{
		Actions[nHK].dwHotKey = Actions[nHK].dwHotKey | 0x10000;
		Actions[ActionCount].dwHotKey = dwHotKey | 0x10000;
	}
	else
		Actions[ActionCount].dwHotKey = dwHotKey;
	Actions[ActionCount].dwHotKeyMods = dwHotKeyMod;
	Actions[ActionCount].dwAction = dwAction;
	Actions[ActionCount].dwTimer = dwTimer;

	if (dwTimer==-1)
	{
		Actions[ActionCount].dwAction = InitClicker(lpActionName);
	}

	ActionCount++;
}

VOID AddMultiAction(int nBox, LPTSTR lpActionName, DWORD dwHotKey, DWORD dwHotKeyMod, LPTSTR lpMultiAction, DWORD dwTimer)
{
	Actions[ActionCount].nBox = nBox;
	lstrcpy(Actions[ActionCount].szName, lpActionName);
	int nHK = ActionSearchHotKey(dwHotKey, dwHotKeyMod);
	if (nHK!=-1)
	{
		Actions[nHK].dwHotKey = Actions[nHK].dwHotKey | 0x10000;
		Actions[ActionCount].dwHotKey = dwHotKey | 0x10000;
	}
	else
		Actions[ActionCount].dwHotKey = dwHotKey;
	Actions[ActionCount].dwHotKeyMods = dwHotKeyMod;
	//Actions[ActionCount].dwAction = dwAction;
	Actions[ActionCount].bMultiAction = TRUE;
	lstrcpy(Actions[ActionCount].szMultiActionStr, lpMultiAction);
	StrListInit(&Actions[ActionCount].MultiAction, 100, 10);
	StrListStrToData(&Actions[ActionCount].MultiAction, lpMultiAction);
	Actions[ActionCount].dwTimer = dwTimer;
	ActionCount++;

}

VOID AddChatAction(int nBox, LPTSTR lpActionName, DWORD dwHotKey, DWORD dwHotKeyMod, LPTSTR lpChatAction, DWORD dwTimer)
{
	Actions[ActionCount].nBox = nBox;
	lstrcpy(Actions[ActionCount].szName, lpActionName);
	int nHK = ActionSearchHotKey(dwHotKey, dwHotKeyMod);
	if (nHK!=-1)
	{
		Actions[nHK].dwHotKey = Actions[nHK].dwHotKey | 0x10000;
		Actions[ActionCount].dwHotKey = dwHotKey | 0x10000;
	}
	else
		Actions[ActionCount].dwHotKey = dwHotKey;
	Actions[ActionCount].dwHotKeyMods = dwHotKeyMod;
	//Actions[ActionCount].dwAction = dwAction;
	Actions[ActionCount].bChatAction = TRUE;
	lstrcpy(Actions[ActionCount].szMultiActionStr, lpChatAction);
	StrListInit(&Actions[ActionCount].MultiAction, 100, 100);
	StrListStrToData(&Actions[ActionCount].MultiAction, lpChatAction);
	//Actions[ActionCount].dwTimer = dwTimer;
	ActionCount++;

}

int GetChar(LPTSTR lpStr, TCHAR chCh)
{
	LPTSTR lpCur = lpStr;
	while (*lpCur)
	{
		if (*lpCur==chCh)
			return lpCur-lpStr+1;
		lpCur++;
	}
	return 0;
}


//BOOL GetNextToken

//VOID TEST()
//{
//		AddBox(TEXT("MyWhitePussy"));
//		AddBox(TEXT("MyBlackPussy"));
//		AddAction(0, TEXT("ActionName 1"), TextToKey("F1"), TextToKey("F1"));
//		AddAction(0, TEXT("ActionName 2"), TextToKey("F2"), TextToKey("F2"));
//		AddAction(1, TEXT("ActionName 3"), TextToKey("F3"), TextToKey("F3"));
//		AddAction(1, TEXT("ActionName 4"), TextToKey("F4"), TextToKey("F4"));
//		AddAction(0, TEXT("ActionName 5"), TextToKey("F6"), TextToKey("F5"));
//		AddAction(0, TEXT("ActionName 6"), TextToKey("F7"), TextToKey("F8"));
//		AddAction(1, TEXT("ActionName 7"), TextToKey("F8"), TextToKey("F9"));
//		AddAction(1, TEXT("ActionName 8"), TextToKey("F9"), TextToKey("F1"));
//
//}


BOOL GetNextToken(LPTSTR String, LPTSTR Token)
{
	// return FALSE if last token
	BOOL bRet = TRUE;
	int nSp = GetChar(String, TCHAR(' '));
	if (!nSp)
	{
		bRet = FALSE;
		nSp = GetChar(String, TCHAR('\n'))-1;
	}
	lstrcpyn(Token, String, nSp);
	return bRet;
}


BOOL LoadConfigFile(LPTSTR lpFileName)
{
	TCHAR String[200];
	TCHAR Token1[20];
	TCHAR Token2[20];
	TCHAR Token3[20];
	TCHAR Token4[200];
	TCHAR Token5[20];
	TCHAR Token6[20];


	DWORD dwConfigSize = MyGetFileSize(lpFileName);
	LPTSTR Config = (LPTSTR)VirtualAlloc(NULL, dwConfigSize, MEM_COMMIT, PAGE_READWRITE);
	if (!LoadFile(lpFileName, Config, dwConfigSize))
		return FALSE;
	int nPos = 0;
	while(TRUE)
	{
		
		int nStrLen = GetChar(&Config[nPos], TCHAR('\n'));
		if (nStrLen)
		{
			nStrLen++;
			lstrcpyn(String, &Config[nPos], nStrLen);

			int nn = 0;
			if (GetNextToken(String, Token1))
			{
				nn = nn+GetChar(&String[nn], TCHAR(' '));
				if (GetNextToken(&String[nn], Token2))
				{
					nn = nn+GetChar(&String[nn], TCHAR(' '));
					if (GetNextToken(&String[nn], Token3))
					{
						nn = nn+GetChar(&String[nn], TCHAR(' '));
						if (GetNextToken(&String[nn], Token4))
						{
							nn = nn+GetChar(&String[nn], TCHAR(' '));
							GetNextToken(&String[nn], Token5);
						}


					}

				}
			}
			DWORD dwHKMods;
			DWORD dwAMods;
			if (lstrcmpi(Token1, TEXT("MAIN"))==0)
				AddBox(Token2, TRUE);
			else if (lstrcmpi(Token1, TEXT("BOX"))==0)
				AddBox(Token2, FALSE);
			else if (lstrcmpi(Token1, TEXT("ACTION"))==0)
			{
				dwHKMods = StripMods(Token3);
				AddAction(BoxCount-1, Token2, TextToKey(Token3), dwHKMods, TextToKey(Token4), 0);
			}
			else if (lstrcmpi(Token1, TEXT("TIMER"))==0)
			{
				dwHKMods = StripMods(Token3);
				AddAction(BoxCount-1, Token2, TextToKey(Token3), dwHKMods, TextToKey(Token4), myatoi(Token5));
			}
			else if (lstrcmpi(Token1, TEXT("MULTIACTION"))==0)
			{
				dwHKMods = StripMods(Token3);
				AddMultiAction(BoxCount-1, Token2, TextToKey(Token3), dwHKMods, Token4, myatoi(Token5));
			}
			else if (lstrcmpi(Token1, TEXT("CLICKER"))==0)
			{
				dwHKMods = StripMods(Token3);
				AddAction(BoxCount-1, Token2, TextToKey(Token3), dwHKMods, 0, -1);
			}
			/*else if (lstrcmpi(Token1, TEXT("CHAT"))==0)
			{
				/////CHAT Social cZ /socialyes|/socialno|/socialdance
				
				dwHKMods = StripMods(Token3);
				//AddChatAction(
			}*/

			nPos = nPos+nStrLen-1;
		}
		else
			return TRUE;
	}

	return FALSE;
}

VOID InitActionHotKey(HWND hDlg)
{
	TCHAR szKey[20];
	bHotKeyActive = !bHotKeyActive;
	if (bHotKeyActive)
	{
		for (int i=0; i<ActionCount; i++)
			if (!RegisterHotKey(hDlg, i, Actions[i].dwHotKeyMods, LOWORD(Actions[i].dwHotKey)))
			{
				if (Actions[i].dwHotKey<0x10000)
				{
					KeyToText(LOWORD(Actions[i].dwHotKey), Actions[i].dwHotKeyMods, szKey);
					wsprintf(szBuffer, TEXT("Не могу зарегистрировать горячую клавишу [%s]!"), szKey);
					MessageBox(hDlg, szBuffer, APP_NAME, MB_ICONSTOP);
				}
			}
		TrayIconSetIcon(&Ti, hNormalCartman);
		ShowBallon(&Ti, APP_NAME, TEXT("Горячие клавиши активированы."));

		SetTimer(hDlg, 1, 10, ActionTimerProc);
	}
	else
	{
		for (int i=0; i<ActionCount; i++)
			UnregisterHotKey(hDlg, i);
		TrayIconSetIcon(&Ti, hGrayedCartman);
		ShowBallon(&Ti, APP_NAME, TEXT("Горячие клавиши деактивированы."));
		KillTimer(hDlg, 1);
	}

}

BOOL CALLBACK SelectBoxDialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int nCurSel;
	switch (uMsg)
	{
		case WM_INITDIALOG:
			hLink = hDlg;
			SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_CARTMAN)));
			hActiveWin = GetForegroundWindow();
			nCurSel = -1;			
			for (int i=0; i<BoxCount; i++)
				if (Boxes[i].hWnd==0)
				{
					wsprintf(szBuffer, TEXT("%s*"), Boxes[i].szName);
					ListBox_AddString(GetDlgItem(hDlg, IDC_BOXLIST), Boxes[i].Main?szBuffer:Boxes[i].szName);
					if (nCurSel==-1)
						nCurSel = i;
				}
				else
				{
					if (Boxes[i].Main)
						wsprintf(szBuffer, TEXT("%s* (0x%.8X)"), Boxes[i].szName, Boxes[i].hWnd);
					else
						wsprintf(szBuffer, TEXT("%s (0x%.8X)"), Boxes[i].szName, Boxes[i].hWnd);
					ListBox_AddString(GetDlgItem(hDlg, IDC_BOXLIST), szBuffer);
				}
				SetFocus(GetDlgItem(hDlg, IDC_BOXLIST));
				ListBox_SetCurSel(GetDlgItem(hDlg, IDC_BOXLIST), nCurSel);
				SetForegroundWindow(hDlg);
			break;
		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDOK:
					Boxes[ListBox_GetCurSel(GetDlgItem(hDlg, IDC_BOXLIST))].hWnd=hActiveWin;
					//SetWindowText(hActiveWin, Boxes[ListBox_GetCurSel(GetDlgItem(hDlg, IDC_BOXLIST))].szName);
					SetForegroundWindow(hActiveWin);
					EndDialog(hDlg, 0);
					break;
				case IDCANCEL:
					EndDialog(hDlg, 0);
					break;
			}
			break;
		case WM_CLOSE:
			EndDialog(hDlg, 0);
			break;
		case WM_DESTROY:
			hLink = 0;
			break;

	}
	return FALSE;
}


void MainDialog_InitActionList(HWND hList)
{
	ListView_SetExtendedListViewStyle(hList, LVS_EX_FULLROWSELECT|LVS_EX_DOUBLEBUFFER|LVS_EX_INFOTIP|LVS_EX_LABELTIP);

	HIMAGELIST hImageList = ImageList_Create(16, 16, ILC_COLOR32, 10, 0);
	ImageList_SetBkColor(hImageList, GetSysColor(COLOR_WINDOW));
	ImageList_AddIcon(hImageList, LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_DOC)));
	ImageList_AddIcon(hImageList, LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_CLOCK)));
	ImageList_AddIcon(hImageList, LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_CLOCKARROW)));
	ImageList_AddIcon(hImageList, LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MULTIDOC)));
	ImageList_AddIcon(hImageList, LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_MULTIDOCARROW)));

	ListView_SetImageList(hList, hImageList, LVSIL_SMALL);
	LV_COLUMN Lvc;
	ZeroMemory(&Lvc, sizeof(LV_COLUMN));
	Lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT;
	Lvc.pszText = szBuffer;
	Lvc.cx = 90;
	lstrcpy(szBuffer, TEXT("Окно"));
	//Lvc.fmt = LVCFMT_RIGHT;
	Lvc.iSubItem = 0;
	ListView_InsertColumn(hList, 0, &Lvc);
	lstrcpy(szBuffer, TEXT("Действие"));
	Lvc.iSubItem = 1;
	Lvc.cx = 150;
	ListView_InsertColumn(hList, 1, &Lvc);
	Lvc.cx = 80;
	lstrcpy(szBuffer, TEXT("Горячая клавиша"));
	Lvc.iSubItem = 1;
	Lvc.fmt = LVCFMT_CENTER;
	ListView_InsertColumn(hList, 2, &Lvc);
	lstrcpy(szBuffer, TEXT("Клавиша в окне"));
	Lvc.iSubItem = 1;
	ListView_InsertColumn(hList, 3, &Lvc);
	lstrcpy(szBuffer, TEXT("Таймер"));
	Lvc.iSubItem = 1;
	Lvc.cx = 60;
	ListView_InsertColumn(hList, 4, &Lvc);
}

void MainDialog_SetActionListIcon(HWND hList, int iItem, int iImgIndex)
{
	LV_ITEM Lvi;
	ZeroMemory(&Lvi, sizeof(LV_ITEM));
	Lvi.iItem = iItem;
	ListView_GetItem(hList, &Lvi);
	Lvi.mask = LVIF_IMAGE;
	Lvi.pszText = szBuffer;
	Lvi.cchTextMax = MAX_PATH;
	Lvi.iImage = iImgIndex;
	ListView_SetItem(hList, &Lvi);
}

void MainDialog_UpdateActionList(HWND hList)
{
	ListView_DeleteAllItems(hList);
	//ImageList_RemoveAll(hProcImageList);
	LV_ITEM Lvi;
	LVSETINFOTIP Lvt;
	int iListCount = 0;
	ZeroMemory(&Lvi, sizeof(LV_ITEM));
	//Lvi.
	Lvi.mask = LVIF_TEXT | LVIF_IMAGE;
	Lvi.pszText = szBuffer;
	Lvi.cchTextMax = MAX_PATH;
	Lvt.cbSize = sizeof(LVSETINFOTIP);
	for (int i=0; i<ActionCount; i++)
	{
		//if (Processes[i].bAvailable)
		if (Actions[i].bMultiAction)
			Lvi.iImage = 3;
		else if (Actions[i].dwTimer)
			Lvi.iImage = 1;
		else
			Lvi.iImage = 0;
		Lvi.iItem = iListCount++;
		//lstrcpy(szBuffer, Action[i].szProcessName);
		//if (Processes[i].bAvailable)
		//	Lvi.iImage = ImageList_AddIcon(hProcImageList, GetShellIcon(szBuffer));
		//else
		//	Lvi.iImage = ImageList_AddIcon(hProcImageList, LoadIcon(NULL, IDI_EXCLAMATION));

		lstrcpy(szBuffer, Boxes[Actions[i].nBox].szName);
		Lvi.iSubItem = 0;
		ListView_InsertItem(hList, &Lvi);
		//Lvt.iItem = iListCount;
		//Lvt.pszText = (LPWSTR)Boxes[Actions[i].nBox].szName;
		//if (!ListView_SetInfoTip(hList, &Lvt))
		//	OutputDebugString(TEXT("NOT SET TIP!!!!!"));

		//wsprintf(szBuffer, TEXT("%i (0x%0.p)"), Processes[i].dwPID, Processes[i].dwPID);
		lstrcpy(szBuffer, Actions[i].szName);
		Lvi.iSubItem = 1;
		Lvi.cchTextMax = lstrlen(szBuffer);
		ListView_SetItem(hList, &Lvi);
		Lvt.pszText = (LPWSTR)szBuffer;
		//ListView_SetInfoTip(hList, &Lvt);
		lstrcpy(szBuffer, TEXT(""));
		if (Actions[i].dwHotKey>0x10000)
		{
			KeyToText(LOWORD(Actions[i].dwHotKey), Actions[i].dwHotKeyMods, szBuffer);
			lstrcat(szBuffer, TEXT("*"));
		}
		else
		{
			KeyToText(Actions[i].dwHotKey, Actions[i].dwHotKeyMods, szBuffer);

		}
		Lvi.iSubItem = 2;
		Lvi.cchTextMax = lstrlen(szBuffer);
		ListView_SetItem(hList, &Lvi);

		if (Actions[i].bMultiAction)
			lstrcpy(szBuffer, Actions[i].szMultiActionStr);
		else
			KeyToText(Actions[i].dwAction, 0, szBuffer);
		Lvi.iSubItem = 3;
		Lvi.cchTextMax = lstrlen(szBuffer);
		ListView_SetItem(hList, &Lvi);

		if (Actions[i].dwTimer)
		{
			wsprintf(szBuffer, TEXT("%d"), Actions[i].dwTimer);
			Lvi.iSubItem = 4;
			Lvi.cchTextMax = lstrlen(szBuffer);
			ListView_SetItem(hList, &Lvi);
		}

		ListView_Scroll(hList, 0, 32000);
	}
}

byte wparam2byte(WPARAM wParam)
{
	return wParam;
}

void CALLBACK ResultCallBack(HWND hWnd, UINT uMsg, ULONG_PTR dwData, LRESULT lResult)
{
	return;
}
int MySendPressKey(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	BOOL bAlt = FALSE;
	HWND hTiny;
	for (;;)
	{
		if (0x8000&GetKeyState(VK_MENU))
			Sleep(100);
		else
			break;
	}

	switch (SendMessageMethod)
	{
		case 0:
			PostMessage(hWnd, WM_KEYUP, wParam, lParam);
			return PostMessage(hWnd, WM_KEYDOWN, wParam, lParam);
			//return PostMessage(hWnd, WM_KEYUP, wParam, lParam);
			return 1;
			break;
		case 1:
			return SendNotifyMessage(hWnd, WM_KEYDOWN, wParam, lParam);
			//return SendNotifyMessage(hWnd, WM_KEYUP, wParam, lParam);
			break;
		case 2:
			SendNotifyMessage(hWnd, WM_KEYUP, wParam, lParam);
			return SendNotifyMessage(hWnd, WM_KEYDOWN, wParam, lParam);
			break;
		case 3:
			//MessageBox(hWnd, "", "", 0);
			//return 1;
			hTiny = GetForegroundWindow();
			SetForegroundWindow(hWnd);
			//SetActiveWindow(hWnd);
			keybd_event(wparam2byte(wParam), 0x45, 1, 0);
			keybd_event(wparam2byte(wParam), 0x45, 3, 0);
			SetForegroundWindow(hTiny);
			//SetActiveWindow(hTiny);
			return (int)hTiny;
			break;
		default:
			return Plugins[SendMessageMethod-3].fnSendPressKey(hWnd, wParam, lParam);

			break;

	}
	
}



/*int MySendMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL bAlt = FALSE;
		
	for (;;)
	{
		if (0x8000&GetKeyState(VK_MENU))
			Sleep(100);
		else
			break;
	}

	switch (SendMessageMethod)
	{
		case 0:
			return PostMessage(hWnd, uMsg, wParam, lParam);
			break;
		case 1:
			return SendNotifyMessage(hWnd, uMsg, wParam, lParam);
			break;
	}
}*/

typedef struct _CLICKER_DATA
{
	DWORD id;
	HWND hWnd;
} CLICKER_DATA, *LPCLICKER_DATA;

DWORD WINAPI StartClickerThread(LPVOID lpParam)
{
	LPCLICKER_DATA data = (LPCLICKER_DATA)lpParam;
	//MessageBox(0, "start clicker", "", MB_ICONSTOP);
	StartClicker(data->id, data->hWnd);
	return 0;
}

VOID CALLBACK ActionTimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	for (int i=0; i<ActionCount; i++)
	{
		if (Actions[i].dwTimer)
		{
			if (Actions[i].bTimerEnable)
			{
				if (Actions[i].dwTimer==-1)
				{
					if (Actions[i].hThread==NULL)
					{
						_CLICKER_DATA data;
						DWORD ThreadId;
						data.hWnd = Boxes[Actions[i].nBox].hWnd;
						data.id = Actions[i].dwAction;
						Actions[i].hThread = CreateThread(NULL, 0, StartClickerThread, (LPVOID)&data, 0, &ThreadId);
						if (Actions[i].hThread == NULL)
						{
							int error = GetLastError();
							wsprintf(szBuffer, "0x%X", error);
							MessageBox(0, "createthread error", szBuffer, MB_ICONSTOP);
						}
					}

				}

				if (Actions[i].dwCounter>=Actions[i].dwTimer)
				{
					if (Actions[i].bMultiAction)
					{
						//MySendMessage(Boxes[Actions[i].nBox].hWnd, WM_KEYDOWN, TextToKey(StrListGet(&Actions[i].MultiAction, Actions[i].dwMultiActionCount)), 1);
						MySendPressKey(Boxes[Actions[i].nBox].hWnd, TextToKey(StrListGet(&Actions[i].MultiAction, Actions[i].dwMultiActionCount)), 1);
						Actions[i].dwMultiActionCount++;
						if (Actions[i].dwMultiActionCount>Actions[i].MultiAction.nCount)
						{
							Actions[i].dwMultiActionCount = 0;
							DoAction(hWnd, i);
						}
					}
					else
						//MySendMessage(Boxes[Actions[i].nBox].hWnd, WM_KEYDOWN, (WPARAM)Actions[i].dwAction, 1);
					{
						MySendPressKey(Boxes[Actions[i].nBox].hWnd, (WPARAM)Actions[i].dwAction, 1);

					}
					Actions[i].dwCounter = 0;
				}
				else
					Actions[i].dwCounter = Actions[i].dwCounter+10;
			}
			else
			{
				if (Actions[i].hThread!=NULL)
				{
					TerminateThread(Actions[i].hThread, 0);
					Actions[i].hThread=NULL;
				}
			}
		}
	}
}

VOID Exit(HWND hDlg)
{
	SaveSettings();
	DeleteTrayIcon(&Ti);
	EndDialog(hDlg, 0);
}

VOID SwitchWindow(HWND hDlg)
{
	DWORD dwStyle = GetWindowLong(hDlg, GWL_STYLE);
	if ((dwStyle & WS_MINIMIZE)==WS_MINIMIZE)
	{
		ShowWindow(hDlg, SW_SHOW);
		ShowWindow(hDlg, SW_RESTORE);
	}
	else
		ShowWindow(hDlg, SW_MINIMIZE);
}

VOID DoAction(HWND hDlg, int nAction)
{

	if (Boxes[Actions[nAction].nBox].hWnd==0)
		return;
	
	if (Actions[nAction].dwTimer)
	{
		Actions[nAction].bTimerEnable = !Actions[nAction].bTimerEnable;
		if (Actions[nAction].bTimerEnable)
		{
			if (ResetActionsList)
				Actions[nAction].dwMultiActionCount = 0;
			Actions[nAction].dwCounter = Actions[nAction].dwTimer;
		}
		if (PlaySounds)
			MessageBeep(Actions[nAction].bTimerEnable?MB_ICONINFORMATION:MB_ICONEXCLAMATION);
		if (Actions[nAction].bMultiAction)
			MainDialog_SetActionListIcon(GetDlgItem(hDlg, IDC_ACTIONLIST), nAction, Actions[nAction].bTimerEnable?4:3);
		else
			MainDialog_SetActionListIcon(GetDlgItem(hDlg, IDC_ACTIONLIST), nAction, Actions[nAction].bTimerEnable?2:1);
	}
	else
	{
		//if (0==MySendMessage(Boxes[Actions[nAction].nBox].hWnd, WM_KEYDOWN, (WPARAM)Actions[nAction].dwAction, 1))
		if (0==MySendPressKey(Boxes[Actions[nAction].nBox].hWnd, (WPARAM)Actions[nAction].dwAction, 1))
		{
			wsprintf(szBuffer, TEXT("Ошибка отправки команды. GetLastError() = %d"), GetLastError());
			MessageBox(GetForegroundWindow(), szBuffer, APP_NAME, MB_ICONSTOP);
		}
	}
}

BOOL CALLBACK MainDialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_ACTIVATE:
			//MakeCoolWindow(hDlg);
			break;
		case WM_INITDIALOG:
			LoadSettings();

			Plugins = InitPluginSystem();
			SearchPlugins();

			RegisterHotKey(hDlg, ID_SELECTHOTKEY, MOD_WIN, VK_F10);
			RegisterHotKey(hDlg, ID_ACTIVATEHOTKEY, MOD_WIN, VK_F11);
			RegisterHotKey(hDlg, ID_SWITCHWINDOW, MOD_WIN, VK_F12);
			SetWindowText(hDlg, APP_NAME TEXT(" ") APP_VER);
			SendMessage(hDlg, WM_SETICON, ICON_BIG, (LPARAM)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_CARTMAN)));
			SetWindowTheme(GetDlgItem(hDlg, IDC_ACTIONLIST), L"Explorer", NULL);
			if (!LoadConfigFile(TEXT("config.ini")))
			{
				MessageBox(hDlg, TEXT("Не могу открыть файл config.ini!"), APP_NAME, MB_ICONSTOP);
				ExitProcess(0);
			}
			MainDialog_InitActionList(GetDlgItem(hDlg, IDC_ACTIONLIST));
			MainDialog_UpdateActionList(GetDlgItem(hDlg, IDC_ACTIONLIST));
			
			Ti.hWnd = hDlg;
			hNormalCartman = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_CARTMAN));
			hGrayedCartman = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_CARTMANGRAYED));
			CreateTrayIcon(&Ti, hDlg, hNormalCartman, APP_NAME);
			InitActionHotKey(hDlg);
			break;

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDOK:
					//InitActionHotKey(hDlg);
					//ShowWindow(hDlg, SW_MINIMIZE);
					break;
				case IDC_SETTINGS:
					DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_SETTINGS), hDlg, (DLGPROC)SettingsDialogProc, 0);
					break;

				case IDC_GETKEY:
					if (bHotKeyActive)
						InitActionHotKey(hDlg);
					DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_GETKEY), hDlg, (DLGPROC)GetKeyDialogProc, 0);
					if (!bHotKeyActive)
						InitActionHotKey(hDlg);
					break;

				case IDC_HELP:
						ShowHelp(hDlg);
					break;

				case IDC_EDIT:
					ShellExecute(hDlg, "open", "config.ini", 0, 0, SW_NORMAL);
					//Exit(hDlg);
					break;

				case IDCANCEL:
					Exit(hDlg);
					break;
			}
			break;

		case WM_SLIP:
				switch(lParam)
				{
					//case WM_LBUTTONDBLCLK:
						//InitActionHotKey(hDlg);
					//break;
					case WM_LBUTTONDOWN:
						SwitchWindow(hDlg);
						break;
				}
			break;

		case WM_SIZE:
			switch(wParam)
			{
				case SIZE_MINIMIZED:
				//case SIZE_RESTORED:
					ShowWindow(hDlg, SW_HIDE);
					break;
			}
			break;

		case WM_HOTKEY:
			if (wParam==ID_SELECTHOTKEY)
			{
				if (hLink)
					SetForegroundWindow(hLink);
				else
					DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_SELECT), hDlg, (DLGPROC)SelectBoxDialogProc, 0);
			}
			else if (wParam==ID_ACTIVATEHOTKEY)
				InitActionHotKey(hDlg);
			else if (wParam==ID_SWITCHWINDOW)
				SwitchWindow(hDlg);
			else
			{
				int ii = wParam;
				if (ii<=ActionCount)
				{
					if (Actions[ii].dwHotKey>0x10000)
						{
							for (int i=0; i<ActionCount; i++)
							{
								if (Actions[ii].dwHotKey==Actions[i].dwHotKey)
									DoAction(hDlg, i);
							}
						}
						else
							DoAction(hDlg, ii);
				}
				else
					MessageBox(hDlg, APP_NAME, TEXT("Непонятное действие"), MB_ICONSTOP);
			}
			break;

		case WM_DESTROY:
			Exit(hDlg);
			break;
	}

	return 0;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	InitCommonControls();
	HWND hCopy = FindWindow(NULL, APP_NAME " " APP_VER);
	if (hCopy)
	{
		ShowWindow(hCopy, SW_RESTORE);
		SetForegroundWindow(hCopy);
	}
	else
	{
		DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_MAIN), 0, (DLGPROC)MainDialogProc, 0);
	}
	ExitProcess(0);
}