# Cómo Actualizar Offsets de CS2

## ⚠️ IMPORTANTE
Los offsets cambian con cada actualización de CS2. Si el ESP u otras features no funcionan, necesitas actualizar los offsets.

## 📥 Método 1: Usar cs2-dumper (Recomendado)

1. Ve a: https://github.com/a2x/cs2-dumper
2. Descarga el último release o mira los offsets en el README
3. Busca los siguientes offsets en `offsets.json` o `offsets.cs`:

### Offsets Principales (client.dll):
```
dwEntityList
dwLocalPlayerPawn  
dwViewMatrix
dwLocalPlayerController
dwViewAngles
```

### Offsets de Entidad:
```
m_iHealth
m_iTeamNum
m_pGameSceneNode
m_vecAbsOrigin
m_hPlayerPawn
m_fFlags
m_vecVelocity
m_bSpotted
m_bDormant
```

4. Abre `src/SDK.h` y actualiza los valores en `namespace Offsets`

## 📝 Método 2: Usar Cheat Engine / ReClass

1. Abre CS2 y Cheat Engine
2. Adjunta Cheat Engine a cs2.exe
3. Busca los offsets manualmente:
   - **LocalPlayer**: Busca tu vida (100), filtra, recibe daño, filtra de nuevo
   - **EntityList**: Busca la vida de un enemigo, encuentra el patrón
   - **ViewMatrix**: Busca valores float que cambien con la cámara

## 🔧 Método 3: Pattern Scanning (Avanzado)

El código ya incluye `Memory.cpp` con funciones de pattern scanning. Puedes:

1. Buscar patterns conocidos en foros/GitHub
2. Añadirlos a `OffsetUpdater.cpp`
3. El sistema los encontrará automáticamente

## 📊 Verificar si los Offsets Funcionan

1. Inyecta la DLL en CS2
2. Abre la consola de debug (se crea automáticamente)
3. Busca estos mensajes:

```
[+] client.dll base: 0x...
[+] EntitySystem: 0x...
[+] ViewMatrix: 0x...
[DEBUG] LocalPlayerPawn address: 0x...
[DEBUG] Player health: 100
[+] Jugador local encontrado correctamente!
```

4. Activa el ESP y busca:
```
[ESP] Entidades válidas encontradas: X
[ESP] Entidades dibujadas: Y
```

## ❌ Problemas Comunes

### "Entidades válidas: 0"
- Los offsets de EntityList están mal
- Actualiza `dwEntityList`

### "Entidades válidas: X, Dibujadas: 0"
- ViewMatrix incorrecta
- Actualiza `dwViewMatrix`
- O los filtros están muy restrictivos (desactiva teamCheck)

### "Player health: -1" o valores raros
- LocalPlayerPawn incorrecto
- Actualiza `dwLocalPlayerPawn`
- O los offsets de m_iHealth están mal

## 🔄 Offsets que Cambian Frecuentemente

Estos offsets cambian con CADA actualización de CS2:
- dwEntityList
- dwLocalPlayerPawn
- dwViewMatrix
- dwViewAngles

Estos offsets son más estables (cambian raramente):
- m_iHealth
- m_iTeamNum
- m_vecAbsOrigin
- m_fFlags

## 📚 Recursos Útiles

- cs2-dumper: https://github.com/a2x/cs2-dumper
- UnknownCheats CS2: https://www.unknowncheats.me/forum/counter-strike-2-a/
- CS2 Offsets Discord: Busca en servidores de game hacking

## 🛠️ Ejemplo de Actualización

Si cs2-dumper muestra:
```json
{
  "dwEntityList": "0x19C5A78",
  "dwLocalPlayerPawn": "0x1825B40"
}
```

Actualiza en `src/SDK.h`:
```cpp
namespace Offsets
{
    constexpr uintptr_t dwEntityList = 0x19C5A78;  // ← CAMBIAR AQUÍ
    constexpr uintptr_t dwLocalPlayerPawn = 0x1825B40;  // ← CAMBIAR AQUÍ
    // ...
}
```

Luego recompila:
```bash
cmake --build cmake-build-release-visual-studio --config Release
```
