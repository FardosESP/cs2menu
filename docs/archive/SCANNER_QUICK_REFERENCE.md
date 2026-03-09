# Offset Scanner - Quick Reference

## Quick Start

1. **Get in-game** (active match, spawned as player)
2. **Inject DLL** (scanner runs automatically)
3. **Check console** for candidates
4. **Update offsets** in `src/SDK.h` (Offsets::Fallback namespace)
5. **Rebuild** and test

## What to Look For

### m_pGameSceneNode (Most Critical!)
```
[CANDIDATE] m_pGameSceneNode at offset 0x338
  Pointer value: 0x1A2B3C4D5E6F
  Points to structure containing world coords at +0xD0
  Coordinates: (1234.5, -567.8, 89.0)
```

**Look for:**
- Valid pointer (8-byte aligned)
- Points to structure with world coordinates
- Coordinates in range -10000 to +10000

### m_vecAbsOrigin
```
[CANDIDATE] m_vecAbsOrigin at offset 0xD0
  Coordinates: (1234.5, -567.8, 89.0)
```

**Look for:**
- 3 consecutive floats (x, y, z)
- Values look like world coordinates
- Changes when you move

### m_vecViewOffset
```
[CANDIDATE] m_vecViewOffset at offset 0xD58
  View offset: (0.0, 0.0, 64.0)
```

**Look for:**
- X and Y close to 0
- Z around 64.0f (standing) or 46.0f (crouching)

### m_fFlags
```
[CANDIDATE] m_fFlags at offset 0x400
  Flags value: 0x101 (257)
  FL_ONGROUND: YES
```

**Look for:**
- Integer value 0-65536
- FL_ONGROUND bit (0x1) set when on ground

## Update Offsets

Edit `cs2menu/src/SDK.h`:

```cpp
namespace Offsets
{
    namespace Fallback
    {
        // Update these values with scanner results
        constexpr uintptr_t m_pGameSceneNode = 0x338;  // ← Change this
        constexpr uintptr_t m_vecAbsOrigin   = 0xD0;   // ← Change this
        constexpr uintptr_t m_vecViewOffset  = 0xD58;  // ← Change this
        constexpr uintptr_t m_fFlags         = 0x400;  // ← Change this
    }
}
```

## Rebuild

```bash
cd cs2menu
cmake --build build --config Release
```

## Common Issues

| Issue | Solution |
|-------|----------|
| "LocalPlayerPawn is NULL" | Join a match and spawn |
| "No candidates found" | Verify you're alive and spawned |
| "No valid entities found" | Check if other players are in match |
| "Origin: Cannot read" | m_pGameSceneNode offset is wrong |
| ESP still doesn't work | Try next candidate offset |

## Testing Checklist

- [ ] In active match (not menu/lobby)
- [ ] Spawned as player (not spectating)
- [ ] Console shows scanner output
- [ ] Found candidates for m_pGameSceneNode
- [ ] Updated offsets in SDK.h
- [ ] Rebuilt project
- [ ] Re-injected DLL
- [ ] Enabled ESP (INSERT key)
- [ ] Boxes appear around players

## Priority Order

Fix offsets in this order:

1. **m_pGameSceneNode** - Most critical for ESP
2. **m_vecAbsOrigin** - Needed for positions
3. **m_vecViewOffset** - Needed for head position
4. **m_fFlags** - Needed for bunny hop and other features

## Scanner Output Location

- Console window (if running from debugger)
- Output log (if redirected)
- Prints automatically on DLL injection

## Need More Help?

See `OFFSET_SCANNER_GUIDE.md` for detailed instructions.
