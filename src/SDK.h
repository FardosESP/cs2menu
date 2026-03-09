#pragma once

#include <Windows.h>
#include <cstdint>
#include "OffsetManager.h"

// --- Estructuras Base ---

struct Vector3
{
    float x, y, z;
};

struct Color
{
    uint8_t r, g, b, a;
};

struct ViewMatrix
{
    float matrix[16];
};

// --- Offsets (DEBEN ir ANTES de las clases que los usan) ---
// Dynamic offsets loaded from OffsetManager
namespace Offsets
{
    // Hardcoded fallback values (Build 14138 - Mar 5, 2026)
    namespace Fallback
    {
        constexpr uintptr_t dwEntityList            = 0x249B2A0;  // 38453920 decimal
        constexpr uintptr_t dwLocalPlayerPawn       = 0x2066E10;  // 33970960 decimal  
        constexpr uintptr_t dwViewMatrix            = 0x2309460;  // 36749024 decimal
        constexpr uintptr_t dwLocalPlayerController = 0x22F1888;  // 36640904 decimal
        constexpr uintptr_t dwViewAngles            = 0x2318668;  // 36796008 decimal
        constexpr uintptr_t dwGlobalVars            = 0x2058FC0;  // 33928128 decimal
        constexpr uintptr_t dwGameRules             = 0x230A160;  // 36745056 decimal
        constexpr uintptr_t dwGameEntitySystem_getHighestEntityIndex = 0x20A0;  // 8352 decimal
        constexpr uintptr_t dwForceJump             = 0x230A1E0;  // 36745696 decimal
        
        // Core player offsets (Build 14138.4 - Mar 9, 2026)
        constexpr uintptr_t m_iHealth        = 0x354;
        constexpr uintptr_t m_iMaxHealth     = 0x350;
        constexpr uintptr_t m_lifeState      = 0x35C;
        constexpr uintptr_t m_iTeamNum       = 0x3F3;
        constexpr uintptr_t m_pGameSceneNode = 0x338;
        constexpr uintptr_t m_vecAbsOrigin   = 0xD0;
        constexpr uintptr_t m_hPlayerPawn    = 0x90C;
        constexpr uintptr_t m_iIDEntIndex    = 0x1598;
        constexpr uintptr_t m_bDormant       = 0xE8;
        
        // Movement & physics
        constexpr uintptr_t m_fFlags         = 0x400;
        constexpr uintptr_t m_vecVelocity    = 0x438;
        constexpr uintptr_t m_MoveType       = 0x3FC;
        constexpr uintptr_t m_flMaxspeed     = 0x57C;
        constexpr uintptr_t m_flDuckAmount   = 0x5B4;
        constexpr uintptr_t m_bDucked        = 0x5BC;
        constexpr uintptr_t m_bDucking       = 0x5BD;
        
        // Aim & view
        constexpr uintptr_t m_angEyeAngles   = 0x3DD0;
        constexpr uintptr_t m_angEyeAnglesVelocity = 0x3EA0;
        constexpr uintptr_t m_vecViewOffset  = 0xD58;
        constexpr uintptr_t m_aimPunchAngle  = 0x16CC;
        constexpr uintptr_t m_aimPunchAngleVel = 0x16D8;
        constexpr uintptr_t m_aimPunchCache  = 0x1728;
        constexpr uintptr_t m_aimPunchTickBase = 0x16E4;
        constexpr uintptr_t m_aimPunchTickFraction = 0x16E8;
        constexpr uintptr_t m_iShotsFired    = 0x270C;
        constexpr uintptr_t m_flLowerBodyYawTarget = 0x3E90;
        constexpr uintptr_t m_flFootYaw      = 0x3A9C;
        constexpr uintptr_t m_flPoseParameter = 0x2764;
        constexpr uintptr_t m_iMostRecentModelBone = 0x1F78;
        constexpr uintptr_t m_angShootAngleHistory = 0x2780;
        
        // ESP & visuals
        constexpr uintptr_t m_bSpotted       = 0x8;
        constexpr uintptr_t m_bSpottedByMask = 0xC;
        constexpr uintptr_t m_bIsScoped      = 0x26F8;
        constexpr uintptr_t m_flFlashDuration = 0x1470;
        constexpr uintptr_t m_flFlashMaxAlpha = 0x1474;
        constexpr uintptr_t m_flFlashBangTime = 0x1478;
        
