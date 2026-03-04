#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <string>
#include <filesystem>
#include <chrono>
#include <thread>

namespace fs = std::filesystem;

// Función para obtener el PID de un proceso por su nombre
DWORD GetProcessIdByName(const std::wstring& processName) {
    DWORD pid = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32W processEntry;
        processEntry.dwSize = sizeof(processEntry);
        if (Process32FirstW(snapshot, &processEntry)) {
            do {
                if (processName == processEntry.szExeFile) {
                    pid = processEntry.th32ProcessID;
                    break;
                }
            } while (Process32NextW(snapshot, &processEntry));
        }
        CloseHandle(snapshot);
    }
    return pid;
}

// Inyección segura mediante LoadLibraryA (Modo Sigiloso)
bool InjectDLL(DWORD pid, const std::string& dllPath) {
    HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (!process) {
        std::cerr << "[!] Error: No se pudo abrir el proceso CS2. (Ejecuta como Administrador)" << std::endl;
        return false;
    }

    LPVOID remoteBuf = VirtualAllocEx(process, NULL, dllPath.size() + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!remoteBuf) {
        std::cerr << "[!] Error: No se pudo reservar memoria en el proceso CS2." << std::endl;
        CloseHandle(process);
        return false;
    }

    if (!WriteProcessMemory(process, remoteBuf, dllPath.c_str(), dllPath.size() + 1, NULL)) {
        std::cerr << "[!] Error: No se pudo escribir en la memoria del proceso CS2." << std::endl;
        VirtualFreeEx(process, remoteBuf, 0, MEM_RELEASE);
        CloseHandle(process);
        return false;
    }

    // Usamos un hilo remoto sigiloso
    HANDLE remoteThread = CreateRemoteThread(process, NULL, 0, (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA"), remoteBuf, 0, NULL);
    if (!remoteThread) {
        std::cerr << "[!] Error: No se pudo crear el hilo remoto para la inyeccion." << std::endl;
        VirtualFreeEx(process, remoteBuf, 0, MEM_RELEASE);
        CloseHandle(process);
        return false;
    }

    std::cout << "[+] Inyeccion completada con exito!" << std::endl;
    
    CloseHandle(remoteThread);
    VirtualFreeEx(process, remoteBuf, 0, MEM_RELEASE);
    CloseHandle(process);
    return true;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "   CS2MENU AUTO-LAUNCHER - MODO SEGURO   " << std::endl;
    std::cout << "========================================" << std::endl;

    std::wstring processName = L"cs2.exe";
    std::string dllName = "cs2menu.dll";
    
    // Obtener ruta completa de la DLL
    fs::path currentPath = fs::current_path();
    fs::path dllPath = currentPath / dllName;

    if (!fs::exists(dllPath)) {
        std::cerr << "[!] Error: No se encuentra " << dllName << " en: " << currentPath << std::endl;
        std::cerr << "[i] Asegurate de compilar el proyecto en modo Release primero." << std::endl;
        system("pause");
        return 1;
    }

    std::cout << "[*] Buscando proceso cs2.exe..." << std::endl;
    DWORD pid = 0;
    while (pid == 0) {
        pid = GetProcessIdByName(processName);
        if (pid == 0) {
            std::cout << "[.] Esperando a que CS2 se inicie... (Abre el juego con -insecure)" << std::endl;
            Sleep(2000);
        }
    }

    std::cout << "[+] CS2 detectado (PID: " << pid << ")" << std::endl;
    
    // RETRASO CRÍTICO: Esperar a que el motor Source 2 esté totalmente cargado
    std::cout << "[*] Esperando 10 segundos para estabilizar el motor grafico..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(10));

    std::cout << "[*] Intentando inyeccion: " << dllPath.string() << std::endl;

    if (InjectDLL(pid, dllPath.string())) {
        std::cout << "[OK] El menu deberia aparecer en el juego. Pulsa INSERT." << std::endl;
    } else {
        std::cerr << "[FAIL] La inyeccion ha fallado." << std::endl;
    }

    system("pause");
    return 0;
}
