# 🎯 CS2Menu Premium HvH Architecture

## Build 14138.5 - Professional HvH Edition

Este documento describe la arquitectura profesional del menú HvH, diseñado para competir con Neverlose, Onetap, y Gamesense.

---

## 🔥 Core Systems (Sistemas Críticos)

### 1. Animation System (Sistema de Animaciones)
**Prioridad: CRÍTICA**

```cpp
class AnimationSystem
{
    // Resolver enemigo basado en animaciones
    void ResolvePlayer(C_CSPlayerPawn* player);
    
    // Fix animaciones locales (desync)
    void FixLocalAnimations(C_CSPlayerPawn* local);
    
    // Detectar side (izquierda/derecha)
    int DetectSide(C_CSPlayerPawn* player);
    
    // Layers de animación
    struct AnimLayer {
        float m_flCycle;
        float m_flWeight;
        float m_flPlaybackRate;
    };
};
```

**Offsets necesarios:**
- `m_flPoseParameter` (0x2764) ✅
- `m_flFootYaw` (0x3A9C) ✅
- `m_flLowerBodyYawTarget` (0x3E90) ✅
- `m_angShootAngleHistory` (0x2780) ✅
- `m_iMostRecentModelBone` (0x1F78) ✅

---

### 2. Rage Aimbot (Aimbot Profesional)
**Prioridad: CRÍTICA**

```cpp
class RageAimbot
{
    // Multipoint (escanear múltiples puntos del hitbox)
    Vector3 GetMultipointPosition(C_CSPlayerPawn* target, int hitbox, int point);
    
    // Hitchance (probabilidad de hit)
    float CalculateHitchance(Vector3 aimAngles, C_CSPlayerPawn* target);
    
    // Autowall (penetración de paredes)
    float GetDamage(Vector3 start, Vector3 end);
    
    // Backtrack integration
    Vector3 GetBestBacktrackPosition(C_CSPlayerPawn* target);
    
    // Resolver integration
    void ApplyResolverCorrection(Vector3& aimPos, C_CSPlayerPawn* target);
};
```

**Features:**
- ✅ Multipoint (head, body, legs)
- ✅ Hitchance calculation
- ✅ Autowall damage prediction
- ✅ Backtrack integration
- ✅ Resolver integration
- ✅ Auto scope
- ✅ Auto stop
- ✅ Recoil compensation

---

### 3. Anti-Aim Professional
**Prioridad: CRÍTICA**

```cpp
class AntiAimPro
{
    // Desync real (fake angles)
    void CreateDesync(Vector3& realAngles, Vector3& fakeAngles);
    
    // Jitter avanzado (unpredictable)
    void AdvancedJitter(Vector3& angles);
    
    // Freestanding (auto-adjust basado en enemigos)
    void Freestanding(Vector3& angles, C_CSPlayerPawn* local);
    
    // Edge AA (detectar paredes)
    void EdgeDetection(Vector3& angles, C_CSPlayerPawn* local);
    
    // Manual AA (keybinds)
    void ManualOverride(Vector3& angles, int direction);
};
```

**Modos:**
1. **Static** - Ángulo fijo
2. **Jitter** - Cambio rápido entre ángulos
3. **Spin** - Rotación continua
4. **Freestanding** - Auto-ajuste inteligente
5. **Manual** - Control manual con teclas

---

### 4. Resolver System
**Prioridad: CRÍTICA**

```cpp
class ResolverPro
{
    struct PlayerRecord {
        float lastLBY;
        float lastFootYaw;
        float lastMoveYaw;
        int missedShots;
        int resolverMode; // 0=LBY, 1=Moving, 2=Bruteforce
    };
    
    // Resolver por LBY
    float ResolveLBY(C_CSPlayerPawn* player);
    
    // Resolver por movimiento
    float ResolveMoving(C_CSPlayerPawn* player);
    
    // Bruteforce (después de fallar)
    float ResolveBruteforce(C_CSPlayerPawn* player);
    
    // Detectar fake
    bool IsFaking(C_CSPlayerPawn* player);
};
```

**Métodos:**
1. **LBY** - Lower Body Yaw
2. **Moving** - Basado en velocidad
3. **Standing** - Parado
4. **Bruteforce** - Probar todos los ángulos
5. **Delta** - Diferencia entre ángulos
6. **LastMove** - Último movimiento

---

### 5. Exploits System
**Prioridad: ALTA**

