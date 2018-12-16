//#include <commctrl.h>
#include "help.h"


BOOL CALLBACK HelpDialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INITDIALOG:
#define APP_HELP "ѕрограмма дл€ назначени€ гор€чих клавиш на передачу сообщений нажати€ клавиш выбранным окнам. "\
	"ћожет быть использована дл€ одновременного управлени€ несколькими окнами в различных онлайн-играх."\
	"\r\n\r\n‘ормат файла config.ini:"\
	"\r\nBOX »м€ќкна"\
	"\r\nACTION »м€ƒействи€ <√ор€ча€ лавиша> < лавиша¬ќкне>"\
	"\r\nTIMER »м€ƒействи€ <√ор€ча€ лавиша> < лавиша¬ќкне> <¬рем€¬ћиллисекундах>"\
	"\r\nMULTIACTION »м€ƒействи€ <√ор€ча€ лавиша> <—писок лавиш¬ќкне> <¬рем€¬ћиллисекундах>"\
	"\r\nƒопустимые клавиши: 1, 2, ..., 9, 0, A, B, ..., Z, F1, F2, ..., F12,"\
	"\r\nInsert, Delete, Home, End, PageUp, PageDown."\
	"\r\n лавиши цифровой клавиатуры: N0, N1, N2, ..., N9."\
	"\r\n“акже клавиши можно задавать через их дес€тичный код, в формате _< од лавиши>"\
	"\r\n\r\nƒл€ того чтобы прив€зать активное окно нажмите [Win]+F10."\
	"\r\nƒл€ того чтобы активировать/деактивировать гор€чие клавиши нажимайте [Win]+F11."\
	"\r\nƒл€ того чтобы скрыть/показать окно программы нажимайте [Win]+F12."
			SetDlgItemText(hDlg, IDC_HELPEDIT, APP_HELP);
			SetDlgItemText(hDlg, IDC_BUILD, APP_BUILD " " APP_PLATFORM);

					//MessageBox(hDlg, APP_NAME " " APP_VER "\n" APP_BUILD " " APP_PLATFORM APP_HELP, TEXT("ѕомощь"), MB_ICONINFORMATION);

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDOK:
				case IDCANCEL:
					EndDialog(hDlg, 0);
					break;
			}
			break;
		case WM_NOTIFY:
			switch (((LPNMHDR)lParam)->code)
			{
			    case NM_CLICK:          // Fall through to the next case.
    			case NM_RETURN:
					PNMLINK pNMLink = (PNMLINK)lParam;
					LITEM   item    = pNMLink->item;
					if ((((LPNMHDR)lParam)->idFrom == IDC_LINK) && (item.iLink == 0))
					{
						ShellExecute(NULL, TEXT("open"), TEXT("http://vk.com/multiboxer"), NULL, NULL, SW_SHOW);
					}
            		/*else if (wcscmp(item.szID, TEXT("idInfo")) == 0)
					{
						MessageBox(hDlg, L"This isn't much help.", L"Example", MB_OK);
					}*/
			    	break;
			}
			break;

		case WM_CLOSE:
			EndDialog(hDlg, 0);
			break;
	}
	return FALSE;
}

VOID ShowHelp(HWND hParent)
{
	DialogBoxParam( GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_HELP), hParent, (DLGPROC)HelpDialogProc, 0);
}