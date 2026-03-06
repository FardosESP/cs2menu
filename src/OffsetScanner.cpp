#include "OffsetScanner.h"
#include "Memory.h"
#include "SDK.h"
#include <iostream>
#include <iomanip>
#include <cmath>

// Safe memory read with exception handling
template<typename T>
bool OffsetScanner::SafeRead(uintptr_t address, T& out)
{
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

// Check if a value looks like a valid pointer (in user space, aligned)
bool OffsetScanner::LooksLikePointer(uintptr_t value)
{
    // Pointers should be:
    // 1. Non-zero
    // 2. In user space (< 0x7FFFFFFFFFFF on x64)
    // 3. Aligned to at least 8 bytes
    // 4. Not too small (> 0x10000 to avoid low addresses)
    
    if (value == 0) return false;
    if (value < 0x10000) return false;
    if (value > 0x7FFFFFFFFFFF) return false;
    if ((value & 0x7) != 0) return false; // Check 8-byte alignment
    
    return true;
}

// Check if values look like world coordinates (typically large floats, -10000 to +10000 range)
bool OffsetScanner::LooksLikeWorldCoords(float x, float y, float z)
{
    // World coordinates in CS2 are typically:
    // - In range -10000 to +10000
    // - Not NaN or Inf
    // - Not all zeros (unless actually at origin)
    // - Not extremely small values (scientific notation like 1e-21)
    
    if (std::isnan(x) || std::isnan(y) || std::isnan(z)) return false;
    if (std::isinf(x) || std::isinf(y) || std::isinf(z)) return false;
    
    if (std::abs(x) > 10000.0f) return false;
    if (std::abs(y) > 10000.0f) return false;
    if (std::abs(z) > 10000.0f) return false;
    
    // Filter out extremely small values (scientific notation)
    if (std::abs(x) < 0.01f && std::abs(y) < 0.01f && std::abs(z) < 0.01f) return false;
    
    // At least one coordinate should be reasonably large (> 10.0)
    if (std::abs(x) < 10.0f && std::abs(y) < 10.0f && std::abs(z) < 10.0f) return false;
    
    return true;
}

// Check if values look like a view offset (Z should be around 64.0f for standing player)
bool OffsetScanner::LooksLikeViewOffset(float x, float y, float z)
{
    // View offset in CS2:
    // - X and Y are typically 0 or very small
    // - Z is around 64.0f when standing, 46.0f when crouching
    
    if (std::isnan(x) || std::isnan(y) || std::isnan(z)) return false;
    if (std::isinf(x) || std::isinf(y) || std::isinf(z)) return false;
    
    if (std::abs(x) > 10.0f) return false;
    if (std::abs(y) > 10.0f) return false;
    
    // Z should be in reasonable range for player height
    if (z < 30.0f || z > 80.0f) return false;
    
    return true;
}

// Validate a pointer by checking if it points to readable memory
bool OffsetScanner::IsValidPointer(uintptr_t ptr)
{
    if (!LooksLikePointer(ptr)) return false;
    
    __try
    {
        // Try to read a byte from the address
        volatile uint8_t test = *(uint8_t*)ptr;
        (void)test; // Suppress unused variable warning
        return true;
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        return false;
    }
}

// Check if memory region is readable
bool OffsetScanner::IsMemoryReadable(uintptr_t address, size_t size)
{
    __try
    {
        for (size_t i = 0; i < size; i += 4096) // Check every page
        {
            volatile uint8_t test = *(uint8_t*)(address + i);
            (void)test;
        }
        return true;
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        return false;
    }
}

// Print a memory region in hex dump format
void OffsetScanner::PrintMemoryRegion(uintptr_t address, size_t size, const char* label)
{
    std::cout << "\n[HEXDUMP] " << label << " at 0x" << std::hex << address << std::dec << std::endl;
    std::cout << "Offset   00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F  ASCII" << std::endl;
    std::cout << "------   -----------------------------------------------  -----" << std::endl;
    
    for (size_t i = 0; i < size; i += 16)
    {
        std::cout << "0x" << std::hex << std::setw(4) << std::setfill('0') << i << "   ";
        
        // Print hex bytes
        char ascii[17] = {0};
        for (size_t j = 0; j < 16 && (i + j) < size; j++)
        {
            uint8_t byte = 0;
            if (SafeRead(address + i + j, byte))
            {
                std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)byte << " ";
                ascii[j] = (byte >= 32 && byte <= 126) ? byte : '.';
            }
            else
            {
                std::cout << "?? ";
                ascii[j] = '?';
            }
        }
        
        // Padding for incomplete lines
        for (size_t j = (i + 16 > size) ? (size - i) : 16; j < 16; j++)
        {
            std::cout << "   ";
        }
        
        std::cout << " " << ascii << std::endl;
    }
    
    std::cout << std::dec << std::endl;
}

