# Cube 2: Sauerbraten - Advanced Cheat Suite

<p align="center">
  <strong>Full-Featured External Cheat for Anticheat Testing & Security Research</strong>
</p>

## ⚠️ IMPORTANT DISCLAIMER

**THIS TOOL IS FOR EDUCATIONAL AND SECURITY TESTING PURPOSES ONLY**

- ✅ **Authorized use**: Anticheat testing, security research, educational purposes, controlled environments
- ❌ **Prohibited use**: Public servers, competitive play, any unauthorized use
- This software demonstrates advanced memory manipulation and game hacking techniques for defensive security purposes
- **Always obtain proper authorization before testing on any system**
- The developers are not responsible for misuse of this software

---

## 🎯 Features Overview

### 🎨 Modern ImGui-based GUI
- **Professional looking overlay menu**
- Dark theme with customizable opacity
- Tabbed interface for organized settings
- Real-time FPS and status display
- Hotkey-based menu toggle (INSERT key)

### 🎯 Aimbot
- **Configurable smoothing** (1-20 smoothness levels)
- **FOV-based targeting** (adjustable field of view)
- **Distance filtering** (max distance control)
- **Target selection**: Head or chest targeting
- **Visibility checks**: Only aim at visible targets
- **Team check**: Avoid targeting teammates
- **Customizable aim key** (default: Right Mouse Button)

### 👁️ ESP (Extra Sensory Perception)
- **Player information display**:
  - Boxes around players
  - Player names
  - Health and armor bars
  - Distance from local player
  - Skeleton rendering
- **Color customization**:
  - Separate colors for enemies and teammates
  - Adjustable box thickness
- **Distance filtering** (max ESP range)

### 🌫️ Wallhack
- **Glow effects** through walls
- **Chams (Chameleon models)** - colored player models
- **Adjustable glow brightness**
- **Separate colors** for enemies and teammates
- **Toggle-able components**

### 🛠️ Miscellaneous Features
- **No Recoil** - eliminates weapon recoil
- **No Spread** - perfect accuracy
- **Infinite Ammo** - never run out of ammunition
- **Rapid Fire** - increased fire rate
- **Speed Hack** - movement speed multiplier (1.0x - 5.0x)

### 🔧 Technical Features
- **External memory reading/writing** using Windows API
- **Process attachment** with automatic detection
- **Real-time memory manipulation**
- **DirectX11 rendering** for overlay
- **Clean architecture** with modular components

---

## 📋 Requirements

### System Requirements
- **OS**: Windows 10/11 (64-bit)
- **Game**: Cube 2: Sauerbraten
- **Privileges**: Administrator rights (for memory access)

### Development Requirements
- **CMake** 3.15 or higher
- **C++ Compiler** with C++17 support:
  - Visual Studio 2019/2022 (MSVC) - Recommended
  - MinGW-w64 (GCC)
  - Clang for Windows
- **Windows SDK** (for DirectX11)
- **Dear ImGui** library (see setup below)

---

## 🛠️ Building the Project

### Step 1: Download ImGui

**ImGui is required for the GUI!** See `external/IMGUI_SETUP.md` for detailed instructions.

**Quick method** (PowerShell in `cube2-esp-cheat/external/` directory):

```powershell
# Download ImGui v1.90.0
$version = "v1.90.0"
$url = "https://github.com/ocornut/imgui/archive/refs/tags/$version.zip"
Invoke-WebRequest -Uri $url -OutFile "imgui.zip"

# Extract and setup
Expand-Archive -Path "imgui.zip" -DestinationPath "."
Rename-Item -Path "imgui-$($version.Substring(1))" -NewName "imgui"
Copy-Item "imgui/backends/imgui_impl_dx11.*" "imgui/"
Copy-Item "imgui/backends/imgui_impl_win32.*" "imgui/"
Remove-Item "imgui.zip"
```

### Step 2: Build with Visual Studio (Recommended)

```bash
# Navigate to project directory
cd cube2-esp-cheat

# Create build directory
mkdir build
cd build

# Generate Visual Studio solution
cmake .. -G "Visual Studio 17 2022" -A x64
# For VS 2019 use: cmake .. -G "Visual Studio 16 2019" -A x64

# Build the project
cmake --build . --config Release

# Executable location: build/bin/Release/Cube2ESP.exe
```

### Step 3: Build with MinGW

```bash
# Create build directory
mkdir build
cd build

# Generate Makefiles
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build .

# Executable location: build/bin/Cube2ESP.exe
```

### Step 4: Using Visual Studio Code

1. Install **CMake Tools** extension
2. Open `cube2-esp-cheat` folder in VSCode
3. Select your compiler kit when prompted
4. Press **F7** or use **Command Palette** → `CMake: Build`

---

## 🚀 Usage Guide

### Running the Cheat

