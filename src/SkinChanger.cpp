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
    
    uintptr_t weaponAddr = (uintptr_t)weapon;
    
    // Check each offset before writing (CRITICAL FIX)
    uintptr_t paintKitAddr = weaponAddr + WeaponOffsets::m_nFallbackPaintKit();
    if (!Memory::IsValidPointer(paintKitAddr)) return;
    
    uintptr_t seedAddr = weaponAddr + WeaponOffsets::m_nFallbackSeed();
    if (!Memory::IsValidPointer(seedAddr)) return;
    
    uintptr_t wearAddr = weaponAddr + WeaponOffsets::m_flFallbackWear();
    if (!Memory::IsValidPointer(wearAddr)) return;
    
    // Force owner to local (FIX: skins not applying)
    Memory::Write<uint64_t>(weaponAddr + WeaponOffsets::m_OriginalOwnerXuidLow(), 0);
    Memory::Write<uint64_t>(weaponAddr + WeaponOffsets::m_OriginalOwnerXuidHigh(), 0);
    
    // Force quality to exotic (4 = unusual/exotic)
    Memory::Write<int>(weaponAddr + WeaponOffsets::m_iEntityQuality(), 4);
    
    // Apply skin
    Memory::Write<int>(paintKitAddr, paintKit);
    Memory::Write<int>(seedAddr, seed);
    Memory::Write<float>(wearAddr, wear);
    
    // Set StatTrak counter if specified
    if (statTrak >= 0)
    {
        uintptr_t statTrakAddr = weaponAddr + WeaponOffsets::m_nFallbackStatTrak();
        if (Memory::IsValidPointer(statTrakAddr))
            Memory::Write<int>(statTrakAddr, statTrak);
    }
    
    // Force full update (CRUCIAL - makes skins visible immediately)
    uintptr_t attrAddr = weaponAddr + WeaponOffsets::m_AttributeManager();
    if (Memory::IsValidPointer(attrAddr))
    {
        uintptr_t itemAddr = attrAddr + WeaponOffsets::m_Item();
        if (Memory::IsValidPointer(itemAddr))
        {
            Memory::Write<int>(itemAddr + WeaponOffsets::m_iItemIDHigh(), -1);
            Memory::Write<int>(itemAddr + WeaponOffsets::m_iItemIDLow(), -1);
        }
    }
    
    std::cout << "[SkinChanger] Applied skin to 0x" << std::hex << weaponAddr << std::dec 
              << " (PaintKit=" << paintKit << ", Seed=" << seed << ", Wear=" << wear << ")" << std::endl;
}

void SkinChanger::ApplyToAllWeapons(C_CSPlayerPawn* localPlayer, int paintKit, int seed, float wear)
{
    if (!localPlayer || !Memory::IsValidPointer((uintptr_t)localPlayer))
        return;
    
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
    if (!weapon || !Memory::IsValidPointer((uintptr_t)weapon))
        return;
    
    // Validate it's actually a weapon (FIX: don't apply to props/bots)
    int defIndex = GetWeaponDefIndex(weapon);
    if (defIndex <= 0)
        return;
    
    // Apply skin to active weapon
    ApplySkin(weapon, paintKit, seed, wear);
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
    
    uintptr_t weaponAddr = (uintptr_t)weapon;
    uintptr_t defIndexAddr = weaponAddr + WeaponOffsets::m_iItemDefinitionIndex();
    
    if (!Memory::IsValidPointer(defIndexAddr))
        return -1;
    
    // Safe read with fallback
    int defIndex = Memory::Read<int>(defIndexAddr);
    
    // Validate def index (weapons are typically 1-65, knives 500-525)
    if (defIndex < 0 || defIndex > 600)
        return -1;
    
    return defIndex;
}

void SkinChanger::SetCustomName(C_BaseEntity* weapon, const char* name)
{
    if (!weapon || !Memory::IsValidPointer((uintptr_t)weapon) || !name)
        return;
    
    uintptr_t weaponAddr = (uintptr_t)weapon;
    uintptr_t customNameAddr = weaponAddr + WeaponOffsets::m_szCustomName();
    
    // Check pointer before writing (CRITICAL FIX)
    if (!Memory::IsValidPointer(customNameAddr))
        return;
    
    // Copy custom name safely (max 32 characters)
    char* customNamePtr = (char*)customNameAddr;
    strncpy_s(customNamePtr, 32, name, _TRUNCATE);
    
    std::cout << "[SkinChanger] Set custom name: " << name << std::endl;
}
