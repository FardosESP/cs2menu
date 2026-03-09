# Problema con EntityList en CS2

## Situación Actual

El cheat no puede leer entidades de la EntityList. Los síntomas son:

1. `ChunkPtr: 0x0` - Todos los chunks devuelven NULL
2. `Entidades NULL: 64` - Ninguna entidad se encuentra
3. `GetBaseEntity()` devuelve NULL para todos los índices

## Causa del Problema

El offset `dwEntityList` (0x24AE628) de cs2-dumper apunta a una dirección, pero la fórmula de lectura no funciona:

```cpp
// Fórmula actual (NO FUNCIONA):
int listIndex = (index & 0x7FFF) >> 9;
uintptr_t listEntry = *(uintptr_t*)(entityList + (8 * listIndex) + 16);
uintptr_t entityPtr = *(uintptr_t*)(listEntry + (120 * (index & 0x1FF)));
```

Cuando leemos `entityList + 16`, obtenemos 0x0, lo que significa que:
- O la estructura de EntityList cambió en CS2
- O `dwEntityList` no apunta directamente a la estructura
- O necesitamos leer un puntero adicional primero

## Posibles Soluciones

### Opción 1: Leer EntityList como Array Simple
Algunos cheats antiguos de CS:GO usaban:
```cpp
uintptr_t entity = *(uintptr_t*)(entityList + (i * 0x10));
```

### Opción 2: Dereferenc iar dwEntityList
Tal vez `dwEntityList` es un puntero a un puntero:
```cpp
uintptr_t entityListPtr = *(uintptr_t*)(clientBase + dwEntityList);
// Luego usar entityListPtr en lugar de (clientBase + dwEntityList)
```

### Opción 3: Usar Pattern Scanning
En lugar de confiar en offsets estáticos, escanear patrones en memoria para encontrar la EntityList dinámicamente.

### Opción 4: Leer desde dwLocalPlayerPawn Directamente
Olvidarse de la EntityList y solo leer el LocalPlayer desde `dwLocalPlayerPawn`, luego iterar desde ahí.

## Lo Que Necesitamos

Para resolver esto necesitamos:

1. **Verificar la estructura real de EntityList en CS2**
   - Usar ReClass.NET o Cheat Engine
   - Inspeccionar la memoria en `clientBase + dwEntityList`
   - Ver cómo están organizadas las entidades

2. **Comparar con otros cheats de CS2**
   - Buscar en GitHub cheats open-source de CS2
   - Ver cómo leen la EntityList
   - Copiar la fórmula que funciona

3. **Probar métodos alternativos**
   - Intentar leer como array simple
   - Intentar dereferenc iar el puntero
   - Intentar diferentes offsets (0x8, 0x10, 0x20, etc.)

## Estado Actual

El cheat está compilado y funciona (no crashea), pero:
- ❌ No puede leer entidades
- ❌ No puede encontrar LocalPlayer
- ❌ ESP no funciona
- ✅ El menú ImGui funciona
- ✅ Los hooks funcionan
- ✅ La inyección funciona

## Próximos Pasos

1. Investigar en UnknownCheats cómo otros leen EntityList en CS2 (2025)
2. Buscar repositorios de GitHub con cheats de CS2 actualizados
3. Usar Cheat Engine para inspeccionar la estructura manualmente
4. Probar diferentes fórmulas de lectura hasta encontrar la correcta

## Referencias

- cs2-dumper: https://github.com/a2x/cs2-dumper
- UnknownCheats CS2: https://www.unknowncheats.me/forum/counter-strike-2-a/
- Fórmula que NO funciona: `(entityList + 8 * ((index & 0x7FFF) >> 9) + 16)`
