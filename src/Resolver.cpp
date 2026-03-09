#include "Resolver.h"
#include "Memory.h"
#include <cmath>

// Normalize angle
static float NormalizeAngle(float angle)
{
    while (angle > 180.0f) angle -= 360.0f;
    while (angle < -180.0f) angle += 360.0f;
    return angle;
}

// Get angle delta
static float AngleDelta(float a, float b)
{
    float delta = NormalizeAngle(a - b);
    return delta;
}

Resolver::PlayerRecord* Resolver::GetRecord(C_CSPlayerPawn* player)
{
    if (!player)
        return nullptr;
    
    uintptr_t addr = (uintptr_t)player;
    
    // Find existing record
    for (auto& record : records)
    {
        if (record.playerAddress == addr)
            return &record;
    }
    
    // Create new record
    PlayerRecord newRecord = {};
    newRecord.playerAddress = addr;
    newRecord.lastLBY = 0.0f;
    newRecord.lastFootYaw = 0.0f;
    newRecord.lastUpdateTime = 0.0f;
    newRecord.missedShots = 0;
    newRecord.bruteforceIndex = 0;
    newRecord.isMoving = false;
    newRecord.isStanding = false;
    
    records.push_back(newRecord);
    return &records.back();
}

void Resolver::Update(C_CSPlayerPawn* player)
{
    if (!enabled || !player)
        return;
    
    __try
    {
        PlayerRecord* record = GetRecord(player);
        if (!record)
            return;
        
        // Update LBY and foot yaw
        record->lastLBY = player->GetLowerBodyYawTarget();
        record->lastFootYaw = player->GetFootYaw();
        
        // Update velocity
        record->lastVelocity = player->GetVelocity();
        
        // Check if moving
        float speed = sqrtf(
            record->lastVelocity.x * record->lastVelocity.x +
            record->lastVelocity.y * record->lastVelocity.y
        );
        record->isMoving = speed > 0.1f;
        record->isStanding = speed < 0.1f;
        
        // Update time
        record->lastUpdateTime = player->GetSimulationTime();
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        // Silently handle exceptions
    }
}

Vector3 Resolver::Resolve(C_CSPlayerPawn* player)
{
    Vector3 resolved = {0, 0, 0};
    
    if (!enabled || !player)
        return resolved;
    
    __try
    {
        PlayerRecord* record = GetRecord(player);
        if (!record)
            return resolved;
        
        float resolvedYaw = 0.0f;
        
        switch (type)
        {
            case ResolverType::LBY:
                resolvedYaw = ResolveLBY(record, player);
                break;
                
            case ResolverType::Moving:
                resolvedYaw = ResolveMoving(record, player);
                break;
                
            case ResolverType::Standing:
                resolvedYaw = ResolveStanding(record, player);
                break;
                
            case ResolverType::Bruteforce:
                resolvedYaw = ResolveBruteforce(record);
                break;
                
            case ResolverType::Delta:
                resolvedYaw = ResolveDelta(record, player);
                break;
                
            case ResolverType::LastMove:
            {
                // Use last velocity direction
                float velYaw = atan2f(record->lastVelocity.y, record->lastVelocity.x) * (180.0f / 3.14159265f);
                resolvedYaw = NormalizeAngle(velYaw);
                break;
            }
            
            default:
                resolvedYaw = record->lastLBY;
                break;
        }
        
        resolved.y = resolvedYaw;
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        // Silently handle exceptions
    }
    
    return resolved;
}

float Resolver::ResolveLBY(PlayerRecord* record, C_CSPlayerPawn* player)
{
    // Use lower body yaw as base
    float lby = player->GetLowerBodyYawTarget();
    
    // If standing, LBY is usually accurate
    if (record->isStanding)
    {
        return lby;
    }
    
    // If moving, use foot yaw
    if (record->isMoving)
    {
        return player->GetFootYaw();
    }
    
    return lby;
}

float Resolver::ResolveMoving(PlayerRecord* record, C_CSPlayerPawn* player)
{
    // Use velocity direction
    Vector3 vel = player->GetVelocity();
    float velYaw = atan2f(vel.y, vel.x) * (180.0f / 3.14159265f);
    
    return NormalizeAngle(velYaw);
}

float Resolver::ResolveStanding(PlayerRecord* record, C_CSPlayerPawn* player)
{
    // Use LBY when standing
    float lby = player->GetLowerBodyYawTarget();
    float footYaw = player->GetFootYaw();
    
    // Calculate delta
    float delta = AngleDelta(lby, footYaw);
    
    // If delta is small, use LBY
    if (fabs(delta) < 35.0f)
    {
        return lby;
    }
    
    // Otherwise, resolve based on delta direction
    if (delta > 0)
    {
        return NormalizeAngle(footYaw + 60.0f);
    }
    else
    {
        return NormalizeAngle(footYaw - 60.0f);
    }
}

float Resolver::ResolveBruteforce(PlayerRecord* record)
{
    // Bruteforce through common angles
    float angles[] = {0.0f, 90.0f, -90.0f, 180.0f, 45.0f, -45.0f, 135.0f, -135.0f};
    int numAngles = sizeof(angles) / sizeof(float);
    
    float baseYaw = record->lastLBY;
    int index = record->bruteforceIndex % numAngles;
    
    return NormalizeAngle(baseYaw + angles[index]);
}

float Resolver::ResolveDelta(PlayerRecord* record, C_CSPlayerPawn* player)
{
    float lby = player->GetLowerBodyYawTarget();
    float footYaw = player->GetFootYaw();
    
    // Calculate delta between LBY and foot yaw
    float delta = AngleDelta(lby, footYaw);
    
    // Resolve based on delta
    if (fabs(delta) < 35.0f)
    {
        // Small delta, use LBY
        return lby;
    }
    else if (delta > 0)
    {
        // Positive delta, resolve right
        return NormalizeAngle(footYaw + 58.0f);
    }
    else
    {
        // Negative delta, resolve left
        return NormalizeAngle(footYaw - 58.0f);
    }
}

void Resolver::Reset()
{
    records.clear();
}
