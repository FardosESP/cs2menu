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
        
        // Set console to support ASCII box drawing
        SetConsoleOutputCP(CP_UTF8);
        
        // Set console title
        SetConsoleTitleA("CS2MENU v1.0 - Premium Edition");
        
        // Professional header with ASCII box drawing
        std::cout << "\n";
        std::cout << "  +===========================================================+\n";
        std::cout << "  |                                                           |\n";
        std::cout << "  |              CS2MENU - PREMIUM EDITION                    |\n";
        std::cout << "  |                   Build 14138.5                           |\n";
        std::cout << "  |                                                           |\n";
        std::cout << "  +===========================================================+\n";
        std::cout << "\n";
    }
}

DWORD WINAPI MainThread(LPVOID lpParameter)
{
    CreateDebugConsole();
    std::cout << "  [→] Inicializando sistema..." << std::endl;
    
    // Wait for game modules to load (dynamic check, no hardcoded delay)
    std::cout << "  [→] Esperando modulos del juego..." << std::endl;
    int attempts = 0;
    while (attempts < 30)  // Max 30 seconds
    {
        HMODULE client = GetModuleHandleA("client.dll");
        HMODULE engine = GetModuleHandleA("engine2.dll");
        
        if (client && engine)
        {
            std::cout << "  [✓] Modulos cargados (client.dll + engine2.dll)" << std::endl;
            break;
        }
        
        std::this_thread::sleep_for(std::chrono::seconds(1));
        attempts++;
    }
    
    if (attempts >= 30)
    {
        std::cout << "  [✗] ERROR: Timeout esperando modulos del juego" << std::endl;
        std::cout << "  [!] La inyeccion fue demasiado temprana" << std::endl;
        std::cout << "  [!] Presiona END para salir" << std::endl;
        
        while (!(GetAsyncKeyState(VK_END) & 0x8000))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        FreeConsole();
        FreeLibraryAndExitThread(static_cast<HMODULE>(lpParameter), 0);
        return 1;
    }

    // Initialize hooks
    try
    {
        Hooks::Initialize();
        std::cout << "  [✓] Hooks instalados correctamente" << std::endl;
    }
    catch (...)
    {
        std::cout << "  [✗] ERROR: Fallo al instalar hooks" << std::endl;
        std::cout << "  [!] Presiona END para salir" << std::endl;
        
        while (!(GetAsyncKeyState(VK_END) & 0x8000))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        FreeConsole();
        FreeLibraryAndExitThread(static_cast<HMODULE>(lpParameter), 0);
        return 1;
    }

    std::cout << "\n  +===========================================================+\n";
    std::cout << "  |                    CONTROLES                              |\n";
    std::cout << "  +===========================================================+\n";
    std::cout << "  |  INSERT  ->  Abrir/Cerrar Menu                            |\n";
    std::cout << "  |  F9      ->  Escanear Offsets (Manual)                    |\n";
    std::cout << "  |  F10     ->  Diagnostico Completo                         |\n";
    std::cout << "  |  END     ->  Desinyectar Cheat                            |\n";
    std::cout << "  +===========================================================+\n";
    std::cout << "\n";

    bool insertWasDown = false;

    while (!(GetAsyncKeyState(VK_END) & 0x8000))
    {
        bool insertDown = (GetAsyncKeyState(VK_INSERT) & 0x8000) != 0;
        if (insertDown && !insertWasDown)
        {
            bShowMenu = !bShowMenu;
            std::cout << "  [" << (bShowMenu ? "✓" : "○") << "] Menu " 
                      << (bShowMenu ? "abierto" : "cerrado") << std::endl;
        }
        insertWasDown = insertDown;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    std::cout << "\n  [→] Desinyectando..." << std::endl;
    
    try
    {
        Hooks::Shutdown();
        ImGui_Renderer::Shutdown();
    }
    catch (...)
    {
        std::cout << "[-] ERROR durante shutdown (ignorando)." << std::endl;
    }
    
    FreeConsole();
    FreeLibraryAndExitThread(static_cast<HMODULE>(lpParameter), 0);
    return 0;
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);
        HANDLE h = CreateThread(nullptr, 0, MainThread, hModule, 0, nullptr);
        if (!h)
        {
            // Failed to create thread - log error
            OutputDebugStringA("[CS2MENU] ERROR: Failed to create main thread\n");
            return FALSE;
        }
        CloseHandle(h);
    }
    return TRUE;
}