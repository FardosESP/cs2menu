#include "Features.h"
#include "Memory.h"
#include "main.h"
#include "imgui.h"
#include "OffsetManager.h"
#include "Aimbot.h"
#include "OffsetScanner.h"
#include "LocalPlayer.h"
#include "EntityCache.h"
#include "EntityCachePro.h"
#include "SkinChanger.h"
#include "AntiAim.h"
#include "Resolver.h"
#include "Backtrack.h"
#include "DiagnosticSystem.h"
#include <iostream>
#include <cmath>

// Configuraciones externas (definidas en ImGui_Renderer.cpp)
extern struct ESPConfig {
    bool enabled, boxes, boxFilled, skeleton;
    bool health, healthBar, name, distance;
    bool weapon, snaplines, dormantCheck, teamCheck;
    bool glow;
    float boxColor[4], teamColor[4];
    float skeletonColor[4], snaplineColor[4];
    float glowEnemyColor[4], glowTeamColor[4];
    float maxDistance, boxThickness;
} cfg_esp;

extern struct AimbotConfig {
    bool enabled, visibleOnly, teamCheck;
    bool autoShoot, silentAim, rcs;
    float fov, smooth, rcsX, rcsY;
    int bone;
} cfg_aim;

extern struct VisualsConfig {
    bool nightmode, noFlash, noSmoke;
    bool fullbright, crosshair, fovChanger, thirdPerson;
    float fovValue, brightness, crosshairSize, crosshairGap;
    float crosshairColor[4];
} cfg_vis;

extern struct MiscConfig {
    bool bhop, autoStrafe, radarHack, speedhack, antiAim;
    float speedValue;
} cfg_misc;

extern struct AntiAimConfig {
    bool enabled;
    int pitchType, yawType;
    float yawOffset, jitterRange, spinSpeed;
    bool fakeLag;
    int fakeLagTicks;
    bool freestanding, edgeAA;
    int manualAA;
} cfg_antiaim;

extern struct ResolverConfig {
    bool enabled;
    int resolverType;
    bool autoResolve, showMisses;
} cfg_resolver;

extern struct BacktrackConfig {
    bool enabled;
    int maxTicks;
    bool visualize;
    float visualizeColor[4];
} cfg_backtrack;

extern struct SkinConfig {
    int knifeModel, knifeFinish;
    bool forceKnife, forceSkins;
    int selectedSkin[30], selectedWear[30], selectedSeed[30];
    float floatValue[30];
} cfg_skin;

// Variables locales
static uintptr_t g_clientBase = 0;
static bool g_bInitialized = false;

void Features::Initialize()
{
    std::cout << "  [→] Inicializando sistema de features..." << std::endl;
    
    // Initialize OffsetManager first
    if (!OffsetManager::Instance().Initialize())
    {
        std::cout << "  [✗] Error al inicializar OffsetManager" << std::endl;
    }
    else
    {
        std::cout << "  [✓] OffsetManager: " << OffsetManager::Instance().GetOffsetCount() 
                  << " offsets cargados" << std::endl;
        std::cout << "  [i] Version: " << OffsetManager::Instance().GetGameVersion() << std::endl;
        
        // Validate offsets
        if (OffsetManager::Instance().ValidateOffsets())
        {
            std::cout << "  [✓] Validacion de offsets exitosa" << std::endl;
        }
        else
        {
            std::cout << "  [!] Advertencia: Algunos offsets pueden ser invalidos" << std::endl;
        }
    }
    
    // Obtener base de client.dll
    g_clientBase = Memory::GetModuleBase("client.dll");
    if (!g_clientBase)
    {
        std::cout << "  [✗] No se pudo obtener client.dll" << std::endl;
        return;
    }
    
    std::cout << "  [✓] client.dll base: 0x" << std::hex << g_clientBase << std::dec << std::endl;
    
    // Verificar información del proceso
    DWORD pid = GetCurrentProcessId();
    char processName[MAX_PATH] = {};
    GetModuleFileNameA(NULL, processName, MAX_PATH);
    
    std::cout << "\n  +===========================================================+\n";
    std::cout << "  |              INFORMACION DEL PROCESO                      |\n";
    std::cout << "  +===========================================================+\n";
    std::cout << "  [i] PID: " << pid << std::endl;
    std::cout << "  [i] Ejecutable: " << processName << std::endl;
    std::cout << "  [i] client.dll: 0x" << std::hex << g_clientBase << std::dec << std::endl;
    
    // Verificar que client.dll es válido leyendo los primeros bytes (MZ header)
    // Note: Removed __try/__except due to C++ object unwinding conflicts
    // If this crashes, it means client.dll base is invalid
    uint16_t mzHeader = Memory::Read<uint16_t>(g_clientBase);
    if (mzHeader == 0x5A4D) // "MZ"
    {
        std::cout << "  [✓] client.dll header valido (MZ signature)" << std::endl;
    }
    else
    {
        std::cout << "  [✗] ERROR: client.dll header invalido! (0x" << std::hex << mzHeader << std::dec << ")" << std::endl;
        std::cout << "  [!] La direccion base es incorrecta" << std::endl;
    }
    std::cout << std::endl;
    
    // Inicializar punteros del SDK usando offsets con validación
    uintptr_t entitySystemAddr = g_clientBase + Offsets::dwEntityList();
    uintptr_t viewMatrixAddr = g_clientBase + Offsets::dwViewMatrix();
    
    // Validate addresses before assigning
    if (!Memory::IsValidPointer(entitySystemAddr))
    {
        std::cout << "[-] EntitySystem address is invalid" << std::endl;
        return;
    }
    
    if (!Memory::IsValidPointer(viewMatrixAddr))
    {
        std::cout << "[-] ViewMatrix address is invalid" << std::endl;
        return;
    }
    
    g_pEntitySystem = (C_CSGameEntitySystem*)entitySystemAddr;
    g_pViewMatrix = (ViewMatrix*)viewMatrixAddr;
    
    std::cout << "  [✓] EntitySystem: 0x" << std::hex << (uintptr_t)g_pEntitySystem << std::dec << std::endl;
    std::cout << "  [✓] ViewMatrix: 0x" << std::hex << (uintptr_t)g_pViewMatrix << std::dec << std::endl;
    
    // Initialize professional LocalPlayer system
    g_LocalPlayer.Initialize(g_clientBase, g_pEntitySystem);
    std::cout << "  [✓] LocalPlayer system initialized (multi-fallback detection)" << std::endl;
    
    // Don't run scanner automatically - wait until in-game
    std::cout << "\n  +===========================================================+\n";
    std::cout << "  |                  SISTEMA LISTO                            |\n";
    std::cout << "  +===========================================================+\n";
    std::cout << "  [i] Deteccion automatica: ACTIVADA" << std::endl;
    std::cout << "  [i] F9  - Escanear offsets manualmente" << std::endl;
    std::cout << "  [i] F10 - Ejecutar diagnostico completo" << std::endl;
    std::cout << "  [i] Funciona en: Online, Offline, Bots, Practica" << std::endl;
    
    g_bInitialized = true;
    std::cout << "  [OK] Todas las features inicializadas\n" << std::endl;
}

