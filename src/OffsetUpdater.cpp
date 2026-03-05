#include "OffsetUpdater.h"
#include "Memory.h"
#include <iostream>

namespace OffsetUpdater
{
    uintptr_t dwEntityList = 0;
    uintptr_t dwLocalPlayerPawn = 0;
    uintptr_t dwViewMatrix = 0;
    uintptr_t dwLocalPlayerController = 0;
    uintptr_t dwViewAngles = 0;

    bool UpdateOffsets()
    {
        std::cout << "[*] Buscando offsets dinámicamente..." << std::endl;
        
        uintptr_t clientBase = Memory::GetModuleBase("client.dll");
        if (!clientBase)
        {
            std::cout << "[-] No se pudo obtener client.dll" << std::endl;
            return false;
        }
        
        // Pattern scanning para EntityList
        // Patrón: 48 8B 0D ? ? ? ? 48 89 7C 24 ? 8B FA
        const char* entityListPattern = "\x48\x8B\x0D\x00\x00\x00\x00\x48\x89\x7C\x24\x00\x8B\xFA";
        const char* entityListMask = "xxx????xxxx?xx";
        
        uintptr_t entityListAddr = Memory::FindPattern("client.dll", entityListPattern, entityListMask);
        if (entityListAddr)
        {
            int offset = *(int*)(entityListAddr + 3);
            dwEntityList = (entityListAddr + 7 + offset) - clientBase;
            std::cout << "[+] EntityList offset: 0x" << std::hex << dwEntityList << std::dec << std::endl;
        }
        
        // Pattern scanning para LocalPlayerPawn
        // Patrón: 48 8B 05 ? ? ? ? 48 85 C0 74 ? 8B 88
        const char* localPlayerPattern = "\x48\x8B\x05\x00\x00\x00\x00\x48\x85\xC0\x74\x00\x8B\x88";
        const char* localPlayerMask = "xxx????xxxx?xx";
        
        uintptr_t localPlayerAddr = Memory::FindPattern("client.dll", localPlayerPattern, localPlayerMask);
        if (localPlayerAddr)
        {
            int offset = *(int*)(localPlayerAddr + 3);
            dwLocalPlayerPawn = (localPlayerAddr + 7 + offset) - clientBase;
            std::cout << "[+] LocalPlayerPawn offset: 0x" << std::hex << dwLocalPlayerPawn << std::dec << std::endl;
        }
        
        // Pattern scanning para ViewMatrix
        // Patrón: 48 8D 0D ? ? ? ? 48 C1 E0 06
        const char* viewMatrixPattern = "\x48\x8D\x0D\x00\x00\x00\x00\x48\xC1\xE0\x06";
        const char* viewMatrixMask = "xxx????xxxx";
        
        uintptr_t viewMatrixAddr = Memory::FindPattern("client.dll", viewMatrixPattern, viewMatrixMask);
        if (viewMatrixAddr)
        {
            int offset = *(int*)(viewMatrixAddr + 3);
            dwViewMatrix = (viewMatrixAddr + 7 + offset) - clientBase;
            std::cout << "[+] ViewMatrix offset: 0x" << std::hex << dwViewMatrix << std::dec << std::endl;
        }
        
        // Si no encontramos los offsets, usar los hardcoded como fallback
        if (!dwEntityList) dwEntityList = 0x18C39F8;
        if (!dwLocalPlayerPawn) dwLocalPlayerPawn = 0x17617D0;
        if (!dwViewMatrix) dwViewMatrix = 0x18A7750;
        
        std::cout << "[+] Offsets actualizados" << std::endl;
        return true;
    }
}
