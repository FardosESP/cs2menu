#include "Memory.h"
#include <Psapi.h>
#include <iostream>

uintptr_t Memory::GetModuleBase(const char* moduleName)
{
    return (uintptr_t)GetModuleHandleA(moduleName);
}

size_t Memory::GetModuleSize(const char* moduleName)
{
    HMODULE hModule = GetModuleHandleA(moduleName);
    if (!hModule) return 0;
    
    MODULEINFO modInfo;
    if (GetModuleInformation(GetCurrentProcess(), hModule, &modInfo, sizeof(MODULEINFO)))
        return modInfo.SizeOfImage;
    
    return 0;
}

uintptr_t Memory::FindPattern(const char* moduleName, const char* pattern, const char* mask)
{
    uintptr_t base = GetModuleBase(moduleName);
    size_t size = GetModuleSize(moduleName);
    
    if (!base || !size) return 0;
    
    return FindPattern(base, size, pattern, mask);
}

uintptr_t Memory::FindPattern(uintptr_t start, size_t size, const char* pattern, const char* mask)
{
    size_t patternLen = strlen(mask);
    
    for (size_t i = 0; i < size - patternLen; i++)
    {
        bool found = true;
        for (size_t j = 0; j < patternLen; j++)
        {
            if (mask[j] != '?' && pattern[j] != *(char*)(start + i + j))
            {
                found = false;
                break;
            }
        }
        
        if (found)
            return start + i;
    }
    
    return 0;
}
