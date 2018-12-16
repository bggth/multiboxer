#include "strlist.h"


int StrListInit(STRLIST *StrList, int nMax, int nStrMaxLen)
{
	StrList->nCount = 0;
	StrList->nMax = nMax;
	StrList->nStrMaxLen = nStrMaxLen;
	StrList->lpBuffer = (LPBYTE)VirtualAlloc(NULL, sizeof(TCHAR)*nMax*nStrMaxLen, MEM_COMMIT, PAGE_READWRITE);
	return nMax*nStrMaxLen;
}


int StrListAddStr(STRLIST *StrList, LPTSTR lpAddStr)
{
	lstrcpy((LPTSTR)&StrList->lpBuffer[StrList->nCount*sizeof(TCHAR)*StrList->nStrMaxLen], lpAddStr);
	return ++StrList->nCount;
}


int StrListInsertStr(STRLIST *StrList, LPSTR lpInsertStr, int nAfterIndex)				// возвращает количество элементов
{
	return StrList->nCount;
}

int StrListDelete(STRLIST *StrList, int nDelIndex)										// возвращает количество элементов
{
	return StrList->nCount;
}

LPTSTR StrListGet(STRLIST *StrList, int nIndex)
{
	return (LPTSTR)&StrList->lpBuffer[sizeof(TCHAR)*StrList->nStrMaxLen*nIndex];
}

int StrListSearchStr(STRLIST *StrList, LPTSTR lpSearchStr)								// возвращает индекс строки lpSearchStr или -1 если строка не найдена
{
	int i;
	for (i=0; i<=StrList->nCount; i++)
	{
		if (lstrcmp(StrListGet(StrList, i), lpSearchStr)==0)
			return i;
	}
	return -1;
}

int StrListStrToData(STRLIST* StrList, LPTSTR str)
{
	memset(StrList->lpBuffer, 0, sizeof(char)*StrList->nMax*StrList->nStrMaxLen);
	//char* elem = (char*) malloc(sizeof(char)*StrList->nMax);
	char* elem = (char*)VirtualAlloc(NULL, StrList->nMax, MEM_COMMIT, PAGE_READWRITE);
	char* pos = strchr(str, '|');
	int i = 0;
	while (pos)
	{
		strncpy(elem, &str[i], pos-&str[i]);
		elem[pos-&str[i]]='\0';
		StrListAddStr(StrList, elem);
		i = i+pos-&str[i]+1;
		pos = strchr(&str[i], '|');
	}
	strncpy(elem, &str[i], strlen(str)-i);
	elem[strlen(str)-i]='\0';
	StrListAddStr(StrList, elem);
	VirtualFree(elem, 0, MEM_RELEASE);
	return StrList->nCount;
}

int StrListFree(STRLIST *StrList)														// возвращает  какую-нибудь
{
	return VirtualFree(StrList->lpBuffer, 0, MEM_RELEASE);
}