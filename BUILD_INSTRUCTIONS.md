# Instrucciones de Compilación - CS2MENU Build 14138.4

## 🎯 Requisitos Previos

### Software Necesario
- **Windows 10/11** (64-bit)
- **Visual Studio 2019 o superior** con:
  - Desarrollo de escritorio con C++
  - CMake tools
  - Windows 10 SDK
- **CMake 3.15+** (incluido con VS)
- **Git** (para clonar el repositorio)

### Verificar Instalación
```bash
cmake --version  # Debe mostrar 3.15 o superior
git --version    # Debe mostrar versión de Git
```

---

## 📦 Compilación Paso a Paso

### Método 1: Visual Studio (Recomendado)

#### 1. Abrir el Proyecto
```bash
# Clonar repositorio
git clone <repo-url>
cd cs2menu

# Abrir con Visual Studio
# File -> Open -> CMake -> Seleccionar CMakeLists.txt
```

#### 2. Configurar Build
- En la barra superior, seleccionar: **x64-Release**
- Esperar a que CMake configure el proyecto
- Verificar que no haya errores en la ventana de Output

#### 3. Compilar
- **Build -> Build All** (Ctrl+Shift+B)
- O click derecho en CMakeLists.txt -> Build

#### 4. Archivos Generados
Los archivos compilados estarán en:
```
cs2menu/build/Release/Release/
├── cs2menu.dll           # DLL principal
├── cs2menu_launcher.exe  # Launcher
└── offsets.json          # Offsets (copiado automáticamente)
```

---

### Método 2: Línea de Comandos

#### 1. Configurar CMake
```bash
cd cs2menu
cmake -B build -DCMAKE_BUILD_TYPE=Release
```

#### 2. Compilar
```bash
cmake --build build --config Release
```

#### 3. Verificar Salida
```bash
dir build\Release\Release
# Debe mostrar: cs2menu.dll, cs2menu_launcher.exe, offsets.json
```

---

### Método 3: CLion

#### 1. Abrir Proyecto
- **File -> Open** -> Seleccionar carpeta cs2menu
- CLion detectará automáticamente CMakeLists.txt

#### 2. Configurar
- Seleccionar perfil: **Release**
- Toolchain: **Visual Studio**

#### 3. Compilar
- **Build -> Build Project** (Ctrl+F9)
- O click en el ícono de martillo

---

## 🔧 Solución de Problemas

### Error: "CMake no encontrado"
```bash
# Instalar CMake manualmente
# Descargar de: https://cmake.org/download/
# O usar chocolatey:
choco install cmake
```

### Error: "No se encuentra cl.exe"
- Abrir **Visual Studio Installer**
- Modificar instalación
- Asegurar que "Desarrollo de escritorio con C++" esté instalado
- Incluir "Windows 10 SDK"

### Error: "ImGui no encontrado"
```bash
# Verificar que la carpeta lib/imgui existe
dir lib\imgui

# Si no existe, clonar ImGui:
git clone https://github.com/ocornut/imgui.git lib/imgui
```

### Error: "MinHook no encontrado"
```bash
# Verificar que la carpeta lib/minhook existe
dir lib\minhook

# Si no existe, clonar MinHook:
git clone https://github.com/TsudaKageyu/minhook.git lib/minhook
```

### Error: "offsets.json no se copia"
- Verificar que offsets.json existe en la raíz del proyecto
- El post-build command lo copia automáticamente
- Si falla, copiar manualmente:
```bash
copy offsets.json build\Release\Release\offsets.json
```

---

## 🧪 Compilar Tests (Opcional)

### Tests Disponibles
1. **test_offsetmanager** - Prueba el sistema de offsets
2. **test_memorysafety** - Prueba la seguridad de memoria
3. **test_offset_notifications** - Prueba notificaciones

### Compilar Tests
```bash
# Los tests se compilan automáticamente con el proyecto
# Ejecutar tests:
cd build\Release\Release
test_offsetmanager.exe
test_memorysafety.exe
test_offset_notifications.exe
```

---

## 📋 Verificación Post-Compilación

### Checklist
- [ ] cs2menu.dll existe y es ~2-3 MB
- [ ] cs2menu_launcher.exe existe y es ~100-200 KB
- [ ] offsets.json existe en el mismo directorio
- [ ] No hay errores en la ventana de Output
- [ ] Todos los archivos están en build/Release/Release/

### Verificar DLL
```bash
# Verificar que la DLL es de 64-bit
dumpbin /headers cs2menu.dll | findstr machine
# Debe mostrar: 8664 machine (x64)
```

### Verificar Dependencias
```bash
# Verificar dependencias de la DLL
dumpbin /dependents cs2menu.dll
# Debe mostrar: d3d11.dll, user32.dll, kernel32.dll, etc.
```

---

## 🚀 Uso Post-Compilación

### 1. Preparar Archivos
```bash
# Copiar archivos a una carpeta limpia
mkdir C:\CS2Menu
copy build\Release\Release\* C:\CS2Menu\
```

### 2. Ejecutar
```bash
# 1. Abrir CS2 y entrar en una partida
# 2. Ejecutar launcher como Administrador
cd C:\CS2Menu
cs2menu_launcher.exe
```

### 3. Verificar Inyección
- Debe aparecer mensaje: "DLL inyectada correctamente"
- En CS2, presionar INSERT para abrir menú
- Si no funciona, presionar F9 para escanear offsets

---

## 🔄 Recompilar Después de Cambios

### Cambios en Código
```bash
# Recompilar solo archivos modificados
cmake --build build --config Release
```

### Cambios en CMakeLists.txt
```bash
# Reconfigurar y recompilar
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

### Limpiar y Recompilar
```bash
# Eliminar build y recompilar desde cero
rmdir /s /q build
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

---

## 📊 Tiempos de Compilación

### Primera Compilación
- **Configuración CMake**: ~30 segundos
- **Compilación completa**: ~2-5 minutos
- **Total**: ~3-6 minutos

### Recompilación Incremental
- **Solo archivos modificados**: ~10-30 segundos
- **Con cambios en headers**: ~1-2 minutos

---

## 🎯 Optimizaciones de Compilación

### Compilación Paralela
```bash
# Usar todos los cores del CPU
cmake --build build --config Release -- /m
```

### Compilación Silenciosa
```bash
# Menos output en consola
cmake --build build --config Release -- /verbosity:minimal
```

### Compilación con Símbolos de Debug
```bash
# Para debugging (más lento)
cmake -B build -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build --config RelWithDebInfo
```

---

## 📝 Notas Finales

### Archivos Importantes
- **CMakeLists.txt** - Configuración de compilación
- **offsets.json** - Offsets del juego (se copia automáticamente)
- **src/** - Código fuente
- **lib/** - Librerías externas (ImGui, MinHook)

### Mantenimiento
- Actualizar offsets.json con cada update de CS2
- Recompilar después de actualizar offsets
- Verificar CHANGELOG.md para cambios

### Soporte
- Consultar TROUBLESHOOTING.md para problemas comunes
- Revisar docs/ para documentación adicional
- Verificar que el build sea 14138.4

---

**Build**: 14138.4  
**Fecha**: 9 de Marzo, 2026  
**Estado**: ✅ Listo para compilar
