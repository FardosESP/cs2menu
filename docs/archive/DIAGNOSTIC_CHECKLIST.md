# CS2 Menu - Diagnostic Checklist

Use esta lista para diagnosticar problemas cuando las features no funcionan.

## ✅ Checklist de Diagnóstico

### 1. Verificar Inyección del DLL
- [ ] La consola de debug aparece
- [ ] Mensaje: `[*] DLL Inyectada. Inicializando...`
- [ ] Mensaje: `[*] Hook instalado.`
- [ ] Mensaje: `[+] ImGui inicializado en hkPresent.`

**Si NO aparece la consola**: El DLL no se inyectó correctamente.

### 2. Verificar Inicialización de Features
- [ ] Mensaje: `[*] Inicializando Features...`
- [ ] Mensaje: `[+] OffsetManager inicializado: XX offsets cargados`
- [ ] Mensaje: `[+] client.dll base: 0xXXXXXXXX`
- [ ] Mensaje: `[+] Features inicializadas correctamente`

**Si hay errores aquí**: Los offsets están desactualizados o CS2 no está corriendo.

### 3. Verificar LocalPlayer
- [ ] Mensaje: `[DEBUG] LocalPawn: 0xXXXXXXXX` (no 0x0)
- [ ] Mensaje: `[DEBUG] Health: XX, Team: X` (valores válidos)

**Si LocalPawn es 0x0 o NULL**: No estás en partida activa.

### 4. Verificar que Features::Update se llama
Después de ~5 segundos en partida, deberías ver:
- [ ] Mensaje: `[Features] Update llamado XXX veces`

**Si NO aparece**: Features::Update no se está ejecutando.

### 5. Verificar estado de LocalPlayer
- [ ] Mensaje: `[DEBUG] Config estado - ESP enabled: 1, Aimbot enabled: X, Crosshair: 1`

O si LocalPlayer es NULL:
- [ ] Mensaje: `[DEBUG] LocalPlayer es NULL - no se ejecutan features`

**Si ves "LocalPlayer es NULL"**: Estás muerto, espectando, o en menú.

### 6. Verificar que RenderESP se llama
Si ESP está habilitado y LocalPlayer es válido:
- [ ] Mensaje: `[ESP] RenderESP llamado XXX veces`

**Si NO aparece**: RenderESP no se está llamando (cfg_esp.enabled = false).

### 7. Verificar punteros de ESP
- [ ] Mensaje: `[ESP] Punteros NULL - EntitySystem: OK, ViewMatrix: OK, LocalPlayer: OK`

**Si alguno es NULL**: Los offsets están incorrectos.

### 8. Verificar entidades encontradas
- [ ] Mensaje: `[ESP] Estado actual - Validas: X, Dibujadas: X`

**Si Validas = 0**: No hay otros jugadores cerca o EntityList no funciona.
**Si Dibujadas = 0**: Los jugadores están fuera de rango o filtrados.

### 9. Verificar Crosshair
El crosshair debería dibujarse SIEMPRE (incluso sin LocalPlayer):
- [ ] Mensaje: `[Crosshair] DrawCustomCrosshair llamado XXX veces`

**Si NO aparece**: DrawCustomCrosshair no se está llamando.

### 10. Verificar Misc Features
- [ ] Mensaje: `[Misc] RunMisc llamado XXX veces`
- [ ] Mensaje: `[Misc] Config - Bhop: X, Radar: X`

**Si NO aparece**: RunMisc no se está llamando.

## 🔍 Interpretación de Resultados

### Caso 1: No aparece NINGÚN mensaje de Features
**Problema**: Features::Update no se está ejecutando.
**Solución**: Verificar que ImGui_Renderer::Render() llama a Features::Update().

### Caso 2: Aparece "LocalPlayer es NULL" constantemente
**Problema**: No estás en partida activa o estás muerto.
**Solución**: 
1. Entra en una partida (Deathmatch recomendado)
2. Asegúrate de estar vivo
3. No estés en pausa ni en menú de escape

### Caso 3: RenderESP se llama pero Dibujadas = 0
**Problema**: No hay jugadores en rango o están filtrados.
**Solución**:
1. Abre el menú (INSERT)
2. Desactiva "Team Check"
3. Desactiva "Dormant Check"
4. Aumenta "Max Distance" a 5000

### Caso 4: Crosshair no se dibuja
**Problema**: DrawCustomCrosshair no se llama o DrawList es NULL.
**Solución**: Verificar que cfg_vis.crosshair = true y que ImGui está inicializado.

### Caso 5: Config muestra valores incorrectos
**Problema**: Las configuraciones no se están leyendo correctamente.
**Solución**: Verificar que ImGui_Renderer.cpp tiene los valores por defecto correctos.

## 📋 Comandos de Diagnóstico Rápido

### Ver solo mensajes de Features
```
[Features] Update llamado
[DEBUG] Config estado
[DEBUG] LocalPlayer es NULL
```

### Ver solo mensajes de ESP
```
[ESP] RenderESP llamado
[ESP] Punteros NULL
[ESP] Estado actual
```

### Ver solo mensajes de Crosshair
```
[Crosshair] DrawCustomCrosshair llamado
```

### Ver solo mensajes de Misc
```
[Misc] RunMisc llamado
[Misc] Config
```

## 🚨 Problemas Comunes

### "Features::Update no se llama"
- ImGui_Renderer::Render() no está llamando a Features::Update()
- Verificar que bImGuiReady = true

### "LocalPlayer siempre NULL"
- Offsets de dwLocalPlayerPawn incorrectos
- No estás en partida activa
- Estás muerto o espectando

### "ESP enabled = 0"
- cfg_esp.enabled está en false
- Verificar ImGui_Renderer.cpp línea ~226

### "DrawList es NULL"
- ImGui no está inicializado correctamente
- ImGui::GetBackgroundDrawList() falla

### "Entidades Validas = 0"
- EntityList offsets incorrectos
- GetBaseEntity() retorna NULL para todos los índices
- No hay otros jugadores en el servidor

## 💡 Tips de Diagnóstico

1. **Siempre revisa la consola de debug primero**
2. **Espera al menos 10 segundos en partida antes de diagnosticar**
3. **Los mensajes de debug aparecen cada ~5 segundos**
4. **Si no ves NINGÚN mensaje de Features, el problema es en ImGui_Renderer**
5. **Si ves mensajes pero nada funciona, el problema es en las configuraciones**
6. **Si todo se llama pero no se dibuja, el problema es en ImGui o DirectX**

## 📞 Información para Reportar

Cuando reportes un problema, incluye:

1. **Toda la salida de la consola** (desde el inicio hasta después de 10 segundos en partida)
2. **Tu versión de CS2** (visible en configuración del juego)
3. **Qué estabas haciendo** (en menú, en partida, muerto, vivo, etc.)
4. **Qué features probaste** (ESP, Aimbot, Crosshair, etc.)
5. **Qué configuración tienes en el menú** (abre con INSERT y revisa)

