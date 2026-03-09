# ENTITY SCANNING AÑADIDO - Build 14138.6 PRO

## ✅ NUEVO: Entity Scanning Profesional

### Características Implementadas

1. **Direct EntityList Access** (Neverlose/Onetap method)
   - NO usa GetBaseEntity() (evita crashes)
   - Lee EntityList directamente con fórmulas premium
   - Decodifica handles manualmente

2. **IsBadReadPtr Validation**
   - ANTES de cada lectura de memoria
   - Valida punteros, list entries, handles
   - Triple protección: IsBadReadPtr + __try/__except + range check

3. **Performance Optimization**
   - Escanea solo cada 10 frames (6 veces por segundo @ 60fps)
   - Reduce overhead mientras mantiene detección fluida
   - Escanea solo primeros 64 entities (players)

4. **Safe Memory Reads**
   - Health: Validado (1-100)
   - Team: Validado
   - Pawn handle: Validado (no 0, no 0xFFFFFFFF)
   - Pointer ranges: Validados (0x10000 - 0x7FFFFFFFFFFF)

### Cómo Funciona

```cpp
// 1. Read EntityList entry
listEntry = *(entitySystem + 8 * ((index & 0x7FFF) >> 9) + 16)

// 2. Read controller pointer
controllerPtr = *(listEntry + 120 * (index & 0x1FF))

// 3. Read pawn handle from controller
pawnHandle = *(controllerPtr + m_hPlayerPawn offset)

// 4. Decode pawn handle
pawnListEntry = *(entitySystem + 8 * ((handle & 0x7FFF) >> 9) + 16)
pawnPtr = *(pawnListEntry + 120 * (handle & 0x1FF))

// 5. Read pawn data (health, team, etc.)
health = *(pawnPtr + m_iHealth offset)
team = *(pawnPtr + m_iTeamNum offset)
```

### Logs Esperados

```
[ESP-PRO] Starting PROFESSIONAL ESP with Entity Scanning...
[ESP-PRO] LocalPlayer OK: 0x3412b288000
[ESP-PRO] Found 10 valid enemy players
[ESP-PRO] Found 9 valid enemy players  ← Alguien murió
[ESP-PRO] Found 10 valid enemy players ← Respawneó
```

### Qué Detecta

- ✅ Enemigos vivos (health > 0)
- ✅ Filtra por team (si teamCheck está activado)
- ✅ Excluye LocalPlayer
- ✅ Solo jugadores válidos (health 1-100)

### Qué NO Hace Todavía

- ⚠️ NO dibuja boxes (próximo paso)
- ⚠️ NO dibuja health bars
- ⚠️ NO dibuja nombres
- ⚠️ Solo LOGGEA el conteo

## 🧪 CÓMO PROBAR

1. Inyecta el DLL
2. Únete a match con bots (10 bots recomendado)
3. Selecciona equipo
4. Activa ESP desde el menú (INSERT → Visuals → ESP)
5. Observa la consola

### Resultado Esperado

```
[ESP-PRO] Found 10 valid enemy players
```

Si ves este mensaje → Entity scanning funciona ✅

### Si NO Ves Enemigos

Posibles causas:
1. TeamCheck activado y estás solo
2. Todos los bots están muertos
3. Estás en menu/lobby (no en partida)

## 📊 COMPARACIÓN

### Antes (Crasheaba)
```cpp
C_BaseEntity* controller = g_pEntitySystem->GetBaseEntity(i);  // CRASH
C_CSPlayerPawn* pawn = g_pEntitySystem->GetPlayerPawn(controller);  // CRASH
int health = pawn->GetHealth();  // CRASH
```

### Ahora (Profesional)
```cpp
// IsBadReadPtr validation
if (IsBadReadPtr((void*)address, size)) continue;

// Direct memory read with __try/__except
__try {
    value = *(type*)address;
} __except(EXCEPTION_EXECUTE_HANDLER) {
    continue;  // Skip invalid entity
}
```

## 🎯 PRÓXIMOS PASOS

Una vez confirmado que detecta enemigos:

1. Añadir WorldToScreen (convertir 3D → 2D)
2. Dibujar boxes simples
3. Añadir health bars
4. Añadir nombres
5. Añadir distancia
6. Añadir skeleton ESP

## 🔧 ARCHIVOS MODIFICADOS

- `src/Features.cpp` - RenderESP() con entity scanning profesional

## 📝 NOTAS TÉCNICAS

- Usa fórmulas de Neverlose/Onetap para EntityList
- Chunk system: 8 * ((index & 0x7FFF) >> 9) + 16
- Entry offset: 120 * (index & 0x1FF)
- Handle decoding: Same formula con handle en vez de index
- Performance: 10 frame interval = ~167ms entre scans

---

**Build**: 14138.6 - Professional Entity Scanning  
**Fecha**: 9 de Marzo, 2026  
**Estado**: Testing - Entity detection only (no drawing yet)
