# Cube 2: Sauerbraten ESP Internal Cheat

**⚠️ FOR EDUCATIONAL AND ANTI-CHEAT TESTING PURPOSES ONLY ⚠️**

An ESP (Extra Sensory Perception) internal cheat for Cube 2: Sauerbraten, designed for testing anti-cheat systems and understanding game security vulnerabilities.

## 📋 Features

- **Player ESP Boxes**: Visual bounding boxes around enemy players
- **Health Bars**: Real-time health visualization
- **Player Names**: Display player names above characters
- **Distance Indicator**: Shows distance to other players
- **Snaplines**: Lines connecting screen center to players
- **Team Color Coding**: Different colors for teammates vs enemies
- **Configurable Hotkeys**: Toggle features on/off in real-time

## 🛠️ Technical Details

### Memory Offsets (may vary by game version)

```cpp
uintptr_t localPlayerOffset = 0x2A5730;
uintptr_t entityListOffset = 0x346C90;
uintptr_t playerCountOffset = 0x346C9C;
```

### Architecture

- **Memory Reading**: Custom memory utilities for reading game state
- **OpenGL Hooking**: Hooks `wglSwapBuffers` for rendering overlay
- **Entity System**: Parses Sauerbraten player entities
- **World-to-Screen**: 3D to 2D projection for accurate ESP rendering

## 🔧 Building

### Prerequisites

- Windows OS (Windows 10/11 recommended)
- CMake 3.10 or higher
- Visual Studio 2019/2022 or MinGW-w64
- OpenGL development libraries

### Build Steps

#### Using Visual Studio

```bash
# Clone the repository
git clone https://github.com/0xTym/sauerbraten-esp.git
cd sauerbraten-esp

# Create build directory
mkdir build
cd build

# Generate Visual Studio solution
cmake .. -G "Visual Studio 17 2022" -A Win32

# Build the project
cmake --build . --config Release
```

#### Using MinGW

```bash
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
cmake --build .
```

The compiled DLL will be in `build/bin/SauerbratenESP.dll`

## 🚀 Usage

### Injection Methods

#### Method 1: Included Stealth Loader (Recommended)

The project includes an advanced stealth loader with manual mapping:

```bash
# Build the loader
cd loader/build
cmake .. -G "Visual Studio 17 2022" -A Win32
cmake --build . --config Release

# Use the loader (manual mapping - stealthiest)
SauerbratenLoader.exe sauerbraten.exe SauerbratenESP.dll

# Or use LoadLibrary mode (faster, less stealth)
SauerbratenLoader.exe sauerbraten.exe SauerbratenESP.dll --loadlib
```

See [loader/README.md](loader/README.md) for detailed documentation.

#### Method 2: External Injectors

Alternatively, use third-party injectors:
- Extreme Injector
- Process Hacker
- Xenos Injector

### Controls

| Key | Function |
|-----|----------|
| `INSERT` | Toggle ESP on/off |
| `DELETE` | Unload cheat |
| `HOME` | Toggle boxes |
| `END` | Toggle snaplines |

### Console Output

A debug console will appear showing:
- Initialization status
- Hook status
- Current configuration
- Real-time feature toggles

## ⚙️ Configuration

Default configuration in `src/esp.h`:

```cpp
struct Config {
    bool enabled = true;
    bool showBox = true;
    bool showName = true;
    bool showHealth = true;
    bool showDistance = true;
    bool showSnaplines = true;

    float boxColor[3] = {1.0f, 0.0f, 0.0f};      // Red for enemies
    float teamColor[3] = {0.0f, 1.0f, 0.0f};     // Green for teammates
    float nameColor[3] = {1.0f, 1.0f, 1.0f};     // White for names
    float snaplineColor[3] = {1.0f, 1.0f, 0.0f}; // Yellow for lines
};
```

## 📁 Project Structure

```
sauerbraten-esp/
├── CMakeLists.txt          # Build configuration
├── README.md               # This file
└── src/
    ├── main.cpp            # DLL entry point and main thread
    ├── esp.cpp/h           # ESP rendering logic
    ├── hooks.cpp/h         # OpenGL hooking system
    ├── memory.h            # Memory reading utilities
    └── entities.h          # Game entity structures
```

## 🔬 Anti-Cheat Testing

This tool is designed to help developers:

1. **Test Detection Methods**: Verify if anti-cheat systems can detect memory reading and rendering hooks
2. **Understand Attack Vectors**: Learn how cheats interact with game memory
3. **Develop Countermeasures**: Create better protection systems
4. **Education**: Understand game security at a technical level

### Detection Points

- **Memory Pattern Scanning**: Signatures of common cheat code
- **Hook Detection**: Inline hooks on OpenGL functions
- **Behavioral Analysis**: Unusual memory access patterns
- **Integrity Checks**: Modified game executable or libraries

## ⚠️ Legal Disclaimer

**IMPORTANT**: This software is provided for **educational and anti-cheat testing purposes only**.

- ✅ Use in private servers for testing
- ✅ Use for security research
- ✅ Use for educational purposes
- ❌ DO NOT use on public servers
- ❌ DO NOT use to gain unfair advantages
- ❌ DO NOT violate game Terms of Service

The authors are not responsible for any misuse of this software. Using cheats in online games may result in permanent bans and legal consequences.

## 🛡️ Detected By

This cheat may be detected by:
- Modern anti-cheat systems
- Server-side anti-cheat (if implemented)
- Memory integrity scanners
- Hook detection tools

## 🤝 Contributing

Contributions are welcome for:
- Improving detection resistance (for research purposes)
- Code optimization
- Better memory patterns
- Cross-platform support (Linux version)
- Documentation improvements

## 📚 Resources

- [Cube 2: Sauerbraten Official Site](http://sauerbraten.org/)
- [Game Hacking Community](https://guidedhacking.com/)
- [OpenGL Documentation](https://www.opengl.org/documentation/)

## 📝 Version Notes

- **Version**: 1.0.0
- **Game Version Tested**: Cube 2: Sauerbraten (2020 Edition)
- **Status**: Working (as of last test)
- **Note**: Offsets may need updating for newer game versions

## 👤 Author

**0xTym**
- GitHub: [@0xTym](https://github.com/0xTym)
- Email: vraquz@icloud.com

## 📄 License

This project is provided as-is for educational purposes. Use at your own risk.

---

**Remember**: Cheating ruins the fun for everyone. Use this tool responsibly and only for legitimate security research and testing purposes.
