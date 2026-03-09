#include "EntityCachePro.h"
#include <iostream>

// Global entity cache instance
CEntityCachePro g_EntityCachePro;

// SAFE implementation - No crashes
void CEntityCachePro::Update(C_CSGameEntitySystem* entitySystem, C_CSPlayerPawn* localPlayer)
{
    if (!entitySystem || !localPlayer)
    {
        m_validCount = 0;
        return;
    }
    
    m_validCount = 0;
    
    // Clear all entries
    for (int i = 0; i < 65; i++)
    {
        m_entities[i].valid = false;
        m_entities[i].controller = nullptr;
        m_entities[i].pawn = nullptr;
    }
    
    // SAFE METHOD: Use GetBaseEntity (slower but safer)
    // Premium cheats use direct memory access, but we'll use safe method first
    __try
    {
        int maxEntities = entitySystem->GetHighestEntityIndex();
        if (maxEntities <= 0 || maxEntities > 8192)
        {
            maxEntities = 64;
        }
        
        // Scan first 64 entities (players)
        for (int i = 1; i <= 64; i++)
        {
            __try
            {
                // Get controller
                C_BaseEntity* controller = entitySystem->GetBaseEntity(i);
                if (!controller) continue;
                
                // Validate controller pointer
                if (!Memory::IsValidPointer((uintptr_t)controller)) continue;
                
                // Get pawn from controller
                C_CSPlayerPawn* pawn = entitySystem->GetPlayerPawn(controller);
                if (!pawn) continue;
                
                // Validate pawn pointer
                if (!Memory::IsValidPointer((uintptr_t)pawn)) continue;
                
                // Skip local player
                if (pawn == localPlayer) continue;
                
                // Validate pawn
                if (!pawn->IsValid()) continue;
                
                // Read health
                int health = pawn->GetHealth();
                if (health <= 0 || health > 100) continue;
                
                // Read team
                int team = pawn->GetTeamNum();
                
                // Entity is VALID - cache it
                m_entities[i].controller = controller;
                m_entities[i].pawn = pawn;
                m_entities[i].health = health;
                m_entities[i].team = team;
                m_entities[i].valid = true;
                m_entities[i].controllerAddr = (uintptr_t)controller;
                m_entities[i].pawnAddr = (uintptr_t)pawn;
                m_validCount++;
            }
            __except(EXCEPTION_EXECUTE_HANDLER)
            {
                // Silently skip this entity
                continue;
            }
        }
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        std::cout << "[EntityCachePro] Exception during Update - clearing cache" << std::endl;
        m_validCount = 0;
    }
}
