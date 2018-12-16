#include <windows.h>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

int InitClicker(LPSTR filename);
void StartClicker(int id, HWND window);
void log(std::string s);
vector<std::string> split(const std::string &s, char delim);
void messagecommand(HWND window, string command);
void mousecommand(HWND window, string command);
void keybcommand(HWND window, string command);
void sleepcommand(string command);
void MySetCursorPos(int x, int y);
void MySendMessage(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);