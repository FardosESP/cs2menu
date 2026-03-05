#pragma once

#include <Windows.h>
#include <cstdint>
#include "OffsetManager.h"

// --- Estructuras Base ---

struct Vector3
{
    float x, y, z;
};

struct ViewMatrix
{
    float matrix[16];
};

// --- Offsets (DEBEN ir ANTES de las clases que los usan) ---
// Dynamic offsets loaded from OffsetManager
namespace Offsets
{
    // Hardcoded fallback values (Build 14138 - 2026-03-05)
    namespace Fallback
    {
        constexpr uintptr_t dwEntityList            = 0x24AE628;
        constexpr uintptr_t dwLocalPlayerPawn       = 0x2066DE0;
        constexpr uintptr_t dwViewMatrix            = 0x2309420;
        constexpr uintptr_t dwLocalPlayerController = 0x22F1888;
        constexpr uintptr_t dwViewAngles            = 0x2318668;
        constexpr uintptr_t dwGlobalVars            = 0x2058FC0;
        constexpr uintptr_t dwGameRules             = 0x230A160;
        constexpr uintptr_t dwGameEntitySystem_getHighestEntityIndex = 0x20A0;
        
        constexpr uintptr_t m_iHealth        = 0x76C;
        constexpr uintptr_t m_iMaxHealth     = 0xB54;
        constexpr uintptr_t m_iTeamNum       = 0xD70;
        constexpr uintptr_t m_pGameSceneNode = 0x338;
        constexpr uintptr_t m_vecAbsOrigin   = 0xD0;
        constexpr uintptr_t m_hPlayerPawn    = 0x90C;
        constexpr uintptr_t m_iIDEntIndex    = 0x3EAC;
        constexpr uintptr_t m_bDormant       = 0xE8;
        
        constexpr uintptr_t m_fFlags         = 0x400;
        constexpr uintptr_t m_vecVelocity    = 0x438;
        constexpr uintptr_t m_angEyeAngles   = 0x3DD0;
        constexpr uintptr_t m_vecViewOffset  = 0xD58;
        constexpr uintptr_t m_aimPunchAngle  = 0x16CC;
        constexpr uintptr_t m_aimPunchCache  = 0x1728;
        constexpr uintptr_t m_iShotsFired    = 0x1414;
        constexpr uintptr_t m_bSpotted       = 0x8;
        constexpr uintptr_t m_bIsScoped      = 0x26F8;
        constexpr uintptr_t m_flFlashDuration = 0x1470;
        
        // Player controller offsets
        constexpr uintptr_t m_iszPlayerName = 0x6F8; // 1784 in decimal
        
        // Weapon offsets
        constexpr uintptr_t m_hActiveWeapon = 0x12F8;
        
        constexpr uintptr_t m_pClippingWeapon = 0x1290;
        constexpr uintptr_t m_iClip1          = 0x1564;
        constexpr uintptr_t m_iClip2          = 0x1568;
        constexpr uintptr_t m_nFallbackPaintKit = 0x31B8;
        constexpr uintptr_t m_nFallbackSeed     = 0x31BC;
        constexpr uintptr_t m_flFallbackWear    = 0x31C0;
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
    
    inline uintptr_t m_iHealth()        { return OffsetManager::Instance().GetOffset("m_iHealth", Fallback::m_iHealth); }
    inline uintptr_t m_iMaxHealth()     { return OffsetManager::Instance().GetOffset("m_iMaxHealth", Fallback::m_iMaxHealth); }
    inline uintptr_t m_iTeamNum()       { return OffsetManager::Instance().GetOffset("m_iTeamNum", Fallback::m_iTeamNum); }
    inline uintptr_t m_pGameSceneNode() { return OffsetManager::Instance().GetOffset("m_pGameSceneNode", Fallback::m_pGameSceneNode); }
    inline uintptr_t m_vecAbsOrigin()   { return OffsetManager::Instance().GetOffset("m_vecAbsOrigin", Fallback::m_vecAbsOrigin); }
    inline uintptr_t m_hPlayerPawn()    { return OffsetManager::Instance().GetOffset("m_hPlayerPawn", Fallback::m_hPlayerPawn); }
    inline uintptr_t m_iIDEntIndex()    { return OffsetManager::Instance().GetOffset("m_iIDEntIndex", Fallback::m_iIDEntIndex); }
    inline uintptr_t m_bDormant()       { return OffsetManager::Instance().GetOffset("m_bDormant", Fallback::m_bDormant); }
    