// Scan for m_pGameSceneNode offset (most critical for ESP)
void OffsetScanner::ScanForGameSceneNode(uintptr_t localPlayerAddr)
{
    std::cout << "\n╔═══════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║         SCANNING FOR m_pGameSceneNode OFFSET             ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << "[SCAN] LocalPlayer address: 0x" << std::hex << localPlayerAddr << std::dec << std::endl;
    std::cout << "[SCAN] Current m_pGameSceneNode offset: 0x" << std::hex << Offsets::m_pGameSceneNode() << std::dec << std::endl;
    std::cout << "[INFO] Looking for valid pointers that point to structures containing world coordinates..." << std::endl;
    
    int candidatesFound = 0;
    
    // Scan range: 0x0 to 0x1000 (4KB) in 8-byte steps (pointer alignment)
    for (uintptr_t offset = 0x0; offset <= 0x1000; offset += 8)
    {
        uintptr_t ptrValue = 0;
        if (!SafeRead(localPlayerAddr + offset, ptrValue))
            continue;
        
        // Check if it looks like a pointer
        if (!LooksLikePointer(ptrValue))
            continue;
        
        // Check if the pointer is valid (readable)
        if (!IsValidPointer(ptrValue))
            continue;
        
        // Now check if this pointer points to a structure that contains world coordinates
        // Try reading at various offsets within the pointed structure
        bool foundCoords = false;
        uintptr_t coordsOffset = 0;
        
        for (uintptr_t testOffset = 0x0; testOffset <= 0x200; testOffset += 4)
        {
            float x = 0, y = 0, z = 0;
            if (SafeRead(ptrValue + testOffset, x) &&
                SafeRead(ptrValue + testOffset + 4, y) &&
                SafeRead(ptrValue + testOffset + 8, z))
            {
                if (LooksLikeWorldCoords(x, y, z))
                {
                    foundCoords = true;
                    coordsOffset = testOffset;
                    
                    std::cout << "\n[CANDIDATE] m_pGameSceneNode at offset 0x" << std::hex << offset << std::dec << std::endl;
                    std::cout << "  Pointer value: 0x" << std::hex << ptrValue << std::dec << std::endl;
                    std::cout << "  Points to structure containing world coords at +0x" << std::hex << coordsOffset << std::dec << std::endl;
                    std::cout << "  Coordinates: (" << x << ", " << y << ", " << z << ")" << std::endl;
                    std::cout << "  Reason: Valid pointer pointing to structure with world coordinates" << std::endl;
                    
                    candidatesFound++;
                    
                    // Only show first 5 candidates to avoid spam
                    if (candidatesFound >= 5)
                    {
                        std::cout << "\n[INFO] Showing first 5 candidates only. More may exist." << std::endl;
                        return;
                    }
                    
                    break; // Found coords in this structure, move to next offset
                }
            }
        }
    }
    
    if (candidatesFound == 0)
    {
        std::cout << "\n[ERROR] No candidates found for m_pGameSceneNode!" << std::endl;
        std::cout << "[INFO] Possible reasons:" << std::endl;
        std::cout << "  1. Player is not spawned in the world yet" << std::endl;
        std::cout << "  2. LocalPlayer address is incorrect" << std::endl;
        std::cout << "  3. Game structure has changed significantly" << std::endl;
        
        // Print memory dump for manual analysis
        PrintMemoryRegion(localPlayerAddr, 256, "First 256 bytes of LocalPlayer");
    }
    else
    {
        std::cout << "\n[SUCCESS] Found " << candidatesFound << " candidate(s) for m_pGameSceneNode" << std::endl;
        std::cout << "[TIP] Test each candidate by updating the offset in SDK.h" << std::endl;
    }
}

