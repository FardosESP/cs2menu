#pragma once

#include <cstdint>

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
// Actualiza con https://github.com/a2x/cs2-dumper tras cada update de CS2
namespace Offsets
{
    constexpr uintptr_t dwEntityList            = 0x18C39F8;
    constexpr uintptr_t dwLocalPlayerPawn       = 0x17617D0;
    constexpr uintptr_t dwViewMatrix            = 0x18A7750;
    constexpr uintptr_t dwLocalPlayerController = 0x1756D18;

    constexpr uintptr_t m_iHealth        = 0x32C;
    constexpr uintptr_t m_iTeamNum       = 0x3B8;
    constexpr uintptr_t m_pGameSceneNode = 0x310;
    constexpr uintptr_t m_vecAbsOrigin   = 0xC8;
    constexpr uintptr_t m_hPlayerPawn    = 0x604;
}

// --- C_BaseEntity ---
class C_BaseEntity
{
public:
    int  GetHealth()  { return *(int*) ((uintptr_t)this + Offsets::m_iHealth);  }
    int  GetTeamNum() { return *(int*) ((uintptr_t)this + Offsets::m_iTeamNum); }
    bool IsDormant()  { return *(bool*)((uintptr_t)this + 0xE8); }
};

// --- C_CSPlayerPawn ---
class C_CSPlayerPawn : public C_BaseEntity
{
public:
    Vector3 GetOrigin()
    {
        uintptr_t sceneNode = *(uintptr_t*)((uintptr_t)this + Offsets::m_pGameSceneNode);
        if (!sceneNode) return {};
        return *(Vector3*)(sceneNode + Offsets::m_vecAbsOrigin);
    }
};

// --- C_CSPlayerController ---
class C_CSPlayerController
{
public:
    uint32_t GetPawnHandle() { return *(uint32_t*)((uintptr_t)this + Offsets::m_hPlayerPawn); }
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
    C_BaseEntity* GetBaseEntity(int index)
    {
        uintptr_t entityList = *(uintptr_t*)((uintptr_t)this + 0x10);
        if (!entityList) return nullptr;

        uintptr_t listEntry = *(uintptr_t*)(entityList + (8ull * (index >> 9)) + 16);
        if (!listEntry) return nullptr;

        return (C_BaseEntity*)(listEntry + 120ull * (index & 0x1FF));
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