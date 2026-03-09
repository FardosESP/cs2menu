# Instrucciones de Prueba - Nueva Versión

## CAMBIOS REALIZADOS

### 1. Scanner Inteligente
- ✅ Ya NO se ejecuta automáticamente al inyectar (cuando estás en menú)
- ✅ Se ejecuta AUTOMÁTICAMENTE cuando entras en partida por primera vez
- ✅ Puedes ejecutarlo manualmente presionando **F9** en cualquier momento

### 2. Detección de Partida
- ✅ El cheat detecta automáticamente cuando entras/sales de partida
- ✅ Muestra mensaje: "ENTRASTE EN PARTIDA - ACTIVANDO ESP"
- ✅ Solo busca entidades cuando estás jugando

### 3. Mensajes de Debug Limpiados
- ✅ Eliminados mensajes repetitivos cada 300 frames
- ✅ Solo muestra información relevante cuando cambia algo
- ✅ Consola más limpia y fácil de leer

## CÓMO PROBAR

### Paso 1: Inyectar la DLL
```
1. Abre CS2 (puede estar en menú principal)
2. Inyecta: build\Release\Release\cs2menu.dll
3. Verás en consola:
   - "Offset scanner disponible - se ejecutara automaticamente cuando entres en partida"
   - "O presiona F9 para ejecutar el scanner manualmente"
```

### Paso 2: Entrar en Partida
```
1. Entra en cualquier partida (Deathmatch, Casual, contra Bots, etc.)
2. Espera a que spawnes (tengas vida)
3. Verás en consola:
   ╔═══════════════════════════════════════════════════════════╗
   ║              ENTRASTE EN PARTIDA - ACTIVANDO ESP          ║
   ╚═══════════════════════════════════════════════════════════╝
   [INFO] LocalPlayer detectado - Health: XX
   [*] Ejecutando scanner de offsets automaticamente...
```

### Paso 3: Verificar Scanner
```
El scanner mostrará:
- ✅ LocalPlayer encontrado (Health, Team)
- ✅ Candidatos para m_pGameSceneNode
- ✅ Candidatos para m_vecAbsOrigin
- ✅ Candidatos para m_vecViewOffset
- ✅ Escaneo de EntityList buscando otros jugadores
```

### Paso 4: Verificar ESP
```
Abre el menú (INSERT) y activa:
- ESP > Boxes: ON
- ESP > Health: ON
- ESP > Distance: ON

Mira la consola:
[ESP] Entidades - Validas: X, Dibujadas: Y

Si X > 0 y Y > 0: ¡FUNCIONA! Deberías ver boxes en pantalla
Si X = 0 y Y = 0: No se detectan jugadores (ver soluciones abajo)
```

## TECLAS IMPORTANTES

- **INSERT**: Abrir/cerrar menú
- **F9**: Ejecutar scanner manualmente (útil si cambias de mapa/partida)
- **END**: Desinyectar DLL

## SI NO FUNCIONA

### Problema 1: No se detectan entidades (Validas: 0)

**Solución A: Probar en partida online**
```
Los bots pueden no aparecer correctamente en EntityList.
Prueba en:
- Deathmatch online
- Casual online
- Competitive online
```

**Solución B: Ejecutar scanner con F9**
```
1. Presiona F9 mientras estás en partida
2. Revisa los candidatos de m_pGameSceneNode
3. Si hay múltiples candidatos, anota los offsets
4. Prueba cada uno manualmente (ver abajo)
```

**Solución C: Probar offsets alternativos**
```
Si el scanner muestra candidatos como:
- 0x268
- 0x598 (actual)
- 0x5a0
- 0x5c8

Edita src/SDK.h y src/OffsetManager.cpp:
Cambia m_pGameSceneNode de 0x598 a otro candidato
Recompila y prueba
```

### Problema 2: Se detectan entidades pero no se dibujan

**Verificar WorldToScreen:**
```
Si ves "Validas: 5, Dibujadas: 0"
El problema es WorldToScreen o ViewMatrix

Solución:
1. Verifica que dwViewMatrix es correcto
2. Prueba actualizar offsets desde cs2-dumper
```

### Problema 3: Crash al entrar en partida

**Solución:**
```
1. Desinyecta (END)
2. Verifica que los offsets son para tu versión de CS2
3. Actualiza offsets desde: https://github.com/a2x/cs2-dumper
4. Reinyecta
```

## ACTUALIZAR OFFSETS MANUALMENTE

Si necesitas probar offsets diferentes:

### Opción 1: Editar Fallback (Recomendado)
```cpp
// En src/SDK.h - namespace Offsets::Fallback
constexpr uintptr_t m_pGameSceneNode = 0x598;  // Cambiar aquí

// En src/OffsetManager.cpp - LoadFallbackOffsets()
m_offsets["m_pGameSceneNode"] = 0x598;  // Cambiar aquí
```

### Opción 2: Usar offsets.json
```json
{
  "client.dll": {
    "m_pGameSceneNode": "0x598",
    "m_vecAbsOrigin": "0xC4",
    "m_vecViewOffset": "0x790"
  }
}
```
Coloca este archivo en la carpeta raíz del proyecto.

## INFORMACIÓN DE DEBUG

### Consola Limpia
La nueva versión muestra:
```
[INFO] Entrando/saliendo de partida
[ESP] Entidades - Validas: X, Dibujadas: Y (solo cuando cambia)
[SCAN] Resultados del scanner (solo cuando se ejecuta)
```

### Consola Anterior (Ruidosa)
La versión anterior mostraba:
```
[Features] Update llamado 300 veces
[ESP] RenderESP llamado 300 veces
[Crosshair] DrawCustomCrosshair llamado 300 veces
... (cada 5 segundos)
```

## PRÓXIMOS PASOS

1. **Inyecta la nueva DLL**
2. **Entra en partida Deathmatch ONLINE** (no bots)
3. **Espera el mensaje "ENTRASTE EN PARTIDA"**
4. **Revisa la consola** para ver cuántas entidades se detectan
5. **Reporta los resultados**:
   - ¿Cuántas entidades válidas?
   - ¿Cuántas dibujadas?
   - ¿Ves boxes en pantalla?
   - ¿Qué dice el scanner sobre m_pGameSceneNode?

## ARCHIVO COMPILADO

La DLL actualizada está en:
```
build\Release\Release\cs2menu.dll
```

Tamaño: ~XXX KB
Fecha: 2026-03-06

## NOTAS IMPORTANTES

- El scanner ya NO se ejecuta en el menú (evita el error "0 entidades")
- Se ejecuta automáticamente al entrar en partida
- Puedes re-ejecutarlo con F9 si cambias de mapa
- Los offsets actuales son: m_pGameSceneNode=0x598, m_vecAbsOrigin=0xC4
- Si no funciona, el scanner te dirá qué offsets probar

¡Buena suerte! 🎯
