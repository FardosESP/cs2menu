#include "Aimbot.h"
#include <iostream>

#define M_PI 3.14159265358979323846

void Aimbot::Update(C_CSPlayerPawn* localPlayer, C_CSGameEntitySystem* entitySystem, uintptr_t clientBase)
{
    if (!localPlayer || !entitySystem || !clientBase)
        return;
    
    // This will be called from Features::RunAimbot()
    // Implementation will be added based on config
}

C_CSPlayerPawn* Aimbot::GetBestTarget(C_CSPlayerPawn* localPlayer, C_CSGameEntitySystem* entitySystem,
                                       float fov, bool visibleOnly, bool teamCheck, float& outFov)
{
    if (!localPlayer || !entitySystem)
        return nullptr;
    
    C_CSPlayerPawn* bestTarget = nullptr;
    float bestFov = fov;
    
    __try
    {
        int localTeam = localPlayer->GetTeamNum();
        Vector3 localEyePos = localPlayer->GetEyePosition();
        
        // Get current view angles (placeholder - needs actual implementation)
        Vector3 viewAngles = {}; // TODO: Read from dwViewAngles
        
        int maxEntities = entitySystem->GetHighestEntityIndex();
        if (maxEntities <= 0 || maxEntities > 8192)
            maxEntities = 64;
        
        // Iterate through all players (increased limit to 2048 for CS2 chunk system)
        int scanLimit = (maxEntities > 2048) ? 2048 : maxEntities;
        for (int i = 1; i <= scanLimit; i++)
        {
            C_BaseEntity* entity = entitySystem->GetBaseEntity(i);
            if (!entity) continue;
            
            if (!Memory::IsValidPointer((uintptr_t)entity)) continue;
            if (!entity->IsValid()) continue;
            
            int health = entity->GetHealth();
            if (health <= 0 || health > 100) continue;
            
            C_CSPlayerPawn* target = (C_CSPlayerPawn*)entity;
            
            // Skip self
            if (target == localPlayer) continue;
            
            // Team check
            if (teamCheck && target->GetTeamNum() == localTeam) continue;
            
            // Dormant check
            if (target->IsDormant()) continue;
            
            // Get target head position
            Vector3 targetPos = target->GetEyePosition();
            
            // Visibility check
            if (visibleOnly && !IsVisible(localPlayer, target, targetPos))
                continue;
            
            // Calculate FOV to target
            Vector3 aimAngles = CalculateAngle(localEyePos, targetPos);
            float targetFov = CalculateFOV(viewAngles, aimAngles);
            
            // Check if this target is better
            if (targetFov < bestFov)
            {
                bestFov = targetFov;
                bestTarget = target;
            }
        }
        
        outFov = bestFov;
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        return nullptr;
    }
    
    return bestTarget;
}

Vector3 Aimbot::CalculateAngle(const Vector3& from, const Vector3& to)
{
    Vector3 delta;
    delta.x = to.x - from.x;
    delta.y = to.y - from.y;
    delta.z = to.z - from.z;
    
    float hyp = sqrtf(delta.x * delta.x + delta.y * delta.y);
    
    Vector3 angles;
    angles.x = atan2f(-delta.z, hyp) * (180.0f / M_PI);
    angles.y = atan2f(delta.y, delta.x) * (180.0f / M_PI);
    angles.z = 0.0f;
    
    if (delta.x >= 0.0f)
        angles.y += 180.0f;
    
    NormalizeAngles(angles);
    return angles;
}

float Aimbot::CalculateFOV(const Vector3& viewAngles, const Vector3& aimAngles)
{
    Vector3 delta;
    delta.x = aimAngles.x - viewAngles.x;
    delta.y = aimAngles.y - viewAngles.y;
    delta.z = 0.0f;
    
    NormalizeAngles(delta);
    
    return sqrtf(delta.x * delta.x + delta.y * delta.y);
}

void Aimbot::NormalizeAngles(Vector3& angles)
{
    // Normalize pitch
    while (angles.x > 89.0f)
        angles.x -= 180.0f;
    while (angles.x < -89.0f)
        angles.x += 180.0f;
    
    // Normalize yaw
    while (angles.y > 180.0f)
        angles.y -= 360.0f;
    while (angles.y < -180.0f)
        angles.y += 360.0f;
    
    // Roll should always be 0
    angles.z = 0.0f;
}

Vector3 Aimbot::SmoothAim(const Vector3& currentAngles, const Vector3& targetAngles, float smoothFactor)
{
    if (smoothFactor <= 0.0f)
        return targetAngles;
    
    Vector3 delta;
    delta.x = targetAngles.x - currentAngles.x;
    delta.y = targetAngles.y - currentAngles.y;
    delta.z = 0.0f;
    
    NormalizeAngles(delta);
    
    Vector3 smoothed;
    smoothed.x = currentAngles.x + delta.x / smoothFactor;
    smoothed.y = currentAngles.y + delta.y / smoothFactor;
    smoothed.z = 0.0f;
    
    NormalizeAngles(smoothed);
    return smoothed;
}

bool Aimbot::IsVisible(C_CSPlayerPawn* localPlayer, C_CSPlayerPawn* target, const Vector3& targetPos)
{
    if (!target) return false;
    
    __try
    {
        // Use spotted flag as approximation
        // In CS2, spotted = visible on radar = likely visible
        return target->IsSpotted();
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        return false;
    }
    
    // TODO: Implement proper ray tracing for perfect accuracy
    // Requires access to game's trace functions
}

Vector3 Aimbot::GetBonePosition(C_CSPlayerPawn* pawn, int boneId)
{
    if (!pawn) return {};
    
    __try
    {
        // Use the pawn's GetBonePosition method
        Vector3 bonePos = pawn->GetBonePosition(boneId);
        
        // If bone position is invalid (all zeros), fallback to eye position
        if (bonePos.x == 0.0f && bonePos.y == 0.0f && bonePos.z == 0.0f)
        {
            return pawn->GetEyePosition();
        }
        
        return bonePos;
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        return {};
    }
}