void Features::Update()
{
    if (!g_bInitialized || !g_clientBase) return;
    
    static bool firstRun = true;
    if (firstRun)
    {
        std::cout << "[DEBUG] Features::Update() - Primera ejecucion" << std::endl;
        std::cout << "[DEBUG] g_clientBase: 0x" << std::hex << g_clientBase << std::dec << std::endl;
        std::cout << "[DEBUG] g_pEntitySystem: 0x" << std::hex << (uintptr_t)g_pEntitySystem << std::dec << std::endl;
        std::cout << "[DEBUG] g_pViewMatrix: 0x" << std::hex << (uintptr_t)g_pViewMatrix << std::dec << std::endl;
        firstRun = false;
    }
    
    // Check for F9 key to manually run scanner
    static bool f9WasPressed = false;
    bool f9IsPressed = (GetAsyncKeyState(VK_F9) & 0x8000) != 0;
    if (f9IsPressed && !f9WasPressed)
    {
        std::cout << "\n[*] F9 presionado - Ejecutando scanner de offsets..." << std::endl;
        OffsetScanner::ScanAndPrintOffsets();
        g_LocalPlayer.SetScannerRan(true);
    }
    f9WasPressed = f9IsPressed;
    
    // Check for F10 key to run full diagnostic
    static bool f10WasPressed = false;
    bool f10IsPressed = (GetAsyncKeyState(VK_F10) & 0x8000) != 0;
    if (f10IsPressed && !f10WasPressed)
    {
        std::cout << "\n[*] F10 presionado - Ejecutando diagnostico completo..." << std::endl;
        DiagnosticSystem::RunFullDiagnostic(g_clientBase, g_pEntitySystem);
        DiagnosticSystem::TestESP(g_clientBase, g_pEntitySystem);
    }
    f10WasPressed = f10IsPressed;
    
    // Check for F11 key to test entity formulas (ESP player detection fix)
    static bool f11WasPressed = false;
    bool f11IsPressed = (GetAsyncKeyState(VK_F11) & 0x8000) != 0;
    if (f11IsPressed && !f11WasPressed)
    {
        std::cout << "\n[*] F11 pressed - Testing entity formulas..." << std::endl;
        DiagnosticSystem::TestEntityFormulas(g_clientBase, g_pEntitySystem);
    }
    f11WasPressed = f11IsPressed;
    
    // PROFESSIONAL METHOD: Get LocalPlayer safely
    // This handles NULL during team selection/respawn
    C_CSPlayerPawn* pLocalPlayer = g_LocalPlayer.GetSafeLocalPlayer();
    
    // If LocalPlayer is NULL, skip this frame (SAFE - no crash)
    // This happens during team selection, respawn, etc.
    if (!pLocalPlayer)
    {
        static bool nullWarned = false;
        static int nullCount = 0;
        
        nullCount++;
        
        // Log only once every 300 frames (5 seconds @ 60fps)
        if (!nullWarned || nullCount % 300 == 0)
        {
            std::cout << "[INFO] LocalPlayer NULL - skipping frame (normal during team select/respawn)" << std::endl;
            nullWarned = true;
        }
        
        return; // SAFE EXIT - No crash
    }
    
    // LocalPlayer is valid - reset warning
    static bool validWarned = false;
    if (!validWarned)
    {
        std::cout << "[INFO] LocalPlayer VALID - features active" << std::endl;
        validWarned = true;
    }
    
    // Execute features only if we have valid local player
    // IMPORTANT: Features will ONLY run when manually enabled from menu
    if (pLocalPlayer)
    {
        // ESP - Only runs when user enables it from menu
        __try
        {
            if (cfg_esp.enabled)
            {
                static bool firstESPRun = true;
                if (firstESPRun)
                {
                    std::cout << "[ESP] ESP activado por primera vez - ejecutando..." << std::endl;
                    firstESPRun = false;
                }
                RenderESP();
            }
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
            std::cout << "[ERROR] ESP crashed!" << std::endl;
        }
        
        /* DISABLED FEATURES - Enable one by one after ESP works
        
        // Aimbot
        __try
        {
            if (cfg_aim.enabled)
                RunAimbot();
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
            std::cout << "[ERROR] Aimbot crashed!" << std::endl;
        }
        
        // Visuals
        __try
        {
            ApplyVisuals();
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
            std::cout << "[ERROR] Visuals crashed!" << std::endl;
        }
        
        // Misc (Bhop, etc)
        __try
        {
            RunMisc();
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
            std::cout << "[ERROR] Misc crashed!" << std::endl;
        }
        
        // Skins
        __try
        {
            ApplySkins();
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
            std::cout << "[ERROR] Skins crashed!" << std::endl;
        }
        
        // Anti-Aim
        __try
        {
            if (cfg_antiaim.enabled)
            {
                uintptr_t viewAnglesAddr = g_clientBase + Offsets::dwViewAngles();
                Vector3 viewAngles = Memory::Read<Vector3>(viewAnglesAddr);
                
                AntiAim::Instance().enabled = true;
                AntiAim::Instance().pitchType = (AntiAim::PitchType)cfg_antiaim.pitchType;
                AntiAim::Instance().yawType = (AntiAim::YawType)cfg_antiaim.yawType;
                AntiAim::Instance().yawOffset = cfg_antiaim.yawOffset;
                AntiAim::Instance().jitterRange = cfg_antiaim.jitterRange;
                AntiAim::Instance().spinSpeed = cfg_antiaim.spinSpeed;
                AntiAim::Instance().fakeLagEnabled = cfg_antiaim.fakeLag;
                
                AntiAim::Instance().Apply(pLocalPlayer, viewAngles);
                Memory::Write<Vector3>(viewAnglesAddr, viewAngles);
                
                if (cfg_antiaim.fakeLag)
                {
                    AntiAim::Instance().FakeLag(cfg_antiaim.fakeLagTicks);
                }
            }
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
            std::cout << "[ERROR] AntiAim crashed!" << std::endl;
        }
        
        // Resolver
        __try
        {
            if (cfg_resolver.enabled)
            {
                int maxEntities = g_pEntitySystem->GetHighestEntityIndex();
                if (maxEntities > 0 && maxEntities < 8192)
                {
                    for (int i = 1; i <= 64; i++)
                    {
                        C_BaseEntity* controller = g_pEntitySystem->GetBaseEntity(i);
                        if (!controller || !Memory::IsValidPointer((uintptr_t)controller)) continue;
                        
                        C_CSPlayerPawn* pawn = g_pEntitySystem->GetPlayerPawn(controller);
                        if (!pawn || !Memory::IsValidPointer((uintptr_t)pawn)) continue;
                        
                        if (pawn == pLocalPlayer) continue;
                        
                        Resolver::Instance().Update(pawn);
                    }
                }
            }
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
            std::cout << "[ERROR] Resolver crashed!" << std::endl;
        }
        
        // Backtrack
        __try
        {
            if (cfg_backtrack.enabled)
            {
                int maxEntities = g_pEntitySystem->GetHighestEntityIndex();
                if (maxEntities > 0 && maxEntities < 8192)
                {
                    for (int i = 1; i <= 64; i++)
                    {
                        C_BaseEntity* controller = g_pEntitySystem->GetBaseEntity(i);
                        if (!controller || !Memory::IsValidPointer((uintptr_t)controller)) continue;
                        
                        C_CSPlayerPawn* pawn = g_pEntitySystem->GetPlayerPawn(controller);
                        if (!pawn || !Memory::IsValidPointer((uintptr_t)pawn)) continue;
                        
                        if (pawn == pLocalPlayer) continue;
                        
                        Backtrack::Instance().Update(pawn);
                    }
                }
            }
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
            std::cout << "[ERROR] Backtrack crashed!" << std::endl;
        }
        
        */ // END DISABLED FEATURES
    }
    
    // Custom crosshair - DISABLED for testing
    // Only enable from menu if needed
    /*
    if (cfg_vis.crosshair)
    {
        __try
        {
            DrawCustomCrosshair();
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
            // Silently ignore crosshair crashes
        }
    }
    */
}