// Scan for m_vecAbsOrigin within a GameSceneNode
void OffsetScanner::ScanForAbsOrigin(uintptr_t sceneNodeAddr)
{
    std::cout << "\n╔═══════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║          SCANNING FOR m_vecAbsOrigin OFFSET              ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << "[SCAN] GameSceneNode address: 0x" << std::hex << sceneNodeAddr << std::dec << std::endl;
    std::cout << "[SCAN] Current m_vecAbsOrigin offset: 0x" << std::hex << Offsets::m_vecAbsOrigin() << std::dec << std::endl;
    
    int candidatesFound = 0;
    
    // Scan range: 0x0 to 0x500 in 4-byte steps (float alignment)
    for (uintptr_t offset = 0x0; offset <= 0x500; offset += 4)
    {
        float x = 0, y = 0, z = 0;
        if (!SafeRead(sceneNodeAddr + offset, x) ||
            !SafeRead(sceneNodeAddr + offset + 4, y) ||
            !SafeRead(sceneNodeAddr + offset + 8, z))
            continue;
        
        if (LooksLikeWorldCoords(x, y, z))
        {
            std::cout << "\n[CANDIDATE] m_vecAbsOrigin at offset 0x" << std::hex << offset << std::dec << std::endl;
            std::cout << "  Coordinates: (" << x << ", " << y << ", " << z << ")" << std::endl;
            std::cout << "  Reason: Values look like world coordinates" << std::endl;
            
            candidatesFound++;
            
            if (candidatesFound >= 5)
            {
                std::cout << "\n[INFO] Showing first 5 candidates only." << std::endl;
                return;
            }
        }
    }
    
    if (candidatesFound == 0)
    {
        std::cout << "\n[ERROR] No candidates found for m_vecAbsOrigin!" << std::endl;
        PrintMemoryRegion(sceneNodeAddr, 256, "First 256 bytes of GameSceneNode");
    }
}

// Scan for m_vecViewOffset (should be around 64.0f for Z)
void OffsetScanner::ScanForViewOffset(uintptr_t localPlayerAddr)
{
    std::cout << "\n╔═══════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║         SCANNING FOR m_vecViewOffset OFFSET              ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << "[SCAN] LocalPlayer address: 0x" << std::hex << localPlayerAddr << std::dec << std::endl;
    std::cout << "[SCAN] Current m_vecViewOffset offset: 0x" << std::hex << Offsets::m_vecViewOffset() << std::dec << std::endl;
    
    int candidatesFound = 0;
    
    // Scan range: 0x0 to 0x2000 in 4-byte steps
    for (uintptr_t offset = 0x0; offset <= 0x2000; offset += 4)
    {
        float x = 0, y = 0, z = 0;
        if (!SafeRead(localPlayerAddr + offset, x) ||
            !SafeRead(localPlayerAddr + offset + 4, y) ||
            !SafeRead(localPlayerAddr + offset + 8, z))
            continue;
        
        if (LooksLikeViewOffset(x, y, z))
        {
            std::cout << "\n[CANDIDATE] m_vecViewOffset at offset 0x" << std::hex << offset << std::dec << std::endl;
            std::cout << "  View offset: (" << x << ", " << y << ", " << z << ")" << std::endl;
            std::cout << "  Reason: Z value (~" << z << ") is in expected range for player eye height" << std::endl;
            
            candidatesFound++;
            
            if (candidatesFound >= 5)
            {
                std::cout << "\n[INFO] Showing first 5 candidates only." << std::endl;
                return;
            }
        }
    }
    
    if (candidatesFound == 0)
    {
        std::cout << "\n[ERROR] No candidates found for m_vecViewOffset!" << std::endl;
    }
}

