#include "ImGui_Renderer.h"
#include "main.h"
#include "SDK.h"

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool    bShowMenu       = true;
static bool bImGuiReady = false;
static HMODULE g_hSDL   = nullptr;
extern UINT g_BackBufferWidth;
extern UINT g_BackBufferHeight;

// Variables del hilo de raton (definidas en DllMain.cpp)
extern POINT g_mousePos;
extern bool  g_mouseL;
extern bool  g_mouseR;
extern bool  g_mouseM;

// ─── Hook de SDL_PollEvent ────────────────────────────────────────────────────
// Esta es la solucion real: interceptar SDL_PollEvent antes de que CS2 lo procese
typedef int (*SDL_PollEvent_t)(void* event);
static SDL_PollEvent_t oSDL_PollEvent = nullptr;

// Offset del tipo de evento en SDL_Event (igual en SDL2 y SDL3)
#define SDL_EVENT_TYPE_OFFSET 0
#define SDL_QUIT_EVENT        0x100
#define SDL_KEYDOWN_EVENT     0x300
#define SDL_KEYUP_EVENT       0x301
#define SDL_MOUSEMOTION_EVENT 0x400
#define SDL_MOUSEBUTTONDOWN   0x401
#define SDL_MOUSEBUTTONUP     0x402
#define SDL_MOUSEWHEEL_EVENT  0x403

// Bytes originales de SDL_PollEvent para restaurar
static BYTE g_sdlOrigBytes[14] = {};
static void* g_sdlPollEventAddr = nullptr;

static int HookedSDL_PollEvent(void* event)
{
    // Llamar la funcion original
    int result = oSDL_PollEvent(event);

    if (result && event && bShowMenu && bImGuiReady)
    {
        uint32_t eventType = *reinterpret_cast<uint32_t*>(event);

        ImGuiIO& io = ImGui::GetIO();

        // Pasar eventos de raton a ImGui y bloquearlos para CS2
        switch (eventType)
        {
            case SDL_MOUSEMOTION_EVENT:
            {
                // Coordenadas del raton en SDL_MouseMotionEvent: x=offset 20, y=offset 24
                int x = *reinterpret_cast<int*>((char*)event + 20);
                int y = *reinterpret_cast<int*>((char*)event + 24);
                io.MousePos = {(float)x, (float)y};
                // Bloquear evento (poner tipo a 0)
                *reinterpret_cast<uint32_t*>(event) = 0;
                break;
            }
            case SDL_MOUSEBUTTONDOWN:
            {
                uint8_t button = *reinterpret_cast<uint8_t*>((char*)event + 16);
                if (button == 1) io.MouseDown[0] = true;
                if (button == 2) io.MouseDown[2] = true;
                if (button == 3) io.MouseDown[1] = true;
                *reinterpret_cast<uint32_t*>(event) = 0;
                break;
            }
            case SDL_MOUSEBUTTONUP:
            {
                uint8_t button = *reinterpret_cast<uint8_t*>((char*)event + 16);
                if (button == 1) io.MouseDown[0] = false;
                if (button == 2) io.MouseDown[2] = false;
                if (button == 3) io.MouseDown[1] = false;
                *reinterpret_cast<uint32_t*>(event) = 0;
                break;
            }
            case SDL_MOUSEWHEEL_EVENT:
            {
                float wheelY = *reinterpret_cast<float*>((char*)event + 20);
                io.MouseWheel += wheelY;
                *reinterpret_cast<uint32_t*>(event) = 0;
                break;
            }
        }
    }

    return result;
}

