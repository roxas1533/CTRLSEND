// dllmain.cpp : DLL アプリケーションのエントリ ポイントを定義します。
#include "pch.h"
#include <string>
#include<fstream>
#include <tchar.h>
#include "dllmain.h"
#define DLLEXPORT extern "C" __declspec(dllexport)

HINSTANCE g_hInst;

#pragma data_seg(".shared")
HHOOK g_hHook = NULL; 

bool isEnter = false;
bool isCtrl = false;
static DWORD dwTargetProcessId = 0;
static DWORD dwTargetProcessId2 = 0;
HWND hWnd;
#pragma data_seg()
#pragma comment(linker, "/SECTION:.shared,RWS")
DLLEXPORT void HookEnd();
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lp) {
	TCHAR strWindowText[1024];

	GetWindowText(hwnd, strWindowText, 1024);
	if (strWindowText[0] == 0) return TRUE;

	SendMessage((HWND)lp, LB_ADDSTRING, 0, (long)strWindowText);
	if (_tcsstr(strWindowText, TEXT("Discord")) != NULL && _tcsstr(strWindowText, TEXT("ジャンプ")) == NULL) {
		unsigned long processID = 0;
		GetWindowThreadProcessId(hwnd, &processID);
		dwTargetProcessId2 = processID;
		//std::wstring tee = std::to_wstring(processID);
		//MessageBox(NULL, strWindowText, L"HookStart", MB_OK);
		hWnd = hwnd;
		return false;
	}
	return TRUE;
}
void SetMyKeyboardProcTarget()
{
	HWND targetWnd;
	unsigned long processID = 0;
	targetWnd = FindWindow(L"Qt5QWindowIcon", NULL);
	unsigned long h = GetWindowThreadProcessId(targetWnd, &processID);

	static HWND hList;
	EnumWindows(EnumWindowsProc, (LPARAM)hList);

	dwTargetProcessId = processID;
}

DLLEXPORT LRESULT CALLBACK CALL(int code, WPARAM wParam, LPARAM lParam) {
	//SetMyKeyboardProcTarget();
	if (code == HC_ACTION)
	{
		CWPSTRUCT* cwp = (CWPSTRUCT*)lParam;

		if (cwp->message == WM_CREATE)
		{
			SetMyKeyboardProcTarget();
		}
	}

	return CallNextHookEx(g_hHook, code, wParam, lParam);
}

