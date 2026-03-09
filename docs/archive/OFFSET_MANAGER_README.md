# OffsetManager - Dynamic Offset Loading System

## Overview

The `OffsetManager` class provides a robust system for loading and managing CS2 game offsets dynamically. It supports multiple input formats, validation, and automatic fallback mechanisms.

## Features

- **Multiple Input Formats**: Supports JSON (cs2-dumper format) and simple config files
- **Automatic Fallback**: Uses hardcoded offsets if no config files are found
- **Validation**: Validates loaded offsets with sanity checks
- **Version Tracking**: Tracks game version/build number
- **Singleton Pattern**: Single global instance for easy access
- **Safe Access**: Returns fallback values for missing offsets

## Usage

### Basic Usage

```cpp
#include "OffsetManager.h"

// Initialize the offset manager (call once at startup)
OffsetManager::Instance().Initialize();

// Get an offset
uintptr_t entityListOffset = OffsetManager::Instance().GetOffset("dwEntityList");

// Get an offset with fallback
uintptr_t healthOffset = OffsetManager::Instance().GetOffset("m_iHealth", 0x76C);

// Check if offset exists
if (OffsetManager::Instance().HasOffset("dwViewMatrix"))
{
    // Use the offset
}
```

### Loading from Files

The `Initialize()` method automatically tries to load offsets in this order:

1. `offsets.json` (cs2-dumper JSON format)
2. `offsets.cfg` (simple key=value format)
3. Hardcoded fallback offsets

You can also manually load from specific files:

```cpp
// Load from JSON
OffsetManager::Instance().LoadFromJSON("custom_offsets.json");

// Load from config
OffsetManager::Instance().LoadFromConfig("custom_offsets.cfg");
```

### Validation

```cpp
// Validate loaded offsets
if (OffsetManager::Instance().ValidateOffsets())
{
    std::cout << "All offsets validated successfully!" << std::endl;
}
else
{
    std::cout << "Offset validation failed!" << std::endl;
}
```

### Manual Offset Management

```cpp
// Set a custom offset
OffsetManager::Instance().SetOffset("customOffset", 0x1234);

// Clear all offsets
OffsetManager::Instance().Clear();

// Load fallback offsets
OffsetManager::Instance().LoadFallbackOffsets();
```

## Configuration File Formats

### JSON Format (cs2-dumper)

Create a file named `offsets.json`:

```json
{
  "client.dll": {
    "dwEntityList": "0x24AE628",
    "dwLocalPlayerPawn": "0x2066DE0",
    "dwViewMatrix": "0x2309420",
    "m_iHealth": "0x76C",
    "m_iTeamNum": "0xD70"
  }
}
```

### Config Format (Simple)

Create a file named `offsets.cfg`:

```
# CS2 Offsets Configuration
# Format: key=value

dwEntityList=0x24AE628
dwLocalPlayerPawn=0x2066DE0
dwViewMatrix=0x2309420
m_iHealth=0x76C
m_iTeamNum=0xD70
```

## Updating Offsets

### From cs2-dumper

1. Visit: https://github.com/a2x/cs2-dumper
2. Download the latest `offsets.json` from releases
3. Place it in the same directory as the executable
4. Restart the application

### Manual Update

1. Copy `offsets.cfg.example` to `offsets.cfg`
2. Update the values with new offsets
3. Restart the application

## Integration Example

Replace hardcoded offsets in your code:

**Before:**
```cpp
constexpr uintptr_t dwEntityList = 0x24AE628;
uintptr_t entityList = clientBase + dwEntityList;
```

**After:**
```cpp
uintptr_t dwEntityList = OffsetManager::Instance().GetOffset("dwEntityList");
uintptr_t entityList = clientBase + dwEntityList;
```

## Validation Rules

The OffsetManager validates offsets with these rules:

- **Critical Offsets**: Must have `dwEntityList`, `dwLocalPlayerPawn`, and `dwViewMatrix`
- **Module Offsets** (starting with `dw`): Should be < 100MB
- **Member Offsets** (starting with `m_`): Typically < 64KB (warning only)

## Error Handling

The OffsetManager provides detailed logging:

```
[OffsetManager] Initializing...
[OffsetManager] Loaded offsets from offsets.json
[OffsetManager] Parsed 32 offsets from JSON
[OffsetManager] Validation passed: 32 offsets loaded
```

If a file is not found:
```
[OffsetManager] Failed to open: offsets.json
[OffsetManager] No offset files found, using fallback offsets
[OffsetManager] Loading fallback offsets (Build 14138)
```

## Testing

Run the unit tests to verify functionality:

```bash
# Build the test
cmake --build cmake-build-release-visual-studio --config Release --target test_offsetmanager

# Run the test
./cmake-build-release-visual-studio/Release/Release/test_offsetmanager.exe
```

## API Reference

### Methods

- `Initialize()` - Initialize and load offsets from files or fallback
- `LoadFromJSON(path)` - Load offsets from JSON file
- `LoadFromConfig(path)` - Load offsets from config file
- `GetOffset(name, fallback)` - Get offset by name with optional fallback
- `HasOffset(name)` - Check if offset exists
- `ValidateOffsets()` - Validate loaded offsets
- `SetOffset(name, value)` - Manually set an offset
- `Clear()` - Clear all offsets
- `LoadFallbackOffsets()` - Load hardcoded fallback offsets
- `GetGameVersion()` - Get game version string
- `SetGameVersion(version)` - Set game version string
- `GetOffsetCount()` - Get number of loaded offsets

## Notes

- The OffsetManager uses a singleton pattern - only one instance exists
- Offsets are stored as `uintptr_t` values
- Hex values in config files must use `0x` prefix
- JSON parsing is simplified and expects cs2-dumper format
- Fallback offsets are from Build 14138 (2026-03-05)

## Future Enhancements

Possible improvements for future versions:

- Automatic offset updates from online sources
- Pattern scanning integration
- Offset history tracking
- Multiple game version support
- Encrypted offset storage
- Automatic validation against game memory