// Scan for m_fFlags (should have FL_ONGROUND bit when on ground)
void OffsetScanner::ScanForFlags(uintptr_t localPlayerAddr)
{
    std::cout << "\n╔═══════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║            SCANNING FOR m_fFlags OFFSET                  ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << "[SCAN] LocalPlayer address: 0x" << std::hex << localPlayerAddr << std::dec << std::endl;
    std::cout << "[SCAN] Current m_fFlags offset: 0x" << std::hex << Offsets::m_fFlags() << std::dec << std::endl;
    std::cout << "[INFO] Looking for integer values with FL_ONGROUND bit (0x1) set..." << std::endl;
    
    int candidatesFound = 0;
    
    // Scan range: 0x0 to 0x2000 in 4-byte steps
    for (uintptr_t offset = 0x0; offset <= 0x2000; offset += 4)
    {
        int flags = 0;
        if (!SafeRead(localPlayerAddr + offset, flags))
            continue;
        
        // Flags should be:
        // 1. Non-zero
        // 2. Not too large (< 0x10000)
        // 3. Typically has FL_ONGROUND (0x1) set when on ground
        
        if (flags == 0) continue;
        if (flags < 0 || flags > 0x10000) continue;
        
        // Check if FL_ONGROUND bit is set
        bool onGround = (flags & 0x1) != 0;
        
        std::cout << "\n[CANDIDATE] m_fFlags at offset 0x" << std::hex << offset << std::dec << std::endl;
        std::cout << "  Flags value: 0x" << std::hex << flags << std::dec << " (" << flags << ")" << std::endl;
        std::cout << "  FL_ONGROUND: " << (onGround ? "YES" : "NO") << std::endl;
        std::cout << "  Reason: Value in expected range for flags" << std::endl;
        
        candidatesFound++;
        
        if (candidatesFound >= 10)
        {
            std::cout << "\n[INFO] Showing first 10 candidates only." << std::endl;
            return;
        }
    }
    
    if (candidatesFound == 0)
    {
        std::cout << "\n[ERROR] No candidates found for m_fFlags!" << std::endl;
    }
}

