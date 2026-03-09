# CS2MENU - Premium HvH Edition

**Build 14138.5** | Counter-Strike 2 External Cheat | Full HvH Suite

---

## 🎯 Features

### 🛡️ Anti-Aim (PREMIUM)
- **7 Pitch Types**: Up, Down, Zero, Fake, FakeUp, FakeDown
- **8 Yaw Types**: Backward, Spin, Jitter, Sideways, Random, FakeBackward, FakeSpin
- **Fake Lag**: Configurable packet choking (up to 14 ticks)
- **Edge AA**: Automatic wall detection and adjustment
- **Freestanding**: Auto-adjust based on enemy positions
- **Manual AA**: Hotkeys for left/right/back
- **Customizable**: Yaw offset, jitter range, spin speed

### 🎯 Resolver (PREMIUM)
- **6 Resolver Types**: LBY, Moving, Standing, Bruteforce, Delta, LastMove
- **LBY Resolver**: Lower Body Yaw prediction
- **Moving Resolver**: Velocity-based angle prediction
- **Standing Resolver**: Delta-based resolution
- **Bruteforce**: 8-angle bruteforce on miss
- **Delta Resolver**: LBY vs FootYaw analysis
- **Per-Player Tracking**: Individual resolver data per enemy

### ⏮️ Backtrack (PREMIUM)
- **200ms History**: Store up to 12 ticks of player positions
- **Smart Selection**: Hitchance-based record selection
- **Auto Validation**: Automatic record expiration
- **Per-Player Records**: Individual history per enemy
- **Configurable**: Max ticks, shot-only mode
- **Lag Compensation**: Shoot at laggy players accurately

### Visual ESP
- **Box ESP** - Cajas 2D/3D alrededor de jugadores
- **Health Bar** - Barra de vida con colores (verde/amarillo/rojo)
- **Skeleton ESP** - Esqueleto de jugadores
- **Name ESP** - Nombres de jugadores
- **Distance ESP** - Distancia en metros
- **Weapon ESP** - Arma equipada
- **Snaplines** - Líneas desde el centro de la pantalla
- **Team Check** - Filtrar compañeros de equipo
- **Glow ESP** - Resplandor alrededor de jugadores

### Aimbot
- **FOV Circle** - Círculo de campo de visión
- **Smooth Aim** - Suavizado de movimiento
- **Visible Only** - Solo apuntar a enemigos visibles
- **Team Check** - No apuntar a compañeros
- **Bone Selection** - Selección de hueso (cabeza, pecho, etc.)
- **Auto Shoot** - Disparo automático
- **RCS** - Control de retroceso

### Skin Changer (NUEVO ✨)
- **16 Modelos de Cuchillos** - Karambit, M9, Butterfly, Flip, Gut, etc.
- **12 Acabados Populares** - Doppler, Fade, Tiger Tooth, Marble Fade, etc.
- **30 Slots de Armas** - Configura skins para todas tus armas
- **Personalización Completa**:
  - Paint Kit (Skin ID)
  - Seed (variación de patrón)
  - Float Value (desgaste 0.0-1.0)
  - StatTrak counter
  - Nombres personalizados
- **Aplicación en Tiempo Real** - Los cambios se aplican instantáneamente
- **UI Intuitiva** - Interfaz fácil de usar en el menú

### Misc
- **Radar Hack** - Mostrar enemigos en radar
- **Bhop** - Salto automático
- **No Flash** - Eliminar ceguera de flashbangs
- **100+ Offsets HvH** - Sistema completo para Hack vs Hack

---

## � Instalación Rápida

### Requisitos
- Windows 10/11 (64-bit)
- Visual Studio 2019+ o CMake 3.15+
- Counter-Strike 2 (Build 14138 o superior)

### Compilación

```bash
# 1. Clonar repositorio
git clone <repo-url>
cd cs2menu

# 2. Compilar (Release)
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

# 3. Los archivos estarán en:
# - build/Release/Release/cs2menu.dll
# - build/Release/Release/cs2menu_launcher.exe
# - build/Release/Release/offsets.json
```

### Uso