bool Features::WorldToScreen(const Vector3& world, Vector3& screen)
{
    // Validate ViewMatrix pointer
    if (!g_pViewMatrix || !Memory::IsValidPointer((uintptr_t)g_pViewMatrix))
    {
        return false;
    }
    
    __try
    {
        float* matrix = g_pViewMatrix->matrix;
        
        float w = matrix[12] * world.x + matrix[13] * world.y + matrix[14] * world.z + matrix[15];
        
        if (w < 0.001f)
            return false;
        
        float invW = 1.0f / w;
        screen.x = (matrix[0] * world.x + matrix[1] * world.y + matrix[2] * world.z + matrix[3]) * invW;
        screen.y = (matrix[4] * world.x + matrix[5] * world.y + matrix[6] * world.z + matrix[7]) * invW;
        
        ImGuiIO& io = ImGui::GetIO();
        float width = io.DisplaySize.x;
        float height = io.DisplaySize.y;
        
        screen.x = (width / 2.0f) + (screen.x * width) / 2.0f;
        screen.y = (height / 2.0f) - (screen.y * height) / 2.0f;
        
        return true;
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        return false;
    }
}

void Features::DrawBox(const Vector3& head, const Vector3& feet, bool filled, float thickness, float color[4])
{
    float height = feet.y - head.y;
    float width = height / 2.0f;
    
    ImVec2 topLeft(head.x - width / 2, head.y);
    ImVec2 bottomRight(head.x + width / 2, feet.y);
    
    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
    ImU32 col = ImGui::ColorConvertFloat4ToU32(ImVec4(color[0], color[1], color[2], color[3]));
    
    if (filled)
    {
        ImU32 fillCol = ImGui::ColorConvertFloat4ToU32(ImVec4(color[0], color[1], color[2], color[3] * 0.3f));
        drawList->AddRectFilled(topLeft, bottomRight, fillCol);
    }
    
    drawList->AddRect(topLeft, bottomRight, col, 0.0f, 0, thickness);
}

