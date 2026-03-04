#include "ImGui_Renderer.h"
#include "main.h"
#include "SDK.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Puntero a la función original de WndProc
WNDPROC oWndProc = nullptr;

// Nuestra función de WndProc hookeada
LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    // Si ImGui está manejando el input, no pasamos el mensaje al juego
    if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        return true;

    return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

// Variables de estado del menú (para ImGui)
namespace
{
    bool bShowMenu = true;
    float fov_value = 90.0f;
    float smooth_value = 5.0f;
    bool esp_enabled = false;
    bool aimbot_enabled = false;
}

void ImGui_Renderer::Initialize()
{
    // Aquí se realizarían inicializaciones previas si fueran necesarias
}

void ImGui_Renderer::Shutdown()
{
    // Aquí se realizarían limpiezas previas si fueran necesarias
}

void ImGui_Renderer::InitImGui(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Obtener el HWND de la ventana del juego
    HWND hwnd = GetForegroundWindow(); // Esto puede no ser siempre el HWND correcto en un juego
                                       // En un hook real, se obtendría el HWND del juego de forma más robusta

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(pDevice, pDeviceContext);

    // Hookear la función WndProc del juego para capturar el input del ratón y teclado
    oWndProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)WndProc);

    // Estilo Neverlose (ejemplo simplificado)
    ImGui::StyleColorsDark();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 6.0f;
    style.FrameRounding = 4.0f;
    style.GrabRounding = 4.0f;
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.06f, 0.06f, 0.94f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.70f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
}

void ImGui_Renderer::ShutdownImGui()
{
    // Restaurar el WndProc original
    HWND hwnd = GetForegroundWindow();
    SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)oWndProc);

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void ImGui_Renderer::DrawMenu()
{
    // Alternar el menú con la tecla INSERT
    if (GetAsyncKeyState(VK_INSERT) & 1)
    {
        bShowMenu = !bShowMenu;
        // Si el menú se abre, habilitar el ratón de ImGui
        ImGui::GetIO().MouseDrawCursor = bShowMenu;
    }

    if (bShowMenu)
    {
        ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_Once);
        ImGui::Begin("CS2 Edu-Tool - Internal (Neverlose Style)", &bShowMenu, ImGuiWindowFlags_NoResize);

        ImGui::Text("Bienvenido al Proyecto Educativo Interno de CS2!");
        ImGui::Text("Solo para uso OFFLINE / -INSECURE.");
        ImGui::Separator();

        if (ImGui::BeginTabBar("##Tabs"))
        {
            if (ImGui::BeginTabItem("Visuals"))
            {
                ImGui::Checkbox("ESP Activado", &esp_enabled);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Aimbot"))
            {
                ImGui::Checkbox("Aimbot Activado", &aimbot_enabled);
                ImGui::SliderFloat("FOV", &fov_value, 1.0f, 180.0f, "%.1f");
                ImGui::SliderFloat("Smooth", &smooth_value, 1.0f, 20.0f, "%.1f");
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Misc"))
            {
                ImGui::Text("Otras opciones aqui...");
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        ImGui::End();
    }
}

void ImGui_Renderer::Render(ID3D11DeviceContext* pDeviceContext, ID3D11RenderTargetView* pRenderTargetView)
{
    // Iniciar un nuevo frame de ImGui
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // Dibujar el menú de ImGui
    DrawMenu();

    // Renderizar los comandos de ImGui
    ImGui::Render();

    pDeviceContext->OMSetRenderTargets(1, &pRenderTargetView, NULL);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
