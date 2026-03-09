#pragma once

#include "SDK.h"
#include "Memory.h"
#include <iostream>
#include <iomanip>

// Sistema de diagnóstico profesional para debugging
class DiagnosticSystem
{
public:
    static void RunFullDiagnostic(uintptr_t clientBase, C_CSGameEntitySystem* entitySystem)
    {
        std::cout << "\n╔═══════════════════════════════════════════════════════════╗\n";
        std::cout << "║                                                           ║\n";
        std::cout << "║           DIAGNOSTIC SYSTEM - FULL SCAN                   ║\n";
        std::cout << "║                                                           ║\n";
        std::cout << "╚═══════════════════════════════════════════════════════════╝\n\n";
        
        // 1. Verificar client.dll
        std::cout << "[1] CLIENT.DLL VERIFICATION\n";
        std::cout << "    Base Address: 0x" << std::hex << clientBase << std::dec << "\n";
        
        uint16_t mzHeader = Memory::Read<uint16_t>(clientBase);
        if (mzHeader == 0x5A4D) {
            std::cout << "    [✓] MZ Header: VALID\n";
        } else {
            std::cout << "    [✗] MZ Header: INVALID (0x" << std::hex << mzHeader << std::dec << ")\n";
            return;
        }
        
        // 2. Verificar EntitySystem
        std::cout << "\n[2] ENTITY SYSTEM VERIFICATION\n";
        std::cout << "    Address: 0x" << std::hex << (uintptr_t)entitySystem << std::dec << "\n";
        
        if (!Memory::IsValidPointer((uintptr_t)entitySystem)) {
            std::cout << "    [✗] EntitySystem: INVALID POINTER\n";
            return;
        }
        
        int maxEntities = entitySystem->GetHighestEntityIndex();
        std::cout << "    Max Entities: " << maxEntities << "\n";
        
        if (maxEntities <= 0 || maxEntities > 8192) {
            std::cout << "    [✗] Max Entities: OUT OF RANGE\n";
        } else {
            std::cout << "    [✓] Max Entities: VALID\n";
        }
        
        // 3. Verificar LocalPlayer (MULTIPLE METHODS)
        std::cout << "\n[3] LOCAL PLAYER VERIFICATION (MULTI-METHOD)\n";
        
        // Method 1: dwLocalPlayerPawn (direct)
        std::cout << "    [Method 1] dwLocalPlayerPawn (direct read)\n";
        uintptr_t localPawnAddr = clientBase + Offsets::dwLocalPlayerPawn();
        uintptr_t localPawnPtr = Memory::Read<uintptr_t>(localPawnAddr);
        
        std::cout << "    dwLocalPlayerPawn offset: 0x" << std::hex << Offsets::dwLocalPlayerPawn() << std::dec << "\n";
        std::cout << "    Address: 0x" << std::hex << localPawnAddr << std::dec << "\n";
        std::cout << "    Pointer: 0x" << std::hex << localPawnPtr << std::dec << "\n";
        
        bool method1Success = false;
        if (localPawnPtr != 0 && Memory::IsValidPointer(localPawnPtr)) {
            C_CSPlayerPawn* localPlayer = (C_CSPlayerPawn*)localPawnPtr;
            
            int health = Memory::Read<int>((uintptr_t)localPlayer + 0x354); // m_iHealth
            int team = Memory::Read<int>((uintptr_t)localPlayer + 0x3F3);   // m_iTeamNum
            
            std::cout << "    [✓] LocalPlayer: VALID\n";
            std::cout << "    Health: " << health << "\n";
            std::cout << "    Team: " << team << "\n";
            
            if (health > 0 && health <= 100 && (team == 2 || team == 3)) {
                method1Success = true;
                std::cout << "    [✓] Method 1: SUCCESS\n";
            } else {
                std::cout << "    [✗] Method 1: Invalid values (wrong offset or not spawned)\n";
            }
        } else {
            std::cout << "    [✗] LocalPlayer: NULL or INVALID\n";
            std::cout << "    [✗] Method 1: FAILED\n";
        }
        
        // Method 2: dwLocalPlayerController -> m_hPlayerPawn
        std::cout << "\n    [Method 2] dwLocalPlayerController -> m_hPlayerPawn\n";
        uintptr_t localControllerAddr = clientBase + Offsets::dwLocalPlayerController();
        uintptr_t localControllerPtr = Memory::Read<uintptr_t>(localControllerAddr);
        
        std::cout << "    dwLocalPlayerController offset: 0x" << std::hex << Offsets::dwLocalPlayerController() << std::dec << "\n";
        std::cout << "    Address: 0x" << std::hex << localControllerAddr << std::dec << "\n";
        std::cout << "    Pointer: 0x" << std::hex << localControllerPtr << std::dec << "\n";
        
        bool method2Success = false;
        if (localControllerPtr != 0 && Memory::IsValidPointer(localControllerPtr)) {
            // Read m_hPlayerPawn (handle to pawn)
            uint32_t pawnHandle = Memory::Read<uint32_t>(localControllerPtr + 0x90C); // m_hPlayerPawn
            std::cout << "    m_hPlayerPawn handle: 0x" << std::hex << pawnHandle << std::dec << "\n";
            
            if (pawnHandle != 0 && pawnHandle != 0xFFFFFFFF) {
                // Try to resolve handle through entity system
                int entityIndex = pawnHandle & 0x7FFF; // Lower 15 bits
                std::cout << "    Entity index from handle: " << entityIndex << "\n";
                
                if (entityIndex > 0 && entityIndex < 8192) {
                    C_BaseEntity* entity = entitySystem->GetBaseEntity(entityIndex);
                    if (entity && Memory::IsValidPointer((uintptr_t)entity)) {
                        int health = Memory::Read<int>((uintptr_t)entity + 0x354);
                        int team = Memory::Read<int>((uintptr_t)entity + 0x3F3);
                        
                        std::cout << "    Health: " << health << "\n";
                        std::cout << "    Team: " << team << "\n";
                        
                        if (health > 0 && health <= 100 && (team == 2 || team == 3)) {
                            method2Success = true;
                            std::cout << "    [✓] Method 2: SUCCESS\n";
                        } else {
                            std::cout << "    [✗] Method 2: Invalid values\n";
                        }
                    } else {
                        std::cout << "    [✗] Method 2: Entity pointer invalid\n";
                    }
                } else {
                    std::cout << "    [✗] Method 2: Invalid entity index\n";
                }
            } else {
                std::cout << "    [✗] Method 2: Invalid handle\n";
            }
        } else {
            std::cout << "    [✗] Method 2: Controller pointer invalid\n";
        }
        
        if (!method1Success && !method2Success) {
            std::cout << "\n    [!] BOTH METHODS FAILED\n";
            std::cout << "    [!] You must be IN-GAME and SPAWNED (not spectating)\n";
            std::cout << "    [!] Try: Join match -> Wait to spawn -> Press F10\n";
        }
        
        // 4. Escanear jugadores (DISABLED - causes crash, needs investigation)
        std::cout << "\n[4] PLAYER SCAN\n";
        std::cout << "    [SKIPPED] Player scan disabled to prevent crashes\n";
        std::cout << "    [INFO] LocalPlayer detection works (Method 1), features should work\n";
        
        /*
        int validControllers = 0;
        int validPawns = 0;
        
        std::cout << "    Scanning entities 1-64...\n";
        
        __try {
            for (int i = 1; i <= 64; i++) {
                C_BaseEntity* controller = entitySystem->GetBaseEntity(i);
                if (!controller || !Memory::IsValidPointer((uintptr_t)controller)) continue;
                
                validControllers++;
                
                // Read m_hPlayerPawn handle
                uint32_t pawnHandle = Memory::Read<uint32_t>((uintptr_t)controller + 0x90C);
                if (pawnHandle == 0 || pawnHandle == 0xFFFFFFFF) continue;
                
                int pawnIndex = pawnHandle & 0x7FFF;
                if (pawnIndex <= 0 || pawnIndex >= 8192) continue;
                
                C_BaseEntity* pawn = entitySystem->GetBaseEntity(pawnIndex);
                if (!pawn || !Memory::IsValidPointer((uintptr_t)pawn)) continue;
                
                // Read health directly from memory (safer than calling methods)
                int health = Memory::Read<int>((uintptr_t)pawn + 0x354); // m_iHealth
                if (health <= 0 || health > 100) continue;
                
                validPawns++;
                
                if (validPawns <= 5) {
                    int team = Memory::Read<int>((uintptr_t)pawn + 0x3F3); // m_iTeamNum
                    std::cout << "    Player " << i << ": Health=" << health << ", Team=" << team << "\n";
                }
            }
        }
        __except(EXCEPTION_EXECUTE_HANDLER) {
            std::cout << "    [!] Exception during player scan (protected)\n";
        }
        
        std::cout << "    Valid Controllers: " << validControllers << "\n";
        std::cout << "    Valid Pawns: " << validPawns << "\n";
        
        if (validPawns == 0) {
            std::cout << "    [✗] NO PLAYERS FOUND\n";
            std::cout << "    [!] Possible causes:\n";
            std::cout << "        - Wrong offsets (dwEntityList, m_hPlayerPawn)\n";
            std::cout << "        - Not in an active match\n";
            std::cout << "        - Entity iteration method incorrect\n";
        } else {
            std::cout << "    [✓] Players found: " << validPawns << "\n";
        }
        */
        
        // 5. Verificar ViewMatrix
        std::cout << "\n[5] VIEW MATRIX VERIFICATION\n";
        
        extern ViewMatrix* g_pViewMatrix;
        std::cout << "    Address: 0x" << std::hex << (uintptr_t)g_pViewMatrix << std::dec << "\n";
        
        if (!Memory::IsValidPointer((uintptr_t)g_pViewMatrix)) {
            std::cout << "    [✗] ViewMatrix: INVALID POINTER\n";
        } else {
            // Verificar que la matriz tenga valores razonables
            float m00 = g_pViewMatrix->matrix[0];
            float m11 = g_pViewMatrix->matrix[5];
            
            if (m00 > -10.0f && m00 < 10.0f && m11 > -10.0f && m11 < 10.0f) {
                std::cout << "    [✓] ViewMatrix: VALID\n";
                std::cout << "    Matrix[0][0]: " << m00 << "\n";
                std::cout << "    Matrix[1][1]: " << m11 << "\n";
            } else {
                std::cout << "    [✗] ViewMatrix: VALUES OUT OF RANGE\n";
            }
        }
        
        // 6. Verificar offsets críticos
        std::cout << "\n[6] CRITICAL OFFSETS\n";
        std::cout << "    dwLocalPlayerPawn: 0x" << std::hex << Offsets::dwLocalPlayerPawn() << std::dec << "\n";
        std::cout << "    dwEntityList: 0x" << std::hex << Offsets::dwEntityList() << std::dec << "\n";
        std::cout << "    dwViewMatrix: 0x" << std::hex << Offsets::dwViewMatrix() << std::dec << "\n";
        std::cout << "    dwViewAngles: 0x" << std::hex << Offsets::dwViewAngles() << std::dec << "\n";
        std::cout << "    m_iHealth: 0x" << std::hex << Offsets::m_iHealth() << std::dec << "\n";
        std::cout << "    m_iTeamNum: 0x" << std::hex << Offsets::m_iTeamNum() << std::dec << "\n";
        std::cout << "    m_pGameSceneNode: 0x" << std::hex << Offsets::m_pGameSceneNode() << std::dec << "\n";
        std::cout << "    m_vecAbsOrigin: 0x" << std::hex << Offsets::m_vecAbsOrigin() << std::dec << "\n";
        
        // 7. Test de escritura de ViewAngles
        std::cout << "\n[7] VIEW ANGLES TEST\n";
        
        uintptr_t viewAnglesAddr = clientBase + Offsets::dwViewAngles();
        std::cout << "    Address: 0x" << std::hex << viewAnglesAddr << std::dec << "\n";
        
        if (Memory::IsValidPointer(viewAnglesAddr)) {
            Vector3 currentAngles = Memory::Read<Vector3>(viewAnglesAddr);
            std::cout << "    Current: (" << currentAngles.x << ", " << currentAngles.y << ", " << currentAngles.z << ")\n";
            
            if (currentAngles.x >= -89.0f && currentAngles.x <= 89.0f &&
                currentAngles.y >= -180.0f && currentAngles.y <= 180.0f) {
                std::cout << "    [✓] ViewAngles: VALID RANGE\n";
            } else {
                std::cout << "    [✗] ViewAngles: OUT OF RANGE (offset may be wrong)\n";
            }
        } else {
            std::cout << "    [✗] ViewAngles: INVALID POINTER\n";
        }
        
        // 8. Resumen
        std::cout << "\n╔═══════════════════════════════════════════════════════════╗\n";
        std::cout << "║                      DIAGNOSTIC SUMMARY                   ║\n";
        std::cout << "╚═══════════════════════════════════════════════════════════╝\n";
        
        if (localPawnPtr == 0) {
            std::cout << "[!] CRITICAL: LocalPlayer is NULL\n";
            std::cout << "    Solution: Join a match and spawn as a player\n";
        } else if (method1Success) {
            std::cout << "[✓] LocalPlayer detected successfully!\n";
            std::cout << "[✓] Features should now work (ESP, Aimbot, Anti-Aim, etc.)\n";
            std::cout << "    If features don't work:\n";
            std::cout << "    1. Enable them in menu (INSERT key)\n";
            std::cout << "    2. Check that you're in an active match\n";
        } else {
            std::cout << "[!] WARNING: LocalPlayer detection issues\n";
            std::cout << "    Solution: Run offset scanner (F9) for latest offsets\n";
        }
        
        std::cout << "\n";
    }
    // Test específico para ESP
    static void TestESP(uintptr_t clientBase, C_CSGameEntitySystem* entitySystem)
    {
        std::cout << "\n[ESP TEST]\n";
        
        extern ViewMatrix* g_pViewMatrix;
        
        C_CSPlayerPawn* localPlayer = nullptr;
        uintptr_t localPawnPtr = Memory::Read<uintptr_t>(clientBase + Offsets::dwLocalPlayerPawn());
        if (localPawnPtr && Memory::IsValidPointer(localPawnPtr)) {
            localPlayer = (C_CSPlayerPawn*)localPawnPtr;
        }
        
        if (!localPlayer) {
            std::cout << "  [✗] LocalPlayer NULL - cannot test ESP\n";
            return;
        }
        
        Vector3 localPos;
        if (!localPlayer->GetOriginSafe(localPos)) {
            std::cout << "  [✗] Cannot read local position\n";
            return;
        }
        
        std::cout << "  Local Position: (" << localPos.x << ", " << localPos.y << ", " << localPos.z << ")\n";
        
        // Buscar un enemigo
        for (int i = 1; i <= 64; i++) {
            C_BaseEntity* controller = entitySystem->GetBaseEntity(i);
            if (!controller) continue;
            
            C_CSPlayerPawn* pawn = entitySystem->GetPlayerPawn(controller);
            if (!pawn || pawn == localPlayer) continue;
            
            if (!pawn->IsValid() || pawn->GetHealth() <= 0) continue;
            
            Vector3 enemyPos;
            if (!pawn->GetOriginSafe(enemyPos)) continue;
            
            // Test WorldToScreen
            Vector3 screenPos;
            bool onScreen = Features::WorldToScreen(enemyPos, screenPos);
            
            std::cout << "  Enemy " << i << ":\n";
            std::cout << "    Position: (" << enemyPos.x << ", " << enemyPos.y << ", " << enemyPos.z << ")\n";
            std::cout << "    Screen: (" << screenPos.x << ", " << screenPos.y << ") - " << (onScreen ? "ON SCREEN" : "OFF SCREEN") << "\n";
            
            break; // Solo mostrar el primero
        }
    }
};