        // Player state
        constexpr uintptr_t m_flSimulationTime = 0x3C8;
        constexpr uintptr_t m_flOldSimulationTime = 0x3CC;
        constexpr uintptr_t m_nTickBase      = 0x6C0;
        constexpr uintptr_t m_iObserverMode  = 0x3A0;
        constexpr uintptr_t m_hObserverTarget = 0x3A4;
        
        // Player equipment
        constexpr uintptr_t m_ArmorValue     = 0x2424;
        constexpr uintptr_t m_bHasDefuser    = 0x2428;
        constexpr uintptr_t m_bHasHelmet     = 0x2429;
        constexpr uintptr_t m_bIsDefusing    = 0x242A;
        constexpr uintptr_t m_bIsRescuing    = 0x242B;
        constexpr uintptr_t m_bIsGrabbingHostage = 0x26FB;
        constexpr uintptr_t m_bIsWalking     = 0x24C8;
        constexpr uintptr_t m_iAccount       = 0x1720;
        
        // Player controller offsets
        constexpr uintptr_t m_iszPlayerName = 0x6F8; // 1784 in decimal
        
        // Weapon offsets
        constexpr uintptr_t m_hActiveWeapon = 0x60;
        constexpr uintptr_t m_hLastWeapon   = 0x64;
        constexpr uintptr_t m_pClippingWeapon = 0x1290;
        constexpr uintptr_t m_iClip1          = 0x1564;
        constexpr uintptr_t m_iClip2          = 0x1568;
        
        // Skin changer offsets
        constexpr uintptr_t m_nFallbackPaintKit = 0x31B8;
        constexpr uintptr_t m_nFallbackSeed     = 0x31BC;
        constexpr uintptr_t m_flFallbackWear    = 0x31C0;
        constexpr uintptr_t m_nFallbackStatTrak = 0x31C4;
        constexpr uintptr_t m_iItemDefinitionIndex = 0x1A8;
        constexpr uintptr_t m_szCustomName      = 0x304;
        
        // Model and bone offsets
        constexpr uintptr_t m_modelState = 0x160;
        constexpr uintptr_t m_boneArray = 0x80;
        constexpr uintptr_t m_nHitboxSet = 0x438;
        
        // Glow offsets (from C_BaseModelEntity)
        constexpr uintptr_t m_Glow = 0xCC0; // CGlowProperty
        
        // CGlowProperty offsets
        constexpr uintptr_t m_fGlowColor = 0x8;           // Vector (RGB)
        constexpr uintptr_t m_iGlowType = 0x30;           // int32
        constexpr uintptr_t m_iGlowTeam = 0x34;           // int32
        constexpr uintptr_t m_nGlowRange = 0x38;          // int32
        constexpr uintptr_t m_nGlowRangeMin = 0x3C;       // int32
        constexpr uintptr_t m_glowColorOverride = 0x40;   // Color (RGBA)
        constexpr uintptr_t m_bFlashing = 0x44;           // bool
        constexpr uintptr_t m_flGlowTime = 0x48;          // float32
        constexpr uintptr_t m_flGlowStartTime = 0x4C;     // float32
        constexpr uintptr_t m_bEligibleForScreenHighlight = 0x50; // bool
        constexpr uintptr_t m_bGlowing = 0x51;            // bool
    }
    
    // Dynamic offset getters - use OffsetManager with fallback values
    inline uintptr_t dwEntityList()            { return OffsetManager::Instance().GetOffset("dwEntityList", Fallback::dwEntityList); }
    inline uintptr_t dwLocalPlayerPawn()       { return OffsetManager::Instance().GetOffset("dwLocalPlayerPawn", Fallback::dwLocalPlayerPawn); }
    inline uintptr_t dwViewMatrix()            { return OffsetManager::Instance().GetOffset("dwViewMatrix", Fallback::dwViewMatrix); }
    inline uintptr_t dwLocalPlayerController() { return OffsetManager::Instance().GetOffset("dwLocalPlayerController", Fallback::dwLocalPlayerController); }
    inline uintptr_t dwViewAngles()            { return OffsetManager::Instance().GetOffset("dwViewAngles", Fallback::dwViewAngles); }
    inline uintptr_t dwGlobalVars()            { return OffsetManager::Instance().GetOffset("dwGlobalVars", Fallback::dwGlobalVars); }
    inline uintptr_t dwGameRules()             { return OffsetManager::Instance().GetOffset("dwGameRules", Fallback::dwGameRules); }
    inline uintptr_t dwGameEntitySystem_getHighestEntityIndex() { return OffsetManager::Instance().GetOffset("dwGameEntitySystem_getHighestEntityIndex", Fallback::dwGameEntitySystem_getHighestEntityIndex); }
    inline uintptr_t dwForceJump()             { return OffsetManager::Instance().GetOffset("dwForceJump", Fallback::dwForceJump); }
    
