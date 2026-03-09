#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <string>
#include <filesystem>
#include <chrono>
#include <thread>
#include <sddl.h>

namespace fs = std::filesystem;

bool IsRunAsAdmin()
{
    BOOL isAdmin = FALSE;
    PSID adminGroup = NULL;
    SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
    
    if (AllocateAndInitializeSid(&ntAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &adminGroup))
    {
        CheckTokenMembership(NULL, adminGroup, &isAdmin);
        FreeSid(adminGroup);
    }
    
    return isAdmin;
}

bool ElevateProcess()
{
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(NULL, exePath, MAX_PATH);
    
    SHELLEXECUTEINFOW sei = { sizeof(sei) };
    sei.lpVerb = L"runas";
    sei.lpFile = exePath;
    sei.hwnd = NULL;
    sei.nShow = SW_NORMAL;
    
    if (!ShellExecuteExW(&sei))
    {
        DWORD error = GetLastError();
        if (error == ERROR_CANCELLED)
        {
            std::cout << "[-] Usuario cancelo la elevacion de privilegios." << std::endl;
        }
        return false;
    }
    
    return true;
}

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

bool IsModuleLoaded(DWORD pid, const std::wstring& moduleName)
{
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
    if (snapshot == INVALID_HANDLE_VALUE)
        return false;
    
    MODULEENTRY32W me = { sizeof(me) };
    bool found = false;
    
    if (Module32FirstW(snapshot, &me))
    {
        do {
            if (_wcsicmp(me.szModule, moduleName.c_str()) == 0)
            {
                found = true;
                break;
            }
        } while (Module32NextW(snapshot, &me));
    }
    
    CloseHandle(snapshot);
    return found;
}

bool IsGameFullyLoaded(DWORD pid)
{
    // Check if critical game modules are loaded
    return IsModuleLoaded(pid, L"client.dll") && 
           IsModuleLoaded(pid, L"engine2.dll");
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

    if (!WriteProcessMemory(process, remoteBuf, dllPath.c_str(), pathSize, NULL))
    {
        std::cout << "[-] WriteProcessMemory fallo. Error: " << GetLastError() << std::endl;
        VirtualFreeEx(process, remoteBuf, 0, MEM_RELEASE);
        CloseHandle(process);
        return false;
    }

    LPTHREAD_START_ROUTINE pLoadLibraryW = (LPTHREAD_START_ROUTINE)GetProcAddress(
        GetModuleHandleW(L"kernel32.dll"), "LoadLibraryW"
    );
    
    if (!pLoadLibraryW)
    {
        std::cout << "[-] No se pudo obtener direccion de LoadLibraryW" << std::endl;
        VirtualFreeEx(process, remoteBuf, 0, MEM_RELEASE);
        CloseHandle(process);
        return false;
    }
    
    std::cout << "[i] LoadLibraryW address: 0x" << std::hex << (uintptr_t)pLoadLibraryW << std::dec << std::endl;
    std::cout << "[i] Remote buffer: 0x" << std::hex << (uintptr_t)remoteBuf << std::dec << std::endl;

    HANDLE remoteThread = CreateRemoteThread(process, NULL, 0, pLoadLibraryW, remoteBuf, 0, NULL);

    if (!remoteThread)
    {
        DWORD error = GetLastError();
        std::cout << "[-] CreateRemoteThread fallo. Error: " << error << std::endl;
        
        // Common error codes
        if (error == ERROR_ACCESS_DENIED)
            std::cout << "[!] ERROR_ACCESS_DENIED - El proceso esta protegido o necesitas mas privilegios" << std::endl;
        else if (error == ERROR_NOT_ENOUGH_MEMORY)
            std::cout << "[!] ERROR_NOT_ENOUGH_MEMORY - No hay suficiente memoria" << std::endl;
        
        VirtualFreeEx(process, remoteBuf, 0, MEM_RELEASE);
        CloseHandle(process);
        return false;
    }

    // FIX: Wait with timeout and check result
    DWORD waitResult = WaitForSingleObject(remoteThread, 30000);  // 30 second timeout
    
    if (waitResult == WAIT_TIMEOUT)
    {
        std::cout << "[-] Timeout esperando a que LoadLibraryW termine." << std::endl;
        TerminateThread(remoteThread, 1);
        CloseHandle(remoteThread);
        VirtualFreeEx(process, remoteBuf, 0, MEM_RELEASE);
        CloseHandle(process);
        return false;
    }
    
    // Check if LoadLibraryW succeeded
    DWORD exitCode = 0;
    GetExitCodeThread(remoteThread, &exitCode);
    
    std::cout << "[i] Thread exit code: 0x" << std::hex << exitCode << std::dec << std::endl;
    
    CloseHandle(remoteThread);
    VirtualFreeEx(process, remoteBuf, 0, MEM_RELEASE);
    CloseHandle(process);
    
    if (exitCode == 0)
    {
        std::cout << "[-] LoadLibraryW retorno NULL - DLL no se cargo." << std::endl;
        std::cout << "[!] Posibles causas:" << std::endl;
        std::cout << "    1. Dependencias faltantes (d3d11.dll, vcruntime, etc)" << std::endl;
        std::cout << "    2. Windows Defender bloqueando la DLL" << std::endl;
        std::cout << "    3. VAC/Anti-cheat bloqueando la inyeccion" << std::endl;
        std::cout << "    4. Ruta de DLL invalida o inaccesible" << std::endl;
        return false;
    }
    
    // Verify DLL actually loaded
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    if (!IsModuleLoaded(pid, L"cs2menu.dll"))
    {
        std::cout << "[-] DLL no aparece en la lista de modulos del proceso." << std::endl;
        return false;
    }
    
    return true;
}

