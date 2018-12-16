#include "clicker.h"
#include "utils.h"

using namespace std;

string newline = "\n";
string space = " ";
int method = 0;
class Script 
{
	vector<string> commands;
	bool active;
	HWND window;
	int pos;
public:
	int id;
	Script()
	{
		//log(");
	}

	void load(string filename)
	{
		ifstream input(filename.c_str());
		string line;
		while(getline(input, line))
		{
			commands.push_back(line);
		}
		//log(commands[0]);
	}

	void execute(HWND hwnd)
	{
		window = hwnd;
		//log("execute");
		pos = 0;
		while (pos<commands.size())
		{
			vector<string> params = split(commands[pos], ' ');
			if (params[0]=="message")
			{
				messagecommand(window, commands[pos]);
			}
			else if (params[0]=="mouse")
			{
				mousecommand(window, commands[pos]);
			}
			else if (params[0]=="keyb")
			{
				keybcommand(window, commands[pos]);
			}

			else if (params[0]=="sleep")
			{
				sleepcommand(commands[pos]);
			}
			else if (params[0]=="goto")
			{
				pos = atoi(params[1].c_str());
				pos--;
			}

			else if (params[0]=="method")
			{
				method = atoi(params[1].c_str());
				pos--;
			}


			pos++;
		}
		//execute(0);
	}
};

vector<Script> scripts;

std::string line;

void split(const string &s, char delim, vector<string> &elems) {
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
}

vector<string> split(const string &s, char delim) {
    vector<string> elems;
    split(s, delim, elems);
    return elems;
} 

void log(string s)
{
	MessageBox(0, s.c_str(), "", 0);
}

int InitClicker(LPSTR filename)
{
	Script s = Script();
	s.id = scripts.size()+1000;
	s.load(filename);
	scripts.push_back(s);
	return s.id;
}

int findscriptbyid(int id)
{
	for (int i=0; i<scripts.size(); i++)
	{
		if (scripts[i].id==id)
			return i;
	}
	return -1;
}

void StartClicker(int id, HWND window)
{
	int i = findscriptbyid(id);
	scripts[i].execute(window);
}

void messagecommand(HWND window, string command)
{
	vector<string> params = split(command, ' ');
	log(params[1]);
}

void sleepcommand(string command)
{
	vector<string> params = split(command, ' ');
	int time = atoi(params[1].c_str());
	MySleep(time);
}


void mousecommand(HWND window, string command)
{
	//mouse cl 1000 1000

	//PostMessage(window, WM_KEYDOWN, MAKEWPARAM('S', 0), 0);

	vector<string> params = split(command, ' ');
	int x = atoi(params[2].c_str());
	int y = atoi(params[3].c_str());

	DWORD wParam = 0;
	DWORD dwMessage1 = 0;
	DWORD dwMessage2 = 0;
	if (params[1].find("r")!=string::npos)
	{
		dwMessage1 = WM_RBUTTONDOWN;
		dwMessage2 = WM_RBUTTONUP;
		wParam = MK_RBUTTON;
	}
	if (params[1].find("l")!=string::npos)
	{
		dwMessage1 = WM_LBUTTONDOWN;
		dwMessage2 = WM_LBUTTONUP;
		//wParam = MK_LBUTTON;
	}

	if (params[1].find("c")!=string::npos)
	{
		//wParam = wParam & MK_CONTROL;
		MySendMessage(window, WM_KEYDOWN, VK_CONTROL, 0);
	}
	if (params[1].find("s")!=string::npos)
	{
		//wParam = wParam + MK_SHIFT;
		MySendMessage(window, WM_KEYDOWN, VK_LSHIFT, 0);

	}

	//SetCursorPos(x, y);
	MySleep(100);
	DWORD lParam = MAKELPARAM(y, x);
	MySendMessage(window, dwMessage1, wParam, lParam);
	MySleep(100);
	MySendMessage(window, dwMessage2, 0, lParam);

	if (params[1].find("c")!=string::npos)
	{
		//wParam = wParam & MK_CONTROL;
		MySendMessage(window, WM_KEYUP, VK_CONTROL, 0);
	}
	if (params[1].find("s")!=string::npos)
	{
		//wParam = wParam + MK_SHIFT;
		MySendMessage(window, WM_KEYUP, VK_LSHIFT, 0);

	}

}

void MySetCursorPos(int x, int y)
{
	INPUT input;
	input.type = INPUT_MOUSE;
	input.mi.mouseData=0;
	input.mi.dx =  x*(65536/GetSystemMetrics(SM_CXSCREEN));//x being coord in pixels
	input.mi.dy =  y*(65536/GetSystemMetrics(SM_CYSCREEN));//y being coord in pixels
	input.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
	SendInput(1,&input,sizeof(input));
}

void keybcommand(HWND window, string command)
{
	//keyb csa A

	int key;
	vector<string> params = split(command, ' ');
	if (params.size()==2)
	{
		key = (int)params[1].c_str()[0];
	}
	else if (params.size()==3)
	{
		key = (int)params[2].c_str()[0];
		
	}
	
	if (params[1].find("c")!=string::npos)
	{
		//wParam = wParam & MK_CONTROL;
		MySendMessage(window, WM_KEYUP, VK_CONTROL, 0);
	}

	if (params[1].find("s")!=string::npos)
	{
		//wParam = wParam + MK_SHIFT;
		MySendMessage(window, WM_KEYDOWN, VK_LSHIFT, 0);

	}

	MySendMessage(window, WM_KEYDOWN, MAKEWPARAM(key, 0), 0);
	MySleep(100);
	MySendMessage(window, WM_KEYUP, MAKEWPARAM(key, 0), 0);


	if (params[1].find("c")!=string::npos)
	{
		//wParam = wParam & MK_CONTROL;
		MySendMessage(window, WM_KEYUP, VK_CONTROL, 0);
	}
	if (params[1].find("s")!=string::npos)
	{
		//wParam = wParam + MK_SHIFT;
		MySendMessage(window, WM_KEYUP, VK_LSHIFT, 0);

	}
}

byte wparam2byte_(WPARAM wParam)
{
	return wParam;
}

void MySendMessage(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	if (method == 0)
	{
		PostMessage(hwnd, umsg, wparam, lparam);
	}
	else if (method == 1)
	{
		SendNotifyMessage(hwnd, umsg, wparam, lparam);
	}
	else if (method == 2)
	{
		HWND htiny = GetForegroundWindow();
		SetForegroundWindow(hwnd);
		//SetActiveWindow(hWnd);
		keybd_event(wparam2byte_(wparam), 0x45, 1, 0);
		keybd_event(wparam2byte_(wparam), 0x45, 3, 0);
		SetForegroundWindow(htiny);
	}
}