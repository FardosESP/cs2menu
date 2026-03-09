#pragma once

#include <Windows.h>
#include <cstdint>
#include "SDK.h"

// Skin Changer for CS2
// Allows changing weapon skins, stickers, and other cosmetic properties
class SkinChanger
{
public:
    static SkinChanger& Instance()
    {
        static SkinChanger instance;
        return instance;
    }
    
    // Apply skin to a weapon entity
    void ApplySkin(C_BaseEntity* weapon, int paintKit, int seed, float wear, int statTrak = -1);
    
    // Apply skin to all weapons of local player
    void ApplyToAllWeapons(C_CSPlayerPawn* localPlayer, int paintKit, int seed, float wear);
    
    // Apply knife skin
    void ApplyKnifeSkin(C_BaseEntity* knife, int paintKit, int seed, float wear, int knifeModel = 0);
    
    // Get weapon definition index (weapon ID)
    int GetWeaponDefIndex(C_BaseEntity* weapon);
    
    // Set custom weapon name
    void SetCustomName(C_BaseEntity* weapon, const char* name);
    
private:
    SkinChanger() = default;
    ~SkinChanger() = default;
    SkinChanger(const SkinChanger&) = delete;
    SkinChanger& operator=(const SkinChanger&) = delete;
};