void Features::DrawSkeleton(C_CSPlayerPawn* pawn, float color[4])
{
    if (!pawn) return;
    
    // CS2 bone IDs (approximate - may need adjustment based on actual game data)
    // These are common bone indices for Source 2 player models
    const int HEAD = 6;
    const int NECK = 5;
    const int SPINE_3 = 4;
    const int SPINE_2 = 3;
    const int SPINE_1 = 2;
    const int PELVIS = 0;
    
    const int LEFT_SHOULDER = 8;
    const int LEFT_ELBOW = 9;
    const int LEFT_HAND = 10;
    
    const int RIGHT_SHOULDER = 13;
    const int RIGHT_ELBOW = 14;
    const int RIGHT_HAND = 15;
    
    const int LEFT_HIP = 22;
    const int LEFT_KNEE = 23;
    const int LEFT_FOOT = 24;
    
    const int RIGHT_HIP = 25;
    const int RIGHT_KNEE = 26;
    const int RIGHT_FOOT = 27;
    
    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
    if (!drawList) return;
    
    ImU32 col = ImGui::ColorConvertFloat4ToU32(ImVec4(color[0], color[1], color[2], color[3]));
    float thickness = cfg_esp.boxThickness;
    
    // Helper struct to hold bone pairs
    struct BonePair { int bone1; int bone2; };
    
    // Define all bone connections
    BonePair bones[] = {
        // Spine
        {PELVIS, SPINE_1}, {SPINE_1, SPINE_2}, {SPINE_2, SPINE_3}, {SPINE_3, NECK}, {NECK, HEAD},
        // Left arm
        {SPINE_3, LEFT_SHOULDER}, {LEFT_SHOULDER, LEFT_ELBOW}, {LEFT_ELBOW, LEFT_HAND},
        // Right arm
        {SPINE_3, RIGHT_SHOULDER}, {RIGHT_SHOULDER, RIGHT_ELBOW}, {RIGHT_ELBOW, RIGHT_HAND},
        // Left leg
        {PELVIS, LEFT_HIP}, {LEFT_HIP, LEFT_KNEE}, {LEFT_KNEE, LEFT_FOOT},
        // Right leg
        {PELVIS, RIGHT_HIP}, {RIGHT_HIP, RIGHT_KNEE}, {RIGHT_KNEE, RIGHT_FOOT}
    };
    
    // Draw all bone connections
    for (int i = 0; i < sizeof(bones) / sizeof(BonePair); i++)
    {
        Vector3 pos1 = pawn->GetBonePosition(bones[i].bone1);
        Vector3 pos2 = pawn->GetBonePosition(bones[i].bone2);
        
        // Skip if positions are invalid
        if ((pos1.x == 0.0f && pos1.y == 0.0f && pos1.z == 0.0f) ||
            (pos2.x == 0.0f && pos2.y == 0.0f && pos2.z == 0.0f))
            continue;
        
        Vector3 screen1, screen2;
        if (WorldToScreen(pos1, screen1) && WorldToScreen(pos2, screen2))
        {
            drawList->AddLine(
                ImVec2(screen1.x, screen1.y),
                ImVec2(screen2.x, screen2.y),
                col,
                thickness
            );
        }
    }
}

