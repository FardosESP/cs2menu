#include <Windows.h>
#include <thread>
#include "main.h"

// Función que se ejecutará en un hilo separado
DWORD WINAPI MainThread(LPVOID lpParam)
{
    // Aquí se inicializará la lógica principal de la herramienta
    // Por ahora, solo un mensaje de depuración
    MessageBoxA(NULL, "DLL Inyectada con éxito!", "CS2 Edu-Tool", MB_OK);

    // Llamar a la función de inicialización de la lógica principal
    // main_init(); // Esto se implementará en main.cpp

    // Bucle principal de la herramienta (si es necesario)
    while (!g_bUnload)
    {
        // Lógica de la herramienta aquí
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // Limpieza antes de desinyección
    // main_shutdown(); // Esto se implementará en main.cpp

    MessageBoxA(NULL, "DLL Desinyectada!", "CS2 Edu-Tool", MB_OK);

    // Liberar la DLL del proceso
    FreeLibraryAndExitThread(static_cast<HMODULE>(lpParam), 0);
    return 0;
}

// Punto de entrada de la DLL
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        // Deshabilitar llamadas a DllMain para otros hilos
        DisableThreadLibraryCalls(hModule);
        // Crear un nuevo hilo para nuestra lógica principal
        CreateThread(NULL, 0, MainThread, hModule, 0, NULL);
        break;
    case DLL_PROCESS_DETACH:
        // Señal para que el hilo principal se detenga
        g_bUnload = true;
        break;
    }
    return TRUE;
}
