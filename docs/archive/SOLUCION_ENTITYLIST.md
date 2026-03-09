# Solución: EntityList Chunks NULL

## PROBLEMA IDENTIFICADO

```
[DEBUG] Chunk 0 pointer: 0x0 ✗ NULL/INVALID
[DEBUG] Chunk 1 pointer: 0x0 ✗ NULL/INVALID
[STATS] NULL entities: 64
```

Los chunk pointers están en NULL porque **dwEntityList NO apunta directamente a un array de chunks**.

## ESTRUCTURA REAL DE CS2

En CS2, `dwEntityList` apunta a `CGameEntitySystem`, que tiene esta estructura:

```cpp
class CGameEntitySystem
{
    // +0x00: vtable
    // +0x10: EntityList (CEntityIdentities)
    // +0x1520: HighestEntityIndex
    
    char pad_0[0x10];           // 0x00
    void* m_pEntityList;        // 0x10 <- AQUÍ están los chunks!
    char pad_1[0x1510];         // 0x18
    int m_iHighestEntityIndex;  // 0x1528
};
```

## FÓRMULA CORRECTA

La dirección `dwEntityList` que obtenemos de cs2-dumper ya apunta a `CGameEntitySystem`.

Para obtener el array de chunks:
```cpp
uintptr_t entitySystem = client.dll + dwEntityList;
uintptr_t entityListArray = *(uintptr_t*)(entitySystem + 0x10);  // Leer puntero en +0x10
```

Luego, para obtener una entidad:
```cpp
int chunkIndex = index / 512;
int indexInChunk = index % 512;

uintptr_t chunkPtr = *(uintptr_t*)(entityListArray + 16 + (chunkIndex * 8));
uintptr_t entityPtr = *(uintptr_t*)(chunkPtr + (indexInChunk * 120));
```

## IMPLEMENTACIÓN

Necesitamos modificar `GetBaseEntity()` en `SDK.h`:

```cpp
C_BaseEntity* GetBaseEntity(int index)
{
    if (index < 0 || index > 8192) return nullptr;
    
    __try
    {
        // CGameEntitySystem structure:
        // +0x00: vtable
        // +0x10: m_pEntityList (pointer to entity list array)
        
        uintptr_t entitySystem = (uintptr_t)this;
        
        // Read the entity list pointer at +0x10
        uintptr_t entityListArray = *(uintptr_t*)(entitySystem + 0x10);
        if (!entityListArray || entityListArray < 0x1000) return nullptr;
        
        // Calculate chunk index
        int chunkIndex = index / 512;
        int indexInChunk = index % 512;
        
        // Read chunk pointer
        uintptr_t chunkPtr = *(uintptr_t*)(entityListArray + 16 + (chunkIndex * 8));
        if (!chunkPtr || chunkPtr < 0x1000) return nullptr;
        
        // Read entity pointer from chunk
        uintptr_t entityPtr = *(uintptr_t*)(chunkPtr + (indexInChunk * 120));
        if (!entityPtr || entityPtr < 0x1000) return nullptr;
        
        return (C_BaseEntity*)entityPtr;
    }
    __except(EXCEPTION_EXECUTE_HANDLER)
    {
        return nullptr;
    }
}
```

## ALTERNATIVA: Usar offset directo

Algunos dumps muestran que el offset puede variar:
- Algunos usan +0x10
- Otros usan +0x08
- Otros usan +0x18

Podemos probar cada uno hasta encontrar el correcto.

## VERIFICACIÓN

Después de implementar, el scanner debería mostrar:
```
[DEBUG] Chunk 0 pointer: 0x[dirección válida] ✓ VALID
[DEBUG] Chunk 1 pointer: 0x[dirección válida] ✓ VALID
[ENTITY 1] Valid player found!
  Health: XX
  Team: X
```

## REFERENCIAS

- CGameEntitySystem offset: https://github.com/a2x/cs2-dumper (buscar m_pEntityList)
- Estructura similar en CSGO/Source 2
- UnknownCheats threads sobre CS2 EntityList
