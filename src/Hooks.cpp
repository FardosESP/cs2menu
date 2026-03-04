#include <Windows.h>
#include <psapi.h>
#include <iostream>
#include <vector>
#include <d3d11.h>
#include <dxgi1_2.h>

#include "Hooks.h"
#include "ImGui_Renderer.h"

// Puntero a la función original de Present
Hooks::Present_t Hooks::oPresent = nullptr;

// Punteros globales para DirectX 11
ID3D11Device* g_pd3d11Device = nullptr;
ID3D11DeviceContext* g_pd3d11DeviceContext = nullptr;
ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

// Función de búsqueda de patrones en memoria (Pattern Scanning)
uint8_t* FindPattern(const char* moduleName, const char* pattern) {
    HMODULE hModule = GetModuleHandleA(moduleName);
    if (!hModule) return nullptr;

    MODULEINFO modInfo;
    if (!GetModuleInformation(GetCurrentProcess(), hModule, &modInfo, sizeof(MODULEINFO)))
        return nullptr;

    uint8_t* startAddr = (uint8_t*)modInfo.lpBaseOfDll;
    uint8_t* endAddr = startAddr + modInfo.SizeOfImage;

    const char* currentPattern = pattern;
    uint8_t* firstMatch = nullptr;

    for (uint8_t* currentAddr = startAddr; currentAddr < endAddr; currentAddr++) {
        if (!*currentPattern) return firstMatch;

        if (*(uint8_t*)currentPattern == '\?' || *currentAddr == strtol(currentPattern, (char**)&currentPattern, 16)) {
            if (!firstMatch) firstMatch = currentAddr;
            if (!currentPattern[2]) return firstMatch;
            currentPattern += (*(uint16_t*)currentPattern == (uint16_t)'\?\?' || *(uint8_t*)currentPattern != '\?') ? 3 : 2;
        } else {
            currentPattern = pattern;
            firstMatch = nullptr;
        }
    }
    return nullptr;
}

// Función de hook para Present (Donde se renderiza ImGui)
HRESULT __stdcall Hooks::hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
    // Solo inicializamos ImGui si el dispositivo y el contexto están disponibles
    if (!g_pd3d11Device)
    {
        if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&g_pd3d11Device)))
        {
            g_pd3d11Device->GetImmediateContext(&g_pd3d11DeviceContext);
            
            ID3D11Texture2D* pBackBuffer;
            if (SUCCEEDED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer))) {
                g_pd3d11Device->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
                pBackBuffer->Release();
                ImGui_Renderer::InitImGui(g_pd3d11Device, g_pd3d11DeviceContext);
            }
        }
    }

    // Renderizar ImGui sobre el frame del juego de forma segura
    if (g_pd3d11DeviceContext && g_mainRenderTargetView) {
        ImGui_Renderer::Render(g_pd3d11DeviceContext, g_mainRenderTargetView);
    }

    return oPresent(pSwapChain, SyncInterval, Flags);
}

void Hooks::Initialize()
{
    // MÉTODO SEGURO: Obtener la dirección de Present desde dxgi.dll mediante patrón
    // Firma actualizada para Windows 10/11 x64 (DXGI.dll Present)
    uint8_t* pPresentAddr = FindPattern("dxgi.dll", "48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 56 41 57 48 83 EC 20 41 8B E8");

    if (pPresentAddr) {
        // Usamos un Hook de salto (JMP) sigiloso
        // Nota: Asegúrate de que MinHook esté bien configurado en CMakeLists.txt
        #include <MinHook.h>
        if (MH_Initialize() == MH_OK) {
            if (MH_CreateHook(pPresentAddr, &hkPresent, reinterpret_cast<LPVOID*>(&oPresent)) == MH_OK) {
                MH_EnableHook(pPresentAddr);
                std::cout << "[+] Hook de Present establecido correctamente." << std::endl;
            }
        }
    } else {
        std::cerr << "[!] Error: No se encontró la dirección de Present en dxgi.dll." << std::endl;
    }
}

void Hooks::Shutdown()
{
    // Desactivar hooks de forma segura
    MH_DisableHook(MH_ALL_HOOKS);
    MH_RemoveHook(MH_ALL_HOOKS);
    MH_Uninitialize();

    // Limpiar recursos de D3D11
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
    if (g_pd3d11DeviceContext) { g_pd3d11DeviceContext->Release(); g_pd3d11DeviceContext = nullptr; }
    if (g_pd3d11Device) { g_pd3d11Device->Release(); g_pd3d11Device = nullptr; }
}