    inline uintptr_t m_iHealth()        { return OffsetManager::Instance().GetOffset("m_iHealth", Fallback::m_iHealth); }
    inline uintptr_t m_iMaxHealth()     { return OffsetManager::Instance().GetOffset("m_iMaxHealth", Fallback::m_iMaxHealth); }
    inline uintptr_t m_lifeState()      { return OffsetManager::Instance().GetOffset("m_lifeState", Fallback::m_lifeState); }
    inline uintptr_t m_iTeamNum()       { return OffsetManager::Instance().GetOffset("m_iTeamNum", Fallback::m_iTeamNum); }
    inline uintptr_t m_pGameSceneNode() { return OffsetManager::Instance().GetOffset("m_pGameSceneNode", Fallback::m_pGameSceneNode); }
    inline uintptr_t m_vecAbsOrigin()   { return OffsetManager::Instance().GetOffset("m_vecAbsOrigin", Fallback::m_vecAbsOrigin); }
    inline uintptr_t m_hPlayerPawn()    { return OffsetManager::Instance().GetOffset("m_hPlayerPawn", Fallback::m_hPlayerPawn); }
    inline uintptr_t m_iIDEntIndex()    { return OffsetManager::Instance().GetOffset("m_iIDEntIndex", Fallback::m_iIDEntIndex); }
    inline uintptr_t m_bDormant()       { return OffsetManager::Instance().GetOffset("m_bDormant", Fallback::m_bDormant); }
    
    inline uintptr_t m_fFlags()         { return OffsetManager::Instance().GetOffset("m_fFlags", Fallback::m_fFlags); }
    inline uintptr_t m_vecVelocity()    { return OffsetManager::Instance().GetOffset("m_vecVelocity", Fallback::m_vecVelocity); }
    inline uintptr_t m_MoveType()       { return OffsetManager::Instance().GetOffset("m_MoveType", Fallback::m_MoveType); }
    inline uintptr_t m_flMaxspeed()     { return OffsetManager::Instance().GetOffset("m_flMaxspeed", Fallback::m_flMaxspeed); }
    inline uintptr_t m_flDuckAmount()   { return OffsetManager::Instance().GetOffset("m_flDuckAmount", Fallback::m_flDuckAmount); }
    inline uintptr_t m_bDucked()        { return OffsetManager::Instance().GetOffset("m_bDucked", Fallback::m_bDucked); }
    inline uintptr_t m_bDucking()       { return OffsetManager::Instance().GetOffset("m_bDucking", Fallback::m_bDucking); }
    
