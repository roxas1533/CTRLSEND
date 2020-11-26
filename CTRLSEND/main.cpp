#include <Windows.h>
#include <iostream>


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg;

    HINSTANCE hinst = LoadLibrary(TEXT("CTRLSENDLL.dll"));

    if (hinst) {
        typedef void (*Install)();
        typedef void (*Uninstall)();


        Install install = (Install)GetProcAddress(hinst, "HookStart");
        Uninstall uninstall = (Uninstall)GetProcAddress(hinst, "HookEnd");

        install();
        

        while (GetMessage(&msg, NULL, 0, 0) > 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        while (1) {
            Sleep(100);
        }
        uninstall();
    }

    return 0;
}