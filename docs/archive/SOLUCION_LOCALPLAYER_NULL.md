# Solución: LocalPlayerPawn is NULL

## 🔍 DIAGNÓSTICO

Según tu consola:
```
[ERROR] LocalPlayerPawn is NULL - you must be in an active game!
[TIP] Join a match, spawn as a player, then run this scan
```

Esto significa que el offset `dwLocalPlayerPawn` está leyendo NULL, lo cual puede pasar por dos razones:

1. **No estás spawneado** (estás en menú, lobby, o muerto)
2. **Offset incorrecto** (desactualizado)

---

## ✅ SOLUCIÓN INMEDIATA

### Paso 1: Verificar que Estás en Partida

Estás jugando contra bots, lo cual está bien. Pero asegúrate de:

1. ✅ Estar **spawneado** (no en espectador)
2. ✅ Estar **vivo** (no muerto esperando respawn)
3. ✅ Estar en **partida activa** (no en menú de pausa)

### Paso 2: Esperar y Reintentar

1. **Spawneate** en el juego
2. Espera 2-3 segundos
3. Presiona **F9** de nuevo

El scanner debería detectar tu LocalPlayer automáticamente.

---

## 🔧 SI SIGUE FALLANDO

### Verificar Offset de dwLocalPlayerPawn

El offset actual es: `33970928` (0x2066DF0)

#### Método 1: Probar Offset Alternativo

Según los logs, tu `client.dll` está en: `0x7ff868560000`

Prueba este offset alternativo (Mar 5, 2026):
```
dwLocalPlayerPawn = 0x2066E10  // 33970960 decimal
```

**Cómo aplicar**:
1. Editar `offsets.json`
2. Cambiar línea:
   ```json
   "dwLocalPlayerPawn": 33970960,
   ```
3. Guardar archivo
4. Presionar F9 en el juego

#### Método 2: Usar Scanner Avanzado

Si tienes acceso al código, puedes añadir un scanner más agresivo:

```cpp
// En OffsetScanner.cpp, añadir:
void ScanLocalPlayerPawn()
{
    uintptr_t clientBase = Memory::GetModuleBase("client.dll");
    
    // Buscar en rango típico (32-34 MB)
    for (uintptr_t offset = 0x2000000; offset < 0x2200000; offset += 0x10)
    {
        uintptr_t addr = clientBase + offset;
        uintptr_t pawn = Memory::Read<uintptr_t>(addr);
        
        if (pawn > 0x10000 && pawn < 0x7FFFFFFFFFFF)
        {
            // Validar que parece un pawn
            int health = Memory::Read<int>(pawn + 0x76C);
            int team = Memory::Read<int>(pawn + 0xD70);
            
            if (health > 0 && health <= 100 && team >= 2 && team <= 3)
            {
                std::cout << "[CANDIDATE] dwLocalPlayerPawn at 0x" 
                          << std::hex << offset << std::dec 
                          << " (Health: " << health << ", Team: " << team << ")" 
                          << std::endl;
            }
        }
    }
}
```

---

## 📊 ENTENDIENDO EL PROBLEMA

### ¿Qué es dwLocalPlayerPawn?

Es un **puntero global** en `client.dll` que apunta a tu jugador local (el que controlas).

```
client.dll + dwLocalPlayerPawn → Tu C_CSPlayerPawn
                                  ├─ Health
                                  ├─ Team
                                  ├─ Position
                                  └─ ...
```

### ¿Por qué puede ser NULL?

1. **Estás en menú**: No hay jugador spawneado
2. **Estás muerto**: El pawn se destruye temporalmente
3. **Offset incorrecto**: Apunta a memoria vacía
4. **Juego actualizó**: Offset cambió de posición

---

## 🎯 SOLUCIÓN DEFINITIVA

### Opción A: Actualizar Offsets (Recomendado)

1. Ir a https://github.com/a2x/cs2-dumper
2. Buscar último commit
3. Abrir `output/offsets.json`
4. Buscar `dwLocalPlayerPawn`
5. Copiar valor (convertir hex a decimal si es necesario)
6. Actualizar tu `offsets.json`

**Ejemplo**:
```json
{
  "client.dll": {
    "dwLocalPlayerPawn": 33975136,  // ← Valor actualizado
    // ... resto de offsets
  }
}
```

### Opción B: Usar Fallback Dinámico

Modificar `Features.cpp` para buscar LocalPlayer dinámicamente:

```cpp
C_CSPlayerPawn* FindLocalPlayer()
{
    // Método 1: Usar dwLocalPlayerPawn
    uintptr_t localPawnAddr = g_clientBase + Offsets::dwLocalPlayerPawn();
    uintptr_t localPawn = Memory::Read<uintptr_t>(localPawnAddr);
    
    if (localPawn != 0)
    {
        int health = Memory::Read<int>(localPawn + Offsets::m_iHealth());
        if (health > 0 && health <= 100)
            return (C_CSPlayerPawn*)localPawn;
    }
    
    // Método 2: Buscar en EntityList (fallback)
    for (int i = 1; i <= 64; i++)
    {
        C_BaseEntity* controller = g_pEntitySystem->GetBaseEntity(i);
        if (!controller) continue;
        
        C_CSPlayerPawn* pawn = g_pEntitySystem->GetPlayerPawn(controller);
        if (!pawn) continue;
        
        // Verificar si es local (puedes usar otros checks)
        int health = pawn->GetHealth();
        if (health > 0 && health <= 100)
        {
            // Asumir que el primero válido es local
            return pawn;
        }
    }
    
    return nullptr;
}
```

---

## 🧪 TESTING

### Checklist de Verificación

Cuando presiones F9, deberías ver:

```
✅ [OK] client.dll base: 0x7ff868560000
✅ [OK] LocalPlayer found at: 0x...
✅ [OK] Health: 100, Team: 2
✅ [OK] Position: (X, Y, Z)
```

Si ves esto, el offset es correcto.

### Si Sigue Fallando

1. **Verificar que estás vivo**:
   - Mira tu HP en el juego
   - Debe ser > 0

2. **Verificar que no estás en espectador**:
   - Presiona M para ver el mapa
   - Debes tener un icono de jugador

3. **Reiniciar CS2**:
   - A veces el juego se queda en estado raro
   - Cerrar completamente y reabrir

4. **Reinyectar DLL**:
   - Presionar END para desinyectar
   - Cerrar launcher
   - Volver a inyectar

---

## 📝 RESUMEN

**Tu situación actual**:
- ✅ DLL inyectada correctamente
- ✅ Hooks funcionando
- ✅ Menú funcional
- ❌ LocalPlayer no detectado

**Próximos pasos**:
1. Spawnearte en el juego (estar vivo)
2. Presionar F9 de nuevo
3. Si falla, actualizar offset de `dwLocalPlayerPawn`
4. Si sigue fallando, usar método de búsqueda dinámica

**Offset a probar**:
```json
"dwLocalPlayerPawn": 33975136  // 0x2066E00 (Mar 5, 2026)
```

---

**Última Actualización**: 2026-03-09  
**Estado**: Diagnóstico completo - Listo para fix