    inline uintptr_t m_angEyeAngles()   { return OffsetManager::Instance().GetOffset("m_angEyeAngles", Fallback::m_angEyeAngles); }
    inline uintptr_t m_angEyeAnglesVelocity() { return OffsetManager::Instance().GetOffset("m_angEyeAnglesVelocity", Fallback::m_angEyeAnglesVelocity); }
    inline uintptr_t m_vecViewOffset()  { return OffsetManager::Instance().GetOffset("m_vecViewOffset", Fallback::m_vecViewOffset); }
    inline uintptr_t m_aimPunchAngle()  { return OffsetManager::Instance().GetOffset("m_aimPunchAngle", Fallback::m_aimPunchAngle); }
    inline uintptr_t m_aimPunchAngleVel() { return OffsetManager::Instance().GetOffset("m_aimPunchAngleVel", Fallback::m_aimPunchAngleVel); }
    inline uintptr_t m_aimPunchCache()  { return OffsetManager::Instance().GetOffset("m_aimPunchCache", Fallback::m_aimPunchCache); }
    inline uintptr_t m_aimPunchTickBase() { return OffsetManager::Instance().GetOffset("m_aimPunchTickBase", Fallback::m_aimPunchTickBase); }
    inline uintptr_t m_aimPunchTickFraction() { return OffsetManager::Instance().GetOffset("m_aimPunchTickFraction", Fallback::m_aimPunchTickFraction); }
    inline uintptr_t m_iShotsFired()    { return OffsetManager::Instance().GetOffset("m_iShotsFired", Fallback::m_iShotsFired); }
    inline uintptr_t m_flLowerBodyYawTarget() { return OffsetManager::Instance().GetOffset("m_flLowerBodyYawTarget", Fallback::m_flLowerBodyYawTarget); }
    inline uintptr_t m_flFootYaw()      { return OffsetManager::Instance().GetOffset("m_flFootYaw", Fallback::m_flFootYaw); }
    inline uintptr_t m_flPoseParameter() { return OffsetManager::Instance().GetOffset("m_flPoseParameter", Fallback::m_flPoseParameter); }
    inline uintptr_t m_iMostRecentModelBone() { return OffsetManager::Instance().GetOffset("m_iMostRecentModelBone", Fallback::m_iMostRecentModelBone); }
    inline uintptr_t m_angShootAngleHistory() { return OffsetManager::Instance().GetOffset("m_angShootAngleHistory", Fallback::m_angShootAngleHistory); }
    
    inline uintptr_t m_bSpotted()       { return OffsetManager::Instance().GetOffset("m_bSpotted", Fallback::m_bSpotted); }
    inline uintptr_t m_bSpottedByMask() { return OffsetManager::Instance().GetOffset("m_bSpottedByMask", Fallback::m_bSpottedByMask); }
    inline uintptr_t m_bIsScoped()      { return OffsetManager::Instance().GetOffset("m_bIsScoped", Fallback::m_bIsScoped); }
    inline uintptr_t m_flFlashDuration() { return OffsetManager::Instance().GetOffset("m_flFlashDuration", Fallback::m_flFlashDuration); }
    inline uintptr_t m_flFlashMaxAlpha() { return OffsetManager::Instance().GetOffset("m_flFlashMaxAlpha", Fallback::m_flFlashMaxAlpha); }
    inline uintptr_t m_flFlashBangTime() { return OffsetManager::Instance().GetOffset("m_flFlashBangTime", Fallback::m_flFlashBangTime); }
    
    inline uintptr_t m_iszPlayerName() { return OffsetManager::Instance().GetOffset("m_iszPlayerName", Fallback::m_iszPlayerName); }
    
    inline uintptr_t m_flSimulationTime() { return OffsetManager::Instance().GetOffset("m_flSimulationTime", Fallback::m_flSimulationTime); }
    inline uintptr_t m_flOldSimulationTime() { return OffsetManager::Instance().GetOffset("m_flOldSimulationTime", Fallback::m_flOldSimulationTime); }
    inline uintptr_t m_nTickBase()      { return OffsetManager::Instance().GetOffset("m_nTickBase", Fallback::m_nTickBase); }
    inline uintptr_t m_iObserverMode()  { return OffsetManager::Instance().GetOffset("m_iObserverMode", Fallback::m_iObserverMode); }
    inline uintptr_t m_hObserverTarget() { return OffsetManager::Instance().GetOffset("m_hObserverTarget", Fallback::m_hObserverTarget); }
    
    inline uintptr_t m_ArmorValue()     { return OffsetManager::Instance().GetOffset("m_ArmorValue", Fallback::m_ArmorValue); }
    inline uintptr_t m_bHasDefuser()    { return OffsetManager::Instance().GetOffset("m_bHasDefuser", Fallback::m_bHasDefuser); }
    inline uintptr_t m_bHasHelmet()     { return OffsetManager::Instance().GetOffset("m_bHasHelmet", Fallback::m_bHasHelmet); }
    inline uintptr_t m_bIsDefusing()    { return OffsetManager::Instance().GetOffset("m_bIsDefusing", Fallback::m_bIsDefusing); }
    inline uintptr_t m_bIsRescuing()    { return OffsetManager::Instance().GetOffset("m_bIsRescuing", Fallback::m_bIsRescuing); }
    inline uintptr_t m_bIsGrabbingHostage() { return OffsetManager::Instance().GetOffset("m_bIsGrabbingHostage", Fallback::m_bIsGrabbingHostage); }
    inline uintptr_t m_bIsWalking()     { return OffsetManager::Instance().GetOffset("m_bIsWalking", Fallback::m_bIsWalking); }
    inline uintptr_t m_iAccount()       { return OffsetManager::Instance().GetOffset("m_iAccount", Fallback::m_iAccount); }
    
