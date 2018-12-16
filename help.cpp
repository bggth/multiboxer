//#include <commctrl.h>
#include "help.h"


BOOL CALLBACK HelpDialogProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INITDIALOG:
#define APP_HELP "��������� ��� ���������� ������� ������ �� �������� ��������� ������� ������ ��������� �����. "\
	"����� ���� ������������ ��� �������������� ���������� ����������� ������ � ��������� ������-�����."\
	"\r\n\r\n������ ����� config.ini:"\
	"\r\nBOX �������"\
	"\r\nACTION ����������� <��������������> <������������>"\
	"\r\nTIMER ����������� <��������������> <������������> <�������������������>"\
	"\r\nMULTIACTION ����������� <��������������> <�����������������> <�������������������>"\
	"\r\n���������� �������: 1, 2, ..., 9, 0, A, B, ..., Z, F1, F2, ..., F12,"\
	"\r\nInsert, Delete, Home, End, PageUp, PageDown."\
	"\r\n������� �������� ����������: N0, N1, N2, ..., N9."\
	"\r\n����� ������� ����� �������� ����� �� ���������� ���, � ������� _<����������>"\
	"\r\n\r\n��� ���� ����� ��������� �������� ���� ������� [Win]+F10."\
	"\r\n��� ���� ����� ������������/�������������� ������� ������� ��������� [Win]+F11."\
	"\r\n��� ���� ����� ������/�������� ���� ��������� ��������� [Win]+F12."
			SetDlgItemText(hDlg, IDC_HELPEDIT, APP_HELP);
			SetDlgItemText(hDlg, IDC_BUILD, APP_BUILD " " APP_PLATFORM);

					//MessageBox(hDlg, APP_NAME " " APP_VER "\n" APP_BUILD " " APP_PLATFORM APP_HELP, TEXT("������"), MB_ICONINFORMATION);

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