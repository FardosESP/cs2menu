#pragma once

#include <Windows.h>
#include <cstdint>

namespace OffsetUpdater
{
    // Actualizar offsets dinámicamente
    bool UpdateOffsets();
    
    // Offsets dinámicos (se actualizan en runtime)
    extern uintptr_t dwEntityList;
    extern uintptr_t dwLocalPlayerPawn;
    extern uintptr_t dwViewMatrix;
    extern uintptr_t dwLocalPlayerController;
    extern uintptr_t dwViewAngles;
}