    inline uintptr_t m_hActiveWeapon() { return OffsetManager::Instance().GetOffset("m_hActiveWeapon", Fallback::m_hActiveWeapon); }
    inline uintptr_t m_hLastWeapon()   { return OffsetManager::Instance().GetOffset("m_hLastWeapon", Fallback::m_hLastWeapon); }
    inline uintptr_t m_pClippingWeapon() { return OffsetManager::Instance().GetOffset("m_pClippingWeapon", Fallback::m_pClippingWeapon); }
    inline uintptr_t m_iClip1()          { return OffsetManager::Instance().GetOffset("m_iClip1", Fallback::m_iClip1); }
    inline uintptr_t m_iClip2()          { return OffsetManager::Instance().GetOffset("m_iClip2", Fallback::m_iClip2); }
    
    inline uintptr_t m_nFallbackPaintKit() { return OffsetManager::Instance().GetOffset("m_nFallbackPaintKit", Fallback::m_nFallbackPaintKit); }
    inline uintptr_t m_nFallbackSeed()     { return OffsetManager::Instance().GetOffset("m_nFallbackSeed", Fallback::m_nFallbackSeed); }
    inline uintptr_t m_flFallbackWear()    { return OffsetManager::Instance().GetOffset("m_flFallbackWear", Fallback::m_flFallbackWear); }
    inline uintptr_t m_nFallbackStatTrak() { return OffsetManager::Instance().GetOffset("m_nFallbackStatTrak", Fallback::m_nFallbackStatTrak); }
    inline uintptr_t m_iItemDefinitionIndex() { return OffsetManager::Instance().GetOffset("m_iItemDefinitionIndex", Fallback::m_iItemDefinitionIndex); }
    inline uintptr_t m_szCustomName()      { return OffsetManager::Instance().GetOffset("m_szCustomName", Fallback::m_szCustomName); }
    
    inline uintptr_t m_modelState() { return OffsetManager::Instance().GetOffset("m_modelState", Fallback::m_modelState); }
    inline uintptr_t m_boneArray() { return OffsetManager::Instance().GetOffset("m_boneArray", Fallback::m_boneArray); }
    inline uintptr_t m_nHitboxSet() { return OffsetManager::Instance().GetOffset("m_nHitboxSet", Fallback::m_nHitboxSet); }
    
    // Glow offsets
    inline uintptr_t m_Glow() { return OffsetManager::Instance().GetOffset("m_Glow", Fallback::m_Glow); }
    inline uintptr_t m_fGlowColor() { return OffsetManager::Instance().GetOffset("m_fGlowColor", Fallback::m_fGlowColor); }
    inline uintptr_t m_iGlowType() { return OffsetManager::Instance().GetOffset("m_iGlowType", Fallback::m_iGlowType); }
    inline uintptr_t m_iGlowTeam() { return OffsetManager::Instance().GetOffset("m_iGlowTeam", Fallback::m_iGlowTeam); }
    inline uintptr_t m_nGlowRange() { return OffsetManager::Instance().GetOffset("m_nGlowRange", Fallback::m_nGlowRange); }
    inline uintptr_t m_nGlowRangeMin() { return OffsetManager::Instance().GetOffset("m_nGlowRangeMin", Fallback::m_nGlowRangeMin); }
    inline uintptr_t m_glowColorOverride() { return OffsetManager::Instance().GetOffset("m_glowColorOverride", Fallback::m_glowColorOverride); }
    inline uintptr_t m_bFlashing() { return OffsetManager::Instance().GetOffset("m_bFlashing", Fallback::m_bFlashing); }
    inline uintptr_t m_flGlowTime() { return OffsetManager::Instance().GetOffset("m_flGlowTime", Fallback::m_flGlowTime); }
    inline uintptr_t m_flGlowStartTime() { return OffsetManager::Instance().GetOffset("m_flGlowStartTime", Fallback::m_flGlowStartTime); }
    inline uintptr_t m_bEligibleForScreenHighlight() { return OffsetManager::Instance().GetOffset("m_bEligibleForScreenHighlight", Fallback::m_bEligibleForScreenHighlight); }
    inline uintptr_t m_bGlowing() { return OffsetManager::Instance().GetOffset("m_bGlowing", Fallback::m_bGlowing); }
    