void Features::RenderESP()
{
    // ═══════════════════════════════════════════════════════════════════════════
    // PREMIUM ESP - Chunk-Based Entity Iteration (Neverlose/Onetap 2025 Method)
    // ═══════════════════════════════════════════════════════════════════════════
    // Formula: chunk = entitySystem + 0x10 + ((i >> 9) * 8)
    //          entity = chunk + ((i & 0x1FF) * 0x78)
    // 
    // Why this works:
    // - CS2 Build 14138.6 uses chunk-based entity storage
    // - Entities divided into chunks of 512 (0x200)
    // - Each entity has 0x78 byte stride within chunk
    // - This is the PROVEN method used by premium paid cheats
    // ═══════════════════════════════════════════════════════════════════════════
    
    __try
    {
        if (!g_pEntitySystem || !g_pViewMatrix)
            return;
        
        C_CSPlayerPawn* pLocalPlayer = g_LocalPlayer.GetSafeLocalPlayer();
        if (!pLocalPlayer)
            return;
        
        static bool firstRun = true;
        if (firstRun)
        {
            std::cout << "[ESP-PREMIUM] Chunk-Based Entity Iteration (Build 14138.6 Compatible)" << std::endl;
            firstRun = false;
        }
        
        // Get LocalPlayer data
        int localHealth = 0, localTeam = 0;
        if (!g_LocalPlayer.GetLocalPlayerHealth(localHealth) || 
            !g_LocalPlayer.GetLocalPlayerTeam(localTeam))
            return;
        
        // Scan every 10 frames for performance
        static int scanCounter = 0;
        scanCounter++;
        if (scanCounter % 10 != 0)
            return;
        
        int validPlayers = 0;
        uintptr_t entitySystemBase = (uintptr_t)g_pEntitySystem;
        uintptr_t localPlayerPtr = (uintptr_t)pLocalPlayer;
        
        // PREMIUM: Chunk-based entity iteration (Neverlose method)
        // Scan player slots 1-64 (index 0 is world entity)
        for (int i = 1; i <= 64; i++)
        {
            __try
            {
                // ═══════════════════════════════════════════════════════════════
                // STEP 1: Get chunk pointer
                // Formula: entitySystem + 0x10 + ((i >> 9) * 8)
                // Divides entity index by 512 to get chunk index
                // ═══════════════════════════════════════════════════════════════
                uintptr_t chunkAddr = entitySystemBase + 0x10 + ((i >> 9) * 8);
                if (IsBadReadPtr((void*)chunkAddr, sizeof(uintptr_t)))
                    continue;
                
                uintptr_t chunk = *(uintptr_t*)chunkAddr;
                if (!chunk || chunk < 0x10000)
                    continue;
                
                // ═══════════════════════════════════════════════════════════════
                // STEP 2: Get entity pointer from chunk
                // Formula: chunk + ((i & 0x1FF) * 0x78)
                // Gets index within chunk (0-511) and multiplies by stride
                // ═══════════════════════════════════════════════════════════════
                uintptr_t entityPtrAddr = chunk + ((i & 0x1FF) * 0x78);
                if (IsBadReadPtr((void*)entityPtrAddr, sizeof(uintptr_t)))
                    continue;
                
                uintptr_t entity = *(uintptr_t*)entityPtrAddr;
                if (!entity || entity < 0x10000 || entity > 0x7FFFFFFFFFFF)
                    continue;
                
                // ═══════════════════════════════════════════════════════════════
                // STEP 3: Validate entity is a player pawn
                // ═══════════════════════════════════════════════════════════════
                
                // Skip local player
                if (entity == localPlayerPtr)
                    continue;
                
                // IsBadReadPtr check for safety (prevent crashes)
                if (IsBadReadPtr((void*)entity, 0x400))
                    continue;
                
                // Validate health (1-100 = alive player)
                uintptr_t healthAddr = entity + Offsets::m_iHealth();
                if (IsBadReadPtr((void*)healthAddr, sizeof(int)))
                    continue;
                
                int health = *(int*)healthAddr;
                if (health <= 0 || health > 100)
                    continue;
                
                // Validate team (2=T, 3=CT)
                uintptr_t teamAddr = entity + Offsets::m_iTeamNum();
                if (IsBadReadPtr((void*)teamAddr, sizeof(int)))
                    continue;
                
                int team = *(int*)teamAddr;
                if (team < 2 || team > 3)
                    continue;
                
                // Team check (skip teammates if enabled)
                if (cfg_esp.teamCheck && team == localTeam)
                    continue;
                
                // ═══════════════════════════════════════════════════════════════
                // VALID PLAYER FOUND - Proceed to ESP rendering
                // ═══════════════════════════════════════════════════════════════
                validPlayers++;
                
                C_CSPlayerPawn* pawn = (C_CSPlayerPawn*)entity;
                
                // Get origin
                Vector3 origin;
                if (!pawn->GetOriginSafe(origin))
                    continue;
                
                // Head/feet positions
                Vector3 headPos = {origin.x, origin.y, origin.z + 75.0f};
                Vector3 feetPos = origin;
                
                // WorldToScreen
                Vector3 screenHead, screenFeet;
                if (!WorldToScreen(headPos, screenHead) || !WorldToScreen(feetPos, screenFeet))
                    continue;
                
                // Draw box
                if (cfg_esp.boxes)
                {
                    float* color = (team == localTeam) ? cfg_esp.teamColor : cfg_esp.boxColor;
                    DrawBox(screenHead, screenFeet, cfg_esp.boxFilled, cfg_esp.boxThickness, color);
                }
                
                // Draw health bar
                if (cfg_esp.healthBar)
                {
                    float height = screenFeet.y - screenHead.y;
                    float width = height / 2.0f;
                    float barWidth = 4.0f;
                    float barHeight = height * (health / 100.0f);
                    
                    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
                    if (!drawList) continue;
                    
                    // Background
                    drawList->AddRectFilled(
                        ImVec2(screenHead.x - width / 2 - barWidth - 2, screenHead.y),
                        ImVec2(screenHead.x - width / 2 - 2, screenFeet.y),
                        IM_COL32(0, 0, 0, 180)
                    );
                    
                    // Health bar
                    float healthPercent = health / 100.0f;
                    ImU32 healthColor = IM_COL32(
                        (int)(255 * (1.0f - healthPercent)),
                        (int)(255 * healthPercent),
                        0,
                        255
                    );
                    
                    drawList->AddRectFilled(
                        ImVec2(screenHead.x - width / 2 - barWidth - 2, screenFeet.y - barHeight),
                        ImVec2(screenHead.x - width / 2 - 2, screenFeet.y),
                        healthColor
                    );
                }
            }
            __except(EXCEPTION_EXECUTE_HANDLER)
            {
                continue;
            }
        }
        
        // Log only when count changes
        static int lastValidCount = -1;
        if (validPlayers != lastValidCount)
        {
            std::cout << "[ESP-PREMIUM] Found " << validPlayers << " players" << std::endl;
            lastValidCount = validPlayers;
        }
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        // Silent fail
    }
}

