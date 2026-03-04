#include "ImGui_Renderer.h"
#include "main.h"
#include "SDK.h"

// Rutas corregidas para que el compilador las encuentre siempre
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Puntero a la función original de WndProc
WNDPROC oWndProc = nullptr;
bool bShowMenu = true;

// Nuestra función de WndProc hookeada
LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    // Tecla INSERT para mostrar/ocultar el menú
    if (uMsg == WM_KEYDOWN && wParam == VK_INSERT)
    {
        bShowMenu = !bShowMenu;
        return true;
    }

    // Si el menú está abierto, ImGui maneja el input
    if (bShowMenu && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        return true;

    return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

void ImGui_Renderer::Initialize()
{
    // Esta función se llamará desde Hooks.cpp cuando se detecte el dispositivo D3D11
}

void ImGui_Renderer::InitImGui(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
    // Inicializar el contexto de ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Configurar estilo
    ImGui::StyleColorsDark();

    // Obtener la ventana del juego para el hook de WndProc
    HWND hWnd = FindWindowA("SDL_app", "Counter-Strike 2");
    if (!hWnd)
        hWnd = GetForegroundWindow();

    // Inicializar backends para Win32 y DirectX 11
    ImGui_ImplWin32_Init(hWnd);
    ImGui_ImplDX11_Init(pDevice, pDeviceContext);

    // Hookear el WndProc para capturar el ratón y teclado
    oWndProc = (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)WndProc);
}

void ImGui_Renderer::Render(ID3D11DeviceContext* pDeviceContext, ID3D11RenderTargetView* pRenderTargetView)
{
    if (!bShowMenu)
        return;

    // Iniciar el nuevo frame de ImGui
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // Dibujar nuestro menú
    DrawMenu();

    // Finalizar el frame y renderizar
    ImGui::Render();
    pDeviceContext->OMSetRenderTargets(1, &pRenderTargetView, NULL);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImGui_Renderer::DrawMenu()
{
    // Ventana de ejemplo del menú
    ImGui::Begin("CS2 Edu-Tool v1.0", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    
    ImGui::Text("Bienvenido al menu de CS2!");
    ImGui::Separator();
    
    static bool bEsp = false;
    ImGui::Checkbox("Visuales (ESP)", &bEsp);
    
    static bool bAimbot = false;
    ImGui::Checkbox("Aimbot", &bAimbot);

    if (ImGui::Button("Desinyectar DLL"))
    {
        g_bUnload = true;
    }

    ImGui::End();
}

void ImGui_Renderer::Shutdown()
{
    // Restaurar el WndProc original
    HWND hWnd = FindWindowA("SDL_app", "Counter-Strike 2");
    if (!hWnd) hWnd = GetForegroundWindow();
    if (oWndProc) SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)oWndProc);

    // Limpieza de ImGui
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void ImGui_Renderer::ShutdownImGui()
{
    Shutdown();
}
