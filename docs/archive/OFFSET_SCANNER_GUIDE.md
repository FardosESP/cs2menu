# CS2 Offset Scanner - Diagnostic Tool Guide

## Overview

The Offset Scanner is a comprehensive memory diagnostic tool designed to find correct offsets for CS2 when ESP is not working due to incorrect offsets from cs2-dumper.

## Problem It Solves

**Current Issue:**
- LocalPlayer is found correctly (Health: 62, Team: 2)
- But `GetOriginSafe()` fails (m_pGameSceneNode is wrong)
- No entities are being found (0 valid entities)
- ESP boxes don't appear on screen

**Root Cause:**
The offsets from cs2-dumper may be outdated or incorrect for your CS2 version, specifically:
- `m_pGameSceneNode` - Critical for getting player positions
- `m_vecAbsOrigin` - World coordinates within GameSceneNode
- `m_vecViewOffset` - Eye position offset
- `m_fFlags` - Player state flags

## How to Use

### Step 1: Get In-Game

**CRITICAL:** You MUST be in an active CS2 match (not menu/lobby) for the scanner to work:

1. Launch CS2
2. Join a match (Deathmatch, Casual, Competitive, etc.)
3. Spawn as a player (not spectating)
4. Inject the cheat DLL

### Step 2: Run the Scanner

The scanner runs automatically when you inject the DLL. It will:

1. Verify LocalPlayer is valid
2. Scan for `m_pGameSceneNode` candidates
3. Scan for `m_vecAbsOrigin` within GameSceneNode
4. Scan for `m_vecViewOffset` candidates
5. Scan for `m_fFlags` candidates
6. Scan EntityList for valid player entities

### Step 3: Review Console Output

The scanner prints detailed output to the console. Look for sections like:

```
╔═══════════════════════════════════════════════════════════╗
║         SCANNING FOR m_pGameSceneNode OFFSET             ║
╚═══════════════════════════════════════════════════════════╝

[CANDIDATE] m_pGameSceneNode at offset 0x338
  Pointer value: 0x1A2B3C4D5E6F
  Points to structure containing world coords at +0xD0
  Coordinates: (1234.5, -567.8, 89.0)
  Reason: Valid pointer pointing to structure with world coordinates
```

### Step 4: Identify Correct Offsets

For each candidate, the scanner shows:

- **Offset address** (hex) - e.g., `0x338`
- **Value found** - e.g., pointer address or coordinates
- **Why it's a candidate** - e.g., "looks like pointer", "looks like world coords"

**What to look for:**

#### m_pGameSceneNode
- Should be a valid pointer (8-byte aligned, in user space)
- Should point to a structure containing world coordinates
- Coordinates should be in range -10000 to +10000
- Coordinates should NOT be all zeros

#### m_vecAbsOrigin
- Should be 3 consecutive floats (x, y, z)
- Values should look like world coordinates
- Should change when you move in-game

#### m_vecViewOffset
- Should be 3 consecutive floats (x, y, z)
- X and Y should be close to 0
- Z should be around 64.0f (standing) or 46.0f (crouching)

#### m_fFlags
- Should be an integer value
- Should have FL_ONGROUND bit (0x1) set when on ground
- Value should be in range 0-65536

### Step 5: Update Offsets

Once you've identified the correct offsets:

1. Open `cs2menu/src/SDK.h`
2. Find the `Offsets::Fallback` namespace
3. Update the offset values:

```cpp
namespace Fallback
{
    // ... other offsets ...
    
    constexpr uintptr_t m_pGameSceneNode = 0x338;  // Update this
    constexpr uintptr_t m_vecAbsOrigin   = 0xD0;   // Update this
    constexpr uintptr_t m_vecViewOffset  = 0xD58;  // Update this
    constexpr uintptr_t m_fFlags         = 0x400;  // Update this
    
    // ... other offsets ...
}
```

4. Rebuild the project:
```bash
cmake --build build --config Release
```

5. Re-inject and test ESP

### Step 6: Verify ESP Works

After updating offsets:

1. Inject the updated DLL
2. Enable ESP in the menu (INSERT key)
3. Check if boxes appear around players
4. If still not working, try the next candidate offset

## Understanding Scanner Output

### Valid Pointer Detection

```
[CANDIDATE] m_pGameSceneNode at offset 0x338
  Pointer value: 0x1A2B3C4D5E6F
```

**What this means:**
- At offset 0x338 from LocalPlayer, there's a pointer
- The pointer value is 0x1A2B3C4D5E6F
- This pointer is 8-byte aligned and in valid memory range

### World Coordinates Detection

```
  Points to structure containing world coords at +0xD0
  Coordinates: (1234.5, -567.8, 89.0)
```

**What this means:**
- The pointer points to a structure
- At offset +0xD0 within that structure, there are world coordinates
- The coordinates (1234.5, -567.8, 89.0) look valid

### Entity List Scanning