DLLEXPORT LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam)
{
	if (code < 0) {
		return CallNextHookEx(g_hHook, code, wParam, lParam);
	}
	DWORD dwPid = GetCurrentProcessId();
	if (dwTargetProcessId == dwPid) {
		if ((lParam & 0x80000000) == 0) {
			if (lParam & (1 << 30))
				return CallNextHookEx(g_hHook, code, wParam, lParam);

			if (wParam == VK_CONTROL) {
				isCtrl = true;
				return 1;
			}
			if (wParam == VK_RETURN) {
				if (!isCtrl) {
					INPUT ipt[4];
					ipt[0].type = INPUT_KEYBOARD;
					ipt[0].ki.wVk = VK_SHIFT;
					ipt[0].ki.wScan = MapVirtualKey(VK_SHIFT, 0);
					ipt[0].ki.dwExtraInfo = GetMessageExtraInfo();
					ipt[1].type = INPUT_KEYBOARD;
					ipt[1].ki.wVk = VK_RETURN;
					ipt[1].ki.wScan = MapVirtualKey(VK_RETURN, 0);
					ipt[1].ki.dwExtraInfo = GetMessageExtraInfo();
					ipt[2].type = INPUT_KEYBOARD;;
					ipt[2].ki.wVk = VK_SHIFT;
					ipt[2].ki.wScan = MapVirtualKey(VK_SHIFT, 0);
					ipt[2].ki.dwExtraInfo = GetMessageExtraInfo();
					ipt[2].ki.dwFlags = KEYEVENTF_KEYUP;
					ipt[3].type = INPUT_KEYBOARD;;
					ipt[3].ki.wVk = VK_RETURN;
					ipt[3].ki.wScan = MapVirtualKey(VK_RETURN, 0);
					ipt[3].ki.dwExtraInfo = GetMessageExtraInfo();
					ipt[3].ki.dwFlags = KEYEVENTF_KEYUP;
					SendInput(4, ipt, sizeof(INPUT));
					return 1;
				}
				else {
					INPUT ipt[3];
					ipt[0].type = INPUT_KEYBOARD;;
					ipt[0].ki.wVk = VK_CONTROL;
					ipt[0].ki.wScan = MapVirtualKey(VK_CONTROL, 0);
					ipt[0].ki.dwExtraInfo = GetMessageExtraInfo();
					ipt[0].ki.dwFlags = KEYEVENTF_KEYUP;
					ipt[1].type = INPUT_KEYBOARD;
					ipt[1].ki.wVk = VK_RETURN;
					ipt[1].ki.wScan = MapVirtualKey(VK_RETURN, 0);
					ipt[1].ki.dwExtraInfo = GetMessageExtraInfo();
					ipt[2].type = INPUT_KEYBOARD;;
					ipt[2].ki.wVk = VK_RETURN;
					ipt[2].ki.wScan = MapVirtualKey(VK_RETURN, 0);
					ipt[2].ki.dwExtraInfo = GetMessageExtraInfo();
					ipt[2].ki.dwFlags = KEYEVENTF_KEYUP;
					SendInput(3, ipt, sizeof(INPUT));
					return 1;
				}
			}
		}
		else {
			if (wParam == VK_CONTROL) {
				isCtrl = false;
			}
		}
	}
	else if (dwPid == dwTargetProcessId2) {
		if ((lParam & 0x80000000) == 0) {
			if (lParam & (1 << 30))
				return CallNextHookEx(g_hHook, code, wParam, lParam);
			if (wParam == VK_CONTROL) {
				isCtrl = true;
				return 1;
			}
			if (wParam == VK_RETURN&&!isEnter&& !isCtrl) {
				INPUT ipt[6];
				//MessageBox(NULL, L"撃ってる", L"HookStart", MB_OK);
				ipt[1].type = INPUT_KEYBOARD;;
				ipt[1].ki.wVk = VK_SHIFT;
				ipt[1].ki.wScan = MapVirtualKey(VK_SHIFT, 0);
				ipt[1].ki.dwExtraInfo = GetMessageExtraInfo();
				ipt[1].ki.dwFlags = KEYEVENTF_SCANCODE|KEYEVENTF_KEYUP;

				ipt[2].type = INPUT_KEYBOARD;
				ipt[2].ki.wVk = VK_SHIFT;
				ipt[2].ki.wScan = MapVirtualKey(VK_SHIFT, 0);
				ipt[2].ki.dwExtraInfo = GetMessageExtraInfo();
				ipt[2].ki.dwFlags = KEYEVENTF_SCANCODE;


				ipt[0].type = INPUT_KEYBOARD;
				ipt[0].ki.wVk = VK_RETURN;
				ipt[0].ki.wScan = MapVirtualKey(VK_RETURN, 0);
				ipt[0].ki.dwExtraInfo = GetMessageExtraInfo();
				ipt[0].ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;

				ipt[3].type = INPUT_KEYBOARD;
				ipt[3].ki.wVk = VK_RETURN;
				ipt[3].ki.wScan = MapVirtualKey(VK_RETURN, 0);
				ipt[3].ki.dwExtraInfo = GetMessageExtraInfo();
				ipt[3].ki.dwFlags = KEYEVENTF_SCANCODE;


				ipt[4].type = INPUT_KEYBOARD;;
				ipt[4].ki.wVk = VK_SHIFT;
				ipt[4].ki.wScan = MapVirtualKey(VK_SHIFT, 0);
				ipt[4].ki.dwExtraInfo = GetMessageExtraInfo();
				ipt[4].ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;

				ipt[5].type = INPUT_KEYBOARD;
				ipt[5].ki.wVk = VK_RETURN;
				ipt[5].ki.wScan = MapVirtualKey(VK_RETURN, 0);
				ipt[5].ki.dwExtraInfo = GetMessageExtraInfo();
				ipt[5].ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;

				SendInput(6, ipt, sizeof(INPUT));

				isEnter = true;
				return 1;
			}
			else if (wParam == VK_RETURN && isCtrl) {
				INPUT ipt[4];
				ipt[0].type = INPUT_KEYBOARD;;
				ipt[0].ki.wVk = VK_CONTROL;
				ipt[0].ki.wScan = MapVirtualKey(VK_CONTROL, 0);
				ipt[0].ki.dwExtraInfo = GetMessageExtraInfo();
				ipt[0].ki.dwFlags = KEYEVENTF_KEYUP;
				ipt[1].type = INPUT_KEYBOARD;;
				ipt[1].ki.wVk = VK_SHIFT;
				ipt[1].ki.wScan = MapVirtualKey(VK_SHIFT, 0);
				ipt[1].ki.dwExtraInfo = GetMessageExtraInfo();
				ipt[1].ki.dwFlags = KEYEVENTF_KEYUP;
				ipt[2].type = INPUT_KEYBOARD;
				ipt[2].ki.wVk = VK_RETURN;
				ipt[2].ki.wScan = MapVirtualKey(VK_RETURN, 0);
				ipt[2].ki.dwExtraInfo = GetMessageExtraInfo();
				ipt[3].type = INPUT_KEYBOARD;;
				ipt[3].ki.wVk = VK_RETURN;
				ipt[3].ki.wScan = MapVirtualKey(VK_RETURN, 0);
				ipt[3].ki.dwExtraInfo = GetMessageExtraInfo();
				ipt[3].ki.dwFlags = KEYEVENTF_KEYUP;
				SendInput(4, ipt, sizeof(INPUT));
				return 1;
			}
		}
		else {
			if (wParam == VK_SHIFT) {
				isEnter = false;
			}
			if (wParam == VK_CONTROL) {
				isCtrl = false;
			}
		}
	}
	return CallNextHookEx(g_hHook, code, wParam, lParam);
}

DLLEXPORT void HookStart()
{
	g_hHook = SetWindowsHookEx(WH_KEYBOARD, (HOOKPROC)KeyboardProc, g_hInst, 0);
	if (g_hHook == NULL) {
		MessageBox(NULL, L"フック開始は失敗しました", L"HookStart", MB_OK);
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