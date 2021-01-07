#include <Windows.h>
#include <iostream>


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    if (!AttachConsole(ATTACH_PARENT_PROCESS)) {
        // エクスプローラから起動した場合は新規にコンソールを割り当てる
        AllocConsole();
    }
    FILE* fpOut = NULL;
    freopen_s(&fpOut,"CONOUT$", "w", stdout);
    MSG msg;

    HINSTANCE hinst = LoadLibrary(TEXT("CTRLSENDLL.dll"));

    if (hinst) {
        typedef void (*Install)();
        typedef void (*Uninstall)();


        Install install = (Install)GetProcAddress(hinst, "HookStart");
        Uninstall uninstall = (Uninstall)GetProcAddress(hinst, "HookEnd");

        install();
        
        std::cout << "メッセージ開始";

        while (GetMessage(&msg, NULL, 0, 0) > 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        std::cout << "メッセージ終了";
        while (1) {
            Sleep(100);
        }
        std::cout << "アンインストール前";
        uninstall();
        std::cout << "アンインストール後";

    }

    return 0;
}