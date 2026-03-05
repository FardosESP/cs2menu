# CS2 Menu - Internal Cheat for Counter-Strike 2

A feature-rich internal cheat for Counter-Strike 2 with ESP, Aimbot, and various visual/misc enhancements.

## ⚠️ Disclaimer

This project is for **educational purposes only**. Using cheats in online games violates the game's Terms of Service and can result in permanent bans. Use at your own risk.

## ✨ Features

### ESP (Wallhack)
- ✅ Player boxes (2D)
- ✅ Health display
- ✅ Health bars
- ✅ Player names
- ✅ Weapon display
- ✅ Distance indicator
- ✅ Snaplines
- ✅ Team/enemy filtering
- ✅ Dormant entity filtering
- 🚧 Skeleton ESP (in progress)
- 🚧 Glow effect (in progress)

### Aimbot
- ✅ FOV-based target selection
- ✅ Smooth aim
- ✅ Angle normalization
- ✅ Silent aim option
- ✅ Team check
- ✅ Visibility check
- 🚧 Auto-shoot (in progress)
- 🚧 Triggerbot (in progress)
- 🚧 RCS (Recoil Control System) (in progress)

### Visuals
- ✅ Custom crosshair
- ✅ No flash
- ✅ FOV changer
- 🚧 No smoke (in progress)
- 🚧 Nightmode (in progress)

### Misc
- ✅ Radar hack
- 🚧 Bunny hop (in progress)
- 🚧 Auto strafe (in progress)

### System Features
- ✅ Dynamic offset loading (OffsetManager)
- ✅ JSON/Config file support for offsets
- ✅ Automatic fallback offsets
- ✅ Memory safety with exception handling
- ✅ Comprehensive logging
- ✅ ImGui-based menu
- ✅ SDL3 mouse/keyboard input handling

## 🚀 Quick Start

### Prerequisites

- Windows 10/11 (64-bit)
- Visual Studio 2022 with C++ development tools
- CMake 3.15 or higher
- Counter-Strike 2 installed

### Building

1. Clone the repository:
```bash
git clone https://github.com/FardosESP/cs2menu.git
cd cs2menu
```

2. Run the automated build script:
```bash
python setup_and_build.py
```

Or build manually:
```bash
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

3. The compiled files will be in `build/Release/Release/`:
   - `cs2menu.dll` - The cheat DLL
   - `cs2menu_launcher.exe` - Automatic injector

### Usage

#### Method 1: Using the Launcher (Recommended)
1. Start Counter-Strike 2
2. Run `cs2menu_launcher.exe` as Administrator
3. The DLL will be automatically injected

#### Method 2: Manual Injection
1. Start Counter-Strike 2
2. Use any DLL injector to inject `cs2menu.dll` into `cs2.exe`

#### Controls
- `INSERT` - Toggle menu visibility
- `END` - Unload cheat

## 📖 Documentation

- [Features TODO](FEATURES_TODO.md) - Complete list of planned features
- [Troubleshooting Guide](TROUBLESHOOTING.md) - Common issues and solutions
- [Offset Manager README](OFFSET_MANAGER_README.md) - How to update offsets
- [How to Update Offsets](COMO_ACTUALIZAR_OFFSETS.md) - Step-by-step offset update guide
- [Tests README](src/TESTS_README.md) - Information about unit tests

## 🔧 Configuration

### Updating Offsets

The cheat uses dynamic offset loading. To update offsets:

1. Download the latest offsets from [cs2-dumper](https://github.com/a2x/cs2-dumper)
2. Place `offsets.json` in the same directory as the DLL
3. Restart CS2 and reinject the cheat

Alternatively, create an `offsets.cfg` file:
```
dwEntityList=0x24AE628
dwLocalPlayerPawn=0x2066DE0
dwViewMatrix=0x2309420
m_iHealth=0x76C
m_iTeamNum=0xD70
```

### Menu Configuration

All features can be configured through the in-game menu (press `INSERT`):

- **ESP Tab**: Configure wallhack settings
- **Aimbot Tab**: Configure aimbot settings
- **Visuals Tab**: Configure visual enhancements
- **Misc Tab**: Configure miscellaneous features
- **Skins Tab**: Skin changer (coming soon)

## 🏗️ Project Structure

```
cs2menu/
├── src/                    # Source code
│   ├── Aimbot.cpp/h       # Aimbot implementation
│   ├── Features.cpp/h     # Main features (ESP, etc.)
│   ├── OffsetManager.cpp/h # Dynamic offset management
│   ├── Memory.cpp/h       # Memory reading/writing utilities
│   ├── SDK.h              # Game structures and offsets
│   ├── Hooks.cpp/h        # DirectX and SDL hooks
│   ├── ImGui_Renderer.cpp/h # Menu rendering
│   └── ...
├── lib/                   # External libraries
│   ├── imgui/            # ImGui library
│   └── minhook/          # MinHook library
├── docs/                  # Documentation
├── CMakeLists.txt        # CMake configuration
└── setup_and_build.py    # Automated build script
```

## 🧪 Testing

The project includes unit tests for critical components:

```bash
# Build and run tests
cmake --build build --config Release --target test_offsetmanager
./build/Release/Release/test_offsetmanager.exe
```

## 🐛 Troubleshooting

### ESP not showing players
1. Make sure ESP is enabled in the menu
2. Disable "Team Check" and "Dormant Check"
3. Increase "Max Distance" to 5000
4. Verify you're in an active match (not in menu)

### Aimbot not working
1. Increase FOV to 180
2. Reduce Smooth to 1.0
3. Disable "Visible Only" and "Team Check"
4. Make sure you're aiming near an enemy

### Features not working at all
1. Check the debug console for errors
2. Verify you're in an active match (not spectating or dead)
3. Update offsets from cs2-dumper
4. See [TROUBLESHOOTING.md](TROUBLESHOOTING.md) for detailed diagnostics

## 📝 Current Status

**Build**: 14138 (2026-03-05)

**Working**:
- ✅ DLL injection and hooking
- ✅ ImGui menu with SDL3 input
- ✅ LocalPlayer detection
- ✅ Basic ESP (boxes, health, distance)
- ✅ Player names and weapon display
- ✅ Aimbot core system
- ✅ Custom crosshair
- ✅ No flash
- ✅ Radar hack

**In Progress**:
- 🚧 EntityList iteration (needs offset verification)
- 🚧 Skeleton ESP
- 🚧 Triggerbot
- 🚧 Bunny hop
- 🚧 Skin changer

## 🤝 Contributing

Contributions are welcome! Please:

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## 📜 License

This project is provided as-is for educational purposes. No warranty or support is provided.

## 🔗 Links

- [cs2-dumper](https://github.com/a2x/cs2-dumper) - Offset dumper for CS2
- [ImGui](https://github.com/ocornut/imgui) - GUI library
- [MinHook](https://github.com/TsudaKageyu/minhook) - Hooking library

## ⚠️ Legal Notice

This software is provided for educational and research purposes only. The developers are not responsible for any misuse or damage caused by this software. Using cheats in online games is against the Terms of Service and may result in permanent bans.

**USE AT YOUR OWN RISK**
