#include <windows.h>


typedef char* (__stdcall* fGetPluginName)(void);
typedef int (__stdcall* fSendPressKey)(HWND, WPARAM, LPARAM);
//typedef int (__stdcall* fBoxSelected(int, HWND);

typedef struct _PLUGIN
{
	TCHAR szName[MAX_PATH];
	TCHAR szModuleName[MAX_PATH];
	HINSTANCE hPlugin;
	fGetPluginName fnGetPluginName;
	fSendPressKey fnSendPressKey;
}*LPPLUGIN, PLUGIN;

PLUGIN* InitPluginSystem();
int SearchPlugins();
int GetPluginsCount();

DWORD __stdcall PluginCall(void* pParam);
