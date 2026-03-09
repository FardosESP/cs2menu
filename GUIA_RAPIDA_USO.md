# Guía Rápida de Uso - CS2MENU

## 🚀 INICIO RÁPIDO

### 1. Preparación

1. **Copiar archivos necesarios**:
   ```
   cs2menu.dll          → Junto al launcher
   offsets.json         → Junto a cs2menu.dll
   ```

2. **Ejecutar Launcher como Administrador**:
   - Click derecho en `Launcher.exe`
   - "Ejecutar como administrador"

### 2. Inyección

1. **Abrir CS2** (esperar a que cargue completamente)
2. **Ejecutar Launcher**
3. Esperar mensaje: `[OK] Inyeccion completada`
4. Ver consola de debug en CS2

### 3. Uso en Juego

#### Controles Básicos
- **INSERT** = Abrir/Cerrar menú
- **F9** = Ejecutar scanner de offsets (si hay problemas)
- **END** = Desinyectar DLL

#### Primera Vez
1. Entrar en una partida (Deathmatch recomendado)
2. Esperar a spawnearse como jugador
3. Presionar **INSERT** para abrir el menú
4. Activar ESP en la pestaña "Visuals"

---

## 📊 INTERPRETANDO LA CONSOLA

### ✅ Inyección Exitosa

```
[+] Modulos del juego cargados correctamente.
[+] Hook instalado correctamente.
[+] OffsetManager inicializado: 35 offsets cargados
[+] Features inicializadas correctamente
[+] ImGui inicializado en hkPresent.
```

### ⚠️ Advertencias Comunes

#### "Failed to open: offsets.json"
**Causa**: Archivo no encontrado en el directorio de CS2.

**Solución**:
1. Copiar `offsets.json` a la misma carpeta que `cs2menu.dll`
2. O dejar que use fallback offsets (automático)

#### "LocalPlayerPawn is NULL"
**Causa**: No estás en una partida activa.

**Solución**:
1. Entrar en una partida (Deathmatch, Casual, etc.)
2. Esperar a spawnearse
3. Presionar F9 para ejecutar scanner

#### "Controllers: 0, Pawns: 0, Dibujados: 0"
**Causa**: Offsets desactualizados o no hay otros jugadores.

**Solución**:
1. Verificar que hay otros jugadores en el servidor
2. Si estás solo, entrar en servidor con jugadores
3. Si hay jugadores pero no detecta, actualizar offsets

---

## 🎮 USANDO EL MENÚ

### Pestaña: Visuals (ESP)

#### ESP Básico
- **Enable ESP**: Activar/desactivar ESP
- **Boxes**: Cajas alrededor de jugadores
- **Box Filled**: Cajas con relleno semi-transparente
- **Health Bar**: Barra de vida al lado de la caja
- **Health Text**: Texto con HP numérico
- **Distance**: Distancia en metros

#### ESP Avanzado
- **Skeleton**: Esqueleto del jugador
- **Name**: Nombre del jugador
- **Weapon**: Arma equipada
- **Snaplines**: Líneas desde el centro de la pantalla
- **Glow**: Resplandor alrededor del jugador

#### Configuración
- **Max Distance**: Distancia máxima de detección (metros)
- **Box Thickness**: Grosor de las líneas
- **Team Check**: No dibujar compañeros de equipo
- **Dormant Check**: No dibujar jugadores dormidos

#### Colores
- **Box Color**: Color de las cajas (enemigos)
- **Team Color**: Color de las cajas (equipo)
- **Skeleton Color**: Color del esqueleto
- **Snapline Color**: Color de las líneas
- **Glow Enemy**: Color del resplandor (enemigos)
- **Glow Team**: Color del resplandor (equipo)

### Pestaña: Aimbot

⚠️ **ADVERTENCIA**: Usar aimbot es muy detectable. Úsalo con precaución.

#### Configuración Básica
- **Enable Aimbot**: Activar/desactivar
- **FOV**: Campo de visión del aimbot (grados)
- **Smooth**: Suavizado del movimiento (1 = instantáneo, 10 = muy suave)
- **Bone**: Hueso objetivo (6 = cabeza)

#### Opciones
- **Visible Only**: Solo apuntar a jugadores visibles
- **Team Check**: No apuntar a compañeros
- **Auto Shoot**: Disparar automáticamente (NO IMPLEMENTADO)
- **Silent Aim**: Aimbot silencioso (NO IMPLEMENTADO)

#### RCS (Recoil Control)
- **Enable RCS**: Control de retroceso (NO IMPLEMENTADO)
- **RCS X/Y**: Compensación horizontal/vertical

### Pestaña: Misc

#### Movimiento
- **Bunny Hop**: Salto automático
- **Auto Strafe**: Strafe automático en el aire (NO IMPLEMENTADO)
- **Speed Hack**: Multiplicador de velocidad (NO IMPLEMENTADO)

#### Utilidades
- **Radar Hack**: Mostrar todos los jugadores en el radar
- **Anti-Aim**: Anti-aim para HVH (NO IMPLEMENTADO)

### Pestaña: Visuals (Otros)

#### Efectos Visuales
- **Night Mode**: Modo nocturno (NO IMPLEMENTADO)
- **No Flash**: Eliminar efecto de flashbang
- **No Smoke**: Ver a través del humo (NO IMPLEMENTADO)
- **Fullbright**: Iluminación completa (NO IMPLEMENTADO)

