#pragma once
#include "SDK.h"
#include "Memory.h"
#include <Windows.h>

class CLocalPlayer
{
private:
    uintptr_t m_clientBase;
    C_CSGameEntitySystem* m_pEntitySystem;
    C_CSPlayerPawn* m_pLocalPawn;
    C_BaseEntity* m_pLocalController;
    bool m_bScannerRan;
    uintptr_t m_lastPawnAddress;
    int m_framesSinceLastUpdate;
    
    template<typename T>
    inline bool SafeRead(uintptr_t address, T& out)
    {
        if (!address || address < 0x10000 || address > 0x7FFFFFFFFFFF)
            return false;
        if (IsBadReadPtr((void*)address, sizeof(T)))
            return false;
        __try {
            out = *(T*)address;
            return true;
        }
        __except(EXCEPTION_EXECUTE_HANDLER) {
            return false;
        }
    }
    
    inline bool IsValidPawnPointer(uintptr_t pawnPtr)
    {
        if (!pawnPtr || pawnPtr < 0x10000 || pawnPtr > 0x7FFFFFFFFFFF)
            return false;
        if ((pawnPtr & 0xFFFFFFFF00000000) == 0x2000000000000000 ||
            (pawnPtr & 0xFFFFFFFF00000000) == 0x1000000000000000)
            return false;
        if (IsBadReadPtr((void*)pawnPtr, 0x200))
            return false;
        int health = 0;
        if (!SafeRead(pawnPtr + Offsets::m_iHealth(), health))
            return false;
        if (health <= 0 || health > 100)
            return false;
        return true;
    }
    
public:
    CLocalPlayer() : m_clientBase(0), m_pEntitySystem(nullptr), 
                     m_pLocalPawn(nullptr), m_pLocalController(nullptr),
                     m_bScannerRan(false), m_lastPawnAddress(0),
                     m_framesSinceLastUpdate(0) {}
    
    void Initialize(uintptr_t clientBase, C_CSGameEntitySystem* entitySystem)
    {
        m_clientBase = clientBase;
        m_pEntitySystem = entitySystem;
    }
    
    void Update()
    {
        if (!m_clientBase) return;
        m_framesSinceLastUpdate++;
        if (m_pLocalPawn && m_framesSinceLastUpdate < 10)
        {
            if (IsValidPawnPointer((uintptr_t)m_pLocalPawn))
                return;
        }
        m_framesSinceLastUpdate = 0;
        m_pLocalPawn = nullptr;
        m_pLocalController = nullptr;
        __try {
            uintptr_t localPawnAddr = m_clientBase + Offsets::dwLocalPlayerPawn();
            uintptr_t pawnPtr = 0;
            if (!SafeRead(localPawnAddr, pawnPtr))
                return;
            if (IsValidPawnPointer(pawnPtr))
            {
                m_pLocalPawn = (C_CSPlayerPawn*)pawnPtr;
                m_lastPawnAddress = pawnPtr;
                return;
            }
        }
        __except(EXCEPTION_EXECUTE_HANDLER) {}
        m_pLocalPawn = nullptr;
        m_pLocalController = nullptr;
        m_lastPawnAddress = 0;
    }
    
    C_CSPlayerPawn* GetPawn() { return m_pLocalPawn; }
    C_BaseEntity* GetController() { return m_pLocalController; }
    bool HasScannerRun() { return m_bScannerRan; }
    void SetScannerRan(bool ran) { m_bScannerRan = ran; }
    
    C_CSPlayerPawn* GetSafeLocalPlayer()
    {
        Update();
        return m_pLocalPawn;
    }
    
    bool GetLocalPlayerHealth(int& outHealth)
    {
        if (!m_pLocalPawn) return false;
        return SafeRead((uintptr_t)m_pLocalPawn + Offsets::m_iHealth(), outHealth);
    }
    
    bool GetLocalPlayerTeam(int& outTeam)
    {
        if (!m_pLocalPawn) return false;
        return SafeRead((uintptr_t)m_pLocalPawn + Offsets::m_iTeamNum(), outTeam);
    }
};

extern CLocalPlayer g_LocalPlayer;
