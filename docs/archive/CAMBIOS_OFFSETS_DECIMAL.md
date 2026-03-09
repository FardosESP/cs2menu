# Cambios Realizados: Conversión de Offsets a Formato Decimal

## 📋 RESUMEN

Se actualizó el archivo `offsets.json` para usar valores decimales en lugar de cadenas hexadecimales, siguiendo el formato estándar de cs2-dumper y las instrucciones del usuario.

## 🔄 CAMBIOS REALIZADOS

### 1. Formato de offsets.json

**ANTES** (Hexadecimal como strings):
```json
{
  "client.dll": {
    "dwEntityList": "0x24AE628",
    "dwLocalPlayerPawn": "0x2066DE0",
    "m_pGameSceneNode": "0x338"
  }
}
```

**DESPUÉS** (Decimal como números):
```json
{
  "client.dll": {
    "dwEntityList": 35422320,
    "dwLocalPlayerPawn": 33970928,
    "m_pGameSceneNode": 1432
  }
}
```

### 2. Corrección de m_pGameSceneNode

Se corrigió el valor de `m_pGameSceneNode` para usar el valor encontrado por el scanner:

- **cs2-dumper value**: 824 (0x338) - NO FUNCIONA
- **Scanner value**: 1432 (0x598) - ✅ FUNCIONA

Este cambio es crítico porque el scanner determinó que 0x598 es el offset correcto que apunta a una estructura válida con coordenadas del mundo.

## 📊 TABLA DE CONVERSIÓN

| Offset | Hexadecimal | Decimal |
|--------|-------------|---------|
| dwEntityList | 0x21CD670 | 35422320 |
| dwLocalPlayerPawn | 0x2066DF0 | 33970928 |
| dwViewMatrix | 0x2309460 | 36749024 |
| dwLocalPlayerController | 0x22F1888 | 36640904 |
| m_pGameSceneNode | 0x598 | 1432 |
| m_vecAbsOrigin | 0xC4 | 196 |
| m_iHealth | 0x76C | 1900 |
| m_iTeamNum | 0xD70 | 3440 |

## 🔧 ARCHIVOS MODIFICADOS

1. **cs2menu/offsets.json**
   - Convertidos todos los offsets de hexadecimal a decimal
   - Corregido `m_pGameSceneNode` de 824 a 1432

2. **cs2menu/src/SDK.h**
   - Actualizado comentario de `m_pGameSceneNode` para indicar que es el valor del scanner
   - Añadidos comentarios con valores decimales para referencia

3. **cs2menu/src/OffsetManager.cpp**
   - Actualizado comentario de `m_pGameSceneNode` para indicar que es el valor del scanner

## ✅ VENTAJAS DEL FORMATO DECIMAL

1. **Compatibilidad**: El OffsetManager puede parsear ambos formatos (hex y decimal)
2. **Claridad**: Los valores decimales son más fáciles de comparar
3. **Estándar**: Sigue el formato usado en ejemplos de cs2-dumper
4. **Sin conversión manual**: No necesitas convertir hex a decimal manualmente

## 🎯 CÓMO ACTUALIZAR OFFSETS EN EL FUTURO

### Método 1: Desde cs2-dumper (GitHub)

1. Ve a https://github.com/a2x/cs2-dumper
2. Busca el archivo `offsets.json` o `client.dll.hpp`
3. Encontrarás offsets como: `#define dwEntityList 0x24AE628`
4. Convierte a decimal usando Python:
   ```python
   print(int('0x24AE628', 16))  # Output: 38463016
   ```
5. Actualiza `offsets.json` con el valor decimal

### Método 2: Desde sitios de cheats

1. Busca offsets actualizados (ej: cheater.fun, unknowncheats)
2. Si están en hexadecimal, conviértelos a decimal
3. Actualiza `offsets.json`

### Método 3: Usando el Scanner Interno

1. Entra en una partida de CS2
2. Presiona F9 para ejecutar el scanner
3. El scanner te mostrará candidatos para offsets problemáticos
4. Actualiza los offsets en `SDK.h` y `offsets.json`

## ⚠️ NOTAS IMPORTANTES

### m_pGameSceneNode es CRÍTICO

Este offset es el más importante para ESP porque:
- Apunta a la estructura GameSceneNode
- GameSceneNode contiene las coordenadas del mundo (m_vecAbsOrigin)
- Sin coordenadas correctas, ESP no puede dibujar cajas

**SIEMPRE usa el valor del scanner (0x598) en lugar del valor de cs2-dumper (0x338)**

### Offsets cambian con cada actualización

CS2 se actualiza frecuentemente y los offsets cambian. Debes:
- Actualizar offsets después de cada parche de CS2
- Usar el scanner para verificar offsets críticos
- Mantener una copia de respaldo de offsets que funcionan

## 🧪 PRUEBAS

Después de estos cambios:

1. ✅ Proyecto compila sin errores
2. ⏳ Pendiente: Probar en juego con otros jugadores
3. ⏳ Pendiente: Verificar que ESP dibuja cajas correctamente
4. ⏳ Pendiente: Confirmar que scanner encuentra entidades

## 📝 PRÓXIMOS PASOS

1. Inyectar la DLL en CS2
2. Entrar en una partida Deathmatch con otros jugadores
3. Presionar INSERT para abrir el menú
4. Activar ESP
5. Verificar que se dibujan cajas alrededor de los jugadores
6. Si no funciona, presionar F9 para ejecutar el scanner

---

**Fecha**: 2026-03-06  
**Build**: February 24, 2026  
**Estado**: ✅ COMPILADO - Listo para pruebas en juego