```
[ENTITY 5] Valid player found!
  Address: 0x1A2B3C4D5E6F
  Health: 87
  Team: 2
  Origin: (1234.5, -567.8, 89.0) ✓
```

**What this means:**
- Entity at index 5 is a valid player
- Health and team values are reasonable
- Origin was successfully retrieved (✓ means valid)

## Troubleshooting

### "LocalPlayerPawn is NULL"

**Problem:** You're not in an active game.

**Solution:**
1. Join a match
2. Spawn as a player
3. Re-inject the DLL

### "No candidates found for m_pGameSceneNode"

**Problem:** Either:
- LocalPlayer address is incorrect
- Game structure has changed significantly
- You're not spawned in the world

**Solution:**
1. Verify you're alive and spawned
2. Check console for LocalPlayer health/team values
3. If health/team are wrong, update those offsets first

### "No valid entities found"

**Problem:** Either:
- EntityList address is incorrect
- Entity iteration logic is wrong
- Offsets for m_iHealth/m_iTeamNum are incorrect

**Solution:**
1. Verify LocalPlayer health/team are correct
2. Check if you're in a match with other players
3. Try updating dwEntityList offset

### "Origin: Cannot read" or "✗ INVALID"

**Problem:** m_pGameSceneNode or m_vecAbsOrigin offset is wrong.

**Solution:**
1. Look at the candidates the scanner found
2. Try each candidate offset one by one
3. Rebuild and test after each change

## Advanced Usage

### Manual Memory Inspection

If the scanner doesn't find candidates, you can manually inspect memory:

1. Look for the hex dump in console output:
```
[HEXDUMP] First 256 bytes of LocalPlayer at 0x1A2B3C4D5E6F
Offset   00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F  ASCII
------   -----------------------------------------------  -----
0x0000   3E 00 00 00 02 00 00 00 ...
```

2. Look for patterns:
   - Pointers: 8 bytes, aligned, large values
   - Floats: 4 bytes, look for coordinate-like values
   - Integers: 4 bytes, look for health (1-100) or team (2-3)

### Testing Multiple Candidates

If the scanner finds multiple candidates:

1. Start with the first candidate
2. Update offset in SDK.h
3. Rebuild and test
4. If doesn't work, try next candidate
5. Repeat until ESP works

### Comparing with cs2-dumper

Check the latest offsets from cs2-dumper:
- https://github.com/a2x/cs2-dumper

Compare with scanner results to see which offsets changed.

## Technical Details

### Scanning Algorithm

The scanner uses several heuristics:

1. **Pointer Detection:**
   - Non-zero
   - In user space (< 0x7FFFFFFFFFFF)
   - 8-byte aligned
   - Points to readable memory

2. **World Coordinates Detection:**
   - 3 consecutive floats
   - In range -10000 to +10000
   - Not all zeros
   - Not NaN or Inf

3. **View Offset Detection:**
   - 3 consecutive floats
   - X and Y close to 0 (< 10.0f)
   - Z in range 30-80 (player eye height)

4. **Flags Detection:**
   - Integer value
   - In range 0-65536
   - May have FL_ONGROUND bit set

### Memory Safety

The scanner uses `__try/__except` blocks to safely read memory:
- Won't crash if reading invalid memory
- Continues scanning even if some reads fail
- Validates all pointers before dereferencing

## Example Workflow

Here's a complete example of using the scanner:

1. **Launch CS2 and join a match**
   - Deathmatch on Dust2
   - Spawn as CT

2. **Inject DLL**
   - Use cs2menu_launcher.exe
   - Check console output

3. **Review scanner output**
   ```
   [CANDIDATE] m_pGameSceneNode at offset 0x340
     Pointer value: 0x1F2E3D4C5B6A
     Points to structure containing world coords at +0xC8
     Coordinates: (1523.4, -234.1, 128.0)
   ```

4. **Update SDK.h**
   ```cpp
   constexpr uintptr_t m_pGameSceneNode = 0x340;  // Changed from 0x338
   constexpr uintptr_t m_vecAbsOrigin   = 0xC8;   // Changed from 0xD0
   ```

5. **Rebuild**
   ```bash
   cmake --build build --config Release
   ```

6. **Test ESP**
   - Re-inject DLL
   - Enable ESP (INSERT key)
   - See boxes around players ✓

## Notes

- The scanner runs automatically on DLL injection
- Output is printed to console (visible in debugger or console window)
- Scanner is safe and won't crash the game
- Results are most accurate when you're alive and spawned
- Some offsets may have multiple valid candidates - test each one

## Support

If ESP still doesn't work after using the scanner:

1. Check that ImGui is drawing (you should see the menu with INSERT)
2. Verify WorldToScreen is working (check ViewMatrix)
3. Ensure you're not filtering out all entities (check team/dormant filters)
4. Try disabling distance filter (set maxDistance very high)

## Credits

Scanner created to diagnose and fix CS2 ESP offset issues.
Based on cs2-dumper offsets: https://github.com/a2x/cs2-dumper
