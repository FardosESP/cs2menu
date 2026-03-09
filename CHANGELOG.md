# Changelog

Todos los cambios notables del proyecto se documentan aquí.

---

## [Build 14138.5 - PREMIUM HVH] - 2026-03-09

### 🎯 PREMIUM HVH FEATURES ADDED

#### Anti-Aim System (AntiAim.h/cpp)
- ✅ 7 Pitch types: None, Up, Down, Zero, Fake, FakeUp, FakeDown
- ✅ 8 Yaw types: None, Backward, Spin, Jitter, Sideways, Random, FakeBackward, FakeSpin
- ✅ Fake Lag (packet choking)
- ✅ Edge Anti-Aim (wall detection)
- ✅ Freestanding (auto-adjust based on enemies)
- ✅ Manual AA (left/right/back hotkeys)
- ✅ Configurable yaw offset, jitter range, spin speed

#### Resolver System (Resolver.h/cpp)
- ✅ 6 Resolver types: None, LBY, Moving, Standing, Bruteforce, Delta, LastMove
- ✅ Lower Body Yaw (LBY) resolver
- ✅ Moving player resolver (velocity-based)
- ✅ Standing player resolver (delta-based)
- ✅ Bruteforce resolver (8 angles)
- ✅ Delta resolver (LBY vs FootYaw)
- ✅ Bruteforce on miss
- ✅ Per-player record tracking

#### Backtrack System (Backtrack.h/cpp)
- ✅ Store up to 12 ticks of player history (200ms)
- ✅ Automatic record validation
- ✅ Best record selection (hitchance-based)
- ✅ Latest record fallback
- ✅ Per-player record management
- ✅ Configurable max ticks
- ✅ Shot-only mode

### 📊 Premium Offsets Added (100+ new)

#### HVH Premium Section
- Weapon timing: m_flNextPrimaryAttack, m_flNextSecondaryAttack, m_flPostponeFireReadyTime
- Reload state: m_bInReload, m_flRecoilIndex, m_fAccuracyPenalty
- Weapon info: m_iItemDefinitionIndex, m_nViewModelIndex, m_flLastShotTime
- Accuracy: m_flCrosshairDistance, m_weaponMode, m_flTurningInaccuracy
- Grenades: m_bPinPulled, m_fThrowTime, m_fThrowStrength, m_bThrowAnimating
- C4: m_flC4Blow, m_bStartedArming, m_fArmedTime, m_bBombPlacedAnimation
- Player state: m_entitySpottedState, m_bIsLocalPlayerController
- Competitive: m_iCompetitiveRanking, m_iCompetitiveWins, m_iCompetitiveRankType
- Death: m_flDeathTime, m_hController
- Physics: m_MoveCollide, m_nActualMoveType, m_nWaterLevel, m_fEffects
- Ground: m_hGroundEntity, m_flFriction, m_flElasticity, m_flGravityScale
- Movement: m_flWaterJumpTime, m_vecLadderNormal, m_flDuckAmount, m_flDuckSpeed
- Duck: m_bDucked, m_bDucking, m_bInDuckJump, m_nDuckTimeMsecs
- Jump: m_nJumpTimeMsecs, m_flLastDuckTime, m_flJumpUntil, m_flJumpVel
- Stamina: m_flStamina, m_flHeightAtJumpStart, m_flHeightAtDuckJumpStart
- Strafe: m_bIsStrafing, m_nTraceCount

#### Animation Layers Section
- m_flAnimTime, m_flSimulationTime, m_flPrevAnimTime
- m_nNewSequenceParity, m_nResetEventsParity, m_nMuzzleFlashParity
- m_hLightingOrigin, m_flFrozen, m_ScaleType
- m_bSuppressAnimSounds, m_nHighestConceptCounter
- m_flLastMadeNoiseTime, m_flTimeOfLastInjury
- m_nRelativeDirectionOfLastInjury