1. **Start Cube 2: Sauerbraten** first
2. **Run Cube2ESP.exe** as Administrator:
   ```bash
   # Right-click → "Run as administrator"
   # Or from elevated command prompt:
   .\build\bin\Release\Cube2ESP.exe
   ```
3. The cheat will automatically detect and attach to `sauerbraten.exe`
4. A GUI window will appear showing the status and menu

### Controls

| Key | Action |
|-----|--------|
| **INSERT** | Toggle menu visibility |
| **Right Mouse** | Activate aimbot (configurable) |
| **GUI** | Click and configure all settings in the menu |

### Menu Navigation

The menu has 5 tabs:

1. **Aimbot** - Configure aim assistance settings
2. **ESP** - Configure visual overlays and player information
3. **Wallhack** - Configure glow and chams effects
4. **Misc** - Additional features (no recoil, speed hack, etc.)
5. **Settings** - Menu configuration and about information

### Configuration Tips

#### For Legitimate Looking Aimbot:
- Enable smoothing
- Set smoothness to 8-15
- Set FOV to 30-60 degrees
- Enable "Visible Only"
- Enable "Team Check"

#### For Obvious Testing:
- Disable smoothing
- Set FOV to 180 degrees
- High speed multiplier
- All ESP features enabled

---

## 📁 Project Structure

```
cube2-esp-cheat/
├── CMakeLists.txt              # Build configuration
├── README.md                   # This file
├── .gitignore                  # Git ignore rules
│
├── external/                   # External dependencies
│   ├── IMGUI_SETUP.md         # ImGui setup instructions
│   └── imgui/                 # ImGui library (download required)
│
├── include/                    # Header files
│   ├── Config.h               # Global configuration structure
│   ├── GameStructures.h       # Game memory structures
│   ├── MemoryReader.h         # Memory read/write utilities
│   ├── ESP.h                  # ESP functionality
│   ├── Aimbot.h               # Aimbot functionality
│   ├── Wallhack.h             # Wallhack functionality
│   └── overlay/               # Overlay system headers
│       ├── Overlay.h          # Overlay window management
│       └── Menu.h             # ImGui menu system
│
├── src/                        # Source files
│   ├── main.cpp               # Main application (GUI version)
│   ├── main_old_console.cpp   # Old console version (backup)
│   ├── MemoryReader.cpp       # Memory operations implementation
│   ├── ESP.cpp                # ESP implementation
│   ├── Aimbot.cpp             # Aimbot implementation
│   ├── Wallhack.cpp           # Wallhack implementation
│   └── overlay/               # Overlay implementation
│       └── Menu.cpp           # ImGui menu rendering
│
└── .vscode/                    # VSCode configuration
    ├── settings.json
    └── launch.json
```

---

## 🔧 Memory Offsets

**Current offsets** (may need updating for different game versions):

```cpp
LOCAL_PLAYER_OFFSET  = 0x2A5730
ENTITY_LIST_OFFSET   = 0x346C90
PLAYER_COUNT_OFFSET  = 0x346C9C
```

### Updating Offsets

If the cheat stops working after a game update:

1. **Use a memory scanner** (Cheat Engine, ReClass.NET)
2. **Find new offsets** for:
   - Local player pointer
   - Entity list pointer
   - Player count
3. **Update** `include/GameStructures.h`:
   ```cpp
   constexpr uintptr_t LOCAL_PLAYER_OFFSET = 0xNEWOFFSET;
   constexpr uintptr_t ENTITY_LIST_OFFSET = 0xNEWOFFSET;
   constexpr uintptr_t PLAYER_COUNT_OFFSET = 0xNEWOFFSET;
   ```
4. **Rebuild** the project

### Player Structure Offsets

| Offset | Type | Description |
|--------|------|-------------|
| 0x0004 | Vector3 | Head position |
| 0x0034 | Vector3 | Body position |
| 0x0040 | Vector3 | Velocity |
| 0x006C | float | Yaw (horizontal angle) |
| 0x0070 | float | Pitch (vertical angle) |
| 0x00C8 | int32 | Health |
| 0x00CC | int32 | Armor |
| 0x0144 | int32 | Current weapon |
| 0x01B8 | char[16] | Player name |
| 0x033C | int32 | Team |

---

## 🎓 Educational Value

This project demonstrates:

### Game Hacking Techniques
- **External memory reading/writing** with Windows API
- **Process and module enumeration** (ToolHelp32 API)
- **Pointer chain resolution**
- **Vector mathematics** for 3D calculations
- **Angle calculations** for aiming systems
- **Smooth interpolation** algorithms

### Rendering & UI
- **DirectX11** basics and rendering pipeline
- **ImGui integration** for modern UIs
- **Overlay rendering** techniques
- **Custom styling** and theming

### Software Engineering
- **Clean architecture** and separation of concerns
- **Modular design** with reusable components
- **CMake build system** configuration
- **Cross-compiler compatibility**
- **Modern C++17** features and best practices

---

## 🔒 Security Considerations

### For Game Developers