// Scan LocalPlayer structure for critical offsets
void OffsetScanner::ScanLocalPlayer(uintptr_t localPlayerAddr)
{
    std::cout << "\n╔═══════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║           SCANNING LOCALPLAYER STRUCTURE                 ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << "[SCAN] LocalPlayer address: 0x" << std::hex << localPlayerAddr << std::dec << std::endl;
    
    // First, verify basic offsets (health, team)
    std::cout << "\n[VERIFY] Checking known offsets..." << std::endl;
    
    int health = 0, team = 0;
    if (SafeRead(localPlayerAddr + Offsets::m_iHealth(), health))
    {
        std::cout << "[OK] m_iHealth (0x" << std::hex << Offsets::m_iHealth() << std::dec << ") = " << health;
        if (health > 0 && health <= 100)
            std::cout << " ✓ VALID" << std::endl;
        else
            std::cout << " ✗ INVALID (expected 1-100)" << std::endl;
    }
    else
    {
        std::cout << "[ERROR] Cannot read m_iHealth" << std::endl;
    }
    
    if (SafeRead(localPlayerAddr + Offsets::m_iTeamNum(), team))
    {
        std::cout << "[OK] m_iTeamNum (0x" << std::hex << Offsets::m_iTeamNum() << std::dec << ") = " << team;
        if (team == 2 || team == 3)
            std::cout << " ✓ VALID" << std::endl;
        else
            std::cout << " ✗ INVALID (expected 2 or 3)" << std::endl;
    }
    else
    {
        std::cout << "[ERROR] Cannot read m_iTeamNum" << std::endl;
    }
    
    // Now scan for m_pGameSceneNode (most critical)
    ScanForGameSceneNode(localPlayerAddr);
    
    // Check if current m_pGameSceneNode works
    uintptr_t sceneNodePtr = 0;
    if (SafeRead(localPlayerAddr + Offsets::m_pGameSceneNode(), sceneNodePtr))
    {
        std::cout << "\n[CHECK] Current m_pGameSceneNode offset points to: 0x" << std::hex << sceneNodePtr << std::dec << std::endl;
        
        if (IsValidPointer(sceneNodePtr))
        {
            std::cout << "[OK] Pointer is valid, scanning for m_vecAbsOrigin..." << std::endl;
            ScanForAbsOrigin(sceneNodePtr);
        }
        else
        {
            std::cout << "[ERROR] Pointer is INVALID (not readable or not aligned)" << std::endl;
        }
    }
    else
    {
        std::cout << "\n[ERROR] Cannot read m_pGameSceneNode at current offset" << std::endl;
    }
    
    // Scan for other offsets
    ScanForViewOffset(localPlayerAddr);
    ScanForFlags(localPlayerAddr);
}

