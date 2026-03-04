#include <Windows.h>
#include <iostream>
#include <thread>
#include <chrono>
#include "Hooks.h"
#include "ImGui_Renderer.h"

extern bool bShowMenu;

void CreateDebugConsole()
{
    if (AllocConsole()) {
        FILE* f;
        freopen_s(&f, "CONOUT$", "w", stdout);
        freopen_s(&f, "CONOUT$", "w", stderr);
        std::cout << "========================================" << std::endl;
        std::cout << "      CS2MENU DEBUG CONSOLE             " << std::endl;
        std::cout << "========================================" << std::endl;
    }
}

DWORD WINAPI MainThread(LPVOID lpParameter)
{
    std::this_thread::sleep_for(std::chrono::seconds(3));
    CreateDebugConsole();
    std::cout << "[*] DLL Inyectada. Inicializando..." << std::endl;

    Hooks::Initialize();

    std::cout << "[*] Hook instalado." << std::endl;
    std::cout << "[*] INSERT = mostrar/ocultar menu" << std::endl;
    std::cout << "[*] END    = desinyectar" << std::endl;

    bool insertWasDown = false;

    while (!(GetAsyncKeyState(VK_END) & 0x8000))
    {
        bool insertDown = (GetAsyncKeyState(VK_INSERT) & 0x8000) != 0;
        if (insertDown && !insertWasDown)
        {
            bShowMenu = !bShowMenu;
            std::cout << "[*] Menu " << (bShowMenu ? "abierto" : "cerrado") << std::endl;
        }
        insertWasDown = insertDown;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    std::cout << "[*] Desinyectando..." << std::endl;
    Hooks::Shutdown();
    ImGui_Renderer::Shutdown();
    FreeConsole();
    FreeLibraryAndExitThread(static_cast<HMODULE>(lpParameter), 0);
    return 0;
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        HANDLE h = CreateThread(nullptr, 0, MainThread, hModule, 0, nullptr);
        if (h) CloseHandle(h);
    }
    return TRUE;
}