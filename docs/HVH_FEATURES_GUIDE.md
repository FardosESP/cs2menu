# Guía Completa de Características HvH

## Build 14138.5 - Premium HvH Edition

---

## 🛡️ Anti-Aim

### ¿Qué es Anti-Aim?
Anti-Aim manipula tus ángulos de visión para hacer que tu modelo de jugador sea más difícil de golpear. Es esencial en HvH (Hack vs Hack).

### Tipos de Pitch

#### Up (-89°)
- Cabeza apunta hacia arriba
- Modelo se inclina hacia atrás
- Bueno contra resolvers básicos

#### Down (89°)
- Cabeza apunta hacia abajo (recomendado)
- Modelo se inclina hacia adelante
- Más difícil de resolver

#### Zero (0°)
- Cabeza recta
- Modelo normal
- Menos efectivo pero más legit

#### Fake
- Envía un ángulo, muestra otro
- Confunde resolvers
- Requiere desync

### Tipos de Yaw

#### Backward (180°)
- Mira hacia atrás
- Clásico y efectivo
- Fácil de usar

#### Spin
- Gira continuamente
- Velocidad configurable
- Difícil de predecir

#### Jitter
- Alterna entre ángulos
- Rango configurable (±60° recomendado)
- Muy efectivo contra resolvers

#### Sideways (±90°)
- Alterna izquierda/derecha
- Bueno para esquinas
- Predecible pero efectivo

#### Random
- Ángulos aleatorios
- Impredecible
- Puede ser inconsistente

#### FakeBackward
- Backward con desync
- Muestra un ángulo diferente
- Requiere fake lag

#### FakeSpin
- Spin con desync
- Muy rápido
- Difícil de resolver

### Configuración Recomendada

**Para Principiantes:**
```
Pitch: Down
Yaw: Backward
Yaw Offset: 0°
Fake Lag: 8 ticks
```

**Para Avanzados:**
```
Pitch: Fake
Yaw: Jitter
Yaw Offset: ±15°
Jitter Range: 60°
Fake Lag: 12 ticks
```

**Para HvH Competitivo:**
```
Pitch: FakeDown
Yaw: FakeSpin
Spin Speed: 15°/tick
Fake Lag: 14 ticks
Edge AA: Enabled
Freestanding: Enabled
```

### Fake Lag

Fake Lag "chokea" paquetes para crear desync entre tu posición real y la que ven los enemigos.

**Configuración:**
- **Ticks**: 1-14 (14 = máximo, 200ms)
- **Recomendado**: 8-12 ticks
- **Nota**: Más ticks = más desync pero más lag

### Manual AA

Hotkeys para controlar manualmente el Anti-Aim:

- **Back**: Mira hacia atrás (180°)
- **Left**: Mira a la izquierda (90°)
- **Right**: Mira a la derecha (-90°)

Útil para:
- Esquinas específicas
- Situaciones 1v1
- Cuando sabes de dónde viene el enemigo

---

## 🎯 Resolver

### ¿Qué es un Resolver?
Un resolver predice los ángulos reales del enemigo cuando usa Anti-Aim, mejorando tu precisión.

### Tipos de Resolver

#### LBY (Lower Body Yaw)
- Usa el ángulo del cuerpo inferior
- Efectivo contra AA básicos
- Preciso cuando el enemigo está quieto

**Cuándo usar:**
- Enemigos parados
- AA simples
- Jugadores sin fake lag

#### Moving
- Basado en la dirección de velocidad
- Predice hacia dónde se mueve
- Muy preciso en movimiento

**Cuándo usar:**
- Enemigos corriendo
- Strafing
- Peeking

#### Standing
- Analiza delta entre LBY y FootYaw
- Resuelve basado en la diferencia
- Efectivo contra jitter

**Cuándo usar:**
- Enemigos quietos
- Jitter AA
- Spin lento

#### Bruteforce
- Prueba 8 ángulos comunes
- Cambia en cada miss
- Eventualmente acierta

**Ángulos probados:**
```
0°, 90°, -90°, 180°, 45°, -45°, 135°, -135°
```

**Cuándo usar:**
- Cuando otros resolvers fallan
- AA complejos
- Como último recurso

#### Delta
- Calcula diferencia LBY vs FootYaw
- Resuelve basado en el signo
- Preciso contra fake angles

**Cuándo usar:**
- Fake AA
- Desync
- AA avanzados

#### LastMove
- Usa última dirección de movimiento
- Simple pero efectivo
- Bueno para peekers

**Cuándo usar:**
- Enemigos peeking
- Movimiento predecible
- Situaciones rápidas

### Configuración Recomendada

**Automático (Recomendado):**
```
Type: LBY
Bruteforce on Miss: Enabled
Bruteforce Steps: 8
```

**Agresivo:**
```
Type: Bruteforce
Steps: 8
```

**Preciso:**
```
Type: Delta
Bruteforce on Miss: Enabled
```

### Cómo Funciona

1. **Update**: El resolver recopila datos del enemigo cada tick
2. **Analyze**: Analiza LBY, FootYaw, velocidad, etc.
3. **Predict**: Predice el ángulo real basado en el tipo
4. **Apply**: Ajusta tu aim al ángulo predicho
5. **Learn**: Si fallas, bruteforce al siguiente ángulo

---

## ⏮️ Backtrack

### ¿Qué es Backtrack?
Backtrack almacena posiciones anteriores del enemigo, permitiéndote disparar a donde estaban hace unos ticks. Útil contra lag y movimiento rápido.

### Cómo Funciona

1. **Record**: Guarda posición del enemigo cada tick
2. **Store**: Mantiene hasta 12 ticks (200ms)
3. **Validate**: Verifica que los records sean válidos
4. **Select**: Elige el mejor record basado en hitchance
5. **Shoot**: Dispara a la posición del record