// Scan EntityList to find valid player entities
void OffsetScanner::ScanEntityList(uintptr_t entityListAddr)
{
    std::cout << "\n╔═══════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║            SCANNING ENTITY LIST FOR PLAYERS              ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << "[SCAN] EntityList address: 0x" << std::hex << entityListAddr << std::dec << std::endl;
    std::cout << "[INFO] Scanning entity indices 1-64 for valid players..." << std::endl;
    
    C_CSGameEntitySystem* entitySystem = (C_CSGameEntitySystem*)entityListAddr;
    int validEntities = 0;
    int nullEntities = 0;
    int invalidHealth = 0;
    int invalidTeam = 0;
    
    // First, let's try MULTIPLE methods to find entities
    std::cout << "\n[DEBUG] Testing different EntityList iteration methods..." << std::endl;
    
    struct TestResult {
        std::string method;
        int validEntities;
        uintptr_t firstEntityAddr;
    };
    
    std::vector<TestResult> results;
    
    // Method 1: NEW CHUNK-BASED METHOD (Source 2 correct way)
    {
        int validCount = 0;
        uintptr_t firstAddr = 0;
        
        // Read entity list array pointer at +0x10
        uintptr_t entityListArray = 0;
        if (SafeRead(entityListAddr + 0x10, entityListArray) && entityListArray > 0x1000)
        {
            for (int i = 1; i <= 64 && validCount < 5; i++)
            {
                // Calculate chunk index and offset
                int chunkIndex = i >> 9;        // i / 512
                int indexInChunk = i & 0x1FF;   // i % 512
                
                // Read chunk pointer from array (+0x10 offset in array)
                uintptr_t chunkPtr = 0;
                if (!SafeRead(entityListArray + 0x10 + (chunkIndex * 8), chunkPtr) || chunkPtr < 0x1000)
                    continue;
                
                // Read entity from chunk (stride 0x78)
                uintptr_t entityPtr = 0;
                if (!SafeRead(chunkPtr + (indexInChunk * 0x78), entityPtr) || entityPtr < 0x1000)
                    continue;
                
                // Validate entity
                int health = 0, team = 0;
                if (SafeRead(entityPtr + Offsets::m_iHealth(), health) &&
                    SafeRead(entityPtr + Offsets::m_iTeamNum(), team) &&
                    health > 0 && health <= 100 && (team == 2 || team == 3))
                {
                    if (validCount == 0) firstAddr = entityPtr;
                    validCount++;
                }
            }
        }
        results.push_back({"NEW: Chunk-based (array+0x10, stride 0x78)", validCount, firstAddr});
    }
    
    // Method 2: Try with different array start offsets
    for (int arrayOffset : {0x00, 0x08, 0x18, 0x20})
    {
        int validCount = 0;
        uintptr_t firstAddr = 0;
        
        uintptr_t entityListArray = 0;
        if (!SafeRead(entityListAddr + 0x10, entityListArray) || entityListArray < 0x1000)
        {
            char hexOffset[16];
            sprintf_s(hexOffset, "0x%x", arrayOffset);
            results.push_back({"Chunk array start +" + std::string(hexOffset), 0, 0});
            continue;
        }
        
        for (int i = 1; i <= 64 && validCount < 5; i++)
        {
            int chunkIndex = i >> 9;
            int indexInChunk = i & 0x1FF;
            
            uintptr_t chunkPtr = 0;
            if (!SafeRead(entityListArray + arrayOffset + (chunkIndex * 8), chunkPtr) || chunkPtr < 0x1000)
                continue;
            
            uintptr_t entityPtr = 0;
            if (!SafeRead(chunkPtr + (indexInChunk * 0x78), entityPtr) || entityPtr < 0x1000)
                continue;
            
            int health = 0, team = 0;
            if (SafeRead(entityPtr + Offsets::m_iHealth(), health) &&
                SafeRead(entityPtr + Offsets::m_iTeamNum(), team) &&
                health > 0 && health <= 100 && (team == 2 || team == 3))
            {
                if (validCount == 0) firstAddr = entityPtr;
                validCount++;
            }
        }
        
        char hexOffset[16];
        sprintf_s(hexOffset, "0x%x", arrayOffset);
        results.push_back({"Chunk array start +" + std::string(hexOffset), validCount, firstAddr});
    }
    
    // Method 3: Try different strides
    for (int stride : {0x10, 0x20, 0x40, 0x80, 0x100, 0x120})
    {
        int validCount = 0;
        uintptr_t firstAddr = 0;
        
        uintptr_t entityListArray = 0;
        if (!SafeRead(entityListAddr + 0x10, entityListArray) || entityListArray < 0x1000)
        {
            char hexStride[16];
            sprintf_s(hexStride, "0x%x", stride);
            results.push_back({"Chunk stride " + std::string(hexStride), 0, 0});
            continue;
        }
        
        for (int i = 1; i <= 64 && validCount < 5; i++)
        {
            int chunkIndex = i >> 9;
            int indexInChunk = i & 0x1FF;
            
            uintptr_t chunkPtr = 0;
            if (!SafeRead(entityListArray + 0x10 + (chunkIndex * 8), chunkPtr) || chunkPtr < 0x1000)
                continue;
            
            uintptr_t entityPtr = 0;
            if (!SafeRead(chunkPtr + (indexInChunk * stride), entityPtr) || entityPtr < 0x1000)
                continue;
            
            int health = 0, team = 0;
            if (SafeRead(entityPtr + Offsets::m_iHealth(), health) &&
                SafeRead(entityPtr + Offsets::m_iTeamNum(), team) &&
                health > 0 && health <= 100 && (team == 2 || team == 3))
            {
                if (validCount == 0) firstAddr = entityPtr;
                validCount++;
            }
        }
        
        char hexStride[16];
        sprintf_s(hexStride, "0x%x", stride);
        results.push_back({"Chunk stride " + std::string(hexStride), validCount, firstAddr});
    }
    
    // Print results
    std::cout << "\n[RESULTS] Testing different methods:" << std::endl;
    int bestMethod = -1;
    int maxEntities = 0;
    for (size_t i = 0; i < results.size(); i++)
    {
        std::cout << "[TEST " << (i+1) << "] " << results[i].method << ": ";
        if (results[i].validEntities > 0)
        {
            std::cout << results[i].validEntities << " entities found ✓";
            if (results[i].firstEntityAddr > 0)
            {
                std::cout << " (first: 0x" << std::hex << results[i].firstEntityAddr << std::dec << ")";
            }
            std::cout << std::endl;
            
            if (results[i].validEntities > maxEntities)
            {
                maxEntities = results[i].validEntities;
                bestMethod = i;
            }
        }
        else
        {
            std::cout << "0 entities ✗" << std::endl;
        }
    }
    
    if (bestMethod >= 0)
    {
        std::cout << "\n[SUCCESS] Best method: " << results[bestMethod].method << std::endl;
        std::cout << "[SUCCESS] Found " << results[bestMethod].validEntities << " valid entities!" << std::endl;
        std::cout << "\n[ACTION] Update GetBaseEntity() in SDK.h to use this method" << std::endl;
        return;
    }
    else
    {
        std::cout << "\n[ERROR] No valid method found!" << std::endl;
        std::cout << "[INFO] Possible reasons:" << std::endl;
        std::cout << "  1. You're the only player in the server" << std::endl;
        std::cout << "  2. dwEntityList offset is completely wrong" << std::endl;
        std::cout << "  3. Entity structure has changed dramatically" << std::endl;
        return;
    }
}

