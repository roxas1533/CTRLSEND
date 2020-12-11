// dllmain.cpp : DLL アプリケーションのエントリ ポイントを定義します。
#include "pch.h"
#include <string>
#include <sstream>
#include<fstream>
#include <tchar.h>
#include<imm.h>
#include <time.h>
#pragma comment(lib, "imm32.lib")
#include "dllmain.h"
#include "mess.h"
#include <vector>
#define DLLEXPORT extern "C" __declspec(dllexport)

HINSTANCE g_hInst;

#pragma data_seg(".shared")
typedef struct _MY_DATA_STRUCT
{
	int myInt32;
	float myFloat;
	bool myBool;
}MY_DATA_STRUCT;

HHOOK g_hHook = NULL;
HHOOK g_hHook2 = NULL;
std::vector<std::vector<INPUT>> in;
bool isEnter = false;
bool isCtrl = false;
bool isLastMulti = false;
bool isImeNotifyStuckForDiscord = false;
bool isImeNotifyStuckForLine = false;
bool lineKeylog;
clock_t  pushTime = 0;
static DWORD dwTargetProcessId = 0;
static DWORD dwTargetProcessId2 = 0;
HWND hWnd;
HIMC lngInputContextHandle;
bool isIme = false;
#pragma data_seg()
#pragma comment(linker, "/SECTION:.shared,RWS")
DLLEXPORT void HookEnd();

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lp) {
	TCHAR strWindowText[1024];

	GetWindowText(hwnd, strWindowText, 1024);
	if (strWindowText[0] == 0) return TRUE;

	SendMessage((HWND)lp, LB_ADDSTRING, 0, (long)strWindowText);
	if (_tcsstr(strWindowText, TEXT("Discord")) != NULL && _tcsstr(strWindowText, TEXT("ジャンプ")) == NULL) {
		GetClassName(hwnd, strWindowText, 1024);
		//if (_tcsstr(strWindowText, TEXT("Chrome_WidgetWin_1")) != NULL)
		unsigned long processID = 0;
		GetWindowThreadProcessId(hwnd, &processID);
		dwTargetProcessId2 = processID;

		//std::stringstream ss;
		//ss << std::hex << hwnd;
		//MessageBox(NULL, ss.str().c_str(), "HookStart", MB_OK);
		hWnd = hwnd;

		return false;
	}
	return TRUE;
}
void SetMyKeyboardProcTarget()
{
	HWND targetWnd;
	unsigned long processID = 0;
	targetWnd = FindWindow("Qt5QWindowIcon", NULL);
	unsigned long h = GetWindowThreadProcessId(targetWnd, &processID);
	lngInputContextHandle = ImmGetContext(targetWnd);


	static HWND hList;
	EnumWindows(EnumWindowsProc, (LPARAM)hList);

	dwTargetProcessId = processID;
}

void addInput(std::vector<INPUT>& inputVec, WORD key, int iskeyUp) {
	INPUT in;
	in.type = INPUT_KEYBOARD;
	in.ki.wVk = key;
	in.ki.wScan = MapVirtualKey(key, 0);
	in.ki.dwExtraInfo = GetMessageExtraInfo();
	in.ki.dwFlags = KEYEVENTF_SCANCODE | iskeyUp;
	inputVec.push_back(in);
}

void MakeInput() {
	std::vector<INPUT> temp;
	addInput(temp, VK_SHIFT, 0);
	addInput(temp, VK_RETURN, 0);
	addInput(temp, VK_SHIFT, KEYEVENTF_KEYUP);
	addInput(temp, VK_RETURN, KEYEVENTF_KEYUP);
	in.push_back(temp);

	temp.clear();
	addInput(temp, VK_CONTROL, KEYEVENTF_KEYUP);
	addInput(temp, VK_RETURN, 0);
	addInput(temp, VK_RETURN, KEYEVENTF_KEYUP);
	in.push_back(temp);

	temp.clear();
	addInput(temp, VK_RETURN, KEYEVENTF_KEYUP);
	addInput(temp, VK_SHIFT, KEYEVENTF_KEYUP);
	addInput(temp, VK_SHIFT, 0);
	addInput(temp, VK_RETURN, 0);
	addInput(temp, VK_SHIFT, KEYEVENTF_KEYUP);
	addInput(temp, VK_RETURN, KEYEVENTF_KEYUP);
	in.push_back(temp);

	temp.clear();
	addInput(temp, VK_CONTROL, KEYEVENTF_KEYUP);
	addInput(temp, VK_RETURN, 0);
	addInput(temp, VK_RETURN, KEYEVENTF_KEYUP);
	in.push_back(temp);
}

