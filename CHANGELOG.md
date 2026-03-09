# Changelog

Todos los cambios notables del proyecto se documentan aquí.

---

## [Build 14138.3] - 2026-03-09

### Added
- ✅ Detección automática de offsets al entrar en partida (sin necesidad de F9)
- ✅ Búsqueda de offsets.json en múltiples ubicaciones
- ✅ Post-build automático para copiar offsets.json
- ✅ Presentación profesional de consola con ASCII art
- ✅ Validación post-inyección en Launcher
- ✅ Sistema de timing dinámico (sin delays hardcoded)

### Fixed
- ✅ Offsets actualizados a Mar 5, 2026 (Build 14138)
- ✅ dwLocalPlayerPawn: 33970960 (0x2066E10)
- ✅ dwEntityList: 38453920 (0x249B2A0)
- ✅ Eliminado delay hardcoded de 15s en Launcher
- ✅ Eliminado delay de 3s en DllMain
- ✅ Caracteres UTF-8 reemplazados por ASCII para compatibilidad
- ✅ OffsetManager busca en múltiples rutas
- ✅ Scanner se ejecuta automáticamente al detectar partida

### Changed
- 🔄 Console output más limpio y profesional
- 🔄 Símbolos ASCII en lugar de UTF-8
- 🔄 Mensajes más concisos y claros
- 🔄 Fallback offsets actualizados a Mar 5, 2026

### Removed
- ❌ 20+ archivos MD redundantes movidos a docs/archive/
- ❌ Delays hardcoded en Launcher y DllMain
- ❌ Necesidad de presionar F9 manualmente

---

## [Build 14138.2] - 2026-03-09

### Fixed
- Aumentado límite de escaneo de entidades de 64 a 2048
- Corregido cálculo de ángulos en Aimbot (atanf → atan2f)
- Uniformada validación de health a 100 (valor máximo real)
- Implementado IsVisible básico usando m_bSpotted flag
- Añadido bounds check en Memory::FindPattern
- Mejorado input capture con WantCaptureMouse/Keyboard flags
- Añadida validación de privilegios de admin en Launcher
- Añadido error handling robusto en DllMain

### Added
- Sistema de logging silencioso (Logger.h)
- Auto-elevación de privilegios en Launcher
- Timeout de 30s para carga de módulos

---

## [Build 14138.1] - 2026-02-24

### Added
- Sistema de chunks/handles para CS2
- OffsetManager con soporte JSON
- Offset Scanner (F9)
- ESP con cajas, health bars, nombres, distancia
- Aimbot con FOV, smooth, visible only
- Radar Hack
- ImGui menu

### Known Issues
- Offsets pueden quedar obsoletos con updates de CS2
- LocalPlayerPawn NULL si no estás spawneado
- ESP no funciona si offsets están desactualizados

---

## Formato

Este changelog sigue [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

### Tipos de Cambios
- `Added` - Nuevas features
- `Changed` - Cambios en funcionalidad existente
- `Deprecated` - Features que serán removidas
- `Removed` - Features removidas
- `Fixed` - Bug fixes
- `Security` - Vulnerabilidades

---

**CS2MENU Premium Edition** - Build 14138.3 (Mar 2026)
