#include <Windows.h>
#include <iostream>


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    if (!AttachConsole(ATTACH_PARENT_PROCESS)) {
        // �G�N�X�v���[������N�������ꍇ�͐V�K�ɃR���\�[�������蓖�Ă�
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
        
        std::cout << "���b�Z�[�W�J�n";

        while (GetMessage(&msg, NULL, 0, 0) > 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        std::cout << "���b�Z�[�W�I��";
        while (1) {
            Sleep(100);
        }
        std::cout << "�A���C���X�g�[���O";
        uninstall();
        std::cout << "�A���C���X�g�[����";

    }

    return 0;
}