### Configuración

**Max Ticks:**
- **Rango**: 1-12 ticks
- **Recomendado**: 12 (máximo legal, 200ms)
- **Nota**: Más ticks = más compensación de lag

**Only On Shot:**
- **Enabled**: Solo backtrack cuando disparas
- **Disabled**: Backtrack siempre activo
- **Recomendado**: Disabled para mejor precisión

### Cuándo Usar

✅ **Usar Backtrack cuando:**
- Enemigos con lag
- Movimiento rápido (strafing)
- Peekers advantage
- HvH competitivo

❌ **No usar cuando:**
- Enemigos estáticos
- Distancias muy largas
- Quieres ser legit

### Hitchance Calculation

El backtrack calcula hitchance basado en:
- **Distancia**: Más cerca = mejor
- **Edad del record**: Más nuevo = mejor
- **Validez**: Record debe ser válido

### Ejemplo de Uso

```cpp
// En tu aimbot
Backtrack::Record* record = Backtrack::Instance().GetBestRecord(enemy, localEyePos);
if (record)
{
    // Aim a la posición del record
    Vector3 targetPos = record->origin;
    // Calcular ángulos y disparar
}
```

---

## 🎮 Integración con Aimbot

### Resolver + Aimbot

```cpp
// 1. Obtener ángulos resueltos
Vector3 resolvedAngles = Resolver::Instance().Resolve(enemy);

// 2. Aplicar al aimbot
Vector3 targetPos = enemy->GetBonePosition(BONE_HEAD);
targetPos.y = resolvedAngles.y; // Usar yaw resuelto

// 3. Calcular aim
Vector3 aimAngles = CalculateAngle(localEyePos, targetPos);
```

### Backtrack + Aimbot

```cpp
// 1. Obtener mejor record
Backtrack::Record* record = Backtrack::Instance().GetBestRecord(enemy, localEyePos);

// 2. Usar posición del record
Vector3 targetPos = record ? record->origin : enemy->GetOrigin();

// 3. Calcular aim
Vector3 aimAngles = CalculateAngle(localEyePos, targetPos);
```

### Resolver + Backtrack + Aimbot

```cpp
// 1. Obtener record
Backtrack::Record* record = Backtrack::Instance().GetBestRecord(enemy, localEyePos);

// 2. Resolver ángulos
Vector3 resolvedAngles = Resolver::Instance().Resolve(enemy);

// 3. Combinar
Vector3 targetPos = record ? record->origin : enemy->GetOrigin();
targetPos.y = resolvedAngles.y;

// 4. Aim
Vector3 aimAngles = CalculateAngle(localEyePos, targetPos);
```

---

## 🔧 Configuraciones Presets

### Legit HvH
```
Anti-Aim:
  Pitch: Down
  Yaw: Backward
  Fake Lag: 6 ticks

Resolver:
  Type: LBY
  Bruteforce: Enabled

Backtrack:
  Max Ticks: 8
  Only On Shot: Disabled
```

### Rage HvH
```
Anti-Aim:
  Pitch: FakeDown
  Yaw: Jitter
  Jitter Range: 60°
  Fake Lag: 14 ticks

Resolver:
  Type: Delta
  Bruteforce: Enabled

Backtrack:
  Max Ticks: 12
  Only On Shot: Disabled
```

### Competitive HvH
```
Anti-Aim:
  Pitch: Fake
  Yaw: FakeSpin
  Spin Speed: 15°
  Fake Lag: 12 ticks
  Edge AA: Enabled
  Freestanding: Enabled

Resolver:
  Type: Bruteforce
  Steps: 8

Backtrack:
  Max Ticks: 12
  Only On Shot: Disabled
```

---

## 📊 Offsets Utilizados

### Anti-Aim
- m_angEyeAngles (0x3DD0)
- m_flLowerBodyYawTarget (0x3E90)
- m_flFootYaw (0x3A9C)
- m_vecVelocity (0x438)
- m_fFlags (0x400)

### Resolver
- m_flLowerBodyYawTarget (0x3E90)
- m_flFootYaw (0x3A9C)
- m_vecVelocity (0x438)
- m_flPoseParameter (0x2764)
- m_angShootAngleHistory (0x2780)

### Backtrack
- m_flSimulationTime (0x3C8)
- m_flOldSimulationTime (0x3CC)
- m_vecAbsOrigin (0xD0)
- m_nTickBase (0x6C0)
- m_flAnimTime (0x3C4)

---

## ⚠️ Advertencias

### Detección
- Anti-Aim es muy detectable
- Usar solo en HvH o servidores privados
- Riesgo alto de VAC ban en matchmaking

### Rendimiento
- Backtrack usa memoria (12 ticks × jugadores)
- Resolver requiere CPU para cálculos
- Puede afectar FPS en PCs lentos

### Legalidad
- Solo para uso educativo
- No usar en servidores oficiales
- Riesgo de ban permanente

---

## 🎓 Tips Avanzados

### Anti-Aim
1. Cambiar yaw offset cada ronda
2. Usar manual AA en situaciones 1v1
3. Ajustar fake lag según ping
4. Edge AA en esquinas cerradas

### Resolver
1. Observar patrones del enemigo
2. Cambiar tipo si fallas mucho
3. Bruteforce como último recurso
4. Combinar con backtrack

### Backtrack
1. Más ticks en enemigos laggy
2. Menos ticks en distancias largas
3. Combinar con resolver
4. Verificar validez de records

---

**Build**: 14138.5  
**Fecha**: 9 de Marzo, 2026  
**Estado**: ✅ Premium HvH Ready
