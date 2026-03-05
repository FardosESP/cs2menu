#pragma once

#include <Windows.h>
#include <cstdint>
#include <vector>

namespace Memory
{
    // Pattern scanning
    uintptr_t FindPattern(const char* moduleName, const char* pattern, const char* mask);
    uintptr_t FindPattern(uintptr_t start, size_t size, const char* pattern, const char* mask);
    
    // Obtener base de módulo
    uintptr_t GetModuleBase(const char* moduleName);
    size_t GetModuleSize(const char* moduleName);
    
    // Leer/escribir memoria
    template<typename T>
    T Read(uintptr_t address)
    {
        if (!address) return T{};
        return *(T*)address;
    }
    
    template<typename T>
    void Write(uintptr_t address, T value)
    {
        if (!address) return;
        *(T*)address = value;
    }
    
    // Safe memory reading with exception handling
    template<typename T>
    bool SafeRead(uintptr_t address, T& out)
    {
        if (!address) return false;
        
        __try
        {
            out = *(T*)address;
            return true;
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
            return false;
        }
    }
    
    // Safe pointer validation
    inline bool IsValidPointer(uintptr_t address)
    {
        if (!address || address < 0x10000) return false;
        
        __try
        {
            volatile char test = *(char*)address;
            (void)test;
            return true;
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
            return false;
        }
    }
}
