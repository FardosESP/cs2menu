#pragma once

#include <atomic>

// Bandera global para indicar que la DLL debe desinyectarse
extern std::atomic<bool> g_bUnload;

// Funciones de inicialización y limpieza de la lógica principal
void main_init();
void main_shutdown();