int main()
{
    std::cout << "========================================" << std::endl;
    std::cout << "   CS2MENU - LAUNCHER                   " << std::endl;
    std::cout << "========================================" << std::endl;
    
    // Check for admin privileges
    if (!IsRunAsAdmin())
    {
        std::cout << "[!] No se esta ejecutando como Administrador." << std::endl;
        std::cout << "[!] La inyeccion puede fallar sin privilegios elevados." << std::endl;
        std::cout << "[?] Deseas reiniciar como Administrador? (S/N): ";
        
        char response;
        std::cin >> response;
        
        if (response == 'S' || response == 's')
        {
            std::cout << "[*] Reiniciando con privilegios elevados..." << std::endl;
            if (ElevateProcess())
            {
                return 0;  // Exit this instance, elevated one will start
            }
            else
            {
                std::cout << "[-] Fallo al elevar privilegios. Continuando sin admin..." << std::endl;
            }
        }
        else
        {
            std::cout << "[*] Continuando sin privilegios de administrador..." << std::endl;
        }
    }
    else
    {
        std::cout << "[+] Ejecutando con privilegios de Administrador." << std::endl;
    }

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
    std::cout << "[*] Esperando a que el juego cargue completamente..." << std::endl;

    // FIX: Dynamic wait for game modules instead of hardcoded 15 seconds
    int attempts = 0;
    const int maxAttempts = 60;  // 60 seconds max
    
    while (!IsGameFullyLoaded(pid) && attempts < maxAttempts)
    {
        std::cout << "[i] Esperando modulos del juego... (" << (attempts + 1) << "s)  \r" << std::flush;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        attempts++;
    }
    std::cout << std::endl;
    
    if (attempts >= maxAttempts)
    {
        std::cout << "[-] Timeout: El juego no cargo completamente en 60 segundos." << std::endl;
        std::cout << "[-] Intenta inyectar manualmente cuando estes en partida." << std::endl;
        Sleep(5000);
        return 1;
    }
    
    std::cout << "[+] Juego completamente cargado (client.dll + engine2.dll detectados)." << std::endl;
    std::cout << "[*] Inyectando DLL..." << std::endl;

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