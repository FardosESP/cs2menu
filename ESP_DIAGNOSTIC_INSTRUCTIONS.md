# ESP Diagnostic Instructions

## Current Status
ESP no detecta jugadores. Estadísticas:
- Failed controller: 43 (la mayoría fallan aquí)
- Failed pawnHandle: 13 (algunos llegan pero handle = 0)
- Failed health: 2 (solo 2 llegan hasta health check)

## Root Cause
El offset `m_hPlayerPawn` (0x90C / 2316 decimal) puede estar desactualizado para Build 14138.6.

## Solution: Use F10 Diagnostic

1. **Inyecta el DLL** en CS2
2. **Únete a un match con bots**
3. **Presiona F10** en el juego
4. **Copia TODO el output** del diagnostic

El diagnostic escaneará memoria y encontrará los offsets correctos automáticamente.

## What F10 Does
```cpp
DiagnosticSystem::RunFullDiagnostic(g_clientBase, g_pEntitySystem);
DiagnosticSystem::TestESP(g_clientBase, g_pEntitySystem);
```

Esto:
- Escanea EntityList
- Encuentra controllers válidos
- Detecta el offset correcto de m_hPlayerPawn
- Prueba lectura de health/team
- Muestra offsets working

## Expected Output
```
[DIAGNOSTIC] Scanning EntityList...
[DIAGNOSTIC] Found controller at index X: 0xABCDEF
[DIAGNOSTIC] m_hPlayerPawn offset test: 0x90C
[DIAGNOSTIC] Pawn handle: 0x1234
[DIAGNOSTIC] Decoded pawn: 0xABCDEF
[DIAGNOSTIC] Health: 100, Team: 2
```

## Next Steps
Una vez tengas el output de F10, actualizaremos los offsets con los valores correctos.