This tool demonstrates why **client-side security alone is insufficient**:

#### Vulnerabilities Exposed:
1. **Unencrypted memory** - All game data readable/writable
2. **Predictable offsets** - Easy to find with pattern scanning
3. **No integrity checks** - Memory modifications undetected
4. **Client trust** - Server trusts client-side data

#### Defense Strategies:
1. **Memory encryption** - Encrypt sensitive data in RAM
2. **ASLR** - Address Space Layout Randomization
3. **Server-side validation** - Never trust client data
4. **Integrity monitoring** - Detect memory tampering
5. **Behavioral analysis** - Detect inhuman patterns
6. **Kernel-mode protection** - Use anti-cheat drivers
7. **Code obfuscation** - Make reverse engineering harder
8. **Frequent updates** - Change offsets regularly

### Detection Methods

#### This cheat can be detected by:
- **Handle enumeration** - Detecting OpenProcess calls
- **Memory access patterns** - Unusual ReadProcessMemory calls
- **Module scanning** - Detecting injected DLLs (if converted to internal)
- **Behavioral analysis** - Perfect aim, inhuman reactions
- **Integrity checks** - Modified game memory
- **Kernel-mode drivers** - Anti-cheat software (EasyAntiCheat, BattlEye)

---

## 🐛 Troubleshooting

### "Failed to find process: sauerbraten.exe"
- ✅ Ensure Cube 2: Sauerbraten is running
- ✅ Check that the process name is exactly `sauerbraten.exe` (use Task Manager)
- ✅ Game might use a different executable name

### "Failed to open process handle"
- ✅ Run Cube2ESP.exe as **Administrator**
- ✅ Disable antivirus temporarily (may block memory access)
- ✅ Check Windows Defender/Firewall settings

### "ImGui not found" (CMake error)
- ✅ Download ImGui as described in `external/IMGUI_SETUP.md`
- ✅ Ensure files are in correct location: `external/imgui/`
- ✅ Verify backend files are copied to imgui root folder

### Aimbot not working
- ✅ Check if aim key is being pressed (default: Right Mouse)
- ✅ Verify FOV settings (increase if too restrictive)
- ✅ Disable "Visible Only" for testing
- ✅ Offsets might be outdated (update and rebuild)

### ESP not showing / incorrect data
- ✅ Offsets are likely outdated after game update
- ✅ Use Cheat Engine to find new offsets
- ✅ Update `GameStructures.h` and rebuild

### Menu not appearing
- ✅ Press **INSERT** key to toggle menu
- ✅ Check if GUI window is minimized or behind game window
- ✅ Verify DirectX11 initialization succeeded

### Crashes or instability
- ✅ Game version might be incompatible
- ✅ Update offsets to match current game version
- ✅ Check Windows Event Viewer for error details
- ✅ Run in Debug mode for detailed error messages

---

## 🔄 Version History

### v2.0.0 - Advanced Suite (Current)
- ✨ Added ImGui-based modern GUI
- ✨ Implemented Aimbot with smoothing
- ✨ Added Wallhack functionality
- ✨ Misc features (no recoil, speed hack, etc.)
- ✨ Memory writing capabilities
- ✨ DirectX11 overlay system
- ✨ Comprehensive configuration system

### v1.0.0 - Initial Release
- ✅ Basic ESP (console-based)
- ✅ External memory reading
- ✅ Player tracking
- ✅ Health/Armor display

---

## 📝 License & Legal

**MIT License** - For educational purposes only

```
Copyright (c) 2024 0xTym

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software for educational and security research purposes.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND.
```

### Legal Notice

- This software is for **educational and security testing purposes only**
- Unauthorized use in online games or public servers is **prohibited**
- The author assumes **no liability** for misuse
- Always obtain **proper authorization** before testing
- Respect **game developers** and **other players**

---

## 👨‍💻 Author

**0xTym**
- 📧 Email: vraquz@icloud.com
- 🐙 GitHub: [@0xTym](https://github.com/0xTym)
- 💼 Purpose: Security Research & Education

---

## 🙏 Acknowledgments

- [Dear ImGui](https://github.com/ocornut/imgui) - Immediate Mode GUI library
- [Cube 2: Sauerbraten](http://sauerbraten.org/) - The game
- Game hacking & reverse engineering community
- Open-source security research contributors

---

## 📚 Further Reading

### Recommended Resources:
- [Game Hacking Academy](https://gamehacking.academy/)
- [Guided Hacking](https://guidedhacking.com/)
- [UnKnoWnCheaTs](https://www.unknowncheats.me/)
- [ImGui Documentation](https://github.com/ocornut/imgui/wiki)
- [DirectX 11 Tutorial](http://www.rastertek.com/tutdx11.html)

---

<p align="center">
  <strong>⚠️ USE RESPONSIBLY AND ETHICALLY ⚠️</strong><br>
  <em>For educational and authorized security testing only</em>
</p>
