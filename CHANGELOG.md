# Changelog

## [Unreleased] - 2026-03-05

### Added
- **OffsetManager System**: Dynamic offset loading from JSON/config files with automatic fallback
- **Aimbot System**: Complete aimbot implementation with:
  - FOV-based target selection
  - Smooth aim with configurable smoothing
  - Angle normalization
  - Silent aim option
  - Team and visibility checks
- **Advanced ESP Features**:
  - Player name display
  - Weapon display
  - Enhanced health and distance indicators
- **Memory Safety**: Comprehensive exception handling and pointer validation
- **Unit Tests**: Test suites for OffsetManager and memory safety
- **Documentation**:
  - Complete README with quick start guide
  - TROUBLESHOOTING guide for common issues
  - OFFSET_MANAGER_README for offset management
  - COMO_ACTUALIZAR_OFFSETS guide (Spanish)
  - FEATURES_TODO comprehensive feature list
- **Configuration Files**: Example offset files (JSON and CFG formats)
- **Logging**: Enhanced debug logging for easier troubleshooting

### Changed
- ESP enabled by default for easier testing
- Increased default max distance to 5000 units
- Disabled team check and dormant check by default
- Custom crosshair enabled by default
- No flash enabled by default
- Improved aimbot FOV default (180 degrees)
- Reduced aimbot smooth default (2.0)

### Fixed
- ESP crash issues with comprehensive null pointer checks
- Memory safety in entity iteration
- SDL3 mouse input handling
- LocalPlayer detection and validation
- ViewMatrix and EntitySystem pointer validation

### Technical Details
- Build: 14138 (2026-03-05)
- Offsets: Dynamic loading with fallback to Build 14138
- Architecture: x64 only
- Compiler: MSVC 2022
- Dependencies: ImGui, MinHook, SDL3

### Known Issues
- EntityList iteration needs verification (some offsets may be incorrect)
- Skeleton ESP not yet implemented
- Triggerbot not yet implemented
- Bunny hop needs user command hooking
- Skin changer not yet implemented

### Performance
- Minimal FPS impact (<5%)
- Safe memory operations with exception handling
- Efficient rendering with ImGui

### Security
- No external process memory reading
- Internal DLL injection only
- Exception handling prevents crashes
- Comprehensive pointer validation

## [0.1.0] - Initial Release

### Added
- Basic ESP (boxes, health bars, distance)
- ImGui menu system
- DirectX 11 hooking
- SDL3 input handling
- Basic crosshair
- Radar hack
- FOV changer
- No flash

