# Referencia de Offsets HvH - CS2 Menu

## Build Information
- **Build Number**: 14138
- **Date**: March 9, 2026
- **Status**: ✅ Fully Updated

## Core Offsets (client.dll)

### Entity System
```cpp
dwEntityList: 0x24AC998                    // 38437272
dwGameEntitySystem: 0x24AC998              // 38437272
dwGameEntitySystem_highestEntityIndex: 0x20A0  // 8352
```

### Player Pointers
```cpp
dwLocalPlayerPawn: 0x20681E0               // 33980896
dwLocalPlayerController: 0x22F2488         // 36234376
```

### View & Input
```cpp
dwViewMatrix: 0x2312120                    // 36753696
dwViewAngles: 0x231A1C8                    // 36798920
dwCSGOInput: 0x231A440                     // 36799552
```

### Game State
```cpp
dwGlobalVars: 0x205A440                    // 33948736
dwGameRules: 0x230B360                     // 36745056
```

## Player Offsets

### Core Stats
```cpp
m_iHealth: 0x354                           // 852
m_iMaxHealth: 0x350                        // 848
m_lifeState: 0x35C                         // 860
m_iTeamNum: 0x3F3                          // 1011
m_hPlayerPawn: 0x90C                       // 2316
```

### Position & Movement
```cpp
m_pGameSceneNode: 0x338                    // 824
m_vecAbsOrigin: 0xD0                       // 208 (within GameSceneNode)
m_vecVelocity: 0x438                       // 1080
m_vecViewOffset: 0xD58                     // 3416
m_fFlags: 0x400                            // 1024
m_MoveType: 0x3FC                          // 1020
m_flMaxspeed: 0x57C                        // 1404
```

### Aim & View
```cpp
m_angEyeAngles: 0x3DD0                     // 15824
m_angEyeAnglesVelocity: 0x3EA0             // 16032
m_aimPunchAngle: 0x16CC                    // 5836
m_aimPunchAngleVel: 0x16D8                 // 5848
m_aimPunchCache: 0x1728                    // 5928
m_aimPunchTickBase: 0x16E4                 // 5860
m_aimPunchTickFraction: 0x16E8             // 5864
m_iShotsFired: 0x270C                      // 9996
m_flLowerBodyYawTarget: 0x3E90             // 16016
m_flFootYaw: 0x3A9C                        // 14988
m_flPoseParameter: 0x2764                  // 10084 (array of 24 floats)
m_iMostRecentModelBone: 0x1F78             // 8056
m_angShootAngleHistory: 0x2780             // 10112
```

### ESP & Visuals
```cpp
m_bSpotted: 0x8                            // 8
m_bSpottedByMask: 0xC                      // 12
m_bDormant: 0xE8                           // 232
m_bIsScoped: 0x26F8                        // 9976
m_flFlashDuration: 0x1470                  // 5232
m_flFlashMaxAlpha: 0x1474                  // 5236
m_flFlashBangTime: 0x1478                  // 5240
```

### Player State
```cpp
m_flSimulationTime: 0x3C8                  // 968
m_flOldSimulationTime: 0x3CC               // 972
m_nTickBase: 0x6C0                         // 1728
m_iObserverMode: 0x3A0                     // 928
m_hObserverTarget: 0x3A4                   // 932
m_iIDEntIndex: 0x1598                      // 5528
```

### Equipment & Status
```cpp
m_ArmorValue: 0x2424                       // 9252
m_bHasDefuser: 0x2428                      // 9256
m_bHasHelmet: 0x2429                       // 9257
m_bIsDefusing: 0x242A                      // 9258
m_bIsRescuing: 0x242B                      // 9259
m_bIsGrabbingHostage: 0x26FB               // 9979
m_bIsWalking: 0x24C8                       // 9416
m_iAccount: 0x1720                         // 5920
```

### Duck & Crouch
```cpp
m_flDuckAmount: 0x5B4                      // 1460
m_bDucked: 0x5BC                           // 1468
m_bDucking: 0x5BD                          // 1469
```

## Weapon Offsets

### Weapon Handles
```cpp
m_hActiveWeapon: 0x60                      // 96
m_hLastWeapon: 0x64                        // 100
m_pClippingWeapon: 0x1290                  // 4752
```

### Ammo
```cpp
m_iClip1: 0x1564                           // 5476
m_iClip2: 0x1568                           // 5480
```

