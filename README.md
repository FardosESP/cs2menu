# CS2 Internal Educational Project (DLL) - Neverlose Style

Este proyecto es una plantilla educativa para entender cómo funcionan las herramientas internas (DLLs) en Counter-Strike 2, con un enfoque en la arquitectura de un software como Neverlose. Está diseñado para ser usado **EXCLUSIVAMENTE EN MODO OFFLINE / VS BOTS CON EL PARÁMETRO `-INSECURE`**.

**ADVERTENCIA:** El uso de cualquier software de inyección en servidores online de CS2 resultará en un baneo permanente de VAC. Este proyecto es para fines de aprendizaje y desarrollo ético.

## Requisitos

*   **Visual Studio 2022:** Con el paquete de desarrollo de C++ para escritorio.
*   **DirectX SDK (opcional, pero recomendado):** Para las cabeceras de DirectX 11.
*   **CS2 con `-insecure`:** Inicia el juego con este parámetro en las opciones de lanzamiento de Steam.

## Estructura del Proyecto

```
cs2_internal_edu_project/
├── README.md
├── CMakeLists.txt             # Archivo de configuración de CMake (para compilar)
├── src/
│   ├── DllMain.cpp            # Punto de entrada de la DLL
│   ├── Hooks.h                # Declaraciones de los hooks de DirectX 11
│   ├── Hooks.cpp              # Implementación de los hooks y la lógica de renderizado
│   ├── ImGui_Renderer.h       # Configuración y funciones de renderizado de ImGui
│   ├── ImGui_Renderer.cpp     # Implementación del renderizado de ImGui
│   ├── SDK.h                  # Definiciones básicas del SDK de Source 2 (entidades, etc.)
│   ├── main.h                 # Declaraciones de la lógica principal de la herramienta
│   └── main.cpp               # Lógica principal de la herramienta (menú ImGui, features)
├── lib/
│   ├── imgui/                 # Submódulo de ImGui (se descargará automáticamente con CMake)
│   └── minhook/               # Submódulo de MinHook (para los hooks de funciones)
└── build/                     # Directorio de compilación (generado por CMake)
```

## Fases de Desarrollo y Aprendizaje

1.  **`DllMain.cpp`:** Entender cómo se carga la DLL en el proceso del juego y cómo se inicia un hilo de trabajo.
2.  **`Hooks.cpp` (DirectX 11 Hooking):** Aprender a interceptar las llamadas a funciones de DirectX 11 para dibujar tu propio contenido (el menú, ESP, etc.) directamente en el frame del juego.
3.  **`ImGui_Renderer.cpp`:** Integrar ImGui para crear una interfaz de usuario avanzada y personalizable, similar a las herramientas profesionales.
4.  **`SDK.h`:** Definir las estructuras y clases del juego (jugadores, entidades) para acceder a sus propiedades (salud, posición, etc.) de forma nativa.
5.  **`main.cpp`:** Implementar las funcionalidades avanzadas (ESP, Aimbot, RCS, Chams, etc.) utilizando el SDK y el renderizado de ImGui.

## Compilación (Usando CMake y Visual Studio)

1.  **Clonar el repositorio (o crear la estructura de carpetas):**
    ```bash
    git clone --recursive <URL_DE_ESTE_REPOSITORIO>
    cd cs2_internal_edu_project
    ```
    (Si no usas git, descarga ImGui y MinHook manualmente en `lib/`)

2.  **Generar el proyecto de Visual Studio con CMake:**
    ```bash
    mkdir build
    cd build
    cmake .. -A x64
    ```
    Esto creará un archivo `cs2_internal_edu_project.sln` en la carpeta `build/`.

3.  **Abrir y Compilar en Visual Studio:**
    *   Abre `cs2_internal_edu_project.sln` con Visual Studio 2022.
    *   Asegúrate de que la configuración sea `Release | x64`.
    *   Compila el proyecto (`Build -> Build Solution`).
    *   Esto generará `cs2_internal_edu_project.dll` en `build/Release/`.

## Inyección (Solo para `-insecure`)

Una vez compilada la DLL, necesitarás un inyector de DLL. Para fines educativos y en modo `-insecure`, puedes usar inyectores de código abierto o escribir uno simple. 

**Pasos Generales:**
1.  Inicia CS2 con el parámetro `-insecure`.
2.  Abre tu inyector de DLL.
3.  Selecciona el proceso `cs2.exe`.
4.  Selecciona la DLL `cs2_internal_edu_project.dll`.
5.  Inyecta la DLL.

Una vez inyectada, el menú de ImGui debería aparecer en el juego (normalmente con la tecla `INSERT`).

Este `README` es el primer paso. Ahora procederé a generar los archivos fuente en C++.
