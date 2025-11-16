# Cube 2: Sauerbraten ESP - Anticheat Testing Tool

External ESP (Extra Sensory Perception) cheat for Cube 2: Sauerbraten, designed for anticheat testing and security research purposes.

## ⚠️ Disclaimer

**This tool is provided for educational and security testing purposes only.**

- ✅ Authorized use: Anticheat testing, security research, educational purposes
- ❌ Unauthorized use: Using this in public servers or competitive play is prohibited
- This tool demonstrates memory reading techniques for defensive security purposes
- Always obtain proper authorization before testing on any system

## 🎯 Features

- **External Memory Reading**: Reads game memory from outside the process
- **Player ESP**: Display information about all players in the game
- **Real-time Updates**: Continuously updates player information
- **Console-based UI**: Clean, formatted output in the terminal

### Displayed Information

- **Local Player**:
  - Name
  - Health and Armor (with visual bars)
  - 3D Position coordinates
  - Current weapon ID

- **Enemy Players**:
  - Player name
  - Health bar visualization
  - Armor points
  - Distance from local player
  - 3D Position coordinates
  - Team affiliation

## 📋 Requirements

- Windows OS (tested on Windows 10/11)
- Cube 2: Sauerbraten game installed
- CMake 3.15 or higher
- C++ compiler with C++17 support:
  - Visual Studio 2019/2022 (MSVC)
  - MinGW-w64 (GCC)
  - Clang for Windows

## 🛠️ Building the Project

### Using Visual Studio (Recommended)

```bash
# Create build directory
mkdir build
cd build

# Generate Visual Studio project files
cmake .. -G "Visual Studio 16 2019" -A x64
# Or for VS 2022: cmake .. -G "Visual Studio 17 2022" -A x64

# Build the project
cmake --build . --config Release

# Executable will be in: build/bin/Release/Cube2ESP.exe
```

### Using MinGW

```bash
# Create build directory
mkdir build
cd build

# Generate Makefiles
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build .

# Executable will be in: build/bin/Cube2ESP.exe
```

### Using Visual Studio Code

1. Install CMake Tools extension
2. Open the `cube2-esp-cheat` folder in VSCode
3. Select your kit (compiler) when prompted
4. Press F7 to build or use Command Palette: `CMake: Build`

## 🚀 Usage

1. **Start Cube 2: Sauerbraten** and join a game or practice mode
2. **Run the ESP tool** as Administrator:
   ```bash
   # Navigate to the build output directory
   cd build/bin/Release  # or Debug

   # Run the executable
   Cube2ESP.exe
   ```
3. The tool will automatically attach to the `sauerbraten.exe` process
4. ESP information will be displayed in the console window
5. Press `Ctrl+C` to exit gracefully

### Running as Administrator

The tool requires administrator privileges to read memory from another process:

- Right-click `Cube2ESP.exe` → "Run as administrator"
- Or run from an elevated command prompt/PowerShell

## 📁 Project Structure

```
cube2-esp-cheat/
├── CMakeLists.txt          # Build configuration
├── README.md               # This file
├── include/                # Header files
│   ├── GameStructures.h    # Cube 2 memory structures
│   ├── MemoryReader.h      # Memory reading utilities
│   └── ESP.h               # ESP functionality
└── src/                    # Source files
    ├── main.cpp            # Entry point
    ├── MemoryReader.cpp    # Memory reading implementation
    └── ESP.cpp             # ESP implementation
```

## 🔧 Memory Offsets

Current offsets (may need updating for different game versions):

```cpp
LOCAL_PLAYER_OFFSET  = 0x2A5730
ENTITY_LIST_OFFSET   = 0x346C90
PLAYER_COUNT_OFFSET  = 0x346C9C
```

### Updating Offsets

If the ESP doesn't work after a game update:

1. Use a memory scanner (Cheat Engine, ReClass.NET)
2. Find the new offsets for:
   - Local player pointer
   - Entity list pointer
   - Player count
3. Update values in `include/GameStructures.h`

## 🎓 Educational Value

This project demonstrates:

- **External memory reading** using Windows API
- **Process and module enumeration** with ToolHelp32
- **Memory pattern scanning** and pointer chains
- **Game structure reverse engineering**
- **Real-time data visualization**
- **Proper C++ project structure** with CMake

## 🔒 Security Considerations

### For Game Developers:

This tool demonstrates why client-side security is insufficient:

1. **Memory protection**: Implement address space layout randomization (ASLR)
2. **Encryption**: Encrypt sensitive game data in memory
3. **Server validation**: Never trust client-side data
4. **Integrity checks**: Implement module/memory integrity verification
5. **Behavioral analysis**: Detect suspicious memory access patterns

### Detection Techniques:

- Handle enumeration monitoring
- Suspicious process detection (debuggers, memory scanners)
- Memory read/write pattern analysis
- Kernel-mode protection drivers

## 🐛 Troubleshooting

### "Failed to find process"
- Ensure Cube 2: Sauerbraten is running
- Check that the process name is `sauerbraten.exe`
- Verify you're running the correct game version

### "Failed to open process handle"
- Run the tool as Administrator
- Check Windows Defender/Antivirus settings
- Ensure no other security software is blocking access

### "Failed to get base address"
- Game version might be different
- Process name might have changed
- Try restarting both the game and the tool

### No players shown or incorrect data
- Offsets might be outdated (game update)
- Use Cheat Engine or similar tool to find new offsets
- Ensure you're in an active game with other players

## 📝 License

This project is for educational purposes only. Use responsibly and ethically.

## 👨‍💻 Author

**0xTym**
- Email: vraquz@icloud.com
- GitHub: [@0xTym](https://github.com/0xTym)

## 🙏 Acknowledgments

- Cube 2: Sauerbraten development team
- Game hacking and security research community
- Open-source reverse engineering tools

---

**Remember**: Always use this tool ethically and legally. Respect game developers, players, and community guidelines.
