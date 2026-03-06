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
    // Hardcoded fallback values (Build Feb 24, 2026)
    namespace Fallback
    {
        constexpr uintptr_t dwEntityList            = 0x21CD670;  // 35422320 decimal
        constexpr uintptr_t dwLocalPlayerPawn       = 0x2066DF0;  // 33970928 decimal  
        constexpr uintptr_t dwViewMatrix            = 0x2309460;  // 36749024 decimal
        constexpr uintptr_t dwLocalPlayerController = 0x22F1888;
        constexpr uintptr_t dwViewAngles            = 0x2318668;
        constexpr uintptr_t dwGlobalVars            = 0x2058FC0;
        constexpr uintptr_t dwGameRules             = 0x230A160;
        constexpr uintptr_t dwGameEntitySystem_getHighestEntityIndex = 0x20A0;
        constexpr uintptr_t dwForceJump             = 0x230A1E0;
        
        constexpr uintptr_t m_iHealth        = 0x76C;
        constexpr uintptr_t m_iMaxHealth     = 0xB54;
        constexpr uintptr_t m_iTeamNum       = 0xD70;
        constexpr uintptr_t m_pGameSceneNode = 0x598;  // Updated from scanner
        constexpr uintptr_t m_vecAbsOrigin   = 0xC4;   // Updated from scanner (offset within GameSceneNode)
        constexpr uintptr_t m_hPlayerPawn    = 0x90C;
        constexpr uintptr_t m_iIDEntIndex    = 0x3EAC;
        constexpr uintptr_t m_bDormant       = 0xE8;
        
        constexpr uintptr_t m_fFlags         = 0x400;
        constexpr uintptr_t m_vecVelocity    = 0x438;
        constexpr uintptr_t m_angEyeAngles   = 0x3DD0;
        constexpr uintptr_t m_vecViewOffset  = 0x790;  // Updated from scanner
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
        
        // Model and bone offsets
        constexpr uintptr_t m_modelState = 0x160; // 352 in decimal
        constexpr uintptr_t m_boneArray = 0x80;   // Offset within CModelState
        
        constexpr uintptr_t m_pClippingWeapon = 0x1290;
        constexpr uintptr_t m_iClip1          = 0x1564;
        constexpr uintptr_t m_iClip2          = 0x1568;
        constexpr uintptr_t m_nFallbackPaintKit = 0x31B8;
        constexpr uintptr_t m_nFallbackSeed     = 0x31BC;
        constexpr uintptr_t m_flFallbackWear    = 0x31C0;
        
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
    
    inline uintptr_t m_modelState() { return OffsetManager::Instance().GetOffset("m_modelState", Fallback::m_modelState); }
    inline uintptr_t m_boneArray() { return OffsetManager::Instance().GetOffset("m_boneArray", Fallback::m_boneArray); }
    
    inline uintptr_t m_pClippingWeapon() { return OffsetManager::Instance().GetOffset("m_pClippingWeapon", Fallback::m_pClippingWeapon); }
    inline uintptr_t m_iClip1()          { return OffsetManager::Instance().GetOffset("m_iClip1", Fallback::m_iClip1); }
    inline uintptr_t m_iClip2()          { return OffsetManager::Instance().GetOffset("m_iClip2", Fallback::m_iClip2); }
    inline uintptr_t m_nFallbackPaintKit() { return OffsetManager::Instance().GetOffset("m_nFallbackPaintKit", Fallback::m_nFallbackPaintKit); }
    inline uintptr_t m_nFallbackSeed()     { return OffsetManager::Instance().GetOffset("m_nFallbackSeed", Fallback::m_nFallbackSeed); }
    inline uintptr_t m_flFallbackWear()    { return OffsetManager::Instance().GetOffset("m_flFallbackWear", Fallback::m_flFallbackWear); }
    
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
            
            // Basic sanity checks - más permisivo
            if (health < 0 || health > 1000) return false;  // Aumentado a 1000
            if (team < 0 || team > 10) return false;  // Aumentado a 10
            
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
    // Handle format: [chunk_index:15][index_in_chunk:9][serial:8]
    C_BaseEntity* DecodeHandle(uint32_t handle)
    {
        if (handle == 0 || handle == 0xFFFFFFFF) return nullptr;
        
        __try
        {
            uintptr_t entitySystem = (uintptr_t)this;
            
            // Read the entity list pointer at +0x10
            uintptr_t entityListArray = *(uintptr_t*)(entitySystem + 0x10);
            if (!entityListArray || entityListArray < 0x1000) return nullptr;
            
            // Extract chunk index and index within chunk from handle
            // Handle bits: [15 bits chunk][9 bits index][8 bits serial]
            int chunkIndex = (handle & 0x7FFF) >> 9;  // Bits 9-23
            int indexInChunk = handle & 0x1FF;         // Bits 0-8
            
            // Read chunk pointer from entity list array
            // Array starts at +16 bytes (0x10), each pointer is 8 bytes
            uintptr_t chunkPtr = *(uintptr_t*)(entityListArray + 0x10 + (chunkIndex * 8));
            if (!chunkPtr || chunkPtr < 0x1000) return nullptr;
            
            // Read entity pointer from chunk
            // Each entity entry is 120 bytes (0x78)
            uintptr_t entityPtr = *(uintptr_t*)(chunkPtr + (indexInChunk * 0x78));
            if (!entityPtr || entityPtr < 0x1000) return nullptr;
            
            return (C_BaseEntity*)entityPtr;
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
            return nullptr;
        }
    }
    
    // Get entity by index (for Controllers - indices 1-64)
    // This returns CCSPlayerController, NOT the pawn
    C_BaseEntity* GetBaseEntity(int index)
    {
        if (index < 0 || index > 8192) return nullptr;
        
        __try
        {
            // CGameEntitySystem structure in CS2:
            // +0x00: vtable
            // +0x10: m_pEntityList (pointer to entity list array)
            // The entity list array contains chunk pointers
            
            uintptr_t entitySystem = (uintptr_t)this;
            
            // Read the entity list pointer at +0x10
            // This is the actual array of chunk pointers
            uintptr_t entityListArray = *(uintptr_t*)(entitySystem + 0x10);
            if (!entityListArray || entityListArray < 0x1000) return nullptr;
            
            // Calculate chunk index (each chunk holds 512 entities)
            int chunkIndex = index >> 9;      // Divide by 512
            int indexInChunk = index & 0x1FF; // Modulo 512
            
            // Read chunk pointer from entity list array
            // Array starts at +16 bytes (0x10), each pointer is 8 bytes
            uintptr_t chunkPtr = *(uintptr_t*)(entityListArray + 0x10 + (chunkIndex * 8));
            if (!chunkPtr || chunkPtr < 0x1000) return nullptr;
            
            // Read entity pointer from chunk
            // Each entity entry is 120 bytes (0x78)
            uintptr_t entityPtr = *(uintptr_t*)(chunkPtr + (indexInChunk * 0x78));
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