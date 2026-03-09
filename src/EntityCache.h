#pragma once

#include "SDK.h"
#include "Memory.h"
#include <vector>
#include <chrono>
#include <mutex>

// Professional entity caching system
// Reduces CPU usage and improves performance like premium cheats
struct CachedEntity
{
    C_CSPlayerPawn* pawn;
    C_BaseEntity* controller;
    
    // Cached data
    Vector3 position;
    Vector3 eyePosition;
    Vector3 headPosition;
    int health;
    int team;
    int index;
    bool dormant;
    bool spotted;
    bool valid;
    
    // Bone positions (for skeleton ESP)
    Vector3 bonePositions[32];
    bool bonesValid;
    
    std::chrono::steady_clock::time_point lastUpdate;
    
    CachedEntity() : pawn(nullptr), controller(nullptr), health(0), team(0),
                     index(0), dormant(false), spotted(false), valid(false),
                     bonesValid(false) {}
};

class CEntityCache
{
private:
    std::vector<CachedEntity> m_entities;
    std::mutex m_mutex;
    std::chrono::steady_clock::time_point m_lastFullUpdate;
    
    const int UPDATE_INTERVAL_MS = 50;  // Update every 50ms (20 FPS cache)
    const int MAX_ENTITIES = 64;        // CS2 max players
    
public:
    CEntityCache()
    {
        m_entities.reserve(MAX_ENTITIES);
    }
    
    // Update cache (call from separate thread or every N frames)
    void Update(C_CSGameEntitySystem* entitySystem, int localTeam)
    {
        if (!entitySystem) return;
        
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastFullUpdate).count();
        
        if (elapsed < UPDATE_INTERVAL_MS)
            return;  // Too soon, skip update
        
        std::lock_guard<std::mutex> lock(m_mutex);
        
        m_entities.clear();
        m_lastFullUpdate = now;
        
        int maxEntities = entitySystem->GetHighestEntityIndex();
        if (maxEntities <= 0 || maxEntities > 8192)
            maxEntities = 64;
        
        int scanLimit = (maxEntities > 2048) ? 2048 : maxEntities;
        
        for (int i = 1; i <= scanLimit; i++)
        {
            C_BaseEntity* controller = entitySystem->GetBaseEntity(i);
            if (!controller) continue;
            
            if (!Memory::IsValidPointer((uintptr_t)controller)) continue;
            if (!controller->IsValid()) continue;
            
            int health = controller->GetHealth();
            if (health <= 0 || health > 100) continue;
            
            C_CSPlayerPawn* pawn = entitySystem->GetPlayerPawn(controller);
            if (!pawn) continue;
            
            // Create cached entity
            CachedEntity cached;
            cached.pawn = pawn;
            cached.controller = controller;
            cached.index = i;
            cached.health = health;
            cached.team = controller->GetTeamNum();
            cached.dormant = controller->IsDormant();
            cached.spotted = pawn->IsSpotted();
            cached.valid = true;
            cached.lastUpdate = now;
            
            // Cache positions (removed __try/__except due to C++ object unwinding)
            cached.position = pawn->GetOrigin();
            cached.eyePosition = pawn->GetEyePosition();
            cached.headPosition = pawn->GetBonePosition(6);  // Head bone
            
            // Cache skeleton bones if needed
            // cached.bonesValid = CacheBones(pawn, cached.bonePositions);
            
            m_entities.push_back(cached);
        }
    }
    
    // Get all cached entities (thread-safe)
    std::vector<CachedEntity> GetEntities()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_entities;  // Copy
    }
    
    // Get entity count
    size_t GetCount()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_entities.size();
    }
    
    // Clear cache
    void Clear()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_entities.clear();
    }
    
private:
    // Cache bone positions for skeleton ESP
    bool CacheBones(C_CSPlayerPawn* pawn, Vector3* outBones)
    {
        if (!pawn) return false;
        
        // Cache important bones (removed __try/__except due to C++ object unwinding)
        const int bones[] = {
            6,  // Head
            5,  // Neck
            4,  // Upper spine
            2,  // Lower spine
            7,  // Left shoulder
            8,  // Left elbow
            9,  // Left hand
            13, // Right shoulder
            14, // Right elbow
            15, // Right hand
            22, // Left hip
            23, // Left knee
            24, // Left foot
            25, // Right hip
            26, // Right knee
            27  // Right foot
        };
        
        for (int i = 0; i < 16; i++)
        {
            outBones[i] = pawn->GetBonePosition(bones[i]);
        }
        
        return true;
    }
};

// Global instance
extern CEntityCache g_EntityCache;