#### Backtrack Section
- m_flOldSimulationTime, m_flSimulationTime
- m_vecOrigin, m_angRotation
- m_nTickBase, m_flAnimTime

#### Exploits Section
- m_nTickBase, m_hActiveWeapon
- m_flNextAttack, m_flNextPrimaryAttack, m_flNextSecondaryAttack
- m_bInReload, m_iClip1, m_iClip2
- m_nSequence, m_flCycle, m_flPlaybackRate

### 🔧 SDK.h Enhancements
- ✅ Added GetFootYaw() method
- ✅ Added GetPoseParameter(index) method (24 parameters)
- ✅ Added GetMostRecentModelBone() method
- ✅ All premium offsets with fallback values
- ✅ Dynamic getters for all new offsets

---

## [Build 14138.4] - 2026-03-09

### Fixed
- ✅ Offsets actualizados a Mar 9, 2026 (Build 14138)
- ✅ dwLocalPlayerPawn: 33980896 (0x20681E0)
- ✅ dwEntityList: 38437272 (0x24AC998)
- ✅ dwLocalPlayerController: 36234376 (0x22F2488)
- ✅ dwCSGOInput: 36799552 (0x231A440)
- ✅ Todos los offsets de botones actualizados

### Added
- ✅ Offsets completos para menú HvH:
  - Player offsets: m_iHealth, m_iTeamNum, m_vecAbsOrigin, m_angEyeAngles
  - Aimbot offsets: m_aimPunchAngle, m_aimPunchAngleVel, m_iShotsFired
  - ESP offsets: m_bSpotted, m_bSpottedByMask, m_bDormant, m_Glow
  - Movement offsets: m_vecVelocity, m_fFlags, m_MoveType, m_flMaxspeed
  - Weapon offsets: m_hActiveWeapon, m_iClip1, m_iClip2, m_flNextPrimaryAttack
  - Skin changer offsets: m_nFallbackPaintKit, m_nFallbackSeed, m_flFallbackWear
  - Misc offsets: m_ArmorValue, m_bHasDefuser, m_bHasHelmet, m_iAccount
  - Advanced offsets: m_flLowerBodyYawTarget, m_angShootAngleHistory
  - Weapon details: m_iItemDefinitionIndex, m_AttributeManager, m_szCustomName
  - Movement details: m_flDuckAmount, m_bDucked, m_bDucking, m_flJumpVel
  - Total: 100+ offsets para funcionalidad HvH completa

### Implemented
- ✅ SkinChanger completo (SkinChanger.h/cpp):
  - Cambio de skins de armas con PaintKit, Seed, Wear
  - Soporte para StatTrak
  - Cambio de modelos de cuchillos (16 modelos disponibles)
  - Nombres personalizados de armas
  - Sistema de AttributeManager integrado
  - UI completa en pestaña "Skins" del menú
  - Aplicación automática de skins en tiempo real
- ✅ SDK.h actualizado con todos los offsets HvH
- ✅ Métodos adicionales en C_CSPlayerPawn:
  - GetLowerBodyYawTarget(), GetShotsFired()
  - GetAimPunchAngle(), GetAimPunchAngleVel()
  - GetArmorValue(), HasDefuser(), HasHelmet()
  - IsDefusing(), IsWalking(), GetMoney()
  - GetDuckAmount(), IsDucked(), IsDucking()
  - GetTickBase(), GetSimulationTime()
- ✅ Offsets dinámicos con OffsetManager para todos los nuevos valores
- ✅ Sistema completo listo para ESP, Aimbot, Skin Changer, y más
- ✅ Features.cpp integrado con SkinChanger
- ✅ Configuración de skins persistente en el menú ImGui

### Technical Details
- 100+ offsets HvH implementados en formato hexadecimal
- Sistema de fallback para todos los offsets
- Validación de punteros en todas las operaciones
- Manejo seguro de excepciones
- Compatibilidad con Build 14138 (Mar 9, 2026)

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