    // Flags
    constexpr int FL_ONGROUND = (1 << 0);
    constexpr int FL_DUCKING  = (1 << 1);
}

// --- C_BaseEntity ---
class C_BaseEntity
{
public:
    // Safe validation method
    bool IsValid()
    {
        if (!this) return false;
        
        __try
        {
            int health = GetHealth();
            int team = GetTeamNum();
            
            // Basic sanity checks
            if (health < 0 || health > 100) return false;  // Max health in CS2 is 100
            if (team < 0 || team > 10) return false;
            
            return true;
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
            return false;
        }
    }
    
    int  GetHealth()  { return *(int*) ((uintptr_t)this + Offsets::m_iHealth());  }
    int  GetTeamNum() { return *(int*) ((uintptr_t)this + Offsets::m_iTeamNum()); }
    bool IsDormant()  { return *(bool*)((uintptr_t)this + Offsets::m_bDormant()); }
    int  GetFlags()   { return *(int*) ((uintptr_t)this + Offsets::m_fFlags());   }
    bool IsSpotted()  { return *(bool*)((uintptr_t)this + Offsets::m_bSpotted()); }
    
    void SetSpotted(bool spotted) { *(bool*)((uintptr_t)this + Offsets::m_bSpotted()) = spotted; }
};

// --- C_CSPlayerPawn ---
class C_CSPlayerPawn : public C_BaseEntity
{
public:
    // Safe origin retrieval with null checks
    bool GetOriginSafe(Vector3& out)
    {
        if (!this) return false;
        
        __try
        {
            uintptr_t sceneNode = *(uintptr_t*)((uintptr_t)this + Offsets::m_pGameSceneNode());
            if (!sceneNode) return false;
            
            out = *(Vector3*)(sceneNode + Offsets::m_vecAbsOrigin());
            return true;
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
            return false;
        }
    }
    
    Vector3 GetOrigin()
    {
        uintptr_t sceneNode = *(uintptr_t*)((uintptr_t)this + Offsets::m_pGameSceneNode());
        if (!sceneNode) return {};
        return *(Vector3*)(sceneNode + Offsets::m_vecAbsOrigin());
    }
    
    Vector3 GetEyePosition()
    {
        Vector3 origin = GetOrigin();
        Vector3 viewOffset = *(Vector3*)((uintptr_t)this + Offsets::m_vecViewOffset());
        return {origin.x + viewOffset.x, origin.y + viewOffset.y, origin.z + viewOffset.z};
    }
    
    Vector3 GetVelocity()
    {
        return *(Vector3*)((uintptr_t)this + Offsets::m_vecVelocity());
    }
    
    bool IsOnGround()
    {
        return (GetFlags() & Offsets::FL_ONGROUND) != 0;
    }
    
    bool IsScoped()
    {
        return *(bool*)((uintptr_t)this + Offsets::m_bIsScoped());
    }
    
    float GetFlashDuration()
    {
        return *(float*)((uintptr_t)this + Offsets::m_flFlashDuration());
    }
    
    uint32_t GetActiveWeaponHandle()
    {
        __try
        {
            return *(uint32_t*)((uintptr_t)this + Offsets::m_hActiveWeapon());
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
            return 0;
        }
    }
    
    // Advanced HvH methods
    float GetLowerBodyYawTarget()
    {
        return *(float*)((uintptr_t)this + Offsets::m_flLowerBodyYawTarget());
    }
    
    float GetFootYaw()
    {
        return *(float*)((uintptr_t)this + Offsets::m_flFootYaw());
    }
    
    float GetPoseParameter(int index = 0)
    {
        // m_flPoseParameter is an array, index 0-24
        return *(float*)((uintptr_t)this + Offsets::m_flPoseParameter() + (index * sizeof(float)));
    }
    
    int GetMostRecentModelBone()
    {
        return *(int*)((uintptr_t)this + Offsets::m_iMostRecentModelBone());
    }
    
    int GetShotsFired()
    {
        return *(int*)((uintptr_t)this + Offsets::m_iShotsFired());
    }
    
    Vector3 GetAimPunchAngle()
    {
        return *(Vector3*)((uintptr_t)this + Offsets::m_aimPunchAngle());
    }
    
