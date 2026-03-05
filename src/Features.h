#pragma once

#include "SDK.h"
#include <Windows.h>

namespace Features
{
    // ESP
    void RenderESP();
    bool WorldToScreen(const Vector3& world, Vector3& screen);
    void DrawBox(const Vector3& head, const Vector3& feet, bool filled, float thickness, float color[4]);
    void DrawSkeleton(C_CSPlayerPawn* pawn, float color[4]);
    
    // Aimbot
    void RunAimbot();
    C_CSPlayerPawn* GetBestTarget(float& bestFov);
    void AimAt(const Vector3& target, float smooth);
    
    // Visuals
    void ApplyVisuals();
    void UpdateFOV();
    void UpdateBrightness();
    void DrawCustomCrosshair();
    
    // Misc
    void RunMisc();
    void BunnyHop();
    void RadarHack();
    
    // Inicialización
    void Initialize();
    void Update();
}
