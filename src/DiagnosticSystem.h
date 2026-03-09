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
        
        // 3. Verificar LocalPlayer
        std::cout << "\n[3] LOCAL PLAYER VERIFICATION\n";
        
        uintptr_t localPawnAddr = clientBase + Offsets::dwLocalPlayerPawn();
        uintptr_t localPawnPtr = Memory::Read<uintptr_t>(localPawnAddr);
        
        std::cout << "    dwLocalPlayerPawn offset: 0x" << std::hex << Offsets::dwLocalPlayerPawn() << std::dec << "\n";
        std::cout << "    Address: 0x" << std::hex << localPawnAddr << std::dec << "\n";
        std::cout << "    Pointer: 0x" << std::hex << localPawnPtr << std::dec << "\n";
        
        if (localPawnPtr == 0 || !Memory::IsValidPointer(localPawnPtr)) {
            std::cout << "    [✗] LocalPlayer: NULL or INVALID\n";
            std::cout << "    [!] You must be IN-GAME (not in menu)\n";
        } else {
            C_CSPlayerPawn* localPlayer = (C_CSPlayerPawn*)localPawnPtr;
            
            int health = localPlayer->GetHealth();
            int team = localPlayer->GetTeamNum();
            
            std::cout << "    [✓] LocalPlayer: VALID\n";
            std::cout << "    Health: " << health << "\n";
            std::cout << "    Team: " << team << "\n";
            
            Vector3 origin;
            if (localPlayer->GetOriginSafe(origin)) {
                std::cout << "    Position: (" << origin.x << ", " << origin.y << ", " << origin.z << ")\n";
                std::cout << "    [✓] Origin: VALID\n";
            } else {
                std::cout << "    [✗] Origin: FAILED TO READ\n";
            }
        }
        
        // 4. Escanear jugadores
        std::cout << "\n[4] PLAYER SCAN\n";
        
        int validControllers = 0;
        int validPawns = 0;
        
        for (int i = 1; i <= 64; i++) {
            C_BaseEntity* controller = entitySystem->GetBaseEntity(i);
            if (!controller || !Memory::IsValidPointer((uintptr_t)controller)) continue;
            
            validControllers++;
            
            C_CSPlayerPawn* pawn = entitySystem->GetPlayerPawn(controller);
            if (!pawn || !Memory::IsValidPointer((uintptr_t)pawn)) continue;
            
            if (!pawn->IsValid()) continue;
            
            int health = pawn->GetHealth();
            if (health <= 0 || health > 100) continue;
            
            validPawns++;
            
            if (validPawns <= 5) { // Mostrar solo los primeros 5
                std::cout << "    Player " << i << ": Health=" << health << ", Team=" << pawn->GetTeamNum() << "\n";
            }
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
        } else if (validPawns == 0) {
            std::cout << "[!] CRITICAL: No players detected\n";
            std::cout << "    Solution: Offsets are outdated, run offset scanner (F9)\n";
        } else {
            std::cout << "[✓] System appears to be working\n";
            std::cout << "    If features still don't work, check:\n";
            std::cout << "    1. Enable features in menu (INSERT key)\n";
            std::cout << "    2. Check feature-specific offsets\n";
            std::cout << "    3. Run offset scanner (F9) for latest offsets\n";
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