static void InstallSDLHook()
{
    if (!g_hSDL) return;

    g_sdlPollEventAddr = GetProcAddress(g_hSDL, "SDL_PollEvent");
    if (!g_sdlPollEventAddr) return;

    // Guardar bytes originales
    memcpy(g_sdlOrigBytes, g_sdlPollEventAddr, 14);

    // Guardar puntero original
    oSDL_PollEvent = (SDL_PollEvent_t)g_sdlPollEventAddr;

    // Instalar JMP de 64 bits
    BYTE jmp64[14] = {
        0xFF, 0x25, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    *reinterpret_cast<uintptr_t*>(&jmp64[6]) = reinterpret_cast<uintptr_t>(&HookedSDL_PollEvent);

    DWORD oldProtect;
    VirtualProtect(g_sdlPollEventAddr, 14, PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy(g_sdlPollEventAddr, jmp64, 14);
    VirtualProtect(g_sdlPollEventAddr, 14, oldProtect, &oldProtect);

    OutputDebugStringA("[+] SDL_PollEvent hookeado\n");
}

static void RemoveSDLHook()
{
    if (!g_sdlPollEventAddr || !g_sdlOrigBytes[0]) return;
    DWORD oldProtect;
    VirtualProtect(g_sdlPollEventAddr, 14, PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy(g_sdlPollEventAddr, g_sdlOrigBytes, 14);
    VirtualProtect(g_sdlPollEventAddr, 14, oldProtect, &oldProtect);
}

// ─── Llamar original SDL_PollEvent restaurando bytes temporalmente ─────────────
static int CallOriginalSDLPollEvent(void* event)
{
    DWORD oldProtect;
    VirtualProtect(g_sdlPollEventAddr, 14, PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy(g_sdlPollEventAddr, g_sdlOrigBytes, 14);
    VirtualProtect(g_sdlPollEventAddr, 14, oldProtect, &oldProtect);

    int result = oSDL_PollEvent(event);

    BYTE jmp64[14] = {
        0xFF, 0x25, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };
    *reinterpret_cast<uintptr_t*>(&jmp64[6]) = reinterpret_cast<uintptr_t>(&HookedSDL_PollEvent);

    VirtualProtect(g_sdlPollEventAddr, 14, PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy(g_sdlPollEventAddr, jmp64, 14);
    VirtualProtect(g_sdlPollEventAddr, 14, oldProtect, &oldProtect);

    return result;
}

// ─── Config structs ───────────────────────────────────────────────────────────
struct ESPConfig {
    bool enabled=false, boxes=true, boxFilled=false, skeleton=true;
    bool health=true, healthBar=true, name=true, distance=true;
    bool weapon=true, snaplines=false, dormantCheck=true, teamCheck=true;
    float boxColor[4]={1,0,0,1}, teamColor[4]={0,.5f,1,1};
    float skeletonColor[4]={1,1,1,.8f}, snaplineColor[4]={1,1,0,1};
    float maxDistance=500.f, boxThickness=1.5f;
};
struct AimbotConfig {
    bool enabled=false, visibleOnly=true, teamCheck=true;
    bool autoShoot=false, silentAim=false, rcs=false;
    float fov=5.f, smooth=8.f, rcsX=1.f, rcsY=1.f;
    int bone=0;
};
struct VisualsConfig {
    bool nightmode=false, noFlash=false, noSmoke=false;
    bool fullbright=false, crosshair=false, fovChanger=false, thirdPerson=false;
    float fovValue=90.f, brightness=1.f, crosshairSize=6.f, crosshairGap=2.f;
    float crosshairColor[4]={0,1,0,1};
};
struct MiscConfig {
    bool bhop=false, autoStrafe=false, radarHack=false, speedhack=false, antiAim=false;
    float speedValue=1.2f;
};
struct SkinConfig {
    int knifeModel=0, knifeFinish=0;
    bool forceKnife=false, forceSkins=false;
    int selectedSkin[30]={}, selectedWear[30]={}, selectedSeed[30]={};
    float floatValue[30]={};
};

static ESPConfig     cfg_esp;
static AimbotConfig  cfg_aim;
static VisualsConfig cfg_vis;
static MiscConfig    cfg_misc;
static SkinConfig    cfg_skin;

static void ApplyStyle()
{
    ImGuiStyle& s = ImGui::GetStyle();
    s.WindowRounding=6; s.FrameRounding=4; s.ScrollbarRounding=4;
    s.GrabRounding=4; s.TabRounding=4; s.ChildRounding=4;
    s.WindowBorderSize=1; s.FrameBorderSize=0;
    s.WindowPadding={10,10}; s.FramePadding={6,4};
    s.ItemSpacing={8,6}; s.ScrollbarSize=10;
    ImVec4* c = s.Colors;
    c[ImGuiCol_WindowBg]           ={.08f,.08f,.10f,.97f};
    c[ImGuiCol_ChildBg]            ={.10f,.10f,.13f,1};
    c[ImGuiCol_Border]             ={.20f,.20f,.25f,1};
    c[ImGuiCol_FrameBg]            ={.14f,.14f,.18f,1};
    c[ImGuiCol_FrameBgHovered]     ={.20f,.20f,.26f,1};
    c[ImGuiCol_FrameBgActive]      ={.24f,.24f,.30f,1};
    c[ImGuiCol_TitleBg]            ={.06f,.06f,.08f,1};
    c[ImGuiCol_TitleBgActive]      ={.08f,.08f,.11f,1};
    c[ImGuiCol_Tab]                ={.12f,.12f,.16f,1};
    c[ImGuiCol_TabHovered]         ={.20f,.45f,.90f,1};
    c[ImGuiCol_TabActive]          ={.16f,.37f,.80f,1};
    c[ImGuiCol_TabUnfocusedActive] ={.12f,.25f,.55f,1};
    c[ImGuiCol_Header]             ={.16f,.37f,.80f,.5f};
    c[ImGuiCol_HeaderHovered]      ={.20f,.45f,.90f,.7f};
    c[ImGuiCol_HeaderActive]       ={.20f,.45f,.90f,1};
    c[ImGuiCol_Button]             ={.14f,.32f,.72f,1};
    c[ImGuiCol_ButtonHovered]      ={.20f,.45f,.90f,1};
    c[ImGuiCol_ButtonActive]       ={.10f,.25f,.60f,1};
    c[ImGuiCol_SliderGrab]         ={.20f,.45f,.90f,1};
    c[ImGuiCol_SliderGrabActive]   ={.26f,.55f,1,1};
    c[ImGuiCol_CheckMark]          ={.26f,.55f,1,1};
    c[ImGuiCol_Separator]          ={.20f,.20f,.28f,1};
    c[ImGuiCol_Text]               ={.90f,.90f,.95f,1};
    c[ImGuiCol_TextDisabled]       ={.45f,.45f,.55f,1};
    c[ImGuiCol_ScrollbarBg]        ={.08f,.08f,.10f,1};
    c[ImGuiCol_ScrollbarGrab]      ={.20f,.45f,.90f,.6f};
    c[ImGuiCol_PopupBg]            ={.10f,.10f,.13f,.97f};
}

static void SectionTitle(const char* t) {
    ImGui::Spacing();
    ImGui::TextColored({.26f,.55f,1,1}, t);
    ImGui::Separator(); ImGui::Spacing();
}
static void Tip(const char* t) {
    if (ImGui::IsItemHovered()) { ImGui::BeginTooltip(); ImGui::TextUnformatted(t); ImGui::EndTooltip(); }
}
static HWND GetCS2Window() {
    HWND h = FindWindowA("SDL_app", nullptr);
    if (!h) h = FindWindowA(nullptr, "Counter-Strike 2");
    return h;
}

static void TabESP() {
    ImGui::BeginChild("##esp",{0,0},false);
    SectionTitle("  ESP General");
    ImGui::Checkbox("Activar ESP",&cfg_esp.enabled);
    if(cfg_esp.enabled){ImGui::Indent();ImGui::Checkbox("Solo enemigos",&cfg_esp.teamCheck);ImGui::Checkbox("Ignorar dormant",&cfg_esp.dormantCheck);ImGui::SliderFloat("Distancia max",&cfg_esp.maxDistance,50,2000,"%.0f u");ImGui::Unindent();}
    SectionTitle("  Cajas");
    ImGui::Checkbox("Cajas##b",&cfg_esp.boxes);
    if(cfg_esp.boxes){ImGui::Indent();ImGui::Checkbox("Relleno",&cfg_esp.boxFilled);ImGui::SliderFloat("Grosor",&cfg_esp.boxThickness,.5f,4,"%.1f");ImGui::ColorEdit4("Color enemigo##bc",cfg_esp.boxColor,ImGuiColorEditFlags_NoInputs|ImGuiColorEditFlags_AlphaBar);ImGui::SameLine();ImGui::ColorEdit4("Color equipo##tc",cfg_esp.teamColor,ImGuiColorEditFlags_NoInputs|ImGuiColorEditFlags_AlphaBar);ImGui::Unindent();}
    SectionTitle("  Esqueleto");
    ImGui::Checkbox("Esqueleto##sk",&cfg_esp.skeleton);
    if(cfg_esp.skeleton){ImGui::Indent();ImGui::ColorEdit4("Color##skc",cfg_esp.skeletonColor,ImGuiColorEditFlags_NoInputs|ImGuiColorEditFlags_AlphaBar);ImGui::Unindent();}
    SectionTitle("  Info jugadores");
    ImGui::Checkbox("Nombre##n",&cfg_esp.name);ImGui::SameLine(160);ImGui::Checkbox("Vida##h",&cfg_esp.health);
    ImGui::Checkbox("Barra vida##hb",&cfg_esp.healthBar);ImGui::SameLine(160);ImGui::Checkbox("Distancia##d",&cfg_esp.distance);
    ImGui::Checkbox("Arma##w",&cfg_esp.weapon);ImGui::SameLine(160);ImGui::Checkbox("Snaplines##sl",&cfg_esp.snaplines);
    if(cfg_esp.snaplines){ImGui::Indent();ImGui::ColorEdit4("Color snapline",cfg_esp.snaplineColor,ImGuiColorEditFlags_NoInputs|ImGuiColorEditFlags_AlphaBar);ImGui::Unindent();}
    ImGui::EndChild();
}
static void TabAimbot() {
    ImGui::BeginChild("##aim",{0,0},false);
    SectionTitle("  Aimbot");
    ImGui::Checkbox("Activar Aimbot",&cfg_aim.enabled);
    if(cfg_aim.enabled){
        ImGui::Indent();
        ImGui::Checkbox("Solo visibles",&cfg_aim.visibleOnly);Tip("No apunta a traves de paredes");
        ImGui::Checkbox("No atacar equipo",&cfg_aim.teamCheck);
        ImGui::Checkbox("Auto disparo",&cfg_aim.autoShoot);Tip("Dispara cuando el objetivo entra en el FOV");
        ImGui::Checkbox("Silent Aim",&cfg_aim.silentAim);Tip("El movimiento del raton no es visible");
        ImGui::Spacing();
        const char* bones[]={"Cabeza","Cuello","Cuerpo"};
        ImGui::Combo("Hueso objetivo",&cfg_aim.bone,bones,3);
        ImGui::SliderFloat("FOV",&cfg_aim.fov,1,180,"%.1f deg");
        ImGui::SliderFloat("Suavizado",&cfg_aim.smooth,1,30,"%.1f");
        ImGui::Unindent();
    }
    SectionTitle("  RCS");
    ImGui::Checkbox("Activar RCS",&cfg_aim.rcs);
    if(cfg_aim.rcs){ImGui::Indent();ImGui::SliderFloat("Horizontal",&cfg_aim.rcsX,0,2,"%.2f");ImGui::SliderFloat("Vertical",&cfg_aim.rcsY,0,2,"%.2f");ImGui::Unindent();}
    ImGui::EndChild();
}
static void TabVisuals() {
    ImGui::BeginChild("##vis",{0,0},false);
    SectionTitle("  Mundo");
    ImGui::Checkbox("Nightmode",&cfg_vis.nightmode);Tip("Oscurece el mapa");
    ImGui::Checkbox("Sin flash",&cfg_vis.noFlash);ImGui::Checkbox("Sin humo",&cfg_vis.noSmoke);
    ImGui::Checkbox("Fullbright",&cfg_vis.fullbright);
    if(cfg_vis.nightmode||cfg_vis.fullbright){ImGui::Indent();ImGui::SliderFloat("Brillo",&cfg_vis.brightness,0,3,"%.2f");ImGui::Unindent();}
    SectionTitle("  Camara");
    ImGui::Checkbox("FOV Changer",&cfg_vis.fovChanger);
    if(cfg_vis.fovChanger){ImGui::Indent();ImGui::SliderFloat("FOV##fov",&cfg_vis.fovValue,60,160,"%.0f");ImGui::Unindent();}
    ImGui::Checkbox("Tercera persona",&cfg_vis.thirdPerson);
    SectionTitle("  Crosshair custom");
    ImGui::Checkbox("Activar crosshair",&cfg_vis.crosshair);
    if(cfg_vis.crosshair){
        ImGui::Indent();
        ImGui::SliderFloat("Tamano##cs",&cfg_vis.crosshairSize,2,20,"%.1f");
        ImGui::SliderFloat("Gap##cg",&cfg_vis.crosshairGap,0,10,"%.1f");
        ImGui::ColorEdit4("Color##cc",cfg_vis.crosshairColor,ImGuiColorEditFlags_NoInputs|ImGuiColorEditFlags_AlphaBar);
        ImGui::Unindent();
        ImGui::Spacing();
        ImVec2 p=ImGui::GetCursorScreenPos();
        float cx=p.x+60,cy=p.y+20,sz=cfg_vis.crosshairSize,gap=cfg_vis.crosshairGap;
        ImU32 col=ImGui::ColorConvertFloat4ToU32({cfg_vis.crosshairColor[0],cfg_vis.crosshairColor[1],cfg_vis.crosshairColor[2],cfg_vis.crosshairColor[3]});
        ImDrawList* dl=ImGui::GetWindowDrawList();
        dl->AddLine({cx-sz-gap,cy},{cx-gap,cy},col,1.5f);dl->AddLine({cx+gap,cy},{cx+sz+gap,cy},col,1.5f);
        dl->AddLine({cx,cy-sz-gap},{cx,cy-gap},col,1.5f);dl->AddLine({cx,cy+gap},{cx,cy+sz+gap},col,1.5f);
        ImGui::Dummy({120,40});ImGui::SameLine();ImGui::TextDisabled("(preview)");
    }
    ImGui::EndChild();
}
static void TabMisc() {
    ImGui::BeginChild("##misc",{0,0},false);
    SectionTitle("  Movimiento");
    ImGui::Checkbox("Bunny Hop",&cfg_misc.bhop);Tip("Salta automaticamente al tocar el suelo");
    ImGui::Checkbox("Auto Strafe",&cfg_misc.autoStrafe);
    SectionTitle("  Utilidad");
    ImGui::Checkbox("Radar Hack",&cfg_misc.radarHack);
    ImGui::Checkbox("Speedhack",&cfg_misc.speedhack);
    if(cfg_misc.speedhack){ImGui::Indent();ImGui::SliderFloat("Multiplicador",&cfg_misc.speedValue,.5f,3,"x%.2f");ImGui::Unindent();}
    ImGui::Checkbox("Anti-Aim",&cfg_misc.antiAim);
    SectionTitle("  Sistema");
    ImGui::TextDisabled("INSERT - mostrar/ocultar menu");
    ImGui::TextDisabled("END    - desinyectar DLL");
    ImGui::Spacing();
    ImGui::PushStyleColor(ImGuiCol_Button,{.55f,.10f,.10f,1});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,{.75f,.15f,.15f,1});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,{.40f,.05f,.05f,1});
    if(ImGui::Button("  Desinyectar DLL  ",{-1,0})) g_bUnload=true;
    ImGui::PopStyleColor(3);
    ImGui::EndChild();
}
static void TabSkins() {
    ImGui::BeginChild("##skin",{0,0},false);
    SectionTitle("  Cuchillo");
    ImGui::Checkbox("Forzar cuchillo",&cfg_skin.forceKnife);
    if(cfg_skin.forceKnife){
        ImGui::Indent();
        const char* knives[]={"Karambit","M9 Bayonet","Bayonet","Butterfly","Flip Knife","Gut Knife","Huntsman","Falchion","Shadow Daggers","Bowie","Ursus","Stiletto","Talon","Navaja","Classic Knife","Skeleton"};
        const char* finishes[]={"Vanilla","Case Hardened","Crimson Web","Fade","Doppler","Tiger Tooth","Marble Fade","Slaughter","Blue Steel","Night","Ultraviolet","Damascus Steel"};
        ImGui::Combo("Modelo##km",&cfg_skin.knifeModel,knives,IM_ARRAYSIZE(knives));
        ImGui::Combo("Acabado##kf",&cfg_skin.knifeFinish,finishes,IM_ARRAYSIZE(finishes));
        ImGui::Unindent();
    }
    SectionTitle("  Armas");
    ImGui::Checkbox("Forzar skins",&cfg_skin.forceSkins);
    if(cfg_skin.forceSkins){
        static const char* weapons[]={"AK-47","M4A4","M4A1-S","AWP","Desert Eagle","USP-S","Glock-18","P250","Five-SeveN","Tec-9","MP9","MAC-10","UMP-45","P90","PP-Bizon","FAMAS","Galil AR","SG 553","AUG","SSG 08","G3SG1","SCAR-20","Nova","XM1014","MAG-7","Sawed-Off","M249","Negev","CZ75-Auto","MP5-SD"};
        static const char* wears[]={"Factory New","Minimal Wear","Field-Tested","Well-Worn","Battle-Scarred"};
        static int sel=0;
        ImGui::PushStyleColor(ImGuiCol_ChildBg,{.07f,.07f,.09f,1});
        ImGui::BeginChild("##wlist",{175,310},true);
        for(int i=0;i<IM_ARRAYSIZE(weapons);i++) if(ImGui::Selectable(weapons[i],sel==i)) sel=i;
        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::SameLine();
        ImGui::BeginChild("##sedit",{0,310},false);
        ImGui::TextColored({.26f,.55f,1,1},"Editando: %s",weapons[sel]);
        ImGui::Separator();ImGui::Spacing();
        ImGui::InputInt("Skin ID",&cfg_skin.selectedSkin[sel]);
        if(cfg_skin.selectedSkin[sel]<0)cfg_skin.selectedSkin[sel]=0;
        ImGui::Combo("Desgaste",&cfg_skin.selectedWear[sel],wears,5);
        ImGui::SliderFloat("Float",&cfg_skin.floatValue[sel],0,1,"%.4f");
        ImGui::InputInt("Seed",&cfg_skin.selectedSeed[sel]);
        if(cfg_skin.selectedSeed[sel]<0)cfg_skin.selectedSeed[sel]=0;
        ImGui::Spacing();ImGui::Separator();ImGui::Spacing();
        float flt=cfg_skin.floatValue[sel];
        const char* wl=flt<.07f?"Factory New":flt<.15f?"Minimal Wear":flt<.38f?"Field-Tested":flt<.45f?"Well-Worn":"Battle-Scarred";
        ImGui::TextDisabled("Calidad: ");ImGui::SameLine();ImGui::TextColored({.26f,.55f,1,1},"%s",wl);
        ImGui::Spacing();
        if(ImGui::Button("Resetear slot",{-1,0})){cfg_skin.selectedSkin[sel]=0;cfg_skin.selectedWear[sel]=0;cfg_skin.selectedSeed[sel]=0;cfg_skin.floatValue[sel]=0;}
        ImGui::EndChild();
    }
    ImGui::EndChild();
}

static void DrawMainMenu()
{
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 center = {275.0f, 215.0f};
    if (io.DisplaySize.x > 0.0f && io.DisplaySize.y > 0.0f)
        center = {io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f};

    ImGui::SetNextWindowSize({550,430}, ImGuiCond_Always);
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, {.5f, .5f});
    ImGui::Begin("##cs2menu",nullptr,
                 ImGuiWindowFlags_NoCollapse|
                 ImGuiWindowFlags_NoScrollbar|
                 ImGuiWindowFlags_NoScrollWithMouse|
                 ImGuiWindowFlags_NoSavedSettings);
    ImDrawList* dl=ImGui::GetWindowDrawList();
    ImVec2 wp=ImGui::GetWindowPos(),ws=ImGui::GetWindowSize();
    dl->AddRectFilled({wp.x,wp.y},{wp.x+ws.x,wp.y+3},IM_COL32(50,120,220,255));
    ImGui::Spacing();
    ImGui::TextColored({.26f,.55f,1,1},"CS2MENU");ImGui::SameLine();
    ImGui::TextDisabled("v2.0   |   INSERT ocultar   |   END salir");
    ImGui::SameLine();
    ImGui::TextDisabled("[Mouse %.0f,%.0f L:%s]",
                        io.MousePos.x, io.MousePos.y,
                        io.MouseDown[0] ? "ON" : "OFF");
    ImGui::Separator();ImGui::Spacing();
    if(ImGui::BeginTabBar("##tabs")){
        if(ImGui::BeginTabItem("  ESP  "))      {TabESP();    ImGui::EndTabItem();}
        if(ImGui::BeginTabItem("  Aimbot  "))   {TabAimbot(); ImGui::EndTabItem();}
        if(ImGui::BeginTabItem("  Visuales  ")) {TabVisuals();ImGui::EndTabItem();}
        if(ImGui::BeginTabItem("  Misc  "))     {TabMisc();   ImGui::EndTabItem();}
        if(ImGui::BeginTabItem("  Skins  "))    {TabSkins();  ImGui::EndTabItem();}
        ImGui::EndTabBar();
    }
    ImGui::End();
}

// ─── Ciclo de vida ────────────────────────────────────────────────────────────
void ImGui_Renderer::Initialize() {}

void ImGui_Renderer::InitImGui(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
    if (bImGuiReady) return;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    // No usar ini de ImGui para que no recuerde una posicion fuera de pantalla
    io.IniFilename = nullptr;
    io.ConfigFlags &= ~ImGuiConfigFlags_NavEnableKeyboard;

    HWND hWnd = GetCS2Window();
    for (int i = 0; i < 6 && !hWnd; i++) { Sleep(500); hWnd = GetCS2Window(); }
    if (!hWnd) return;

    RECT rect; GetClientRect(hWnd, &rect);
    io.DisplaySize = {(float)(rect.right-rect.left),(float)(rect.bottom-rect.top)};
    io.MouseDrawCursor = true;

    ApplyStyle();
    ImGui_ImplDX11_Init(pDevice, pDeviceContext);

    // Buscar SDL
    g_hSDL = GetModuleHandleA("SDL2.dll");
    if (!g_hSDL) g_hSDL = GetModuleHandleA("SDL3.dll");

    // Hookear SDL_PollEvent - LA solucion real para el input
    InstallSDLHook();

    bImGuiReady = true;
    OutputDebugStringA("[+] ImGui + SDL_PollEvent hook listos\n");
}

void ImGui_Renderer::Render(ID3D11DeviceContext* pDeviceContext, ID3D11RenderTargetView* pRenderTargetView)
{
    if (!bImGuiReady || !bShowMenu) return;

    ImGuiIO& io = ImGui::GetIO();

    // Usar el tamano REAL del backbuffer para ImGui (coincide con la resolucion de render)
    if (g_BackBufferWidth > 0 && g_BackBufferHeight > 0)
        io.DisplaySize = {(float)g_BackBufferWidth, (float)g_BackBufferHeight};

    // Actualizar raton usando Win32 cada frame y escalarlo a coordenadas de backbuffer
    HWND hWnd = GetCS2Window();
    if (hWnd && io.DisplaySize.x > 0.0f && io.DisplaySize.y > 0.0f)
    {
        RECT client{};
        if (GetClientRect(hWnd, &client))
        {
            float clientW = (float)(client.right - client.left);
            float clientH = (float)(client.bottom - client.top);

            POINT p;
            if (GetCursorPos(&p))
            {
                ScreenToClient(hWnd, &p);
                float x = (float)p.x;
                float y = (float)p.y;

                if (clientW > 0.0f && clientH > 0.0f)
                {
                    float scaleX = io.DisplaySize.x / clientW;
                    float scaleY = io.DisplaySize.y / clientH;
                    x *= scaleX;
                    y *= scaleY;
                }

                io.MousePos = {x, y};
            }
        }

        io.MouseDown[0] = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
        io.MouseDown[1] = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
        io.MouseDown[2] = (GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0;
    }

    static DWORD lastTime = GetTickCount();
    DWORD now = GetTickCount();
    io.DeltaTime = max((now - lastTime) / 1000.f, 1.f / 60.f);
    lastTime = now;

    io.MouseDrawCursor = true;

    ImGui_ImplDX11_NewFrame();
    ImGui::NewFrame();
    DrawMainMenu();
    ImGui::Render();
    pDeviceContext->OMSetRenderTargets(1, &pRenderTargetView, NULL);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ImGui_Renderer::DrawMenu() { DrawMainMenu(); }

void ImGui_Renderer::Shutdown()
{
    if (!bImGuiReady) return;
    RemoveSDLHook();
    ImGui_ImplDX11_Shutdown();
    ImGui::DestroyContext();
    bImGuiReady = false;
}