void Features::RunAimbot()
{
    if (!g_clientBase || !g_pEntitySystem)
        return;
    
    if (!cfg_aim.enabled)
        return;
    
    // Get LocalPlayer from professional system
    C_CSPlayerPawn* pLocalPlayer = g_LocalPlayer.GetPawn();
    if (!pLocalPlayer)
        return;
    
    __try
    {
        // Get best target using Aimbot class
        float bestFov = 0.0f;
        C_CSPlayerPawn* target = Aimbot::Instance().GetBestTarget(
            pLocalPlayer, 
            g_pEntitySystem,
            cfg_aim.fov,
            cfg_aim.visibleOnly,
            cfg_aim.teamCheck,
            bestFov
        );
        
        if (!target)
            return;
        
        // Get target bone position (head by default)
        Vector3 targetPos = Aimbot::Instance().GetBonePosition(target, cfg_aim.bone);
        
        // Calculate aim angles
        Vector3 localEyePos = pLocalPlayer->GetEyePosition();
        Vector3 aimAngles = Aimbot::Instance().CalculateAngle(localEyePos, targetPos);
        
        // Get current view angles
        uintptr_t viewAnglesAddr = g_clientBase + Offsets::dwViewAngles();
        Vector3 currentAngles = Memory::Read<Vector3>(viewAnglesAddr);
        
        // Apply smooth aim if configured
        Vector3 finalAngles = aimAngles;
        if (cfg_aim.smooth > 1.0f)
        {
            finalAngles = Aimbot::Instance().SmoothAim(currentAngles, aimAngles, cfg_aim.smooth);
        }
        
        // Write new view angles
        if (!cfg_aim.silentAim)
        {
            Memory::Write<Vector3>(viewAnglesAddr, finalAngles);
        }
        
        // Auto-shoot if enabled and on target
        if (cfg_aim.autoShoot && bestFov < 1.0f)
        {
            // TODO: Implement mouse click injection
        }
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        // Silently handle exceptions
    }
}