    Vector3 GetAimPunchAngleVel()
    {
        return *(Vector3*)((uintptr_t)this + Offsets::m_aimPunchAngleVel());
    }
    
    int GetArmorValue()
    {
        return *(int*)((uintptr_t)this + Offsets::m_ArmorValue());
    }
    
    bool HasDefuser()
    {
        return *(bool*)((uintptr_t)this + Offsets::m_bHasDefuser());
    }
    
    bool HasHelmet()
    {
        return *(bool*)((uintptr_t)this + Offsets::m_bHasHelmet());
    }
    
    bool IsDefusing()
    {
        return *(bool*)((uintptr_t)this + Offsets::m_bIsDefusing());
    }
    
    bool IsWalking()
    {
        return *(bool*)((uintptr_t)this + Offsets::m_bIsWalking());
    }
    
    int GetMoney()
    {
        return *(int*)((uintptr_t)this + Offsets::m_iAccount());
    }
    
    float GetDuckAmount()
    {
        return *(float*)((uintptr_t)this + Offsets::m_flDuckAmount());
    }
    
    bool IsDucked()
    {
        return *(bool*)((uintptr_t)this + Offsets::m_bDucked());
    }
    
    bool IsDucking()
    {
        return *(bool*)((uintptr_t)this + Offsets::m_bDucking());
    }
    
    int GetTickBase()
    {
        return *(int*)((uintptr_t)this + Offsets::m_nTickBase());
    }
    
    float GetSimulationTime()
    {
        return *(float*)((uintptr_t)this + Offsets::m_flSimulationTime());
    }
    
    float GetOldSimulationTime()
    {
        return *(float*)((uintptr_t)this + Offsets::m_flOldSimulationTime());
    }
    
    // Get bone position by bone ID
    Vector3 GetBonePosition(int boneId)
    {
        // Get game scene node
        uintptr_t sceneNode = *(uintptr_t*)((uintptr_t)this + Offsets::m_pGameSceneNode());
        if (!sceneNode) return {};
        
        // Get model state from scene node
        uintptr_t modelState = sceneNode + Offsets::m_modelState();
        if (!modelState) return {};
        
        // Get bone array from model state
        uintptr_t boneArray = *(uintptr_t*)(modelState + Offsets::m_boneArray());
        if (!boneArray) return {};
        
        // Each bone is a 3x4 matrix (32 bytes)
        // Position is stored in the last column (indices 3, 7, 11)
        uintptr_t boneMatrix = boneArray + (boneId * 32);
        
        Vector3 bonePos;
        bonePos.x = *(float*)(boneMatrix + 12); // matrix[0][3]
        bonePos.y = *(float*)(boneMatrix + 28); // matrix[1][3]
        bonePos.z = *(float*)(boneMatrix + 44); // matrix[2][3]
        
        return bonePos;
    }
    
    // Glow methods
    void SetGlow(bool enabled, Color color, int glowType = 3, int range = 0, int rangeMin = 0)
    {
        __try
        {
            // Get glow property address
            uintptr_t glowProperty = (uintptr_t)this + Offsets::m_Glow();
            if (!glowProperty) return;
            
            // Set glow enabled
            *(bool*)(glowProperty + Offsets::m_bGlowing()) = enabled;
            
            if (enabled)
            {
                // Set glow type (3 = full glow)
                *(int*)(glowProperty + Offsets::m_iGlowType()) = glowType;
                
                // Set glow color (RGB as Vector3)
                Vector3* glowColor = (Vector3*)(glowProperty + Offsets::m_fGlowColor());
                glowColor->x = color.r / 255.0f;
                glowColor->y = color.g / 255.0f;
                glowColor->z = color.b / 255.0f;
                
                // Set glow color override (RGBA)
                *(Color*)(glowProperty + Offsets::m_glowColorOverride()) = color;
                
                // Set glow range
                *(int*)(glowProperty + Offsets::m_nGlowRange()) = range;
                *(int*)(glowProperty + Offsets::m_nGlowRangeMin()) = rangeMin;
                
                // Enable screen highlight
                *(bool*)(glowProperty + Offsets::m_bEligibleForScreenHighlight()) = true;
            }
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
            // Silently fail
        }
    }
    
