#include "Hooks.h"
#include "ImGui_Renderer.h"
#include <iostream>
#include <MinHook.h>

// Puntero a la función original de Present
Hooks::Present_t Hooks::oPresent = nullptr;

// Puntero al dispositivo D3D11 y al contexto
ID3D11Device* g_pd3d11Device = nullptr;
ID3D11DeviceContext* g_pd3d11DeviceContext = nullptr;
ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

// Función de hook para Present
HRESULT __stdcall Hooks::hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
    if (!g_pd3d11Device)
    {
        // Inicializar D3D11 y ImGui la primera vez que se llama a Present
        pSwapChain->GetDevice(__uuidof(g_pd3d11Device), reinterpret_cast<void**>(&g_pd3d11Device));
        g_pd3d11Device->GetImmediateContext(&g_pd3d11DeviceContext);

        // Obtener el render target view
        ID3D11Texture2D* pBackBuffer;
        pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));
        g_pd3d11Device->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
        pBackBuffer->Release();

        // Inicializar ImGui con el dispositivo D3D11
        ImGui_Renderer::InitImGui(g_pd3d11Device, g_pd3d11DeviceContext);
    }

    // Renderizar ImGui
    ImGui_Renderer::Render(g_pd3d11DeviceContext, g_mainRenderTargetView);

    // Llamar a la función original de Present
    return oPresent(pSwapChain, SyncInterval, Flags);
}

// Inicializar los hooks
void Hooks::Initialize()
{
    std::cout << "[CS2 Edu-Tool] Inicializando hooks..." << std::endl;

    // Crear un dispositivo dummy para obtener la tabla de funciones virtuales (VTable)
    D3D_FEATURE_LEVEL featureLevel;
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = GetForegroundWindow(); // Usar la ventana activa
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    IDXGISwapChain* pSwapChain = nullptr;
    ID3D11Device* pDevice = nullptr;
    ID3D11DeviceContext* pContext = nullptr;

    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, NULL, 0, D3D11_SDK_VERSION, &sd, &pSwapChain, &pDevice, &featureLevel, &pContext) == S_OK)
    {
        void** pVTable = *reinterpret_cast<void***>(pSwapChain);
        oPresent = reinterpret_cast<Present_t>(pVTable[8]); // Present es el índice 8 en la VTable de IDXGISwapChain

        // Inicializar MinHook
        if (MH_Initialize() != MH_OK)
        {
            std::cerr << "[CS2 Edu-Tool] Error: No se pudo inicializar MinHook." << std::endl;
            return;
        }

        // Crear el hook para Present
        if (MH_CreateHook(oPresent, &hkPresent, reinterpret_cast<LPVOID*>(&oPresent)) != MH_OK)
        {
            std::cerr << "[CS2 Edu-Tool] Error: No se pudo crear el hook para Present." << std::endl;
            return;
        }

        // Habilitar el hook
        if (MH_EnableHook(oPresent) != MH_OK)
        {
            std::cerr << "[CS2 Edu-Tool] Error: No se pudo habilitar el hook para Present." << std::endl;
            return;
        }

        std::cout << "[CS2 Edu-Tool] Hook de Present creado y habilitado." << std::endl;

        // Liberar recursos del dispositivo dummy
        pDevice->Release();
        pContext->Release();
        pSwapChain->Release();
    }
    else
    {
        std::cerr << "[CS2 Edu-Tool] Error: No se pudo crear el dispositivo D3D11 dummy para hooking." << std::endl;
    }
}

// Desinicializar los hooks
void Hooks::Shutdown()
{
    std::cout << "[CS2 Edu-Tool] Deshabilitando y eliminando hooks..." << std::endl;

    if (MH_DisableHook(oPresent) != MH_OK)
    {
        std::cerr << "[CS2 Edu-Tool] Error: No se pudo deshabilitar el hook para Present." << std::endl;
    }
    if (MH_RemoveHook(oPresent) != MH_OK)
    {
        std::cerr << "[CS2 Edu-Tool] Error: No se pudo eliminar el hook para Present." << std::endl;
    }
    if (MH_Uninitialize() != MH_OK)
    {
        std::cerr << "[CS2 Edu-Tool] Error: No se pudo desinicializar MinHook." << std::endl;
    }

    // Liberar recursos de D3D11 y ImGui
    ImGui_Renderer::ShutdownImGui();
    if (g_mainRenderTargetView) g_mainRenderTargetView->Release();
    if (g_pd3d11DeviceContext) g_pd3d11DeviceContext->Release();
    if (g_pd3d11Device) g_pd3d11Device->Release();

    std::cout << "[CS2 Edu-Tool] Hooks deshabilitados y recursos liberados." << std::endl;
}
