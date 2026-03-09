#pragma once

#include "SDK.h"
#include "Memory.h"

// Professional LocalPlayer detection system
// Works in online matches, offline/bots, and practice mode like premium cheats (Neverlose, Onetap, etc.)
class CLocalPlayer
{
private:
    uintptr_t m_clientBase;
    C_CSGameEntitySystem* m_pEntitySystem;
    C_CSPlayerPawn* m_pLocalPawn;
    C_BaseEntity* m_pLocalController;
    bool m_bScannerRan;
    
public:
    CLocalPlayer() : m_clientBase(0), m_pEntitySystem(nullptr), 
                     m_pLocalPawn(nullptr), m_pLocalController(nullptr),
                     m_bScannerRan(false) {}
    
    void Initialize(uintptr_t clientBase, C_CSGameEntitySystem* entitySystem)
    {
        m_clientBase = clientBase;
        m_pEntitySystem = entitySystem;
    }
    
    void Update()
    {
        if (!m_clientBase || !m_pEntitySystem) return;
        
        // Try multiple methods to find local player (professional multi-fallback)
        m_pLocalPawn = nullptr;
        m_pLocalController = nullptr;
        
        // Method 1: dwLocalPlayerPawn (most reliable)
        uintptr_t localPawnAddr = m_clientBase + Offsets::dwLocalPlayerPawn();
        uintptr_t pawnPtr = Memory::Read<uintptr_t>(localPawnAddr);
        if (pawnPtr && Memory::IsValidPointer(pawnPtr))
        {
            m_pLocalPawn = (C_CSPlayerPawn*)pawnPtr;
            if (m_pLocalPawn && m_pLocalPawn->IsValid() && m_pLocalPawn->GetHealth() > 0)
            {
                return; // Success
            }
        }
        
        // Method 2: dwLocalPlayerController -> GetPlayerPawn
        uintptr_t localControllerAddr = m_clientBase + Offsets::dwLocalPlayerController();
        uintptr_t controllerPtr = Memory::Read<uintptr_t>(localControllerAddr);
        if (controllerPtr && Memory::IsValidPointer(controllerPtr))
        {
            m_pLocalController = (C_BaseEntity*)controllerPtr;
            m_pLocalPawn = m_pEntitySystem->GetPlayerPawn(m_pLocalController);
            if (m_pLocalPawn && m_pLocalPawn->IsValid() && m_pLocalPawn->GetHealth() > 0)
            {
                return; // Success
            }
        }
        
        // Method 3: Scan entity list for local player (fallback for offline/bots)
        int maxEntities = m_pEntitySystem->GetHighestEntityIndex();
        if (maxEntities > 0 && maxEntities < 8192)
        {
            for (int i = 1; i <= 64; i++) // Only scan first 64 (players)
            {
                C_BaseEntity* controller = m_pEntitySystem->GetBaseEntity(i);
                if (!controller || !Memory::IsValidPointer((uintptr_t)controller)) continue;
                
                C_CSPlayerPawn* pawn = m_pEntitySystem->GetPlayerPawn(controller);
                if (!pawn || !Memory::IsValidPointer((uintptr_t)pawn)) continue;
                
                if (pawn->IsValid() && pawn->GetHealth() > 0)
                {
                    // Check if this is local player by comparing with dwLocalPlayerPawn
                    if ((uintptr_t)pawn == pawnPtr)
                    {
                        m_pLocalPawn = pawn;
                        m_pLocalController = controller;
                        return; // Success
                    }
                }
            }
        }
        
        // All methods failed
        m_pLocalPawn = nullptr;
        m_pLocalController = nullptr;
    }
    
    C_CSPlayerPawn* GetPawn() { return m_pLocalPawn; }
    C_BaseEntity* GetController() { return m_pLocalController; }
    bool HasScannerRun() { return m_bScannerRan; }
    void SetScannerRan(bool ran) { m_bScannerRan = ran; }
};

// Global instance declaration
extern CLocalPlayer g_LocalPlayer;
