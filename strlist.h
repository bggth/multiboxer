/*


	StrList library - ���������� ��� ������ �� �������� ����� LPTSTR
	
	
*/

#include <windows.h>


typedef struct _STRLIST
{
	int		nMax;				// ������������ ���������� ���������
	int		nCount;				// ���������� ���������
	int		nStrMaxLen;			// ������������ ����� ��������
	LPBYTE	lpBuffer;			// ����� ��� �����
} STRLIST, *LPSTRLIST;


int StrListInit(STRLIST *StrList, int nMax, int nStrMaxLen);							// ���������� ������ lpBuffer � ������
int StrListAddStr(STRLIST *StrList, LPTSTR lpAddStr);									// ���������� ���������� ���������
int StrListInsertStr(STRLIST *StrList, LPSTR lpInsertStr, int nAfterIndex);				// ���������� ���������� ���������
int StrListDelete(STRLIST *StrList, int nDelIndex);										// ���������� ���������� ���������
LPTSTR StrListGet(STRLIST *StrList, int nIndex);
int StrListStrToData(STRLIST* StrList, LPTSTR str);
int StrListSearchStr(STRLIST *StrList, LPTSTR lpSearchStr);								// ���������� ������ ������ lpSearchStr
int StrListFree(STRLIST *StrList);														// ���������� ����� �����-������
