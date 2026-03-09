#pragma once

#include "SDK.h"
#include "Memory.h"
#include <cstring>

// Professional EntityCache System
// Used by premium CS2 cheats (Neverlose, Onetap, etc.) for safe entity iteration
// This prevents crashes from invalid pointers and improves performance

class CEntityCachePro
{
private:
    struct CachedEntity
    {
        C_BaseEntity* controller;
        C_CSPlayerPawn* pawn;
        int health;
        int team;
        bool valid;
        uintptr_t controllerAddr;
        uintptr_t pawnAddr;
    };
    
    CachedEntity m_entities[65]; // Max 64 players + 1 for safety
    int m_validCount;
    
public:
    CEntityCachePro() : m_validCount(0)
    {
        memset(m_entities, 0, sizeof(m_entities));
    }
    
    // Update cache - call once per frame BEFORE using entities
    // Implementation in EntityCachePro.cpp (SAFE version)
    void Update(C_CSGameEntitySystem* entitySystem, C_CSPlayerPawn* localPlayer);
    
    // Get cached pawn (SAFE - already validated)
    C_CSPlayerPawn* GetPawn(int index)
    {
        if (index < 0 || index >= 65) return nullptr;
        if (!m_entities[index].valid) return nullptr;
        return m_entities[index].pawn;
    }
    
    // Get all valid pawns (SAFE - already validated)
    void GetAllPawns(C_CSPlayerPawn** outPawns, int& outCount, int maxCount)
    {
        outCount = 0;
        for (int i = 1; i <= 64 && outCount < maxCount; i++)
        {
            if (m_entities[i].valid && m_entities[i].pawn)
            {
                outPawns[outCount++] = m_entities[i].pawn;
            }
        }
    }
    
    // Get entity info (SAFE - already validated)
    bool GetEntityInfo(int index, int& outHealth, int& outTeam)
    {
        if (index < 0 || index >= 65) return false;
        if (!m_entities[index].valid) return false;
        
        outHealth = m_entities[index].health;
        outTeam = m_entities[index].team;
        return true;
    }
    
    // Get valid entity count
    int GetValidCount() const { return m_validCount; }
    
    // Check if entity is valid
    bool IsValid(int index) const
    {
        if (index < 0 || index >= 65) return false;
        return m_entities[index].valid;
    }
};

// Global entity cache instance
extern CEntityCachePro g_EntityCachePro;
