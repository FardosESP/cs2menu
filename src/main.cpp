#include "main.h"
#include "SDK.h"
#include "Hooks.h"
#include "ImGui_Renderer.h"
#include <iostream>
#include <thread>
#include <chrono>

// Bandera global de desinyección (usada por el botón del menú y DllMain)
std::atomic<bool> g_bUnload = false;

// Punteros globales del SDK
// NOTA: estos se deben inicializar leyendo la memoria del juego con pattern scanning.
// Los valores actuales son nullptr -> las features de ESP/Aimbot no funcionarán
// hasta que implementes la búsqueda de offsets con PatternScan en cada módulo.
CGlobalVarsBase*      g_pGlobalVars    = nullptr;
C_CSGameEntitySystem* g_pEntitySystem  = nullptr;
IVEngineClient*       g_pEngineClient  = nullptr;
ViewMatrix*           g_pViewMatrix    = nullptr;

// FIX: main_init y main_shutdown eliminados - eran código muerto.
// DllMain (en DllMain.cpp) llama directamente a Hooks::Initialize()
// y gestiona el ciclo de vida de la DLL.
// Si en el futuro quieres inicializar los punteros del SDK, hazlo AQUÍ
// añadiendo una función init_sdk() y llamándola desde DllMain.cpp.