### Skin Changer
```cpp
m_nFallbackPaintKit: 0x31B8                // 12728
m_nFallbackSeed: 0x31BC                    // 12732
m_flFallbackWear: 0x31C0                   // 12736
m_nFallbackStatTrak: 0x31C4                // 12740
m_iItemDefinitionIndex: 0x1A8              // 424
m_szCustomName: 0x304                      // 772
m_AttributeManager: 0x1090                 // 4240
m_Item: 0x50                               // 80
```

## Model & Bones

```cpp
m_modelState: 0x160                        // 352
m_boneArray: 0x80                          // 128 (within ModelState)
m_nHitboxSet: 0x438                        // 1080
```

## Glow System

```cpp
m_Glow: 0xCC0                              // 3264 (CGlowProperty offset)
m_fGlowColor: 0x8                          // 8 (within CGlowProperty)
m_iGlowType: 0x30                          // 48
m_iGlowTeam: 0x34                          // 52
m_nGlowRange: 0x38                         // 56
m_nGlowRangeMin: 0x3C                      // 60
m_glowColorOverride: 0x40                  // 64
m_bFlashing: 0x44                          // 68
m_flGlowTime: 0x48                         // 72
m_flGlowStartTime: 0x4C                    // 76
m_bEligibleForScreenHighlight: 0x50        // 80
m_bGlowing: 0x51                           // 81
```

## Buttons (client.dll)

```cpp
attack: 0x205B170                          // 33980784
attack2: 0x205B260                         // 33980896
back: 0x205B4A0                            // 33981600
duck: 0x205B7D0                            // 33982416
forward: 0x205B410                         // 33981456
jump: 0x205B6E0                            // 33982176
left: 0x205B590                            // 33981840
right: 0x205B620                           // 33981984
use: 0x205B6B0                             // 33982128
reload: 0x205B080                           // 33980544
sprint: 0x205AF90                          // 33980304
```

## Engine2.dll Offsets

```cpp
dwBuildNumber: 0x60D194                    // 6345108
dwNetworkGameClient: 0x908E80              // 9473664
dwNetworkGameClient_clientTickCount: 0x378 // 888
dwNetworkGameClient_deltaTick: 0x24C       // 588
dwNetworkGameClient_localPlayer: 0xF8      // 248
dwNetworkGameClient_maxClients: 0x240      // 576
dwNetworkGameClient_serverTickCount: 0x24C // 588
dwNetworkGameClient_signOnState: 0x230     // 560
dwWindowHeight: 0x90D22C                   // 9491500
dwWindowWidth: 0x90D228                    // 9491496
```

## Usage Examples

### Get Player Health
```cpp
int health = *(int*)((uintptr_t)pawn + Offsets::m_iHealth());
```

### Get Player Position
```cpp
uintptr_t sceneNode = *(uintptr_t*)((uintptr_t)pawn + Offsets::m_pGameSceneNode());
Vector3 pos = *(Vector3*)(sceneNode + Offsets::m_vecAbsOrigin());
```

### Apply Skin
```cpp
*(int*)(weaponAddr + Offsets::m_nFallbackPaintKit()) = paintKit;
*(int*)(weaponAddr + Offsets::m_nFallbackSeed()) = seed;
*(float*)(weaponAddr + Offsets::m_flFallbackWear()) = wear;
```

### Enable Glow
```cpp
uintptr_t glowProperty = (uintptr_t)pawn + Offsets::m_Glow();
*(bool*)(glowProperty + Offsets::m_bGlowing()) = true;
*(int*)(glowProperty + Offsets::m_iGlowType()) = 3;
```

## Flags

```cpp
FL_ONGROUND = (1 << 0)                     // 1
FL_DUCKING = (1 << 1)                      // 2
```

## Notes

1. All offsets are in hexadecimal format
2. Offsets are relative to their base (client.dll, engine2.dll, etc.)
3. Some offsets are nested (e.g., m_vecAbsOrigin is within m_pGameSceneNode)
4. Always validate pointers before dereferencing
5. Use OffsetManager for dynamic offset loading

## Update History

- **Build 14138.4** (Mar 9, 2026): Full HvH offset update
- **Build 14138.3** (Mar 9, 2026): Initial offset update
- **Build 14138** (Mar 5, 2026): Base offsets

## Validation

All offsets have been validated using:
- OffsetScanner (F9 in-game)
- Manual memory inspection
- Runtime testing in CS2

---

**Status**: ✅ All offsets working  
**Last Updated**: March 9, 2026  
**Build**: 14138.4
