# Guía de Compilación e Inyección para CS2 Internal Educational Project

Esta guía detalla los pasos necesarios para compilar la DLL educativa de CS2 utilizando CMake y Visual Studio 2022, y cómo inyectarla en el juego para fines de aprendizaje. Recuerda que este proyecto es **EXCLUSIVAMENTE PARA USO OFFLINE / VS BOTS CON EL PARÁMETRO `-INSECURE`**.

## 1. Requisitos Previos

Antes de comenzar, asegúrate de tener instalados los siguientes componentes en tu sistema:

*   **Visual Studio 2022:** Descarga e instala la edición Community (gratuita) desde el sitio web de Microsoft. Durante la instalación, asegúrate de seleccionar la carga de trabajo **"Desarrollo para escritorio con C++"**.
*   **CMake:** Descarga e instala la última versión de CMake desde su sitio web oficial. Asegúrate de añadir CMake al PATH del sistema durante la instalación.
*   **Git (opcional pero recomendado):** Para clonar el repositorio y sus submódulos fácilmente. Si no lo tienes, deberás descargar ImGui y MinHook manualmente.
*   **Counter-Strike 2:** Con el parámetro de lanzamiento `-insecure` configurado en Steam. Para ello, haz clic derecho en CS2 en tu biblioteca de Steam, selecciona "Propiedades", y en la sección "Opciones de lanzamiento" escribe `-insecure`.

## 2. Configuración del Proyecto

### Paso 2.1: Clonar el Repositorio (o Crear la Estructura)

Si tienes Git, puedes clonar el repositorio y sus submódulos automáticamente:

```bash
git clone --recursive https://github.com/tu_usuario/cs2_internal_edu_project.git # Reemplaza con la URL de tu repositorio
cd cs2_internal_edu_project
```

Si no usas Git, descarga los archivos del proyecto y crea la siguiente estructura de directorios. Luego, descarga [ImGui](https://github.com/ocornut/imgui) y [MinHook](https://github.com/TsudaKageyu/minhook) y colócalos en las carpetas `lib/imgui` y `lib/minhook` respectivamente.

```
cs2_internal_edu_project/
├── CMakeLists.txt
├── src/
│   ├── DllMain.cpp
│   ├── Hooks.h
│   ├── Hooks.cpp
│   ├── ImGui_Renderer.h
│   ├── ImGui_Renderer.cpp
│   ├── SDK.h
│   ├── main.h
│   └── main.cpp
├── lib/
│   ├── imgui/             # Contenido del repositorio ImGui
│   └── minhook/           # Contenido del repositorio MinHook
└── build/                 # Se creará automáticamente
```

### Paso 2.2: Generar el Proyecto de Visual Studio con CMake

Navega a la raíz del directorio `cs2_internal_edu_project` en tu terminal (CMD o PowerShell) y ejecuta los siguientes comandos:

```bash
mkdir build
cd build
cmake .. -A x64
```

*   `mkdir build`: Crea un directorio llamado `build` donde CMake generará los archivos del proyecto.
*   `cd build`: Entra en el directorio `build`.
*   `cmake .. -A x64`: Ejecuta CMake. `..` le dice a CMake que el archivo `CMakeLists.txt` está en el directorio padre. `-A x64` especifica que queremos generar un proyecto para la arquitectura de 64 bits, que es la de CS2.

Este proceso creará un archivo de solución de Visual Studio (`.sln`) llamado `cs2_internal_edu_project.sln` dentro de la carpeta `build/`.

## 3. Compilación de la DLL

### Paso 3.1: Abrir el Proyecto en Visual Studio

1.  Abre el archivo `cs2_internal_edu_project.sln` que se encuentra en la carpeta `build/` con Visual Studio 2022.
2.  Una vez abierto, en la barra de herramientas superior de Visual Studio, asegúrate de que la configuración de la solución esté establecida en `Release` y la plataforma de la solución en `x64`.

    *   **Importante:** Compilar en `Release` es crucial para que la DLL sea lo más pequeña y eficiente posible, y para evitar problemas de depuración que pueden causar detecciones o fallos en el juego.

### Paso 3.2: Compilar la Solución

1.  En Visual Studio, ve al menú `Build` (Compilar).
2.  Selecciona `Build Solution` (Compilar Solución).

Si todo va bien, Visual Studio compilará el proyecto y generará el archivo `cs2_internal_edu_project.dll` en la carpeta `build/Release/`.

## 4. Inyección de la DLL

La inyección de una DLL es el proceso de cargar tu librería dinámica en el espacio de memoria de otro proceso (en este caso, `cs2.exe`). Para fines educativos y en modo `-insecure`, puedes usar un inyector de DLL de código abierto. 

**Advertencia:** Nunca uses inyectores o DLLs en servidores online. Esto es solo para aprendizaje en entornos controlados.

### Paso 4.1: Iniciar CS2 en Modo `-insecure`

1.  Abre Steam y lanza Counter-Strike 2. Asegúrate de que el parámetro `-insecure` esté configurado en las opciones de lanzamiento.
2.  Una vez que el juego esté en el menú principal o en una partida contra bots, déjalo abierto.

### Paso 4.2: Usar un Inyector de DLL

Existen varios inyectores de DLL de código abierto. Puedes buscar en GitHub "DLL Injector C#" o "DLL Injector C++" para encontrar uno. Para este ejemplo, asumiremos un inyector genérico:

1.  Abre el inyector de DLL de tu elección (ejecútalo como administrador si es necesario).
2.  En el inyector, busca la opción para seleccionar un proceso y elige `cs2.exe`.
3.  Busca la opción para seleccionar una DLL y navega hasta la ubicación de tu DLL compilada: `cs2_internal_edu_project/build/Release/cs2_internal_edu_project.dll`.
4.  Haz clic en el botón de "Inject" (Inyectar).

Si la inyección es exitosa, deberías ver un `MessageBoxA` con el mensaje "DLL Inyectada con éxito!" (definido en `DllMain.cpp`). Después de cerrar este mensaje, el menú de ImGui debería aparecer en el juego al presionar la tecla `INSERT`.

## 5. Uso Educativo

Una vez que la DLL esté inyectada y el menú de ImGui sea visible, puedes empezar a explorar y modificar el código fuente para añadir nuevas funcionalidades. Aquí tienes algunas ideas:

*   **ESP:** Implementa la lógica para dibujar cajas, esqueletos, barras de salud, etc., utilizando las funciones de ImGui y los datos del SDK.
*   **Aimbot:** Desarrolla la lógica de cálculo de ángulos y movimiento del ratón para un autoapuntado suave.
*   **Chams/Glow:** Investiga cómo modificar los materiales o shaders del juego para resaltar a los jugadores.
*   **Radar:** Crea un mini-mapa en el menú de ImGui que muestre la posición de los enemigos.

Recuerda que el objetivo es aprender cómo se construyen estas herramientas, no usarlas para hacer trampas. ¡Diviértete explorando el mundo del desarrollo de software de sistema!
