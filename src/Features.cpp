#include "Features.h"
#include "Memory.h"
#include "main.h"
#include "imgui.h"
#include "OffsetManager.h"
#include "Aimbot.h"
#include <iostream>
#include <cmath>

// Configuraciones externas (definidas en ImGui_Renderer.cpp)
extern struct ESPConfig {
    bool enabled, boxes, boxFilled, skeleton;
    bool health, healthBar, name, distance;
    bool weapon, snaplines, dormantCheck, teamCheck;
    float boxColor[4], teamColor[4];
    float skeletonColor[4], snaplineColor[4];
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

// Variables locales
static uintptr_t g_clientBase = 0;
static C_CSPlayerPawn* g_pLocalPlayer = nullptr;
static bool g_bInitialized = false;

void Features::Initialize()
{
    std::cout << "[*] Inicializando Features..." << std::endl;
    
    // Initialize OffsetManager first
    std::cout << "[*] Inicializando OffsetManager..." << std::endl;
    if (!OffsetManager::Instance().Initialize())
    {
        std::cout << "[-] Error al inicializar OffsetManager" << std::endl;
    }
    else
    {
        std::cout << "[+] OffsetManager inicializado: " << OffsetManager::Instance().GetOffsetCount() 
                  << " offsets cargados" << std::endl;
        std::cout << "[+] Version del juego: " << OffsetManager::Instance().GetGameVersion() << std::endl;
        
        // Validate offsets
        if (OffsetManager::Instance().ValidateOffsets())
        {
            std::cout << "[+] Validacion de offsets exitosa" << std::endl;
        }
        else
        {
            std::cout << "[!] Advertencia: Algunos offsets pueden ser invalidos" << std::endl;
        }
    }
    
    // Obtener base de client.dll
    g_clientBase = Memory::GetModuleBase("client.dll");
    if (!g_clientBase)
    {
        std::cout << "[-] No se pudo obtener client.dll" << std::endl;
        return;
    }
    
    std::cout << "[+] client.dll base: 0x" << std::hex << g_clientBase << std::dec << std::endl;
    
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
    
    std::cout << "[+] EntitySystem: 0x" << std::hex << (uintptr_t)g_pEntitySystem << std::dec << std::endl;
    std::cout << "[+] ViewMatrix: 0x" << std::hex << (uintptr_t)g_pViewMatrix << std::dec << std::endl;
    
    g_bInitialized = true;
    std::cout << "[+] Features inicializadas correctamente" << std::endl;
    std::cout << "[INFO] El cheat buscara jugadores automaticamente cuando entres en partida" << std::endl;
}

void Features::Update()
{
    if (!g_bInitialized || !g_clientBase) return;
    
    // Debug: verificar que Update se llama
    static int updateCount = 0;
    updateCount++;
    if (updateCount % 300 == 0)
    {
        std::cout << "[Features] Update llamado " << updateCount << " veces" << std::endl;
    }
    
    // Obtener jugador local de forma segura
    __try
    {
        // MÉTODO SIMPLE: Leer LocalPlayer directamente desde dwLocalPlayerPawn
        uintptr_t localPawnAddr = g_clientBase + Offsets::dwLocalPlayerPawn();
        uintptr_t localPawn = Memory::Read<uintptr_t>(localPawnAddr);
        
        // Mensaje de debug solo la primera vez
        static bool firstCheck = true;
        if (firstCheck)
        {
            std::cout << "\n[DEBUG] LocalPawnAddr: 0x" << std::hex << localPawnAddr << std::dec << std::endl;
            std::cout << "[DEBUG] LocalPawn: 0x" << std::hex << localPawn << std::dec << std::endl;
            
            if (localPawn != 0)
            {
                // Escanear memoria AMPLIAMENTE para encontrar health (1-100) y team (2 o 3)
                std::cout << "\n[SCAN] Buscando offsets correctos (rango amplio 0x0-0x1000)..." << std::endl;
                
                int healthCandidates = 0;
                int teamCandidates = 0;
                
                for (int offset = 0x0; offset <= 0x1000; offset += 4)
                {
                    __try
                    {
                        int value = Memory::Read<int>(localPawn + offset);
                        
                        // Buscar health (1-100)
                        if (value > 0 && value <= 100)
                        {
                            std::cout << "[CANDIDATE] Health en 0x" << std::hex << offset << std::dec 
                                      << " = " << value << std::endl;
                            healthCandidates++;
                            if (healthCandidates >= 5) break; // Limitar output
                        }
                        
                        // Buscar team (2 o 3)
                        if (value == 2 || value == 3)
                        {
                            std::cout << "[CANDIDATE] Team en 0x" << std::hex << offset << std::dec 
                                      << " = " << value << std::endl;
                            teamCandidates++;
                        }
                    }
                    __except(EXCEPTION_EXECUTE_HANDLER)
                    {
                        continue;
                    }
                }
                
                if (healthCandidates == 0 && teamCandidates == 0)
                {
                    std::cout << "\n[ERROR] No se encontraron valores validos!" << std::endl;
                    std::cout << "[INFO] Posibles causas:" << std::endl;
                    std::cout << "  1. No estas en partida activa (estas en menu/lobby)" << std::endl;
                    std::cout << "  2. El puntero LocalPawn es incorrecto" << std::endl;
                    std::cout << "  3. Los offsets de cs2-dumper son para otra version de CS2" << std::endl;
                    std::cout << "\n[TIP] Asegurate de estar JUGANDO (no en pausa, no en menu)" << std::endl;
                }
                
                // Mostrar valores con offsets actuales
                int health = Memory::Read<int>(localPawn + Offsets::m_iHealth());
                int team = Memory::Read<int>(localPawn + Offsets::m_iTeamNum());
                std::cout << "\n[DEBUG] Con offsets actuales (0x" << std::hex << Offsets::m_iHealth() 
                          << ", 0x" << Offsets::m_iTeamNum() << std::dec << "):" << std::endl;
                std::cout << "[DEBUG] Health: " << health << ", Team: " << team << std::endl;
            }
            else
            {
                std::cout << "[WARNING] LocalPawn es NULL - no estas en partida?" << std::endl;
            }
            
            firstCheck = false;
        }
        
        // Validar y asignar LocalPlayer
        if (localPawn != 0)
        {
            int health = Memory::Read<int>(localPawn + Offsets::m_iHealth());
            if (health > 0 && health <= 200)
            {
                g_pLocalPlayer = (C_CSPlayerPawn*)localPawn;
            }
            else
            {
                g_pLocalPlayer = nullptr;
            }
        }
        else
        {
            g_pLocalPlayer = nullptr;
        }
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        g_pLocalPlayer = nullptr;
    }
    
    // Ejecutar features solo si tenemos jugador local válido
    if (g_pLocalPlayer)
    {
        if (cfg_esp.enabled)
            RenderESP();
        
        if (cfg_aim.enabled)
            RunAimbot();
        
        ApplyVisuals();
        RunMisc();
    }
    else
    {
        // Debug: mostrar por qué no se ejecutan las features
        static int noPlayerCount = 0;
        noPlayerCount++;
        if (noPlayerCount % 300 == 0) // Cada ~5 segundos (asumiendo 60 FPS)
        {
            std::cout << "[DEBUG] LocalPlayer es NULL - no se ejecutan features" << std::endl;
            std::cout << "[TIP] Asegurate de estar en partida activa y vivo" << std::endl;
        }
    }
    
    // Crosshair custom se dibuja siempre
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
    // Placeholder - necesita offsets de bones
}

void Features::RenderESP()
{
    // Debug: verificar que se llama
    static int callCount = 0;
    callCount++;
    if (callCount % 300 == 0)
    {
        std::cout << "[ESP] RenderESP llamado " << callCount << " veces" << std::endl;
    }
    
    // Validate critical pointers before proceeding
    if (!g_pEntitySystem || !g_pViewMatrix || !g_pLocalPlayer)
    {
        static int nullCount = 0;
        nullCount++;
        if (nullCount % 300 == 0)
        {
            std::cout << "[ESP] Punteros NULL - EntitySystem: " << (g_pEntitySystem ? "OK" : "NULL")
                      << ", ViewMatrix: " << (g_pViewMatrix ? "OK" : "NULL")
                      << ", LocalPlayer: " << (g_pLocalPlayer ? "OK" : "NULL") << std::endl;
        }
        return;
    }
    
    // Additional pointer validation using safe checks
    if (!Memory::IsValidPointer((uintptr_t)g_pEntitySystem) ||
        !Memory::IsValidPointer((uintptr_t)g_pViewMatrix))
    {
        return;
    }
    
    // Validate that the local player is valid
    __try
    {
        if (!g_pLocalPlayer->IsValid() || g_pLocalPlayer->GetHealth() <= 0)
        {
            return;
        }
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        return;
    }
    
    int localTeam = 0;
    Vector3 localPos = {};
    
    // Safely read local player data
    __try
    {
        localTeam = g_pLocalPlayer->GetTeamNum();
        if (!g_pLocalPlayer->GetOriginSafe(localPos))
        {
            return;
        }
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        return;
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
    
    int validEntities = 0;
    int drawnEntities = 0;
    
    // Iterate through player entities (1-64)
    for (int i = 1; i <= 64 && i <= maxEntities; i++)
    {
        __try
        {
            // Safely get entity with null check
            C_BaseEntity* entity = g_pEntitySystem->GetBaseEntity(i);
            if (!entity) continue;
            
            // Validate entity pointer
            if (!Memory::IsValidPointer((uintptr_t)entity)) continue;
            
            // Use safe validation method
            if (!entity->IsValid()) continue;
            
            validEntities++;
            
            // Validate health range
            int health = entity->GetHealth();
            if (health <= 0 || health > 200) continue;
            
            C_CSPlayerPawn* pawn = (C_CSPlayerPawn*)entity;
            
            // Skip local player
            if (pawn == g_pLocalPlayer) continue;
            
            // Dormant check
            if (cfg_esp.dormantCheck && pawn->IsDormant()) continue;
            
            // Team check
            int team = pawn->GetTeamNum();
            if (cfg_esp.teamCheck && team == localTeam) continue;
            
            // Safely get entity origin
            Vector3 origin = {};
            if (!pawn->GetOriginSafe(origin)) continue;
            
            // Distance check
            float dx = origin.x - localPos.x;
            float dy = origin.y - localPos.y;
            float dz = origin.z - localPos.z;
            float distance = sqrtf(dx*dx + dy*dy + dz*dz);
            
            if (distance > cfg_esp.maxDistance) continue;
            
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
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
            // Silently continue on exception
            continue;
        }
    }
    
    static bool espDebugPrinted = false;
    if (!espDebugPrinted)
    {
        std::cout << "[ESP] Entidades válidas encontradas: " << validEntities << std::endl;
        std::cout << "[ESP] Entidades dibujadas: " << drawnEntities << std::endl;
        espDebugPrinted = true;
    }
    
    // Debug periódico cada 5 segundos
    static int frameCount = 0;
    frameCount++;
    if (frameCount % 300 == 0) // ~5 segundos a 60 FPS
    {
        std::cout << "[ESP] Estado actual - Validas: " << validEntities << ", Dibujadas: " << drawnEntities << std::endl;
        std::cout << "[ESP] Config - Enabled: " << cfg_esp.enabled 
                  << ", Boxes: " << cfg_esp.boxes 
                  << ", MaxDist: " << cfg_esp.maxDistance << std::endl;
    }
}

void Features::RunAimbot()
{
    if (!g_pLocalPlayer || !g_pEntitySystem || !g_clientBase)
        return;
    
    if (!cfg_aim.enabled)
        return;
    
    __try
    {
        // Get best target using Aimbot class
        float bestFov = 0.0f;
        C_CSPlayerPawn* target = Aimbot::Instance().GetBestTarget(
            g_pLocalPlayer, 
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
        Vector3 localEyePos = g_pLocalPlayer->GetEyePosition();
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
    if (!g_pLocalPlayer) return;
    
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
            float flashDuration = g_pLocalPlayer->GetFlashDuration();
            if (flashDuration > 0.0f)
            {
                // Establecer duración del flash a 0
                *(float*)((uintptr_t)g_pLocalPlayer + Offsets::m_flFlashDuration()) = 0.0f;
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
    if (!g_pLocalPlayer || !g_clientBase) return;
    
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
    if (!g_pLocalPlayer) return;
    
    __try
    {
        // Verificar si el jugador está en el suelo
        bool onGround = g_pLocalPlayer->IsOnGround();
        
        // Si está en el suelo y se mantiene presionada la barra espaciadora, saltar
        if (onGround)
        {
            // Aquí necesitaríamos inyectar el comando de salto
            // Por ahora es un placeholder - requiere hooking de comandos
        }
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        // Ignorar errores
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
        
        // Iterar sobre todos los jugadores
        for (int i = 1; i <= 64 && i <= maxEntities; i++)
        {
            C_BaseEntity* entity = g_pEntitySystem->GetBaseEntity(i);
            if (!entity) continue;
            
            // Validate entity pointer
            if (!Memory::IsValidPointer((uintptr_t)entity)) continue;
            
            // Use safe validation
            if (!entity->IsValid()) continue;
            
            // Validar que es un jugador válido
            int health = entity->GetHealth();
            if (health <= 0 || health > 200) continue;
            
            // Marcar como spotted (visible en el radar)
            if (!entity->IsSpotted())
            {
                entity->SetSpotted(true);
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
    
    // Líneas horizontales y verticales
    dl->AddLine(ImVec2(cx - sz - gap, cy), ImVec2(cx - gap, cy), col, 2.0f);
    dl->AddLine(ImVec2(cx + gap, cy), ImVec2(cx + sz + gap, cy), col, 2.0f);
    dl->AddLine(ImVec2(cx, cy - sz - gap), ImVec2(cx, cy - gap), col, 2.0f);
    dl->AddLine(ImVec2(cx, cy + gap), ImVec2(cx, cy + sz + gap), col, 2.0f);
}