    inline uintptr_t m_fFlags()         { return OffsetManager::Instance().GetOffset("m_fFlags", Fallback::m_fFlags); }
    inline uintptr_t m_vecVelocity()    { return OffsetManager::Instance().GetOffset("m_vecVelocity", Fallback::m_vecVelocity); }
    inline uintptr_t m_angEyeAngles()   { return OffsetManager::Instance().GetOffset("m_angEyeAngles", Fallback::m_angEyeAngles); }
    inline uintptr_t m_vecViewOffset()  { return OffsetManager::Instance().GetOffset("m_vecViewOffset", Fallback::m_vecViewOffset); }
    inline uintptr_t m_aimPunchAngle()  { return OffsetManager::Instance().GetOffset("m_aimPunchAngle", Fallback::m_aimPunchAngle); }
    inline uintptr_t m_aimPunchCache()  { return OffsetManager::Instance().GetOffset("m_aimPunchCache", Fallback::m_aimPunchCache); }
    inline uintptr_t m_iShotsFired()    { return OffsetManager::Instance().GetOffset("m_iShotsFired", Fallback::m_iShotsFired); }
    inline uintptr_t m_bSpotted()       { return OffsetManager::Instance().GetOffset("m_bSpotted", Fallback::m_bSpotted); }
    inline uintptr_t m_bIsScoped()      { return OffsetManager::Instance().GetOffset("m_bIsScoped", Fallback::m_bIsScoped); }
    inline uintptr_t m_flFlashDuration() { return OffsetManager::Instance().GetOffset("m_flFlashDuration", Fallback::m_flFlashDuration); }
    
    inline uintptr_t m_iszPlayerName() { return OffsetManager::Instance().GetOffset("m_iszPlayerName", Fallback::m_iszPlayerName); }
    inline uintptr_t m_hActiveWeapon() { return OffsetManager::Instance().GetOffset("m_hActiveWeapon", Fallback::m_hActiveWeapon); }
    
    inline uintptr_t m_pClippingWeapon() { return OffsetManager::Instance().GetOffset("m_pClippingWeapon", Fallback::m_pClippingWeapon); }
    inline uintptr_t m_iClip1()          { return OffsetManager::Instance().GetOffset("m_iClip1", Fallback::m_iClip1); }
    inline uintptr_t m_iClip2()          { return OffsetManager::Instance().GetOffset("m_iClip2", Fallback::m_iClip2); }
    inline uintptr_t m_nFallbackPaintKit() { return OffsetManager::Instance().GetOffset("m_nFallbackPaintKit", Fallback::m_nFallbackPaintKit); }
    inline uintptr_t m_nFallbackSeed()     { return OffsetManager::Instance().GetOffset("m_nFallbackSeed", Fallback::m_nFallbackSeed); }
    inline uintptr_t m_flFallbackWear()    { return OffsetManager::Instance().GetOffset("m_flFallbackWear", Fallback::m_flFallbackWear); }
    
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
            if (health < 0 || health > 500) return false;
            if (team < 0 || team > 3) return false;
            
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
    
    // Obtener entidad por índice (método correcto para CS2)
    C_BaseEntity* GetBaseEntity(int index)
    {
        if (index < 0 || index > 8192) return nullptr;
        
        // Fórmula correcta para CS2 (con precedencia de operadores correcta):
        // list_entry = EntityList + (8 * ((index & 0x7FFF) >> 9)) + 16
        // entity = list_entry + (120 * (index & 0x1FF))
        
        uintptr_t entityList = (uintptr_t)this;
        
        int listIndex = (index & 0x7FFF) >> 9;
        uintptr_t listEntry = *(uintptr_t*)(entityList + (8 * listIndex) + 16);
        if (!listEntry) return nullptr;
        
        uintptr_t entityPtr = *(uintptr_t*)(listEntry + (120 * (index & 0x1FF)));
        if (!entityPtr) return nullptr;
        
        return (C_BaseEntity*)entityPtr;
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