```cpp
class ExploitsSystem
{
    // Doubletap (disparar 2 veces en 1 tick)
    void Doubletap();
    
    // Hideshots (ocultar disparo)
    void Hideshots();
    
    // Fakelag (choke packets)
    void Fakelag(int ticks);
    
    // Fakewalk (caminar sin hacer ruido)
    void Fakewalk();
};
```

---

### 6. Backtrack Professional
**Prioridad: ALTA**

```cpp
class BacktrackPro
{
    struct BacktrackRecord {
        Vector3 origin;
        Vector3 headPos;
        float simTime;
        bool valid;
    };
    
    // Guardar hasta 200ms de historia (12 ticks @ 64 tick)
    std::deque<BacktrackRecord> records[64];
    
    // Seleccionar mejor tick
    BacktrackRecord GetBestRecord(C_CSPlayerPawn* player);
    
    // Validar tick (no muy viejo)
    bool IsRecordValid(BacktrackRecord& record);
};
```

---

### 7. Movement System
**Prioridad: MEDIA**

```cpp
class MovementSystem
{
    // Bunny hop perfecto
    void Bhop(C_CSPlayerPawn* local);
    
    // Auto strafe (air strafe)
    void AutoStrafe(C_CSPlayerPawn* local);
    
    // Edge jump (saltar en el borde)
    void EdgeJump(C_CSPlayerPawn* local);
    
    // Fast ladder
    void FastLadder(C_CSPlayerPawn* local);
};
```

---

## 🎨 UI/UX Premium

### Menu Design
- **Estilo:** Modern/Minimalist (como Neverlose)
- **Colores:** Dark theme con acentos azules
- **Tabs:** ESP, Aimbot, Anti-Aim, Visuals, Misc, Skins, Config
- **Keybinds:** Sistema de hotkeys para cada feature
- **Configs:** Save/Load configuraciones

### Performance
- **Entity Cache:** Cachear entidades para reducir CPU
- **Multi-threading:** Separar render de lógica
- **Optimización:** Máximo 60 FPS en el menú

---

## 🔒 Security & Anti-Detection

### Anti-VAC
1. **No strings detectables** - Ofuscar strings
2. **No patterns conocidos** - Randomizar código
3. **Memory protection** - VirtualProtect en hooks
4. **Screenshot cleaner** - Detectar screenshots y ocultar menú

### Anti-Untrusted
1. **Angle clamping** - Limitar ángulos a valores válidos
2. **Silent aim limits** - No más de 180° de diferencia
3. **Movement validation** - No velocidades imposibles
4. **Packet validation** - No enviar packets inválidos

---

## 📊 Performance Targets

| Feature | Target FPS Impact | Memory Usage |
|---------|------------------|--------------|
| ESP | < 5 FPS | < 10 MB |
| Aimbot | < 2 FPS | < 5 MB |
| Anti-Aim | < 1 FPS | < 2 MB |
| Resolver | < 3 FPS | < 8 MB |
| Backtrack | < 2 FPS | < 15 MB |
| **Total** | **< 15 FPS** | **< 50 MB** |

---

## 🚀 Roadmap

### Phase 1: Core (ACTUAL)
- [x] Offsets actualizados (Build 14138.5)
- [x] Anti-Aim básico
- [x] Resolver básico
- [x] Backtrack básico
- [ ] Integración completa

### Phase 2: Advanced
- [ ] Animation system completo
- [ ] Rage aimbot con multipoint
- [ ] Hitchance calculation
- [ ] Autowall
- [ ] Doubletap/Hideshots

### Phase 3: Premium
- [ ] Fakelag avanzado
- [ ] Freestanding AA
- [ ] Edge AA
- [ ] Config system
- [ ] Lua scripting

### Phase 4: Polish
- [ ] UI/UX premium
- [ ] Performance optimization
- [ ] Anti-detection
- [ ] Documentation

---

## 💡 Best Practices

1. **Siempre validar pointers** antes de leer/escribir
2. **Usar Memory::SafeRead** en lugar de raw pointers
3. **Cachear entidades** para reducir scans
4. **Limitar ángulos** para evitar untrusted
5. **Ofuscar strings** para evitar detección
6. **Thread-safe** todas las operaciones críticas
7. **Logging profesional** con niveles (debug, info, error)

---

**Última actualización:** Build 14138.5 (Mar 9, 2026)
**Estado:** En desarrollo activo
**Target:** Premium HvH (Neverlose-tier)
