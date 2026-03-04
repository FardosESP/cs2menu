#pragma once

#include <d3d11.h>

namespace ImGui_Renderer
{
    // Inicializar ImGui con el dispositivo y contexto D3D11
    void InitImGui(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);

    // Renderizar la interfaz de ImGui
    void Render(ID3D11DeviceContext* pDeviceContext, ID3D11RenderTargetView* pRenderTargetView);

    // Apagar ImGui
    void ShutdownImGui();

    // Función para dibujar el menú de ImGui (se llamará desde Render)
    void DrawMenu();

    // Inicialización y limpieza general (para main.cpp)
    void Initialize();
    void Shutdown();
}
