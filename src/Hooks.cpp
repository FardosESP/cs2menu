#include <Windows.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <d3d11.h>
#include "Hooks.h"
#include "ImGui_Renderer.h"

Hooks::Present_t Hooks::oPresent = nullptr;

ID3D11Device*           g_pd3d11Device         = nullptr;
ID3D11DeviceContext*    g_pd3d11DeviceContext   = nullptr;
ID3D11RenderTargetView* g_mainRenderTargetView  = nullptr;
static bool             g_bImGuiInitialized     = false;

// Guardamos los bytes originales de Present para restaurarlos al llamar oPresent
static BYTE g_originalBytes[14] = {};

// Llama a la función original restaurando temporalmente los bytes
HRESULT CallOriginalPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
    // 1. Restaurar bytes originales
    DWORD oldProtect;
    VirtualProtect((void*)Hooks::oPresent, 14, PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy((void*)Hooks::oPresent, g_originalBytes, 14);
    VirtualProtect((void*)Hooks::oPresent, 14, oldProtect, &oldProtect);

    // 2. Llamar la función real
    HRESULT hr = Hooks::oPresent(pSwapChain, SyncInterval, Flags);

    // 3. Reaponer nuestro hook
    BYTE jmp64[14] = {
        0xFF, 0x25, 0x00, 0x00, 0x00, 0x00,             // JMP QWORD PTR [RIP+0]
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  // dirección absoluta de 64 bits
    };
    *reinterpret_cast<uintptr_t*>(&jmp64[6]) = reinterpret_cast<uintptr_t>(&Hooks::hkPresent);

    VirtualProtect((void*)Hooks::oPresent, 14, PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy((void*)Hooks::oPresent, jmp64, 14);
    VirtualProtect((void*)Hooks::oPresent, 14, oldProtect, &oldProtect);

    return hr;
}

HRESULT __stdcall Hooks::hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
    if (!g_bImGuiInitialized)
    {
        if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&g_pd3d11Device)))
        {
            g_pd3d11Device->GetImmediateContext(&g_pd3d11DeviceContext);
            ID3D11Texture2D* pBackBuffer = nullptr;
            if (SUCCEEDED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer)))
            {
                g_pd3d11Device->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
                pBackBuffer->Release();
                ImGui_Renderer::InitImGui(g_pd3d11Device, g_pd3d11DeviceContext);
                g_bImGuiInitialized = true;
                std::cout << "[+] ImGui inicializado en hkPresent." << std::endl;
            }
        }
    }

    if (g_bImGuiInitialized && g_pd3d11DeviceContext && g_mainRenderTargetView)
        ImGui_Renderer::Render(g_pd3d11DeviceContext, g_mainRenderTargetView);

    return CallOriginalPresent(pSwapChain, SyncInterval, Flags);
}

void Hooks::Initialize()
{
    // Buscar ventana de CS2
    HWND hWnd = nullptr;
    for (int i = 0; i < 20 && !hWnd; i++)
    {
        hWnd = FindWindowA("SDL_app", nullptr);
        if (!hWnd) hWnd = FindWindowA(nullptr, "Counter-Strike 2");
        if (!hWnd) std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    if (!hWnd)
    {
        std::cout << "[-] No se encontro la ventana de CS2." << std::endl;
        return;
    }
    std::cout << "[*] Ventana de CS2 encontrada: " << hWnd << std::endl;

    // Crear SwapChain dummy para obtener la dirección de Present
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    DXGI_SWAP_CHAIN_DESC sd        = {};
    sd.BufferCount                 = 1;
    sd.BufferDesc.Format           = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.Width            = 1;
    sd.BufferDesc.Height           = 1;
    sd.BufferUsage                 = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow                = hWnd;
    sd.SampleDesc.Count            = 1;
    sd.Windowed                    = TRUE;
    sd.SwapEffect                  = DXGI_SWAP_EFFECT_DISCARD;

    IDXGISwapChain*      pDummySwapChain = nullptr;
    ID3D11Device*        pDummyDevice    = nullptr;
    ID3D11DeviceContext* pDummyContext   = nullptr;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0,
        &featureLevel, 1, D3D11_SDK_VERSION,
        &sd, &pDummySwapChain, &pDummyDevice, nullptr, &pDummyContext
    );

    if (FAILED(hr))
    {
        std::cout << "[-] D3D11CreateDeviceAndSwapChain fallo: 0x" << std::hex << hr << std::endl;
        return;
    }

    // Obtener dirección de Present desde la VTable (índice 8)
    void** pVTable = *reinterpret_cast<void***>(pDummySwapChain);
    oPresent = reinterpret_cast<Present_t>(pVTable[8]);
    std::cout << "[*] Direccion de Present: " << (void*)oPresent << std::endl;

    pDummySwapChain->Release();
    pDummyDevice->Release();
    pDummyContext->Release();

    // Guardar los 14 bytes originales antes de parchear
    memcpy(g_originalBytes, (void*)oPresent, 14);

    // Hook de 64 bits: JMP absoluto con dirección de 64 bits (14 bytes)
    // FF 25 00000000  →  JMP QWORD PTR [RIP+0]
    // seguido de la dirección de 8 bytes de nuestra función
    BYTE jmp64[14] = {
        0xFF, 0x25, 0x00, 0x00, 0x00, 0x00,             // JMP QWORD PTR [RIP+0]
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  // dirección absoluta 64-bit
    };
    *reinterpret_cast<uintptr_t*>(&jmp64[6]) = reinterpret_cast<uintptr_t>(&hkPresent);

    DWORD oldProtect;
    VirtualProtect((void*)oPresent, 14, PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy((void*)oPresent, jmp64, 14);
    VirtualProtect((void*)oPresent, 14, oldProtect, &oldProtect);

    std::cout << "[+] Hook de 64 bits aplicado correctamente." << std::endl;
}

void Hooks::Shutdown()
{
    // Restaurar bytes originales al desinyectar
    if (oPresent)
    {
        DWORD oldProtect;
        VirtualProtect((void*)oPresent, 14, PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy((void*)oPresent, g_originalBytes, 14);
        VirtualProtect((void*)oPresent, 14, oldProtect, &oldProtect);
        std::cout << "[*] Hook restaurado correctamente." << std::endl;
    }

    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
    if (g_pd3d11DeviceContext)  { g_pd3d11DeviceContext->Release();  g_pd3d11DeviceContext  = nullptr; }
    if (g_pd3d11Device)         { g_pd3d11Device->Release();         g_pd3d11Device         = nullptr; }
}