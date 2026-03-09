#include <Windows.h>
#include <d3d11.h>
#include <TlHelp32.h>
#include <string>
#include <thread>
#include <chrono>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "ManualMap.h"

#pragma comment(lib, "d3d11.lib")

// Premium Launcher - Neverlose/Onetap Style
// Modern UI with animations, blur effects, and professional design

// DirectX 11 globals
static ID3D11Device* g_pd3dDevice = nullptr;
static ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
static IDXGISwapChain* g_pSwapChain = nullptr;
static ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;

// Window globals
static HWND g_hwnd = nullptr;
static WNDCLASSEXW g_wc = {};

// Launcher state
enum class LauncherState
{
    IDLE,
    SEARCHING,
    INJECTING,
    SUCCESS,
    FAILED  // Changed from ERROR to avoid Windows.h macro conflict
};

struct LauncherData
{
    LauncherState state;
    std::string statusMessage;
    float progress;
    float animationTime;
    bool showSettings;
    int injectionMethod;
    DWORD targetPID;
    
    LauncherData() : state(LauncherState::IDLE), statusMessage("Ready to inject"), 
                     progress(0.0f), animationTime(0.0f), showSettings(false), 
                     injectionMethod(1), targetPID(0) {}
};

static LauncherData g_launcherData;

// Colors - Neverlose style (dark purple/blue theme)
namespace ColorScheme
{
    inline ImVec4 Background() { return ImVec4(0.08f, 0.08f, 0.12f, 0.95f); }
    inline ImVec4 WindowBg() { return ImVec4(0.10f, 0.10f, 0.15f, 1.00f); }
    inline ImVec4 Header() { return ImVec4(0.12f, 0.12f, 0.18f, 1.00f); }
    inline ImVec4 Accent() { return ImVec4(0.40f, 0.35f, 0.90f, 1.00f); }
    inline ImVec4 AccentHovered() { return ImVec4(0.50f, 0.45f, 1.00f, 1.00f); }
    inline ImVec4 AccentActive() { return ImVec4(0.35f, 0.30f, 0.80f, 1.00f); }
    inline ImVec4 Success() { return ImVec4(0.30f, 0.85f, 0.50f, 1.00f); }
    inline ImVec4 Error() { return ImVec4(0.90f, 0.30f, 0.30f, 1.00f); }
    inline ImVec4 Text() { return ImVec4(0.95f, 0.95f, 0.95f, 1.00f); }
    inline ImVec4 TextDim() { return ImVec4(0.60f, 0.60f, 0.65f, 1.00f); }
    inline ImVec4 Border() { return ImVec4(0.20f, 0.20f, 0.25f, 1.00f); }
}

// Forward declarations
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void SetupImGuiStyle();
void RenderLauncher();
DWORD FindProcessByName(const wchar_t* processName);
void InjectionThread();

