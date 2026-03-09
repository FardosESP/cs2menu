#pragma once

#include <Windows.h>
#include "SDK.h"
#include <vector>
#include <deque>

// Backtrack system for HvH
// Stores player positions from previous ticks to shoot at laggy players
class Backtrack
{
public:
    static Backtrack& Instance()
    {
        static Backtrack instance;
        return instance;
    }
    
    struct Record
    {
        Vector3 origin;
        Vector3 angles;
        float simulationTime;
        int tickCount;
        bool valid;
    };
    
    struct PlayerRecords
    {
        uintptr_t playerAddress;
        std::deque<Record> records;
    };
    
    // Update backtrack records
    void Update(C_CSPlayerPawn* player);
    
    // Get best backtrack record for a player
    Record* GetBestRecord(C_CSPlayerPawn* player, Vector3 localEyePos);
    
    // Get latest valid record
    Record* GetLatestRecord(C_CSPlayerPawn* player);
    
    // Clear all records
    void Clear();
    
    // Configuration
    bool enabled = false;
    int maxTicks = 12; // Max ticks to backtrack (200ms at 64 tick)
    bool onlyOnShot = false;
    
private:
    Backtrack() = default;
    ~Backtrack() = default;
    Backtrack(const Backtrack&) = delete;
    Backtrack& operator=(const Backtrack&) = delete;
    
    std::vector<PlayerRecords> allRecords;
    
    PlayerRecords* GetPlayerRecords(C_CSPlayerPawn* player);
    bool IsRecordValid(const Record& record, float currentTime);
    float GetHitchance(const Record& record, Vector3 localEyePos, C_CSPlayerPawn* player);
};
