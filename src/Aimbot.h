#pragma once

#include "SDK.h"
#include "Memory.h"
#include <cmath>

class Aimbot
{
public:
    static Aimbot& Instance()
    {
        static Aimbot instance;
        return instance;
    }
    
    // Main aimbot update function
    void Update(C_CSPlayerPawn* localPlayer, C_CSGameEntitySystem* entitySystem, uintptr_t clientBase);
    
    // Target selection
    C_CSPlayerPawn* GetBestTarget(C_CSPlayerPawn* localPlayer, C_CSGameEntitySystem* entitySystem, 
                                   float fov, bool visibleOnly, bool teamCheck, float& outFov);
    
    // Angle calculations
    Vector3 CalculateAngle(const Vector3& from, const Vector3& to);
    float CalculateFOV(const Vector3& viewAngles, const Vector3& aimAngles);
    void NormalizeAngles(Vector3& angles);
    
    // Smooth aim
    Vector3 SmoothAim(const Vector3& currentAngles, const Vector3& targetAngles, float smoothFactor);
    
    // Visibility check
    bool IsVisible(C_CSPlayerPawn* localPlayer, C_CSPlayerPawn* target, const Vector3& targetPos);
    
    // Bone positions
    Vector3 GetBonePosition(C_CSPlayerPawn* pawn, int boneId);
    
private:
    Aimbot() = default;
    ~Aimbot() = default;
    Aimbot(const Aimbot&) = delete;
    Aimbot& operator=(const Aimbot&) = delete;
};
