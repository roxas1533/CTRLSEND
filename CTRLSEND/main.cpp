#include <Windows.h>
#include <iostream>
#include "../CTRLSENDLL/dllmain.h"
unsigned long GetTargetThreadIdFromWindow(const wchar_t* className,const  wchar_t* windowName)
{
    HWND targetWnd;
    HANDLE hProcess;
    unsigned long processID = 0;

    targetWnd = FindWindow(className, windowName);
   unsigned long h= GetWindowThreadProcessId(targetWnd, &processID);
   std::cout << processID;
   return h;
}

int main(int argc, TCHAR* argv[]) {
    MSG msg;
    unsigned long threadID = GetTargetThreadIdFromWindow(L"Qt5QWindowIcon", NULL);
    printf("TID: %i", threadID);

    HINSTANCE hinst = LoadLibrary(TEXT("CTRLSENDLL.dll"));

    if (hinst&&threadID) {
        typedef void (*Install)(unsigned long);
        typedef void (*Uninstall)();

        dwPid = threadID;

        Install install = (Install)GetProcAddress(hinst, "HookStart");
        Uninstall uninstall = (Uninstall)GetProcAddress(hinst, "HookEnd");

        std::cout << dwPid;
        install(threadID);
        

        while (GetMessage(&msg, NULL, 0, 0) > 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        getchar();
        uninstall();
    }

    return 0;
}