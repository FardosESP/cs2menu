#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <string>
#include <filesystem>
#include <chrono>
#include <thread>

namespace fs = std::filesystem;

DWORD GetProcessIdByName(const std::wstring& processName)
{
    DWORD pid = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32W pe = { sizeof(pe) };
        if (Process32FirstW(snapshot, &pe))
        {
            do {
                if (processName == pe.szExeFile) { pid = pe.th32ProcessID; break; }
            } while (Process32NextW(snapshot, &pe));
        }
        CloseHandle(snapshot);
    }
    return pid;
}

// FIX Bug #2: usar wstring y LoadLibraryW para soportar rutas con espacios/unicode
bool InjectDLL(DWORD pid, const std::wstring& dllPath)
{
    HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (!process)
    {
        std::cout << "[-] OpenProcess fallo. Error: " << GetLastError() << std::endl;
        return false;
    }

    size_t pathSize = (dllPath.size() + 1) * sizeof(wchar_t);
    LPVOID remoteBuf = VirtualAllocEx(process, NULL, pathSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!remoteBuf)
    {
        std::cout << "[-] VirtualAllocEx fallo. Error: " << GetLastError() << std::endl;
        CloseHandle(process);
        return false;
    }

    WriteProcessMemory(process, remoteBuf, dllPath.c_str(), pathSize, NULL);

    LPTHREAD_START_ROUTINE pLoadLibraryW = (LPTHREAD_START_ROUTINE)GetProcAddress(
        GetModuleHandleW(L"kernel32.dll"), "LoadLibraryW"
    );

    HANDLE remoteThread = CreateRemoteThread(process, NULL, 0, pLoadLibraryW, remoteBuf, 0, NULL);

    if (remoteThread)
    {
        // FIX Bug #1: esperar a que LoadLibraryW termine ANTES de liberar la memoria
        WaitForSingleObject(remoteThread, 5000);
        CloseHandle(remoteThread);
        VirtualFreeEx(process, remoteBuf, 0, MEM_RELEASE); // ahora es seguro liberar
        CloseHandle(process);
        return true;
    }

    std::cout << "[-] CreateRemoteThread fallo. Error: " << GetLastError() << std::endl;
    VirtualFreeEx(process, remoteBuf, 0, MEM_RELEASE);
    CloseHandle(process);
    return false;
}

int main()
{
    std::cout << "========================================" << std::endl;
    std::cout << "   CS2MENU - LAUNCHER                   " << std::endl;
    std::cout << "========================================" << std::endl;

    std::wstring processName = L"cs2.exe";
    // FIX Bug #2: ruta como wstring desde el principio
    fs::path dllPath = fs::current_path() / L"cs2menu.dll";

    if (!fs::exists(dllPath))
    {
        std::cout << "[-] No se encontro cs2menu.dll en: " << dllPath.string() << std::endl;
        std::cout << "[-] Asegurate de que la DLL esta en la misma carpeta que el launcher." << std::endl;
        Sleep(5000);
        return 1;
    }

    std::cout << "[*] DLL encontrada: " << dllPath.string() << std::endl;
    std::cout << "[*] Esperando a que abras CS2..." << std::endl;

    DWORD pid = 0;
    while (pid == 0)
    {
        pid = GetProcessIdByName(processName);
        if (pid == 0) Sleep(500);
    }

    std::cout << "[+] CS2 detectado! (PID: " << pid << ")" << std::endl;
    std::cout << "[*] Esperando 15 segundos para que el juego cargue..." << std::endl;

    for (int i = 15; i > 0; i--)
    {
        std::cout << "[i] Inyectando en " << i << " segundos...  \r" << std::flush;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    std::cout << std::endl;

    if (InjectDLL(pid, dllPath.wstring()))
    {
        std::cout << "[OK] Inyeccion completada. Pulsa INSERT en CS2 para ver el menu." << std::endl;
    }
    else
    {
        std::cout << "[FAIL] Error al inyectar. Prueba ejecutar como Administrador." << std::endl;
    }

    Sleep(5000);
    return 0;
}