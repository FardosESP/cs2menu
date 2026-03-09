#pragma once

#include <Windows.h>
#include "SDK.h"
#include <vector>

// Resolver system for HvH
// Predicts enemy anti-aim angles to improve hit chance
class Resolver
{
public:
    static Resolver& Instance()
    {
        static Resolver instance;
        return instance;
    }
    
    // Resolver types
    enum class ResolverType
    {
        None,
        LBY,           // Lower Body Yaw
        Moving,        // Moving resolver
        Standing,      // Standing resolver
        Bruteforce,    // Bruteforce all angles
        Delta,         // Delta resolver
        LastMove       // Last move direction
    };
    
    // Resolve player's real angles
    Vector3 Resolve(C_CSPlayerPawn* player);
    
    // Update resolver data for a player
    void Update(C_CSPlayerPawn* player);
    
    // Reset resolver data
    void Reset();
    
    // Configuration
    ResolverType type = ResolverType::LBY;
    bool enabled = false;
    bool bruteforceOnMiss = true;
    int bruteforceSteps = 8;
    
private:
    Resolver() = default;
    ~Resolver() = default;
    Resolver(const Resolver&) = delete;
    Resolver& operator=(const Resolver&) = delete;
    
    struct PlayerRecord
    {
        uintptr_t playerAddress;
        float lastLBY;
        float lastFootYaw;
        Vector3 lastVelocity;
        Vector3 lastAngles;
        float lastUpdateTime;
        int missedShots;
        int bruteforceIndex;
        bool isMoving;
        bool isStanding;
    };
    
    std::vector<PlayerRecord> records;
    
    PlayerRecord* GetRecord(C_CSPlayerPawn* player);
    float ResolveLBY(PlayerRecord* record, C_CSPlayerPawn* player);
    float ResolveMoving(PlayerRecord* record, C_CSPlayerPawn* player);
    float ResolveStanding(PlayerRecord* record, C_CSPlayerPawn* player);
    float ResolveBruteforce(PlayerRecord* record);
    float ResolveDelta(PlayerRecord* record, C_CSPlayerPawn* player);
};
