#pragma once

#include <Windows.h>
#include "SDK.h"

// Anti-Aim system for HvH
// Manipulates view angles to make the player harder to hit
class AntiAim
{
public:
    static AntiAim& Instance()
    {
        static AntiAim instance;
        return instance;
    }
    
    // Anti-Aim types
    enum class PitchType
    {
        None,
        Up,
        Down,
        Zero,
        Fake,
        FakeUp,
        FakeDown
    };
    
    enum class YawType
    {
        None,
        Backward,
        Spin,
        Jitter,
        Sideways,
        Random,
        FakeBackward,
        FakeSpin
    };
    
    // Apply anti-aim to view angles
    void Apply(C_CSPlayerPawn* localPlayer, Vector3& viewAngles);
    
    // Fake lag (choke packets)
    void FakeLag(int ticksToChoke);
    
    // Edge anti-aim (auto-adjust based on walls)
    void EdgeAA(C_CSPlayerPawn* localPlayer, Vector3& viewAngles);
    
    // Freestanding anti-aim (auto-adjust based on enemy positions)
    void Freestanding(C_CSPlayerPawn* localPlayer, Vector3& viewAngles);
    
    // Manual anti-aim (left/right/back)
    void ManualAA(Vector3& viewAngles, int direction); // 0=back, 1=left, 2=right
    
    // Configuration
    PitchType pitchType = PitchType::Down;
    YawType yawType = YawType::Backward;
    bool enabled = false;
    bool fakeLagEnabled = false;
    int fakeLagTicks = 14;
    float yawOffset = 0.0f;
    float jitterRange = 60.0f;
    float spinSpeed = 10.0f;
    
private:
    AntiAim() = default;
    ~AntiAim() = default;
    AntiAim(const AntiAim&) = delete;
    AntiAim& operator=(const AntiAim&) = delete;
    
    float currentYaw = 0.0f;
    int chokedTicks = 0;
};
