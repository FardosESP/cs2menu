#include "Features.h"
#include "Memory.h"
#include "main.h"
#include "imgui.h"
#include "OffsetManager.h"
#include "Aimbot.h"
#include "OffsetScanner.h"
#include "LocalPlayer.h"
#include "EntityCache.h"
#include "SkinChanger.h"
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
    std::cout << "  [i] Escaneo manual: Presiona F9 si es necesario" << std::endl;
    std::cout << "  [i] Funciona en: Online, Offline, Bots, Practica" << std::endl;
    
    g_bInitialized = true;
    std::cout << "  [OK] Todas las features inicializadas\n" << std::endl;
}

void Features::Update()
{
    if (!g_bInitialized || !g_clientBase) return;
    
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
    
    // Update LocalPlayer with professional multi-fallback detection
    g_LocalPlayer.Update();
    
    // Get LocalPlayer pawn
    C_CSPlayerPawn* pLocalPlayer = g_LocalPlayer.GetPawn();
    
    // Run scanner automatically when entering game for the first time
    if (pLocalPlayer && !g_LocalPlayer.HasScannerRun())
    {
        std::cout << "[*] Ejecutando scanner de offsets automaticamente..." << std::endl;
        OffsetScanner::ScanAndPrintOffsets();
        g_LocalPlayer.SetScannerRan(true);
    }
    
    // Update entity cache (for performance)
    if (pLocalPlayer)
    {
        int localTeam = pLocalPlayer->GetTeamNum();
        g_EntityCache.Update(g_pEntitySystem, localTeam);
    }
    
    // Execute features only if we have valid local player
    if (pLocalPlayer)
    {
        if (cfg_esp.enabled)
            RenderESP();
        
        if (cfg_aim.enabled)
            RunAimbot();
        
        ApplyVisuals();
        RunMisc();
        ApplySkins();
    }
    
    // Custom crosshair is always drawn
    if (cfg_vis.crosshair)
        DrawCustomCrosshair();
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
    // Validate critical pointers before proceeding
    if (!g_pEntitySystem || !g_pViewMatrix)
        return;
    
    // Get LocalPlayer from professional system
    C_CSPlayerPawn* pLocalPlayer = g_LocalPlayer.GetPawn();
    if (!pLocalPlayer)
        return;
    
    // Validate that the local player is valid
    __try
    {
        if (!pLocalPlayer->IsValid() || pLocalPlayer->GetHealth() <= 0)
            return;
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        return;
    }
    
    int localTeam = 0;
    Vector3 localPos = {0, 0, 0};  // Inicializar con valores por defecto
    
    // Safely read local player data
    __try
    {
        localTeam = pLocalPlayer->GetTeamNum();
        
        // Intentar obtener posición, pero continuar si falla
        if (!pLocalPlayer->GetOriginSafe(localPos))
        {
            // Si falla, usar posición por defecto y continuar de todos modos
            localPos = {0, 0, 0};
            
            static int originFailCount = 0;
            originFailCount++;
            if (originFailCount % 300 == 0)
            {
                std::cout << "[ESP] GetOriginSafe() falla, pero continuando sin filtro de distancia" << std::endl;
            }
            // NO RETORNAR - continuar sin distancia
        }
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        localTeam = 2;  // Valor por defecto
        localPos = {0, 0, 0};
    }
    
    ImDrawList* drawList = ImGui::GetBackgroundDrawList();
    if (!drawList) return;
    
    // Safely get the maximum entity index
    int maxEntities = 0;
    __try
    {
        maxEntities = g_pEntitySystem->GetHighestEntityIndex();
        if (maxEntities <= 0 || maxEntities > 8192)
        {
            maxEntities = 64;
        }
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        maxEntities = 64;
    }
    
    int validControllers = 0;
    int validPawns = 0;
    int drawnEntities = 0;
    
    // Iterate through player CONTROLLERS (1-2048 for CS2 chunk system)
    // In CS2, we must iterate controllers first, then get their pawns
    // Players are typically in range 1-64, but we scan more for safety
    int scanLimit = (maxEntities > 2048) ? 2048 : maxEntities;
    for (int i = 1; i <= scanLimit; i++)
    {
        __try
        {
            // Step 1: Get controller entity
            C_BaseEntity* controller = g_pEntitySystem->GetBaseEntity(i);
            if (!controller) continue;
            
            // Validate controller pointer
            if (!Memory::IsValidPointer((uintptr_t)controller)) continue;
            
            validControllers++;
            
            // Step 2: Get pawn from controller using handle decoding
            C_CSPlayerPawn* pawn = g_pEntitySystem->GetPlayerPawn(controller);
            if (!pawn) continue;
            
            // Validate pawn pointer
            if (!Memory::IsValidPointer((uintptr_t)pawn)) continue;
            
            // Use safe validation method on pawn
            if (!pawn->IsValid()) continue;
            
            validPawns++;
            
            // Validate health range
            int health = pawn->GetHealth();
            if (health <= 0 || health > 100) continue;
            
            // Skip local player
            if (pawn == pLocalPlayer) continue;
            
            // Dormant check
            if (cfg_esp.dormantCheck && pawn->IsDormant()) continue;
            
            // Team check
            int team = pawn->GetTeamNum();
            if (cfg_esp.teamCheck && team == localTeam) continue;
            
            // Safely get entity origin
            Vector3 origin = {};
            bool hasOrigin = pawn->GetOriginSafe(origin);
            
            // Si no tenemos origin, SKIP este jugador
            if (!hasOrigin)
                continue;
            
            // Distance check
            float distance = 0.0f;
            if (localPos.x != 0 || localPos.y != 0 || localPos.z != 0)
            {
                float dx = origin.x - localPos.x;
                float dy = origin.y - localPos.y;
                float dz = origin.z - localPos.z;
                distance = sqrtf(dx*dx + dy*dy + dz*dz);
                
                if (distance > cfg_esp.maxDistance) continue;
            }
            else
            {
                // Si no tenemos posición local, no filtrar por distancia
                distance = 0.0f;
            }
            
            // Calculate screen positions
            Vector3 headPos = origin;
            headPos.z += 75.0f;
            Vector3 feetPos = origin;
            
            Vector3 screenHead, screenFeet;
            if (!WorldToScreen(headPos, screenHead)) continue;
            if (!WorldToScreen(feetPos, screenFeet)) continue;
            
            drawnEntities++;
            
            // Select color based on team
            float* boxColor = (team == localTeam) ? cfg_esp.teamColor : cfg_esp.boxColor;
            
            // Draw box
            if (cfg_esp.boxes)
            {
                DrawBox(screenHead, screenFeet, cfg_esp.boxFilled, cfg_esp.boxThickness, boxColor);
            }
            
            // Draw snaplines
            if (cfg_esp.snaplines)
            {
                ImGuiIO& io = ImGui::GetIO();
                ImVec2 screenCenter(io.DisplaySize.x / 2, io.DisplaySize.y);
                ImU32 col = ImGui::ColorConvertFloat4ToU32(ImVec4(
                    cfg_esp.snaplineColor[0], cfg_esp.snaplineColor[1],
                    cfg_esp.snaplineColor[2], cfg_esp.snaplineColor[3]
                ));
                drawList->AddLine(screenCenter, ImVec2(screenFeet.x, screenFeet.y), col, 1.0f);
            }
            
            // Draw info text
            float textY = screenHead.y - 15.0f;
            
            if (cfg_esp.health)
            {
                char healthText[32];
                sprintf_s(healthText, "HP: %d", health);
                drawList->AddText(ImVec2(screenHead.x - 20, textY), IM_COL32(255, 255, 255, 255), healthText);
                textY -= 15.0f;
            }
            
            if (cfg_esp.distance)
            {
                char distText[32];
                sprintf_s(distText, "%.0fm", distance / 50.0f);
                drawList->AddText(ImVec2(screenHead.x - 20, textY), IM_COL32(255, 255, 255, 255), distText);
                textY -= 15.0f;
            }
            
            // Draw player name (requires controller)
            if (cfg_esp.name)
            {
                __try
                {
                    // Get player controller from entity index
                    // Note: This is a simplified approach - may need adjustment
                    uintptr_t controllerAddr = g_clientBase + Offsets::dwLocalPlayerController();
                    uintptr_t controller = Memory::Read<uintptr_t>(controllerAddr);
                    
                    if (controller != 0)
                    {
                        C_CSPlayerController* pController = (C_CSPlayerController*)controller;
                        const char* playerName = pController->GetPlayerName();
                        
                        if (playerName && playerName[0] != '\0')
                        {
                            // Truncate long names
                            char truncatedName[32];
                            strncpy_s(truncatedName, playerName, 20);
                            truncatedName[20] = '\0';
                            
                            drawList->AddText(ImVec2(screenHead.x - 30, textY), IM_COL32(255, 255, 0, 255), truncatedName);
                            textY -= 15.0f;
                        }
                    }
                }
                __except(EXCEPTION_EXECUTE_HANDLER)
                {
                    // Silently continue if name reading fails
                }
            }
            
            // Draw weapon name
            if (cfg_esp.weapon)
            {
                __try
                {
                    uint32_t weaponHandle = pawn->GetActiveWeaponHandle();
                    if (weaponHandle != 0)
                    {
                        // Extract entity index from handle
                        int weaponIndex = weaponHandle & 0x7FFF;
                        
                        if (weaponIndex > 0 && weaponIndex < 8192)
                        {
                            C_BaseEntity* weaponEntity = g_pEntitySystem->GetBaseEntity(weaponIndex);
                            if (weaponEntity && Memory::IsValidPointer((uintptr_t)weaponEntity))
                            {
                                // For now, just show "Weapon" - proper weapon name requires more offsets
                                drawList->AddText(ImVec2(screenFeet.x - 20, screenFeet.y + 5), 
                                    IM_COL32(200, 200, 200, 255), "Weapon");
                            }
                        }
                    }
                }
                __except(EXCEPTION_EXECUTE_HANDLER)
                {
                    // Silently continue if weapon reading fails
                }
            }
            
            // Health bar
            if (cfg_esp.healthBar)
            {
                float barHeight = screenFeet.y - screenHead.y;
                float barWidth = 4.0f;
                float healthPercent = (float)health / 100.0f;
                
                ImVec2 barPos(screenHead.x - (barHeight / 4) - 6, screenHead.y);
                ImVec2 barSize(barWidth, barHeight);
                
                // Background
                drawList->AddRectFilled(barPos, ImVec2(barPos.x + barSize.x, barPos.y + barSize.y), 
                    IM_COL32(0, 0, 0, 180));
                
                // Health bar
                ImU32 healthColor = IM_COL32(
                    (int)((1.0f - healthPercent) * 255),
                    (int)(healthPercent * 255),
                    0, 255
                );
                drawList->AddRectFilled(barPos, 
                    ImVec2(barPos.x + barSize.x, barPos.y + barSize.y * healthPercent), 
                    healthColor);
            }
            
            // Skeleton ESP
            if (cfg_esp.skeleton)
            {
                DrawSkeleton(pawn, cfg_esp.skeletonColor);
            }
            
            // Glow ESP
            if (cfg_esp.glow)
            {
                __try
                {
                    // Convert float colors to Color struct (0-255)
                    Color enemyGlowColor = {
                        (uint8_t)(cfg_esp.glowEnemyColor[0] * 255),
                        (uint8_t)(cfg_esp.glowEnemyColor[1] * 255),
                        (uint8_t)(cfg_esp.glowEnemyColor[2] * 255),
                        (uint8_t)(cfg_esp.glowEnemyColor[3] * 255)
                    };
                    
                    Color teamGlowColor = {
                        (uint8_t)(cfg_esp.glowTeamColor[0] * 255),
                        (uint8_t)(cfg_esp.glowTeamColor[1] * 255),
                        (uint8_t)(cfg_esp.glowTeamColor[2] * 255),
                        (uint8_t)(cfg_esp.glowTeamColor[3] * 255)
                    };
                    
                    // Apply team-based glow
                    pawn->SetGlowTeamBased(localTeam, enemyGlowColor, teamGlowColor);
                }
                __except(EXCEPTION_EXECUTE_HANDLER)
                {
                    // Silently continue if glow fails
                }
            }
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
            // Silently continue on exception
            continue;
        }
    }
    
    // Report findings only when entities are found
    static int lastControllerCount = 0;
    static int lastPawnCount = 0;
    static int lastDrawnCount = 0;
    
    if (validControllers != lastControllerCount || validPawns != lastPawnCount || drawnEntities != lastDrawnCount)
    {
        std::cout << "[ESP] Controllers: " << validControllers << ", Pawns: " << validPawns << ", Dibujados: " << drawnEntities << std::endl;
        lastControllerCount = validControllers;
        lastPawnCount = validPawns;
        lastDrawnCount = drawnEntities;
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
