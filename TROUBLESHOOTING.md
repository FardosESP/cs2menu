# CS2 Menu - Troubleshooting Guide

## Problema: "Ninguna función funciona"

### Diagnóstico paso a paso:

#### 1. Verificar que el DLL se inyectó correctamente

Cuando inyectas el DLL, deberías ver una consola de debug con mensajes como:

```
========================================
      CS2MENU DEBUG CONSOLE             
========================================
[*] DLL Inyectada. Inicializando...
[*] Ventana de CS2 encontrada: XXXXXXXX
[*] Direccion de Present: XXXXXXXX
[+] Hook de 64 bits aplicado correctamente.
[*] Hook instalado.
[*] INSERT = mostrar/ocultar menu
[*] END    = desinyectar
```

**Si no ves esta consola:**
- El DLL no se inyectó correctamente
- Usa el launcher: `cs2menu_launcher.exe`
- O inyecta manualmente con un inyector externo

#### 2. Verificar inicialización de Features

Después de que ImGui se inicialice (cuando entras al juego), deberías ver:

```
[+] ImGui inicializado en hkPresent.
[*] Inicializando Features...
[*] Inicializando OffsetManager...
[+] OffsetManager inicializado: XX offsets cargados
[+] Version del juego: Build 14138 (2026-03-05)
[+] Validacion de offsets exitosa
[+] client.dll base: 0xXXXXXXXX
[+] EntitySystem: 0xXXXXXXXX
[+] ViewMatrix: 0xXXXXXXXX
[+] Features inicializadas correctamente
```

**Si ves errores aquí:**
- `[-] No se pudo obtener client.dll` → El juego no está corriendo o el proceso es incorrecto
- `[-] EntitySystem address is invalid` → Los offsets están desactualizados
- `[-] ViewMatrix address is invalid` → Los offsets están desactualizados

#### 3. Verificar que estás en partida activa

Las features (ESP, Aimbot, etc.) SOLO funcionan cuando estás en una partida activa. Deberías ver:

```
[DEBUG] LocalPawnAddr: 0xXXXXXXXX
[DEBUG] LocalPawn: 0xXXXXXXXX
[SCAN] Buscando offsets correctos (rango amplio 0x0-0x1000)...
[CANDIDATE] Health en 0x76c = 62
[CANDIDATE] Team en 0xd70 = 2
[DEBUG] Con offsets actuales (0x76c, 0xd70):
[DEBUG] Health: 62, Team: 2
```

**Si ves:**
- `[WARNING] LocalPawn es NULL - no estas en partida?` → Estás en el menú principal, no en partida
- `[ERROR] No se encontraron valores validos!` → Los offsets están incorrectos o no estás jugando

**IMPORTANTE:** Debes estar:
- ✅ En una partida activa (Deathmatch, Casual, Competitive, etc.)
- ✅ Vivo (no muerto ni espectando)
- ✅ Jugando (no en pausa, no en menú de escape)

#### 4. Verificar que las features están habilitadas en el menú

Presiona `INSERT` para abrir el menú y verifica:

**ESP:**
- ✅ ESP Enabled
- ✅ Boxes
- ✅ Health
- ✅ Distance
- Ajusta "Max Distance" si no ves a nadie

**Aimbot:**
- ✅ Aimbot Enabled
- Ajusta FOV (campo de visión)
- Ajusta Smooth (suavizado)

**Visuals:**
- ✅ Custom Crosshair (debería funcionar siempre)
- ✅ No Flash
- ✅ FOV Changer

**Misc:**
- ✅ Radar Hack
- ✅ Bunny Hop

#### 5. Problemas comunes y soluciones

##### ESP no muestra a nadie

**Causas posibles:**
1. No hay enemigos cerca → Aumenta "Max Distance" en el menú
2. "Team Check" activado → Desactívalo para ver a todos
3. "Dormant Check" activado → Desactívalo
4. Offsets incorrectos → Actualiza offsets desde cs2-dumper

**Solución:**
```
1. Abre el menú (INSERT)
2. Ve a la pestaña ESP
3. Desactiva "Team Check"
4. Desactiva "Dormant Check"
5. Aumenta "Max Distance" a 5000
6. Verifica que "ESP Enabled" esté activado
```

##### Aimbot no funciona

**Causas posibles:**
1. FOV muy pequeño → Aumenta el FOV
2. Smooth muy alto → Reduce el smooth
3. No hay enemigos en el FOV
4. Offsets de viewAngles incorrectos

**Solución:**
```
1. Abre el menú (INSERT)
2. Ve a la pestaña Aimbot
3. Aumenta FOV a 180
4. Reduce Smooth a 1.0
5. Desactiva "Visible Only"
6. Desactiva "Team Check"
```

##### Crosshair custom no aparece

**Causas posibles:**
1. No está habilitado en el menú
2. Tamaño muy pequeño
3. Color transparente

**Solución:**
```
1. Abre el menú (INSERT)
2. Ve a la pestaña Visuals
3. Activa "Custom Crosshair"
4. Aumenta "Size" a 10
5. Aumenta "Gap" a 5
6. Cambia el color a algo visible (rojo, verde, etc.)
```

##### No Flash no funciona

**Causas posibles:**
1. Offset de m_flFlashDuration incorrecto
2. No estás siendo flasheado

**Solución:**
```
1. Activa "No Flash" en el menú
2. Pídele a un compañero que te flashee
3. Si sigue sin funcionar, los offsets están desactualizados
```

#### 6. Actualizar offsets

Si nada funciona, probablemente los offsets están desactualizados. Sigue estos pasos:

1. Ve a: https://github.com/a2x/cs2-dumper
2. Descarga el último `offsets.json` y `client_dll.json`
3. Colócalos en la carpeta del ejecutable
4. Reinicia CS2 y vuelve a inyectar el DLL

O usa el OffsetManager:
```
1. Crea un archivo "offsets.json" en la carpeta del DLL
2. Copia los offsets de cs2-dumper
3. El OffsetManager los cargará automáticamente
```

#### 7. Verificar versión de CS2

El cheat está diseñado para **CS2 Build 14138 (2026-03-05)**.

Para verificar tu versión de CS2:
1. Abre CS2
2. Ve a Configuración → Juego
3. Mira la versión en la esquina inferior

Si tu versión es diferente, DEBES actualizar los offsets.

#### 8. Logs de debug útiles

Cuando reportes un problema, incluye estos logs de la consola:

```
[+] client.dll base: 0xXXXXXXXX
[+] EntitySystem: 0xXXXXXXXX
[+] ViewMatrix: 0xXXXXXXXX
[DEBUG] LocalPawn: 0xXXXXXXXX
[DEBUG] Health: XX, Team: X
[ESP] Entidades válidas encontradas: X
[ESP] Entidades dibujadas: X
```

## Resumen de verificación rápida

✅ **Checklist:**
- [ ] Consola de debug visible
- [ ] "Features inicializadas correctamente" en consola
- [ ] Estás en partida activa (no en menú)
- [ ] Estás vivo (no muerto)
- [ ] Features habilitadas en el menú (INSERT)
- [ ] LocalPawn no es NULL
- [ ] Health y Team se leen correctamente
- [ ] Offsets actualizados para tu versión de CS2

## Contacto y soporte

Si después de seguir todos estos pasos el problema persiste:

1. Copia TODA la salida de la consola de debug
2. Indica tu versión de CS2
3. Describe exactamente qué no funciona
4. Indica qué configuración tienes en el menú