C_CSPlayerPawn* Features::GetBestTarget(float& bestFov)
{
    return nullptr;
}

void Features::AimAt(const Vector3& target, float smooth)
{
    // Placeholder
}

void Features::ApplyVisuals()
{
    C_CSPlayerPawn* pLocalPlayer = g_LocalPlayer.GetPawn();
    if (!pLocalPlayer) return;
    
    __try
    {
        // FOV Changer
        if (cfg_vis.fovChanger && cfg_vis.fovValue != 90.0f)
        {
            UpdateFOV();
        }
        
        // No Flash
        if (cfg_vis.noFlash)
        {
            float flashDuration = pLocalPlayer->GetFlashDuration();
            if (flashDuration > 0.0f)
            {
                // Establecer duración del flash a 0
                *(float*)((uintptr_t)pLocalPlayer + Offsets::m_flFlashDuration()) = 0.0f;
            }
        }
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        // Ignorar errores
    }
}

void Features::UpdateFOV()
{
    C_CSPlayerPawn* pLocalPlayer = g_LocalPlayer.GetPawn();
    if (!pLocalPlayer || !g_clientBase) return;
    
    __try
    {
        // Obtener el controller del jugador local
        uintptr_t controllerAddr = g_clientBase + Offsets::dwLocalPlayerController();
        uintptr_t controller = Memory::Read<uintptr_t>(controllerAddr);
        
        if (controller != 0)
        {
            // m_iDesiredFOV está en el controller
            uint32_t* fovPtr = (uint32_t*)(controller + 0x78C); // m_iDesiredFOV offset
            uint32_t desiredFOV = (uint32_t)cfg_vis.fovValue;
            
            if (*fovPtr != desiredFOV)
            {
                *fovPtr = desiredFOV;
            }
        }
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        // Ignorar errores
    }
}

void Features::UpdateBrightness()
{
    // Placeholder
}

void Features::RunMisc()
{
    if (cfg_misc.bhop)
        BunnyHop();
    
    if (cfg_misc.radarHack)
        RadarHack();
}

void Features::BunnyHop()
{
    C_CSPlayerPawn* pLocalPlayer = g_LocalPlayer.GetPawn();
    if (!pLocalPlayer) return;
    
    __try
    {
        // Check if space bar is being held
        if (GetAsyncKeyState(VK_SPACE) & 0x8000)
        {
            // Check if player is on the ground
            int flags = pLocalPlayer->GetFlags();
            bool onGround = (flags & Offsets::FL_ONGROUND) != 0;
            
            if (onGround)
            {
                // Force jump by setting the jump flag
                uintptr_t buttonAddr = g_clientBase + Offsets::dwForceJump();
                
                // Write jump command (65537 = jump pressed, 256 = jump released)
                Memory::Write<int>(buttonAddr, 65537);
                
                // Small delay then release
                Sleep(1);
                Memory::Write<int>(buttonAddr, 256);
            }
        }
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        // Ignore errors
    }
}

void Features::RadarHack()
{
    if (!g_pEntitySystem || !Memory::IsValidPointer((uintptr_t)g_pEntitySystem))
    {
        return;
    }
    
    __try
    {
        // Obtener el índice máximo de entidades
        int maxEntities = g_pEntitySystem->GetHighestEntityIndex();
        if (maxEntities <= 0 || maxEntities > 8192) maxEntities = 64;
        
        // Iterar sobre todos los jugadores (Controllers) - aumentado a 2048
        int scanLimit = (maxEntities > 2048) ? 2048 : maxEntities;
        for (int i = 1; i <= scanLimit; i++)
        {
            // Get controller
            C_BaseEntity* controller = g_pEntitySystem->GetBaseEntity(i);
            if (!controller) continue;
            
            // Validate controller pointer
            if (!Memory::IsValidPointer((uintptr_t)controller)) continue;
            
            // Get pawn from controller
            C_CSPlayerPawn* pawn = g_pEntitySystem->GetPlayerPawn(controller);
            if (!pawn) continue;
            
            // Validate pawn pointer
            if (!Memory::IsValidPointer((uintptr_t)pawn)) continue;
            
            // Use safe validation
            if (!pawn->IsValid()) continue;
            
            // Validar que es un jugador válido
            int health = pawn->GetHealth();
            if (health <= 0 || health > 100) continue;
            
            // Marcar como spotted (visible en el radar)
            if (!pawn->IsSpotted())
            {
                pawn->SetSpotted(true);
            }
        }
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        // Ignorar errores
    }
}