DLLEXPORT LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam) {
	if (code < 0) {
		return CallNextHookEx(g_hHook, code, wParam, lParam);
	}

	DWORD dwPid = GetCurrentProcessId();
	if (dwTargetProcessId == dwPid) {
		if (in.size() == 0)
			MakeInput();
		if ((lParam & 0x80000000) == 0) {
			lineKeylog = true;
			if (lParam & (1 << 30))
				return CallNextHookEx(g_hHook, code, wParam, lParam);

			if (wParam == VK_CONTROL) {
				isCtrl = true;
			}
			if (wParam == VK_RETURN && isImeNotifyStuckForLine) {
				isImeNotifyStuckForLine = false;
				return CallNextHookEx(g_hHook, code, wParam, lParam);
			}
			if (wParam == VK_RETURN) {
				if (!isCtrl) {
					SendInput(in[0].size(), in.at(0).data(), sizeof(INPUT));
					return 1;
				}
				else {
					SendInput(in[1].size(), in.at(1).data(), sizeof(INPUT));
					return 1;
				}
			}
		}
		else {
			if (wParam == VK_CONTROL) {
				isCtrl = false;
			}
			lineKeylog = false;
		}
		return CallNextHookEx(g_hHook, code, wParam, lParam);
	}
	else if (dwTargetProcessId2 == dwPid) {
		if (in.size() == 0)
			MakeInput();
		if ((lParam & 0x80000000) == 0) {
			if (lParam & (1 << 30))
				return CallNextHookEx(g_hHook, code, wParam, lParam);

			if (clock() - pushTime > 30) {
				isEnter = false;
			}
			pushTime = clock();
			if (wParam == VK_RETURN && isLastMulti) {
				isLastMulti = false;
				return CallNextHookEx(g_hHook, code, wParam, lParam);
			}
			if (wParam == VK_CONTROL) {
			
					isCtrl = true;
					return 1;
			}
				if (wParam == VK_RETURN && !isEnter && !isCtrl) {
					isEnter = true;
					SendInput(in[2].size(), in[2].data(), sizeof(INPUT));
					return 1;
				}
				else if (wParam == VK_RETURN && !isEnter && isCtrl) {
					isEnter = true;

					SendInput(in[3].size(), in[3].data(), sizeof(INPUT));
					return 1;
				}
			}
			else {
				if (wParam == VK_CONTROL) {
					isCtrl = false;
				}
			}
		}

		return CallNextHookEx(g_hHook, code, wParam, lParam);
}
LRESULT CALLBACK GetIme(int code, WPARAM wParam, LPARAM lParam){
	DWORD dwPid = GetCurrentProcessId();

	if (dwTargetProcessId == dwPid) {
		if (((CWPSTRUCT*)lParam)->message == WM_IME_COMPOSITION) {
			if(lineKeylog)
				isImeNotifyStuckForLine = true;
		}
	}else if (dwTargetProcessId2 == dwPid) {
		if (((CWPSTRUCT*)lParam)->message == WM_IME_NOTIFY) {
			isImeNotifyStuckForDiscord = true;
		}
		if (((CWPSTRUCT*)lParam)->message == EM_GETSEL) {
			isIme = true;
			isLastMulti = true;
			if (isImeNotifyStuckForDiscord) {
				if (*((LPDWORD)(((CWPSTRUCT*)lParam)->wParam)) == 100) {
					isIme = true;
				}
				else {
					isIme = false;
					isLastMulti = false;
				}
				isImeNotifyStuckForDiscord = false;
			}
		}
	}
	return CallNextHookEx(g_hHook2, code, wParam, lParam);
}
DLLEXPORT void HookStart()
{

	g_hHook = SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC)KeyboardProc, g_hInst, 0);

	if (g_hHook == NULL) {
		MessageBox(NULL, "フック開始は失敗しました", "HookStart", MB_OK);
		return;
	}
	g_hHook2 = SetWindowsHookEx(WH_CALLWNDPROC, (HOOKPROC)GetIme, g_hInst, 0);

	if (g_hHook2 == NULL) {
		MessageBox(NULL, "GetImeの開始失敗", "HookStart", MB_OK);
		return;
	}
}

DLLEXPORT void HookEnd()
{
	UnhookWindowsHookEx(g_hHook);
}

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
		SetMyKeyboardProcTarget();
		g_hInst = (HINSTANCE)hModule;   // DLLモジュールのハンドル取得
	}
	return TRUE;
}