#include "plugin.h"
#include "utils.h"



static PLUGIN* Plugins;
TCHAR szPluginPath[MAX_PATH];
TCHAR szPluginSearch[MAX_PATH];
static TCHAR szBuffer[MAX_PATH];
int PluginsCount = 0;

int LoadPlugin(LPTSTR lpFileName);

PLUGIN* InitPluginSystem()
{
	Plugins = (PLUGIN*)VirtualAlloc(NULL, 100*sizeof(PLUGIN), MEM_COMMIT, PAGE_READWRITE);


	return Plugins;
}

int GetPluginsCount()
{
	return PluginsCount;
}

int SearchPlugins()
{
	WIN32_FIND_DATA Find;
	HANDLE hFind;
	GetAppPath(szPluginPath);
	lstrcat(szPluginPath, TEXT("\\plugins"));
	lstrcpy(szPluginSearch, szPluginPath);
	lstrcat(szPluginSearch, TEXT("\\*.dll"));

	ZeroMemory(&Find, sizeof(WIN32_FIND_DATA));

	hFind = FindFirstFile(szPluginSearch, &Find);

	if (hFind != INVALID_HANDLE_VALUE)
		do
		{
			LoadPlugin(Find.cFileName);
		} while(FindNextFile(hFind, &Find));
	FindClose(hFind);

	return PluginsCount;
}

int LoadPlugin(LPTSTR lpFileName)
{
	fGetPluginName func;
	fSendPressKey func2;
	lstrcpy(szBuffer, szPluginPath);
	lstrcat(szBuffer, TEXT("\\"));
	lstrcat(szBuffer, lpFileName);
	HINSTANCE hPlugin = LoadLibrary(szBuffer);
	if (!hPlugin)
		return 0;

	func = (fGetPluginName) GetProcAddress(hPlugin, "GetPluginName");
	func2 = (fSendPressKey) GetProcAddress(hPlugin, "SendPressKey");

	if (func)
	{
		PluginsCount++;
		Plugins[PluginsCount].hPlugin= hPlugin;
		Plugins[PluginsCount].fnGetPluginName = func;
		Plugins[PluginsCount].fnSendPressKey = func2;
		lstrcpy(Plugins[PluginsCount].szModuleName, szBuffer);
		
		__try
		{
	
			lstrcpy(Plugins[PluginsCount].szName, Plugins[PluginsCount].fnGetPluginName());
		}
		__finally
		{
		}
		return PluginsCount;

		
	}

}