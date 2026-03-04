#pragma once

#include <d3d11.h>
#include <wrl/client.h>

namespace Hooks
{
    // Puntero a la función original de Present
    typedef HRESULT(__stdcall* Present_t)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
    extern Present_t oPresent;

    // Función de hook para Present
    HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);

    // Inicializar y desinicializar los hooks
    void Initialize();
    void Shutdown();
}