// Main entry point
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    // Create window
    g_wc = { sizeof(g_wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"CS2MenuLauncher", nullptr };
    RegisterClassExW(&g_wc);
    
    // Create borderless window
    g_hwnd = CreateWindowExW(
        WS_EX_TOPMOST | WS_EX_LAYERED,
        g_wc.lpszClassName,
        L"CS2MENU Premium",
        WS_POPUP,
        (GetSystemMetrics(SM_CXSCREEN) - 600) / 2,
        (GetSystemMetrics(SM_CYSCREEN) - 400) / 2,
        600, 400,
        nullptr, nullptr, g_wc.hInstance, nullptr
    );

    // Set window transparency
    SetLayeredWindowAttributes(g_hwnd, 0, 245, LWA_ALPHA);

    // Initialize Direct3D
    if (!CreateDeviceD3D(g_hwnd))
    {
        CleanupDeviceD3D();
        UnregisterClassW(g_wc.lpszClassName, g_wc.hInstance);
        return 1;
    }

    ShowWindow(g_hwnd, SW_SHOWDEFAULT);
    UpdateWindow(g_hwnd);

    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr; // Disable imgui.ini
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    // Setup style
    SetupImGuiStyle();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(g_hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    // Load fonts
    io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeui.ttf", 16.0f);
    ImFont* titleFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeuib.ttf", 28.0f);
    ImFont* iconFont = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\segoeuib.ttf", 48.0f);

    // Main loop
    bool done = false;
    while (!done)
    {
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Start ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // Render launcher UI
        RenderLauncher();

        // Rendering
        ImGui::Render();
        const float clear_color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, nullptr);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(1, 0);
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    DestroyWindow(g_hwnd);
    UnregisterClassW(g_wc.lpszClassName, g_wc.hInstance);

    return 0;
}

void SetupImGuiStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();
    
    // Rounding - más suave y moderno
    style.WindowRounding = 12.0f;
    style.ChildRounding = 8.0f;
    style.FrameRounding = 6.0f;
    style.PopupRounding = 6.0f;
    style.ScrollbarRounding = 6.0f;
    style.GrabRounding = 6.0f;
    style.TabRounding = 6.0f;
    
    // Spacing - más amplio y respirable
    style.WindowPadding = ImVec2(20, 20);
    style.FramePadding = ImVec2(12, 8);
    style.ItemSpacing = ImVec2(12, 10);
    style.ItemInnerSpacing = ImVec2(10, 8);
    style.IndentSpacing = 28.0f;
    style.ScrollbarSize = 14.0f;
    style.GrabMinSize = 12.0f;
    
    // Borders - más sutiles
    style.WindowBorderSize = 0.0f;
    style.ChildBorderSize = 1.0f;
    style.PopupBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f;
    
    // Colors - tema premium oscuro con acentos morados
    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.08f, 0.12f, 0.95f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.12f, 0.12f, 0.18f, 0.60f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.10f, 0.10f, 0.15f, 0.98f);
    colors[ImGuiCol_Border] = ImVec4(0.25f, 0.25f, 0.30f, 0.50f);
    colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.30f);
    
    colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.15f, 0.22f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.20f, 0.20f, 0.28f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.18f, 0.18f, 0.25f, 1.00f);
    
    colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.10f, 0.15f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.12f, 0.12f, 0.18f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.10f, 0.10f, 0.15f, 0.75f);
    
    colors[ImGuiCol_MenuBarBg] = ImVec4(0.12f, 0.12f, 0.18f, 1.00f);
    
    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.10f, 0.15f, 0.60f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.35f, 0.35f, 0.40f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.45f, 0.45f, 0.50f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.55f, 0.55f, 0.60f, 1.00f);
    
    colors[ImGuiCol_CheckMark] = ImVec4(0.50f, 0.40f, 1.00f, 1.00f);
    
    colors[ImGuiCol_SliderGrab] = ImVec4(0.45f, 0.35f, 0.95f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.55f, 0.45f, 1.00f, 1.00f);
    
    colors[ImGuiCol_Button] = ImVec4(0.45f, 0.35f, 0.95f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.55f, 0.45f, 1.00f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.40f, 0.30f, 0.85f, 1.00f);
    
    colors[ImGuiCol_Header] = ImVec4(0.40f, 0.35f, 0.90f, 0.55f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.45f, 0.40f, 0.95f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.50f, 0.45f, 1.00f, 1.00f);
    
    colors[ImGuiCol_Separator] = ImVec4(0.25f, 0.25f, 0.30f, 0.50f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.40f, 0.35f, 0.90f, 0.78f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.50f, 0.45f, 1.00f, 1.00f);
    
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.40f, 0.35f, 0.90f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.45f, 0.40f, 0.95f, 0.67f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.50f, 0.45f, 1.00f, 0.95f);
    
    colors[ImGuiCol_Tab] = ImVec4(0.15f, 0.15f, 0.22f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.45f, 0.40f, 0.95f, 0.80f);
    colors[ImGuiCol_TabActive] = ImVec4(0.40f, 0.35f, 0.90f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.12f, 0.12f, 0.18f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.15f, 0.15f, 0.22f, 1.00f);
    
    colors[ImGuiCol_PlotLines] = ImVec4(0.50f, 0.40f, 1.00f, 1.00f);
    colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.60f, 0.50f, 1.00f, 1.00f);
    colors[ImGuiCol_PlotHistogram] = ImVec4(0.50f, 0.40f, 1.00f, 1.00f);
    colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.60f, 0.50f, 1.00f, 1.00f);
    
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.40f, 0.35f, 0.90f, 0.35f);
    colors[ImGuiCol_DragDropTarget] = ImVec4(0.50f, 0.45f, 1.00f, 0.90f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.50f, 0.45f, 1.00f, 1.00f);
    
    colors[ImGuiCol_Text] = ImVec4(0.95f, 0.95f, 0.97f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.55f, 1.00f);
}