// Main scanning function
void OffsetScanner::ScanAndPrintOffsets()
{
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                                                           ║" << std::endl;
    std::cout << "║           CS2 OFFSET SCANNER - DIAGNOSTIC TOOL            ║" << std::endl;
    std::cout << "║                                                           ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << "\n[INFO] This tool will scan memory to find correct offsets" << std::endl;
    std::cout << "[INFO] Make sure you are IN AN ACTIVE GAME (not menu/lobby)" << std::endl;
    std::cout << "[INFO] Results will show candidate offsets - test each one" << std::endl;
    std::cout << "\n";
    
    // Get client.dll base
    uintptr_t clientBase = Memory::GetModuleBase("client.dll");
    if (!clientBase)
    {
        std::cout << "[ERROR] Cannot find client.dll!" << std::endl;
        return;
    }
    
    std::cout << "[OK] client.dll base: 0x" << std::hex << clientBase << std::dec << std::endl;
    
    // Get LocalPlayer address
    uintptr_t localPawnAddr = clientBase + Offsets::dwLocalPlayerPawn();
    uintptr_t localPawn = 0;
    
    if (!SafeRead(localPawnAddr, localPawn))
    {
        std::cout << "[ERROR] Cannot read LocalPlayerPawn address!" << std::endl;
        return;
    }
    
    if (localPawn == 0)
    {
        std::cout << "[ERROR] LocalPlayerPawn is NULL - you must be in an active game!" << std::endl;
        std::cout << "[TIP] Join a match, spawn as a player, then run this scan" << std::endl;
        return;
    }
    
    std::cout << "[OK] LocalPlayerPawn: 0x" << std::hex << localPawn << std::dec << std::endl;
    
    // Scan LocalPlayer structure
    ScanLocalPlayer(localPawn);
    
    // Get EntityList address
    uintptr_t entityListAddr = clientBase + Offsets::dwEntityList();
    std::cout << "\n[OK] EntityList address: 0x" << std::hex << entityListAddr << std::dec << std::endl;
    
    // Scan EntityList
    ScanEntityList(entityListAddr);
    
    std::cout << "\n";
    std::cout << "╔═══════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                    SCAN COMPLETE                          ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << "\n[NEXT STEPS]" << std::endl;
    std::cout << "1. Review the candidates above" << std::endl;
    std::cout << "2. Update offsets in SDK.h (Offsets::Fallback namespace)" << std::endl;
    std::cout << "3. Rebuild and test ESP functionality" << std::endl;
    std::cout << "4. If ESP still doesn't work, run scan again and try other candidates" << std::endl;
    std::cout << "\n";
}
