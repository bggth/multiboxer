/*


	StrList library - библиотека бля работы со списками строк LPTSTR
	
	
*/

#include <windows.h>


typedef struct _STRLIST
{
	int		nMax;				// максимальное количество элементов
	int		nCount;				// количество элементов
	int		nStrMaxLen;			// максимальная длина элемента
	LPBYTE	lpBuffer;			// буфер для строк
} STRLIST, *LPSTRLIST;


int StrListInit(STRLIST *StrList, int nMax, int nStrMaxLen);							// возвращает размер lpBuffer в байтах
int StrListAddStr(STRLIST *StrList, LPTSTR lpAddStr);									// возвращает количество элементов
int StrListInsertStr(STRLIST *StrList, LPSTR lpInsertStr, int nAfterIndex);				// возвращает количество элементов
int StrListDelete(STRLIST *StrList, int nDelIndex);										// возвращает количество элементов
LPTSTR StrListGet(STRLIST *StrList, int nIndex);
int StrListStrToData(STRLIST* StrList, LPTSTR str);
int StrListSearchStr(STRLIST *StrList, LPTSTR lpSearchStr);								// возвращает индекс строки lpSearchStr
int StrListFree(STRLIST *StrList);														// возвращает хуету какую-нибудь
