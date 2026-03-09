#include "Backtrack.h"
#include "Memory.h"
#include <algorithm>

Backtrack::PlayerRecords* Backtrack::GetPlayerRecords(C_CSPlayerPawn* player)
{
    if (!player)
        return nullptr;
    
    uintptr_t addr = (uintptr_t)player;
    
    // Find existing records
    for (auto& pr : allRecords)
    {
        if (pr.playerAddress == addr)
            return &pr;
    }
    
    // Create new records
    PlayerRecords newPR;
    newPR.playerAddress = addr;
    allRecords.push_back(newPR);
    return &allRecords.back();
}

bool Backtrack::IsRecordValid(const Record& record, float currentTime)
{
    if (!record.valid)
        return false;
    
    // Check if record is within valid time range
    float delta = currentTime - record.simulationTime;
    
    // Max 200ms backtrack (0.2 seconds)
    if (delta > 0.2f || delta < 0.0f)
        return false;
    
    return true;
}

void Backtrack::Update(C_CSPlayerPawn* player)
{
    if (!enabled || !player)
        return;
    
    __try
    {
        PlayerRecords* pr = GetPlayerRecords(player);
        if (!pr)
            return;
        
        // Get current data
        Vector3 origin;
        if (!player->GetOriginSafe(origin))
            return;
        
        float simTime = player->GetSimulationTime();
        int tickBase = player->GetTickBase();
        
        // Create new record
        Record newRecord;
        newRecord.origin = origin;
        newRecord.angles = {0, 0, 0}; // TODO: Get actual angles
        newRecord.simulationTime = simTime;
        newRecord.tickCount = tickBase;
        newRecord.valid = true;
        
        // Add to front of deque
        pr->records.push_front(newRecord);
        
        // Remove old records
        while (pr->records.size() > (size_t)maxTicks)
        {
            pr->records.pop_back();
        }
        
        // Invalidate records that are too old
        float currentTime = simTime;
        for (auto& record : pr->records)
        {
            if (!IsRecordValid(record, currentTime))
            {
                record.valid = false;
            }
        }
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        // Silently handle exceptions
    }
}

Backtrack::Record* Backtrack::GetBestRecord(C_CSPlayerPawn* player, Vector3 localEyePos)
{
    if (!enabled || !player)
        return nullptr;
    
    PlayerRecords* pr = GetPlayerRecords(player);
    if (!pr || pr->records.empty())
        return nullptr;
    
    // Find record with best hitchance
    Record* bestRecord = nullptr;
    float bestHitchance = 0.0f;
    
    float currentTime = player->GetSimulationTime();
    
    for (auto& record : pr->records)
    {
        if (!IsRecordValid(record, currentTime))
            continue;
        
        float hitchance = GetHitchance(record, localEyePos, player);
        if (hitchance > bestHitchance)
        {
            bestHitchance = hitchance;
            bestRecord = &record;
        }
    }
    
    return bestRecord;
}

Backtrack::Record* Backtrack::GetLatestRecord(C_CSPlayerPawn* player)
{
    if (!enabled || !player)
        return nullptr;
    
    PlayerRecords* pr = GetPlayerRecords(player);
    if (!pr || pr->records.empty())
        return nullptr;
    
    float currentTime = player->GetSimulationTime();
    
    // Return first valid record (most recent)
    for (auto& record : pr->records)
    {
        if (IsRecordValid(record, currentTime))
            return &record;
    }
    
    return nullptr;
}

float Backtrack::GetHitchance(const Record& record, Vector3 localEyePos, C_CSPlayerPawn* player)
{
    // Calculate distance to target
    float dx = record.origin.x - localEyePos.x;
    float dy = record.origin.y - localEyePos.y;
    float dz = record.origin.z - localEyePos.z;
    float distance = sqrtf(dx*dx + dy*dy + dz*dz);
    
    // Closer = better hitchance
    float hitchance = 1.0f / (1.0f + distance / 1000.0f);
    
    // Prefer newer records
    float age = player->GetSimulationTime() - record.simulationTime;
    hitchance *= (1.0f - age / 0.2f);
    
    return hitchance;
}

void Backtrack::Clear()
{
    allRecords.clear();
}