void RenderLauncher()
{
    ImGuiIO& io = ImGui::GetIO();
    ImDrawList* draw = ImGui::GetBackgroundDrawList();
    
    // Update animation time
    g_launcherData.animationTime += io.DeltaTime;
    
    // Background gradient
    draw->AddRectFilledMultiColor(
        ImVec2(0, 0), 
        ImVec2(io.DisplaySize.x, io.DisplaySize.y),
        IM_COL32(12, 12, 18, 255),
        IM_COL32(18, 18, 26, 255),
        IM_COL32(22, 22, 32, 255),
        IM_COL32(15, 15, 22, 255)
    );
    
    // Animated particles/dots in background
    for (int i = 0; i < 20; i++)
    {
        float x = (sinf(g_launcherData.animationTime * 0.3f + i) * 0.5f + 0.5f) * io.DisplaySize.x;
        float y = (cosf(g_launcherData.animationTime * 0.2f + i * 0.5f) * 0.5f + 0.5f) * io.DisplaySize.y;
        float alpha = (sinf(g_launcherData.animationTime + i) * 0.5f + 0.5f) * 0.15f;
        draw->AddCircleFilled(ImVec2(x, y), 2.0f, IM_COL32(102, 89, 230, (int)(alpha * 255)));
    }
    
    // Center content area
    float centerX = io.DisplaySize.x * 0.5f;
    float centerY = io.DisplaySize.y * 0.5f;
    float panelWidth = 480.0f;
    float panelHeight = 520.0f;
    float panelX = centerX - panelWidth * 0.5f;
    float panelY = centerY - panelHeight * 0.5f;
    
    // Main panel shadow
    draw->AddRectFilled(
        ImVec2(panelX + 4, panelY + 4),
        ImVec2(panelX + panelWidth + 4, panelY + panelHeight + 4),
        IM_COL32(0, 0, 0, 80),
        12.0f
    );
    
    // Main panel background
    draw->AddRectFilled(
        ImVec2(panelX, panelY),
        ImVec2(panelX + panelWidth, panelY + panelHeight),
        IM_COL32(20, 20, 28, 245),
        12.0f
    );
    
    // Top accent bar with gradient
    draw->AddRectFilledMultiColor(
        ImVec2(panelX, panelY),
        ImVec2(panelX + panelWidth, panelY + 4),
        IM_COL32(102, 89, 230, 255),
        IM_COL32(127, 89, 230, 255),
        IM_COL32(127, 89, 230, 255),
        IM_COL32(102, 89, 230, 255)
    );
    
    // Animated glow line
    float glowOffset = (sinf(g_launcherData.animationTime * 2.0f) * 0.5f + 0.5f) * panelWidth;
    draw->AddRectFilledMultiColor(
        ImVec2(panelX + glowOffset - 100, panelY),
        ImVec2(panelX + glowOffset + 100, panelY + 4),
        IM_COL32(102, 89, 230, 0),
        IM_COL32(180, 160, 255, 200),
        IM_COL32(180, 160, 255, 200),
        IM_COL32(102, 89, 230, 0)
    );
    
    // Close button
    ImGui::SetNextWindowPos(ImVec2(panelX + panelWidth - 45, panelY + 15));
    ImGui::SetNextWindowSize(ImVec2(35, 35));
    ImGui::Begin("##CloseBtn", nullptr, 
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | 
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar | 
        ImGuiWindowFlags_NoBackground);
    
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.1f, 0.1f, 0.8f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.1f, 0.1f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.1f, 0.1f, 1.0f));
    if (ImGui::Button("X", ImVec2(30, 30)))
        PostQuitMessage(0);
    ImGui::PopStyleColor(3);
    ImGui::End();
    
    // Main content window
    ImGui::SetNextWindowPos(ImVec2(panelX + 30, panelY + 30));
    ImGui::SetNextWindowSize(ImVec2(panelWidth - 60, panelHeight - 60));
    ImGui::Begin("##Content", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoBackground);
    
    // Logo section
    ImGui::SetCursorPosX((panelWidth - 60 - 200) * 0.5f);
    ImGui::PushFont(io.Fonts->Fonts[1]); // Title font
    
    // Animated glow effect on title
    float titleGlow = 0.4f + 0.3f * sinf(g_launcherData.animationTime * 1.5f);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.5f, 1.0f, titleGlow));
    ImGui::Text("CS2MENU");
    ImGui::PopStyleColor();
    ImGui::PopFont();
    
    ImGui::SetCursorPosX((panelWidth - 60 - 150) * 0.5f);
    ImGui::PushStyleColor(ImGuiCol_Text, ColorScheme::TextDim());
    ImGui::Text("Premium HvH Edition");
    ImGui::PopStyleColor();
    
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    ImGui::Spacing();
    
    // Status section with fancy box
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.12f, 0.12f, 0.18f, 0.6f));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.0f);
    ImGui::BeginChild("##StatusBox", ImVec2(0, 140), true, ImGuiWindowFlags_NoScrollbar);
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    // Status icon and text centered
    const char* statusIcon = "●";
    ImVec4 statusColor = ColorScheme::TextDim();
    const char* statusText = g_launcherData.statusMessage.c_str();
    
    switch (g_launcherData.state)
    {
    case LauncherState::IDLE:
        statusColor = ImVec4(0.5f, 0.5f, 0.6f, 1.0f);
        statusIcon = "●";
        break;
    case LauncherState::SEARCHING:
        statusColor = ColorScheme::Accent();
        statusIcon = "⟳";
        break;
    case LauncherState::INJECTING:
        statusColor = ColorScheme::Accent();
        statusIcon = "⚡";
        break;
    case LauncherState::SUCCESS:
        statusColor = ColorScheme::Success();
        statusIcon = "✓";
        break;
    case LauncherState::FAILED:
        statusColor = ColorScheme::Error();
        statusIcon = "✗";
        break;
    }
    
    // Center the status text
    float textWidth = ImGui::CalcTextSize(statusText).x;
    ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - textWidth - 30) * 0.5f);
    
    ImGui::PushStyleColor(ImGuiCol_Text, statusColor);
    ImGui::PushFont(io.Fonts->Fonts[1]);
    ImGui::Text("%s", statusIcon);
    ImGui::PopFont();
    ImGui::PopStyleColor();
    
    ImGui::SameLine();
    ImGui::Text("%s", statusText);
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    // Progress bar with glow
    if (g_launcherData.progress > 0.0f)
    {
        ImGui::SetCursorPosX(20);
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.5f, 0.4f, 1.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.15f, 0.15f, 0.2f, 0.8f));
        ImGui::ProgressBar(g_launcherData.progress, ImVec2(ImGui::GetContentRegionAvail().x - 20, 8), "");
        ImGui::PopStyleColor(2);
        
        // Glow effect on progress bar
        ImVec2 barPos = ImGui::GetItemRectMin();
        ImVec2 barSize = ImGui::GetItemRectSize();
        ImDrawList* dl = ImGui::GetWindowDrawList();
        dl->AddRectFilled(
            ImVec2(barPos.x, barPos.y),
            ImVec2(barPos.x + barSize.x * g_launcherData.progress, barPos.y + barSize.y),
            IM_COL32(127, 89, 230, 100),
            4.0f
        );
    }
    
    ImGui::Spacing();
    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    // Injection method selector
    ImGui::Text("Injection Method");
    ImGui::Spacing();
    
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.15f, 0.15f, 0.22f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.20f, 0.20f, 0.28f, 1.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);
    
    ImGui::SetNextItemWidth(-1);
    const char* methods[] = { "LoadLibrary (Basic)", "Manual Map (Stealth)", "Thread Hijack (Advanced)" };
    ImGui::Combo("##Method", &g_launcherData.injectionMethod, methods, IM_ARRAYSIZE(methods));
    
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(2);
    
    ImGui::Spacing();
    ImGui::Spacing();
    
    // Info text
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.6f, 0.8f));
    ImGui::TextWrapped("Manual Map bypasses most anti-cheat detection by avoiding LoadLibrary and erasing PE headers.");
    ImGui::PopStyleColor();
    
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    
    // Inject button with glow
    bool canInject = (g_launcherData.state == LauncherState::IDLE || g_launcherData.state == LauncherState::FAILED);
    
    if (!canInject)
        ImGui::BeginDisabled();
    
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f);
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.45f, 0.35f, 0.95f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.55f, 0.45f, 1.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.40f, 0.30f, 0.85f, 1.0f));
    
    ImGui::PushFont(io.Fonts->Fonts[1]);
    if (ImGui::Button("INJECT", ImVec2(-1, 55)))
    {
        std::thread(InjectionThread).detach();
    }
    ImGui::PopFont();
    
    // Button glow effect
    if (canInject)
    {
        ImVec2 btnMin = ImGui::GetItemRectMin();
        ImVec2 btnMax = ImGui::GetItemRectMax();
        ImDrawList* dl = ImGui::GetWindowDrawList();
        float glowAlpha = 0.3f + 0.2f * sinf(g_launcherData.animationTime * 3.0f);
        dl->AddRect(btnMin, btnMax, IM_COL32(127, 89, 230, (int)(glowAlpha * 255)), 8.0f, 0, 2.0f);
    }
    
    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar();
    
    if (!canInject)
        ImGui::EndDisabled();
    
    ImGui::Spacing();
    
    // Footer
    ImGui::SetCursorPosY(ImGui::GetWindowSize().y - 25);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.4f, 0.4f, 0.5f, 0.6f));
    ImGui::Text("Build 14138.6 - Premium Edition");
    ImGui::PopStyleColor();
    
    ImGui::End();
}

