#include <windows.h>
#include <commctrl.h>
#include "resource.h"

#define APP_NAME TEXT("Мультибоксер")
#define APP_VER TEXT("0.17 *CLICKAL*")
#define APP_BUILD TEXT(__DATE__" " __TIME__)
#if _WIN64
#define APP_PLATFORM TEXT(" (x64 platform build)")
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#define APP_PLATFORM TEXT(" (x86 platform build)")
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif

VOID ShowHelp(HWND hParent);