void Features::DrawCustomCrosshair()
{
    ImGuiIO& io = ImGui::GetIO();
    float cx = io.DisplaySize.x / 2.0f;
    float cy = io.DisplaySize.y / 2.0f;
    float sz = cfg_vis.crosshairSize;
    float gap = cfg_vis.crosshairGap;
    
    ImU32 col = ImGui::ColorConvertFloat4ToU32(ImVec4(
        cfg_vis.crosshairColor[0], cfg_vis.crosshairColor[1],
        cfg_vis.crosshairColor[2], cfg_vis.crosshairColor[3]
    ));
    
    ImDrawList* dl = ImGui::GetBackgroundDrawList();
    if (!dl) return;
    
    // Líneas horizontales y verticales
    dl->AddLine(ImVec2(cx - sz - gap, cy), ImVec2(cx - gap, cy), col, 2.0f);
    dl->AddLine(ImVec2(cx + gap, cy), ImVec2(cx + sz + gap, cy), col, 2.0f);
    dl->AddLine(ImVec2(cx, cy - sz - gap), ImVec2(cx, cy - gap), col, 2.0f);
    dl->AddLine(ImVec2(cx, cy + gap), ImVec2(cx, cy + sz + gap), col, 2.0f);
}

void Features::ApplySkins()
{
    C_CSPlayerPawn* pLocalPlayer = g_LocalPlayer.GetPawn();
    if (!pLocalPlayer) return;
    
    __try
    {
        // Apply knife skin if enabled
        if (cfg_skin.forceKnife)
        {
            uint32_t weaponHandle = pLocalPlayer->GetActiveWeaponHandle();
            if (weaponHandle != 0 && weaponHandle != 0xFFFFFFFF)
            {
                int weaponIndex = weaponHandle & 0x7FFF;
                if (weaponIndex > 0 && weaponIndex < 8192)
                {
                    C_BaseEntity* weapon = g_pEntitySystem->GetBaseEntity(weaponIndex);
                    if (weapon && Memory::IsValidPointer((uintptr_t)weapon))
                    {
                        int defIndex = SkinChanger::Instance().GetWeaponDefIndex(weapon);
                        
                        // Check if it's a knife (def index 42, 59, 500-516)
                        if (defIndex == 42 || defIndex == 59 || (defIndex >= 500 && defIndex <= 516))
                        {
                            // Map knife model selection to actual knife IDs
                            int knifeModels[] = {507, 508, 500, 515, 505, 506, 509, 512, 516, 514, 519, 522, 523, 520, 503, 525};
                            int selectedModel = (cfg_skin.knifeModel >= 0 && cfg_skin.knifeModel < 16) ? 
                                                knifeModels[cfg_skin.knifeModel] : 507;
                            
                            // Map finish selection to paint kit IDs (simplified)
                            int finishIds[] = {0, 44, 12, 38, 418, 411, 413, 59, 42, 43, 37, 74};
                            int selectedFinish = (cfg_skin.knifeFinish >= 0 && cfg_skin.knifeFinish < 12) ?
                                                 finishIds[cfg_skin.knifeFinish] : 0;
                            
                            SkinChanger::Instance().ApplyKnifeSkin(weapon, selectedFinish, 0, 0.01f, selectedModel);
                        }
                    }
                }
            }
        }
        
        // Apply weapon skins if enabled
        if (cfg_skin.forceSkins)
        {
            uint32_t weaponHandle = pLocalPlayer->GetActiveWeaponHandle();
            if (weaponHandle != 0 && weaponHandle != 0xFFFFFFFF)
            {
                int weaponIndex = weaponHandle & 0x7FFF;
                if (weaponIndex > 0 && weaponIndex < 8192)
                {
                    C_BaseEntity* weapon = g_pEntitySystem->GetBaseEntity(weaponIndex);
                    if (weapon && Memory::IsValidPointer((uintptr_t)weapon))
                    {
                        int defIndex = SkinChanger::Instance().GetWeaponDefIndex(weapon);
                        
                        // Map weapon def index to our slot (simplified - would need full mapping)
                        // For now, just apply to slot 0 (AK-47) as example
                        static int lastAppliedWeapon = -1;
                        if (defIndex != lastAppliedWeapon && cfg_skin.selectedSkin[0] > 0)
                        {
                            float wear = cfg_skin.floatValue[0];
                            if (wear == 0.0f)
                            {
                                // Map wear selection to float value
                                float wearValues[] = {0.01f, 0.10f, 0.25f, 0.40f, 0.70f};
                                wear = (cfg_skin.selectedWear[0] >= 0 && cfg_skin.selectedWear[0] < 5) ?
                                       wearValues[cfg_skin.selectedWear[0]] : 0.01f;
                            }
                            
                            SkinChanger::Instance().ApplySkin(
                                weapon,
                                cfg_skin.selectedSkin[0],
                                cfg_skin.selectedSeed[0],
                                wear
                            );
                            
                            lastAppliedWeapon = defIndex;
                        }
                    }
                }
            }
        }
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        // Silently handle exceptions
    }
}
