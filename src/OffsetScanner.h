#pragma once

#include <Windows.h>
#include <cstdint>
#include <vector>
#include <string>

/**
 * OffsetScanner - Memory diagnostic tool for finding correct CS2 offsets
 * 
 * Purpose: Scan memory around known structures to find correct offsets when
 * the current offsets from cs2-dumper are incorrect or outdated.
 * 
 * Features:
 * - Scan LocalPlayer structure for m_pGameSceneNode, m_vecAbsOrigin, etc.
 * - Scan EntityList to find valid player entities
 * - Validate pointers and detect patterns
 * - Print detailed diagnostic output
 */
class OffsetScanner
{
public:
    // Main scanning function - call this from Features::Initialize()
    static void ScanAndPrintOffsets();
    
    // Scan LocalPlayer structure for critical offsets
    static void ScanLocalPlayer(uintptr_t localPlayerAddr);
    
    // Scan EntityList for valid player entities
    static void ScanEntityList(uintptr_t entityListAddr);
    
    // Scan for m_pGameSceneNode offset (most critical for ESP)
    static void ScanForGameSceneNode(uintptr_t localPlayerAddr);
    
    // Scan for m_vecAbsOrigin within a GameSceneNode
    static void ScanForAbsOrigin(uintptr_t sceneNodeAddr);
    
    // Scan for m_vecViewOffset (should be around 64.0f for Z)
    static void ScanForViewOffset(uintptr_t localPlayerAddr);
    
    // Scan for m_fFlags (should have FL_ONGROUND bit when on ground)
    static void ScanForFlags(uintptr_t localPlayerAddr);
    
private:
    // Helper: Check if a value looks like a valid pointer
    static bool LooksLikePointer(uintptr_t value);
    
    // Helper: Check if a value looks like world coordinates
    static bool LooksLikeWorldCoords(float x, float y, float z);
    
    // Helper: Check if a value looks like a view offset
    static bool LooksLikeViewOffset(float x, float y, float z);
    
    // Helper: Safe memory read with exception handling
    template<typename T>
    static bool SafeRead(uintptr_t address, T& out);
    
    // Helper: Print a memory region in hex dump format
    static void PrintMemoryRegion(uintptr_t address, size_t size, const char* label);
    
    // Helper: Validate a pointer by checking if it points to readable memory
    static bool IsValidPointer(uintptr_t ptr);
    
    // Helper: Check if memory region is readable
    static bool IsMemoryReadable(uintptr_t address, size_t size);
};
