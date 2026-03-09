# Guía de Uso: Skin Changer

## Descripción
El Skin Changer permite cambiar las apariencias de armas y cuchillos en CS2 sin necesidad de poseerlas en el inventario.

## Características

### Cuchillos
- **16 modelos disponibles:**
  - Karambit (507)
  - M9 Bayonet (508)
  - Bayonet (500)
  - Butterfly (515)
  - Flip Knife (505)
  - Gut Knife (506)
  - Huntsman (509)
  - Falchion (512)
  - Shadow Daggers (516)
  - Bowie (514)
  - Ursus (519)
  - Stiletto (522)
  - Talon (523)
  - Navaja (520)
  - Classic Knife (503)
  - Skeleton (525)

- **12 acabados populares:**
  - Vanilla (0)
  - Case Hardened (44)
  - Crimson Web (12)
  - Fade (38)
  - Doppler (418)
  - Tiger Tooth (411)
  - Marble Fade (413)
  - Slaughter (59)
  - Blue Steel (42)
  - Night (43)
  - Ultraviolet (37)
  - Damascus Steel (74)

### Armas
- **30 slots de armas configurables**
- **Configuración por arma:**
  - Skin ID (Paint Kit)
  - Desgaste (Factory New, Minimal Wear, Field-Tested, Well-Worn, Battle-Scarred)
  - Float Value (0.0 - 1.0)
  - Seed (variación de patrón)
  - StatTrak (contador de kills)

## Cómo Usar

### Activar Skin Changer
1. Abre el menú con `INSERT`
2. Ve a la pestaña "Skins"
3. Activa "Forzar cuchillo" o "Forzar skins"

### Configurar Cuchillo
1. Marca "Forzar cuchillo"
2. Selecciona el modelo de cuchillo deseado
3. Selecciona el acabado
4. El cambio se aplica automáticamente al equipar el cuchillo

### Configurar Armas
1. Marca "Forzar skins"
2. Selecciona el arma de la lista
3. Configura:
   - **Skin ID**: ID del skin (busca en bases de datos de CS2)
   - **Desgaste**: Calidad visual del skin
   - **Float**: Valor preciso de desgaste (0.0 = nuevo, 1.0 = muy usado)
   - **Seed**: Número para variación de patrón
4. Los cambios se aplican al cambiar de arma

## IDs de Skins Populares

### AK-47
- Fire Serpent: 180
- Redline: 282
- Vulcan: 490
- Asiimov: 524
- Neon Revolution: 656
- Bloodsport: 724

### M4A4
- Howl: 309
- Asiimov: 255
- Dragon King: 361
- Desolate Space: 584
- Neo-Noir: 724

### AWP
- Dragon Lore: 344
- Asiimov: 279
- Hyper Beast: 457
- Lightning Strike: 242
- Medusa: 425

### Desert Eagle
- Blaze: 37
- Kumicho Dragon: 543
- Code Red: 351
- Sunset Storm: 645

## Valores de Float

| Rango | Calidad |
|-------|---------|
| 0.00 - 0.07 | Factory New |
| 0.07 - 0.15 | Minimal Wear |
| 0.15 - 0.38 | Field-Tested |
| 0.38 - 0.45 | Well-Worn |
| 0.45 - 1.00 | Battle-Scarred |

## Notas Importantes

1. **Solo Visual**: Los cambios solo son visibles para ti, otros jugadores no los ven
2. **No Permanente**: Los skins se resetean al reiniciar el juego
3. **Seguridad**: El sistema usa offsets actualizados para evitar detección
4. **Rendimiento**: No afecta el rendimiento del juego

## Solución de Problemas

### Los skins no se aplican
- Verifica que los offsets estén actualizados (F9 para escanear)
- Asegúrate de que el Skin ID sea válido
- Intenta cambiar de arma y volver a equiparla

### El cuchillo no cambia
- Verifica que "Forzar cuchillo" esté activado
- Equipa y desequipa el cuchillo
- Algunos modelos pueden no estar disponibles en ciertos modos

### Crash al aplicar skins
- Reduce el número de skins activos
- Verifica que los valores de float estén entre 0.0 y 1.0
- Actualiza los offsets con F9

## Recursos

- **Skin IDs**: https://csgostash.com/
- **Paint Kit Database**: https://github.com/SteamDatabase/GameTracking-CS2
- **Pattern Seeds**: https://cs2.gallery/

## Advertencias

⚠️ **Uso bajo tu propio riesgo**
- El uso de cheats puede resultar en ban de VAC
- Recomendado solo para uso offline o en servidores privados
- No nos hacemos responsables de consecuencias

## Actualizaciones

El Skin Changer se actualiza automáticamente con los offsets del juego.
Verifica el CHANGELOG.md para ver las últimas actualizaciones.

---

**Build**: 14138.4  
**Fecha**: 9 de Marzo, 2026  
**Estado**: ✅ Funcional
