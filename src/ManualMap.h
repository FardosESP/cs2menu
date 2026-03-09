#pragma once
#include <Windows.h>

namespace ManualMap
{
    // Professional manual mapping injection
    // Bypasses standard anti-cheat detection methods
    bool InjectDll(HANDLE hProcess, const char* dllPath);
    
    // Advanced injection with custom entry point
    bool InjectDllEx(HANDLE hProcess, const char* dllPath, const char* entryPoint);
    
    // Check if process is protected (anti-cheat, anti-debug)
    bool IsProcessProtected(HANDLE hProcess);
    
    // Erase PE headers after injection (anti-dump)
    bool ErasePEHeaders(HANDLE hProcess, void* baseAddress);
}
