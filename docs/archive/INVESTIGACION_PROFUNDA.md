# Investigación Profunda: Cómo Funcionan los Cheats de CS2

## PASO 1: FUNDAMENTOS - ¿Cómo funciona un cheat?

### 1.1 Arquitectura Básica
```
[CS2 Process] <-- [DLL Inyectada] --> [Memoria del Juego]
     |                                        |
     |                                        |
  [client.dll]                         [Estructuras]
     |                                        |
     +-- LocalPlayer                          |
     +-- EntityList ← AQUÍ ESTÁ EL PROBLEMA   |
     +-- ViewMatrix                           |
```

### 1.2 Lo que SÍ funciona
✅ LocalPlayer encontrado (Health: 62, Team: 2)
✅ m_pGameSceneNode del LocalPlayer funciona (coordenadas válidas)
✅ ViewMatrix accesible
✅ ImGui rendering funciona
✅ Offsets de LocalPlayer correctos

### 1.3 Lo que NO funciona
❌ EntityList NO encuentra otros jugadores
❌ Todos los métodos de iteración fallan (0 entidades)

## PASO 2: ANÁLISIS DEL PROBLEMA

### 2.1 ¿Qué es EntityList?

En CS2, `EntityList` es una estructura que contiene TODOS los objetos del juego:
- Jugadores (nosotros y enemigos)
- Armas
- Granadas
- Props del mapa
- Efectos visuales
- etc.

### 2.2 ¿Cómo se accede normalmente?

**Método Tradicional (CSGO/Source 1):**
```cpp
// En CSGO era simple:
uintptr_t entity = *(uintptr_t*)(entityList + (i * 0x10));
```

**Método CS2 (Source 2) - MÁS COMPLEJO:**
CS2 usa un sistema de "Entity Component System" (ECS) diferente.

### 2.3 Teorías sobre por qué falla

**Teoría 1: dwEntityList apunta a algo diferente**
- Quizás `dwEntityList` ya NO es un array directo
- Puede ser un puntero a una clase con métodos virtuales
- Necesitamos llamar a un método, no leer memoria directamente

**Teoría 2: Solo hay 1 jugador en el servidor**
- Estás probando contra bots
- Los bots pueden no estar en EntityList de la misma forma
- Necesitas probar en servidor online con jugadores reales

**Teoría 3: dwEntityList offset está desactualizado**
- El offset 0x24AE628 puede ser de una versión antigua
- CS2 se actualiza frecuentemente
- Necesitas offsets más recientes

**Teoría 4: Estructura completamente diferente**
- CS2 puede usar un sistema completamente nuevo
- No es un array simple
- Requiere ingeniería inversa más profunda

## PASO 3: PRUEBAS SISTEMÁTICAS

### 3.1 Verificar si estás solo en el servidor
```
ACCIÓN: Únete a un servidor Deathmatch ONLINE con 10+ jugadores
RAZÓN: Los bots pueden no aparecer en EntityList
```

### 3.2 Verificar dwEntityList
```cpp
// Leer los primeros 256 bytes de dwEntityList
// Ver qué estructura tiene realmente
for (int i = 0; i < 256; i += 8) {
    uintptr_t value = *(uintptr_t*)(dwEntityList + i);
    printf("[+0x%x] = 0x%llx\n", i, value);
}
```

### 3.3 Buscar EntityList manualmente
```cpp
// Escanear memoria cerca de LocalPlayer
// Los jugadores suelen estar cerca en memoria
uintptr_t localPlayer = 0x7ffb1abe06f8;
for (uintptr_t addr = localPlayer - 0x100000; 
     addr < localPlayer + 0x100000; 
     addr += 0x1000) {
    // Buscar patrones de health/team válidos
}
```

## PASO 4: SOLUCIONES ALTERNATIVAS

### 4.1 Usar GetHighestEntityIndex
```cpp
// CS2 tiene un método para obtener el índice máximo
int maxIndex = *(int*)(entitySystem + 0x1520);
// Luego iterar solo hasta ese índice
```

### 4.2 Usar Controller en lugar de Pawn
```cpp
// En CS2, los jugadores tienen:
// - Controller (información del jugador)
// - Pawn (entidad física)
// Quizás necesitamos iterar Controllers, no Pawns
```

### 4.3 Usar m_iIDEntIndex (Crosshair ID)
```cpp
// Cuando apuntas a alguien, m_iIDEntIndex tiene su índice
// Podemos usar esto para verificar si EntityList funciona
int crosshairID = *(int*)(localPlayer + 0x3EAC);
if (crosshairID > 0) {
    // Intentar obtener esa entidad específica
    uintptr_t entity = GetEntity(crosshairID);
}
```

## PASO 5: INVESTIGACIÓN EN FOROS

### 5.1 Búsquedas recomendadas
- "CS2 entity list iteration 2026"
- "CS2 GetBaseEntity not working"
- "CS2 entity system changed"
- "CS2 latest offsets march 2026"

### 5.2 Fuentes confiables
- UnknownCheats.me (foros de game hacking)
- GitHub: a2x/cs2-dumper (offsets actualizados)
- GitHub: sezzyaep/CS2-OFFSETS
- Discord de comunidades de CS2 cheating

## PASO 6: PLAN DE ACCIÓN

### Acción Inmediata 1: Probar en servidor online
```
1. Únete a Deathmatch oficial de Valve
2. Espera a que haya 10+ jugadores
3. Presiona F9 para ejecutar scanner
4. Reporta resultados
```

### Acción Inmediata 2: Verificar offsets actualizados
```
1. Ir a https://github.com/a2x/cs2-dumper
2. Descargar offsets.json más reciente
3. Comparar dwEntityList con el nuestro (0x24AE628)
4. Si es diferente, actualizar
```

### Acción Inmediata 3: Escanear estructura de dwEntityList
```
Voy a crear un scanner que:
1. Lee los primeros 512 bytes de dwEntityList
2. Busca patrones de punteros válidos
3. Intenta encontrar la estructura real
```

### Acción Inmediata 4: Usar método alternativo
```
Si EntityList no funciona:
1. Usar solo LocalPlayer
2. Usar m_iIDEntIndex para detectar enemigo en crosshair
3. Implementar Triggerbot (dispara cuando apuntas)
4. Esto al menos demuestra que el cheat funciona
```

## PASO 7: CONCLUSIÓN ACTUAL

### Lo que sabemos con certeza:
1. ✅ El cheat se inyecta correctamente
2. ✅ LocalPlayer funciona perfectamente
3. ✅ Los offsets de LocalPlayer son correctos
4. ✅ ImGui rendering funciona
5. ❌ EntityList NO funciona con ningún método probado

### Hipótesis más probable:
**El offset `dwEntityList` (0x24AE628) está desactualizado o la estructura cambió.**

CS2 se actualiza frecuentemente y los offsets cambian. Necesitamos:
1. Offsets más recientes de cs2-dumper
2. O ingeniería inversa manual para encontrar EntityList

### Próximo paso crítico:
**Probar en servidor online con jugadores reales.**
Si aún así no funciona, entonces definitivamente es un problema de offsets/estructura.

## REFERENCIAS

- [CS2 Dumper](https://github.com/a2x/cs2-dumper)
- [UnknownCheats CS2](https://www.unknowncheats.me/forum/counter-strike-2-a/)
- [CS2 Offsets](https://github.com/sezzyaep/CS2-OFFSETS)