#### Crosshair Personalizado
- **Custom Crosshair**: Activar mira personalizada
- **Size**: Tamaño de la mira
- **Gap**: Espacio central
- **Color**: Color de la mira

#### FOV
- **FOV Changer**: Cambiar campo de visión
- **FOV Value**: Valor del FOV (90 = default)

#### Tercera Persona
- **Third Person**: Vista en tercera persona (NO IMPLEMENTADO)

---

## 🔧 TROUBLESHOOTING

### ESP No Muestra Jugadores

**Síntomas**: Menú funciona, pero no se dibujan cajas.

**Diagnóstico**:
1. Presionar F9 para ejecutar scanner
2. Ver consola: `Controllers: X, Pawns: Y, Dibujados: Z`

**Soluciones**:

#### Si Controllers = 0
- Offsets de `dwEntityList` incorrectos
- Actualizar offsets desde cs2-dumper

#### Si Controllers > 0 pero Pawns = 0
- Offset de `m_hPlayerPawn` incorrecto
- Problema con DecodeHandle

#### Si Pawns > 0 pero Dibujados = 0
- Problema con `m_pGameSceneNode` o `m_vecAbsOrigin`
- Ejecutar scanner y probar candidatos

### Aimbot No Apunta

**Síntomas**: Aimbot activado pero no mueve la mira.

**Causas Posibles**:
1. FOV muy pequeño (aumentar a 180)
2. "Visible Only" activado sin jugadores visibles
3. Offset de `dwViewAngles` incorrecto

**Solución**:
1. Aumentar FOV a 180
2. Desactivar "Visible Only"
3. Verificar que hay jugadores cerca

### Crashes o Freezes

**Síntomas**: CS2 se cierra o congela.

**Causas Posibles**:
1. Offsets muy desactualizados
2. Lectura de memoria inválida
3. Conflicto con otro cheat

**Solución**:
1. Actualizar offsets a última versión
2. Desinyectar (END) y reiniciar CS2
3. Usar solo este cheat (desactivar otros)

### Menú No Abre

**Síntomas**: INSERT no hace nada.

**Causas Posibles**:
1. Hook de SDL no instalado
2. ImGui no inicializado
3. Conflicto de teclas

**Solución**:
1. Ver consola: debe decir `[+] ImGui inicializado`
2. Esperar 5-10 segundos después de inyectar
3. Probar otra tecla (modificar código)

---

## 📝 ACTUALIZANDO OFFSETS

### Método 1: Automático (Recomendado)

1. Ir a https://github.com/a2x/cs2-dumper
2. Buscar archivo `offsets.json` más reciente
3. Copiar valores a tu `offsets.json`
4. Reiniciar CS2 y reinyectar

### Método 2: Scanner Interno

1. Entrar en partida con jugadores
2. Presionar F9
3. Ver candidatos en consola
4. Probar cada candidato manualmente

### Método 3: Manual

1. Buscar offsets en foros (UnknownCheats, etc.)
2. Convertir hex a decimal si es necesario
3. Actualizar `offsets.json`
4. Reinyectar

---

## ⚠️ ADVERTENCIAS DE SEGURIDAD

### VAC (Valve Anti-Cheat)

Este cheat es **DETECTABLE** por VAC. Usar bajo tu propio riesgo.

**Recomendaciones**:
- Usar en cuenta secundaria
- No usar en matchmaking competitivo
- Usar en servidores community/casual
- No abusar (ser discreto)

### Detección

**Comportamientos Detectables**:
- Aimbot muy obvio (FOV pequeño, sin smooth)
- Movimientos no humanos
- Información imposible (wallhack obvio)
- Reportes de otros jugadores

**Cómo Reducir Riesgo**:
- Usar smooth alto en aimbot (5-10)
- No apuntar a través de paredes
- No usar en partidas competitivas
- Jugar "normalmente" con ayudas sutiles

---

## 🎯 CONFIGURACIÓN RECOMENDADA

### Para Principiantes (Discreto)

```
ESP:
- Boxes: ON
- Health Bar: ON
- Distance: ON
- Max Distance: 3000
- Team Check: ON
- Glow: OFF

Aimbot:
- Enable: OFF (o FOV: 180, Smooth: 10)

Misc:
- Radar Hack: ON
- No Flash: ON
```

### Para Usuarios Avanzados

```
ESP:
- Boxes: ON
- Box Filled: ON (alpha bajo)
- Skeleton: ON
- Health Bar: ON
- Name: ON
- Weapon: ON
- Glow: ON
- Max Distance: 5000

Aimbot:
- Enable: ON
- FOV: 90
- Smooth: 3-5
- Visible Only: ON
- Team Check: ON

Misc:
- Bunny Hop: ON
- Radar Hack: ON
- No Flash: ON
```

---

## 📞 SOPORTE

### Logs

Los logs se guardan en:
- Consola de debug (en CS2)
- `cs2menu.log` (si Logger está activado)

### Reportar Bugs

Incluir:
1. Versión de CS2
2. Build del cheat
3. Offsets usados
4. Consola completa
5. Pasos para reproducir

---

**Última Actualización**: 2026-03-09  
**Build**: 14138.2  
**Estado**: Beta - Usar bajo tu propio riesgo