void InjectionThread()
{
    g_launcherData.state = LauncherState::SEARCHING;
    g_launcherData.statusMessage = "Searching for CS2...";
    g_launcherData.progress = 0.1f;
    
    // Find CS2 process
    DWORD pid = 0;
    for (int i = 0; i < 30; i++)
    {
        pid = FindProcessByName(L"cs2.exe");
        if (pid != 0) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    
    if (pid == 0)
    {
        g_launcherData.state = LauncherState::FAILED;
        g_launcherData.statusMessage = "CS2 not found! Please start the game.";
        g_launcherData.progress = 0.0f;
        return;
    }
    
    g_launcherData.statusMessage = "CS2 found! Opening process...";
    g_launcherData.progress = 0.3f;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (!hProcess)
    {
        g_launcherData.state = LauncherState::FAILED;
        g_launcherData.statusMessage = "Failed to open process. Run as admin!";
        g_launcherData.progress = 0.0f;
        return;
    }
    
    g_launcherData.state = LauncherState::INJECTING;
    g_launcherData.statusMessage = "Injecting DLL...";
    g_launcherData.progress = 0.6f;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    // Get DLL path
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(NULL, path, MAX_PATH);
    std::wstring fullPath(path);
    size_t lastSlash = fullPath.find_last_of(L"\\/");
    if (lastSlash != std::wstring::npos)
    {
        fullPath = fullPath.substr(0, lastSlash + 1);
    }
    fullPath += L"cs2menu.dll";
    
    // Convert to char*
    std::string dllPathStr(fullPath.begin(), fullPath.end());
    
    bool success = false;
    if (g_launcherData.injectionMethod == 1) // Manual Map
    {
        success = ManualMap::InjectDll(hProcess, dllPathStr.c_str());
    }
    else
    {
        g_launcherData.state = LauncherState::FAILED;
        g_launcherData.statusMessage = "Method not implemented yet!";
        g_launcherData.progress = 0.0f;
        CloseHandle(hProcess);
        return;
    }
    
    CloseHandle(hProcess);
    
    if (success)
    {
        g_launcherData.state = LauncherState::SUCCESS;
        g_launcherData.statusMessage = "Injection successful! Press INSERT in-game.";
        g_launcherData.progress = 1.0f;
    }
    else
    {
        g_launcherData.state = LauncherState::FAILED;
        g_launcherData.statusMessage = "Injection failed! Check anti-virus.";
        g_launcherData.progress = 0.0f;
    }
}

DWORD FindProcessByName(const wchar_t* processName)
{
    DWORD pid = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32W pe = { sizeof(pe) };
        if (Process32FirstW(snapshot, &pe))
        {
            do {
                if (_wcsicmp(processName, pe.szExeFile) == 0)
                {
                    pid = pe.th32ProcessID;
                    break;
                }
            } while (Process32NextW(snapshot, &pe));
        }
        CloseHandle(snapshot);
    }
    return pid;
}

// DirectX 11 setup
bool CreateDeviceD3D(HWND hWnd)
{
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    HRESULT res = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
    if (res != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = nullptr; }
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
            return 0;
        if (g_pd3dDevice != nullptr)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_NCHITTEST:
    {
        LRESULT hit = DefWindowProc(hWnd, msg, wParam, lParam);
        if (hit == HTCLIENT)
            hit = HTCAPTION; // Make window draggable
        return hit;
    }
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}