1. **Abrir CS2** y entrar en una partida
2. **Ejecutar Launcher** como Administrador
3. El launcher esperará a que CS2 cargue completamente
4. **Inyección automática** cuando el juego esté listo
5. **INSERT** para abrir/cerrar menú
6. **F9** para escanear offsets (si es necesario)
7. **END** para desinyectar

---

## ⚙️ Configuración

### offsets.json

El archivo `offsets.json` contiene los offsets de memoria para el build actual de CS2. Se actualiza automáticamente con cada update del juego.

```json
{
  "client.dll": {
    "dwLocalPlayerPawn": 33970960,
    "dwEntityList": 38453920,
    "dwViewMatrix": 36749024,
    ...
  }
}
```

### Actualizar Offsets

Si CS2 se actualiza y el cheat deja de funcionar:

1. Presiona **F9** en el juego para escanear offsets
2. O descarga offsets actualizados de [cs2-dumper](https://github.com/a2x/cs2-dumper)
3. Copia los valores a `offsets.json` (formato decimal)

---

## 🎮 Controles

| Tecla | Acción |
|-------|--------|
| **INSERT** | Abrir/Cerrar menú |
| **F9** | Escanear offsets |
| **END** | Desinyectar cheat |

---

## 📋 Troubleshooting

### LocalPlayerPawn is NULL
- Asegúrate de estar **spawneado** (vivo, no muerto)
- Espera 2-3 segundos después de spawnear
- Presiona **F9** para escanear offsets
- Verifica que `offsets.json` existe en la carpeta del DLL

### ESP no muestra jugadores
- Verifica que estás en una partida con otros jugadores
- Presiona **F9** para actualizar offsets
- Revisa que `dwEntityList` está actualizado

### Launcher falla al inyectar
- Ejecuta como **Administrador**
- Espera a que CS2 cargue completamente (menú principal)
- Verifica que `cs2menu.dll` está en la misma carpeta

Ver [TROUBLESHOOTING.md](TROUBLESHOOTING.md) para más detalles.

---

## 📝 Changelog

Ver [CHANGELOG.md](CHANGELOG.md) para historial completo de cambios.

### Última Versión (Build 14138.3)

**Fixes Aplicados:**
- ✅ Offsets actualizados a Mar 5, 2026
- ✅ Sistema de timing dinámico en Launcher
- ✅ Validación post-inyección
- ✅ Presentación profesional de consola
- ✅ Eliminados delays hardcoded
- ✅ Verificación de módulos del juego

---

## ⚠️ Disclaimer

Este proyecto es solo para **fines educativos**. El uso de cheats en juegos online puede resultar en:
- Ban permanente de tu cuenta
- Ban de hardware (HWID)
- Violación de términos de servicio

**Usa bajo tu propio riesgo.** Los desarrolladores no se hacen responsables de ningún ban o consecuencia.

---

## 📚 Documentación Adicional

- [COMPILATION_AND_INJECTION_GUIDE.md](COMPILATION_AND_INJECTION_GUIDE.md) - Guía detallada de compilación
- [GUIA_RAPIDA_USO.md](GUIA_RAPIDA_USO.md) - Guía rápida en español
- [FEATURES_TODO.md](FEATURES_TODO.md) - Features pendientes
- [docs/archive/](docs/archive/) - Documentación histórica

---

## 🔧 Desarrollo

### Estructura del Proyecto

```
cs2menu/
├── src/
│   ├── DllMain.cpp          # Entry point
│   ├── Features.cpp         # ESP, Radar, etc.
│   ├── Aimbot.cpp          # Aimbot logic
│   ├── SDK.h               # Game SDK
│   ├── OffsetManager.cpp   # Offset management
│   ├── Launcher.cpp        # Injector
│   └── ...
├── offsets.json            # Memory offsets
├── CMakeLists.txt          # Build configuration
└── README.md               # This file
```

### Contribuir

1. Fork el repositorio
2. Crea una rama para tu feature (`git checkout -b feature/nueva-feature`)
3. Commit tus cambios (`git commit -am 'Añadir nueva feature'`)
4. Push a la rama (`git push origin feature/nueva-feature`)
5. Crea un Pull Request

---

**CS2MENU Premium Edition** - Build 14138.3 (Mar 2026)