    void SetGlowTeamBased(int localTeam, Color enemyColor, Color teamColor)
    {
        int entityTeam = GetTeamNum();
        Color glowColor = (entityTeam == localTeam) ? teamColor : enemyColor;
        SetGlow(true, glowColor);
    }
};

// --- C_CSPlayerController ---
class C_CSPlayerController
{
public:
    uint32_t GetPawnHandle() { return *(uint32_t*)((uintptr_t)this + Offsets::m_hPlayerPawn()); }
    
    const char* GetPlayerName()
    {
        __try
        {
            return (const char*)((uintptr_t)this + Offsets::m_iszPlayerName());
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
            return "Unknown";
        }
    }
};

// --- CGlobalVarsBase ---
class CGlobalVarsBase
{
public:
    float realtime;
    int   framecount;
    float absoluteframetime;
    float absoluteframestarttimestddev;
    float curtime;
    float frametime;
    int   maxClients;
    int   tickcount;
    float interval_per_tick;
};

// --- C_CSGameEntitySystem ---
class C_CSGameEntitySystem
{
public:
    // Obtener el índice máximo de entidades
    int GetHighestEntityIndex()
    {
        return *(int*)((uintptr_t)this + Offsets::dwGameEntitySystem_getHighestEntityIndex());
    }
    
    // Decode entity handle to get actual pointer
    // CS2 uses handles instead of direct pointers
    C_BaseEntity* DecodeHandle(uint32_t handle)
    {
        if (handle == 0 || handle == 0xFFFFFFFF) return nullptr;
        
        __try
        {
            uintptr_t entitySystem = (uintptr_t)this;
            
            // Use same method as GetBaseEntity
            int index = handle & 0x7FFF;  // Extract index from handle
            
            uintptr_t listEntry = *(uintptr_t*)(entitySystem + 8 * ((index & 0x7FFF) >> 9) + 16);
            if (!listEntry || listEntry < 0x1000) return nullptr;
            
            uintptr_t entityPtr = *(uintptr_t*)(listEntry + 120 * (index & 0x1FF));
            if (!entityPtr || entityPtr < 0x1000) return nullptr;
            
            return (C_BaseEntity*)entityPtr;
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
            return nullptr;
        }
    }
    
    // Get entity by index using CORRECT CS2 method
    // Based on working code from UnknownCheats
    C_BaseEntity* GetBaseEntity(int index)
    {
        if (index < 0 || index > 8192) return nullptr;
        
        __try
        {
            uintptr_t entitySystem = (uintptr_t)this;
            
            // CORRECT METHOD: Read list entry directly
            // Formula: entitySystem + 8 * ((index & 0x7FFF) >> 9) + 16
            uintptr_t listEntry = *(uintptr_t*)(entitySystem + 8 * ((index & 0x7FFF) >> 9) + 16);
            if (!listEntry || listEntry < 0x1000) return nullptr;
            
            // Read entity from list entry
            // Formula: listEntry + 120 * (index & 0x1FF)
            uintptr_t entityPtr = *(uintptr_t*)(listEntry + 120 * (index & 0x1FF));
            if (!entityPtr || entityPtr < 0x1000) return nullptr;
            
            return (C_BaseEntity*)entityPtr;
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
            return nullptr;
        }
    }
    
    // Get player pawn from controller (CORRECT METHOD for CS2)
    // Controllers and Pawns are separate - must decode handle
    C_CSPlayerPawn* GetPlayerPawn(C_BaseEntity* controller)
    {
        if (!controller) return nullptr;
        
        __try
        {
            // Read pawn handle from controller
            uint32_t pawnHandle = *(uint32_t*)((uintptr_t)controller + Offsets::m_hPlayerPawn());
            if (pawnHandle == 0 || pawnHandle == 0xFFFFFFFF) return nullptr;
            
            // Decode handle to get actual pawn pointer
            return (C_CSPlayerPawn*)DecodeHandle(pawnHandle);
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
            return nullptr;
        }
    }
};

// --- IVEngineClient ---
class IVEngineClient
{
public:
    void GetScreenSize(int& width, int& height)
    {
        width  = 1920;
        height = 1080;
    }
};

// --- Punteros Globales ---
extern CGlobalVarsBase*      g_pGlobalVars;
extern C_CSGameEntitySystem* g_pEntitySystem;
extern IVEngineClient*       g_pEngineClient;
extern ViewMatrix*           g_pViewMatrix;