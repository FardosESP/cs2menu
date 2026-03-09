#include "SkinChanger.h"
#include "Memory.h"
#include "OffsetManager.h"
#include <iostream>

// Weapon offsets from offsets.json
namespace WeaponOffsets
{
    inline uintptr_t m_iItemDefinitionIndex() { return OffsetManager::Instance().GetOffset("m_iItemDefinitionIndex", 0x1A8); }
    inline uintptr_t m_AttributeManager() { return OffsetManager::Instance().GetOffset("m_AttributeManager", 0x1090); }
    inline uintptr_t m_Item() { return OffsetManager::Instance().GetOffset("m_Item", 0x50); }
    inline uintptr_t m_iEntityQuality() { return OffsetManager::Instance().GetOffset("m_iEntityQuality", 0x1AC); }
    inline uintptr_t m_iEntityLevel() { return OffsetManager::Instance().GetOffset("m_iEntityLevel", 0x1B0); }
    inline uintptr_t m_iAccountID() { return OffsetManager::Instance().GetOffset("m_iAccountID", 0x1B8); }
    inline uintptr_t m_iItemIDHigh() { return OffsetManager::Instance().GetOffset("m_iItemIDHigh", 0x1C0); }
    inline uintptr_t m_iItemIDLow() { return OffsetManager::Instance().GetOffset("m_iItemIDLow", 0x1BC); }
    inline uintptr_t m_szCustomName() { return OffsetManager::Instance().GetOffset("m_szCustomName", 0x304); }
    inline uintptr_t m_OriginalOwnerXuidLow() { return OffsetManager::Instance().GetOffset("m_OriginalOwnerXuidLow", 0x31B4); }
    inline uintptr_t m_OriginalOwnerXuidHigh() { return OffsetManager::Instance().GetOffset("m_OriginalOwnerXuidHigh", 0x31B8); }
    inline uintptr_t m_nFallbackPaintKit() { return OffsetManager::Instance().GetOffset("m_nFallbackPaintKit", 0x31B8); }
    inline uintptr_t m_nFallbackSeed() { return OffsetManager::Instance().GetOffset("m_nFallbackSeed", 0x31BC); }
    inline uintptr_t m_flFallbackWear() { return OffsetManager::Instance().GetOffset("m_flFallbackWear", 0x31C0); }
    inline uintptr_t m_nFallbackStatTrak() { return OffsetManager::Instance().GetOffset("m_nFallbackStatTrak", 0x31C4); }
}

void SkinChanger::ApplySkin(C_BaseEntity* weapon, int paintKit, int seed, float wear, int statTrak)
{
    if (!weapon || !Memory::IsValidPointer((uintptr_t)weapon))
        return;
    
    __try
    {
        uintptr_t weaponAddr = (uintptr_t)weapon;
        
        // Set fallback paint kit (skin ID)
        *(int*)(weaponAddr + WeaponOffsets::m_nFallbackPaintKit()) = paintKit;
        
        // Set fallback seed (pattern variation)
        *(int*)(weaponAddr + WeaponOffsets::m_nFallbackSeed()) = seed;
        
        // Set fallback wear (0.0 = Factory New, 1.0 = Battle-Scarred)
        *(float*)(weaponAddr + WeaponOffsets::m_flFallbackWear()) = wear;
        
        // Set StatTrak counter if specified
        if (statTrak >= 0)
        {
            *(int*)(weaponAddr + WeaponOffsets::m_nFallbackStatTrak()) = statTrak;
        }
        
        // Force update by modifying item ID
        // This tricks the game into thinking it's a new item
        uintptr_t attributeManager = weaponAddr + WeaponOffsets::m_AttributeManager();
        uintptr_t item = attributeManager + WeaponOffsets::m_Item();
        
        // Set item ID high to -1 to force update
        *(int*)(item + WeaponOffsets::m_iItemIDHigh()) = -1;
        
        // Set entity quality (4 = unusual, makes skins work)
        *(int*)(weaponAddr + WeaponOffsets::m_iEntityQuality()) = 4;
        
        std::cout << "[SkinChanger] Applied skin: PaintKit=" << paintKit 
                  << ", Seed=" << seed << ", Wear=" << wear << std::endl;
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        std::cout << "[SkinChanger] Error applying skin" << std::endl;
    }
}

void SkinChanger::ApplyToAllWeapons(C_CSPlayerPawn* localPlayer, int paintKit, int seed, float wear)
{
    if (!localPlayer || !Memory::IsValidPointer((uintptr_t)localPlayer))
        return;
    
    __try
    {
        // Get active weapon handle
        uint32_t weaponHandle = localPlayer->GetActiveWeaponHandle();
        if (weaponHandle == 0 || weaponHandle == 0xFFFFFFFF)
            return;
        
        // Extract entity index from handle
        int weaponIndex = weaponHandle & 0x7FFF;
        
        if (weaponIndex <= 0 || weaponIndex >= 8192)
            return;
        
        // Get weapon entity from entity system
        extern C_CSGameEntitySystem* g_pEntitySystem;
        if (!g_pEntitySystem)
            return;
        
        C_BaseEntity* weapon = g_pEntitySystem->GetBaseEntity(weaponIndex);
        if (!weapon)
            return;
        
        // Apply skin to active weapon
        ApplySkin(weapon, paintKit, seed, wear);
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        std::cout << "[SkinChanger] Error applying to all weapons" << std::endl;
    }
}

void SkinChanger::ApplyKnifeSkin(C_BaseEntity* knife, int paintKit, int seed, float wear, int knifeModel)
{
    if (!knife || !Memory::IsValidPointer((uintptr_t)knife))
        return;
    
    __try
    {
        // Apply skin first
        ApplySkin(knife, paintKit, seed, wear);
        
        // Change knife model if specified
        if (knifeModel > 0)
        {
            uintptr_t knifeAddr = (uintptr_t)knife;
            
            // Set item definition index to change knife model
            // Common knife IDs:
            // 500 = Bayonet, 503 = Classic Knife, 505 = Flip Knife
            // 506 = Gut Knife, 507 = Karambit, 508 = M9 Bayonet
            // 509 = Huntsman, 512 = Falchion, 514 = Bowie
            // 515 = Butterfly, 516 = Shadow Daggers
            *(int*)(knifeAddr + WeaponOffsets::m_iItemDefinitionIndex()) = knifeModel;
            
            std::cout << "[SkinChanger] Applied knife model: " << knifeModel << std::endl;
        }
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        std::cout << "[SkinChanger] Error applying knife skin" << std::endl;
    }
}

int SkinChanger::GetWeaponDefIndex(C_BaseEntity* weapon)
{
    if (!weapon || !Memory::IsValidPointer((uintptr_t)weapon))
        return -1;
    
    __try
    {
        uintptr_t weaponAddr = (uintptr_t)weapon;
        return *(int*)(weaponAddr + WeaponOffsets::m_iItemDefinitionIndex());
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        return -1;
    }
}

void SkinChanger::SetCustomName(C_BaseEntity* weapon, const char* name)
{
    if (!weapon || !Memory::IsValidPointer((uintptr_t)weapon) || !name)
        return;
    
    __try
    {
        uintptr_t weaponAddr = (uintptr_t)weapon;
        
        // Copy custom name (max 32 characters)
        char* customNamePtr = (char*)(weaponAddr + WeaponOffsets::m_szCustomName());
        strncpy_s(customNamePtr, 32, name, _TRUNCATE);
        
        std::cout << "[SkinChanger] Set custom name: " << name << std::endl;
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        std::cout << "[SkinChanger] Error setting custom name" << std::endl;
    }
}
