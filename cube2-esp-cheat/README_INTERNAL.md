# Cube 2: Sauerbraten - Internal Cheat (DLL Version)

<p align="center">
  <strong>Internal DLL Cheat for Advanced Anticheat Testing & Security Research</strong>
</p>

## ⚠️ CRITICAL DISCLAIMER

**THIS IS AN INTERNAL CHEAT - FOR AUTHORIZED TESTING ONLY**

- ✅ **Authorized use**: Anticheat development, security research, controlled environments
- ❌ **Prohibited use**: Public servers, competitive play, any unauthorized use
- **DLL injection modifies game process memory - use extreme caution**
- **Always obtain proper authorization before testing**
- **The developers are not responsible for misuse**

---

## 🎯 Internal vs External

### **Internal Cheat (This Version)**
- ✅ Runs **inside** the game process (DLL injection)
- ✅ **Direct memory access** (no ReadProcessMemory/WriteProcessMemory)
- ✅ **Faster and more efficient**
- ✅ **DirectX11 hook** for seamless overlay
- ✅ Harder to detect by basic anticheats
- ⚠️ Requires DLL injection
- ⚠️ Game crash = cheat crash

### **External Cheat** (See main README.md)
- Runs as separate process
- Uses Windows API for memory access
- Easier to develop and debug
- Safer (can't crash game)

---

## 🚀 Features

### 🎯 Aimbot
- Direct memory manipulation
- Configurable smoothing (1-20 levels)
- FOV-based targeting
- Distance filtering
- Head/chest targeting
- Visibility checks
- Team check

### 👁️ ESP (Planned)
- Overlay through DirectX hook
- Player boxes, names, health
- Distance indicators
- Skeleton rendering

### 🌫️ Wallhack (Framework)
- Direct renderer manipulation
- Glow effects
- Chams support

### 🎨 Modern ImGui Overlay
- Hooked into game's DirectX11
- Seamless integration
- Dark theme
- Tabbed interface
- Real-time configuration

---

## 📋 Requirements

- **OS**: Windows 10/11 (64-bit)
- **Game**: Cube 2: Sauerbraten
- **Privileges**: Administrator rights (for injection)
- **Development**:
  - CMake 3.15+
  - Visual Studio 2019/2022 or MinGW-w64
  - Windows SDK (DirectX11)
  - Dear ImGui (auto-download available)

---

## 🛠️ Building

### Step 1: Download ImGui

```powershell
cd external
.\download_imgui.ps1
```

### Step 2: Build DLL and Injector

```bash
# Create build directory
mkdir build-internal
cd build-internal

# Generate with CMake (use CMakeLists_Internal.txt)
cmake .. -G "Visual Studio 17 2022" -A x64 -C ../CMakeLists_Internal.txt

# For explicit file:
cmake -DCMAKE_CONFIGURATION_TYPES="Release;Debug" -S .. -B . -C ../CMakeLists_Internal.txt

# Or use the file directly:
cmake .. -G "Visual Studio 17 2022" -A x64
# Then manually use CMakeLists_Internal.txt

# Better approach - just rename it:
# Copy CMakeLists_Internal.txt to CMakeLists.txt in a separate build folder
```

**Recommended Build Method:**

```bash
# Navigate to project root
cd cube2-esp-cheat

# Create separate build directory
mkdir build-internal
cd build-internal

# Copy internal CMake file
copy ..\CMakeLists_Internal.txt CMakeLists.txt

# Generate
cmake . -G "Visual Studio 17 2022" -A x64

# Build
cmake --build . --config Release
```

This will create:
- `build-internal/bin/Release/Cube2Internal.dll` - The cheat DLL
- `build-internal/bin/Release/Cube2Injector.exe` - The injector

---

## 🚀 Usage

### Method 1: Using the Injector (Recommended)

1. **Start Cube 2: Sauerbraten**
2. **Run Cube2Injector.exe as Administrator**:
   ```bash
   cd build-internal\bin\Release
   Cube2Injector.exe
   ```
3. The injector will:
   - Find sauerbraten.exe process
   - Inject Cube2Internal.dll
   - Show success message

4. **Check the game** - a console window will appear showing:
   ```
   [+] Cube 2: Sauerbraten Internal Cheat v2.0
   [+] DLL Injected Successfully!
   [+] Module Base: 0x...
   [+] Press INSERT to toggle menu
   [+] Press END to unload cheat
   ```

### Method 2: Manual Injection

Use any DLL injector:
- Extreme Injector
- Xenos Injector
- Process Hacker (Inject DLL)

Target: `sauerbraten.exe`
DLL: `Cube2Internal.dll`

---

## 🎮 Controls

| Key | Action |
|-----|--------|
| **INSERT** | Toggle menu on/off |
| **END** | Unload cheat (safe unload) |
| **Right Mouse** | Activate aimbot (when enabled) |

---

## 📁 Project Structure (Internal)

```
cube2-esp-cheat/
├── CMakeLists_Internal.txt     # Build config for internal DLL
├── README_INTERNAL.md           # This file
│
├── include/internal/            # Internal cheat headers
│   ├── DirectMemory.h          # Direct memory access
│   ├── AimbotInternal.h        # Aimbot for internal
│   └── D3D11Hook.h             # DirectX11 hooking
│
├── src/internal/                # Internal implementations
│   ├── DllMain.cpp             # DLL entry point
│   ├── DirectMemory.cpp        # Memory operations
│   ├── AimbotInternal.cpp      # Aimbot logic
│   └── D3D11Hook.cpp           # D3D11 Present hook
│
└── src/injector/                # Injector tool
    └── Injector.cpp            # DLL injection utility
```

---

## 🔧 Technical Details

### How It Works

1. **DLL Injection**:
   - Injector finds sauerbraten.exe process
   - Allocates memory in target process
   - Writes DLL path to target memory
   - Creates remote thread calling LoadLibraryW
   - DLL loads into game process

2. **DllMain Entry**:
   - Creates main cheat thread
   - Initializes DirectMemory (gets module base)
   - Sets up aimbot
   - Hooks DirectX11 Present function

3. **DirectX11 Hook**:
   - Finds game's SwapChain VTable
   - Hooks Present function
   - Renders ImGui overlay on each frame
   - Intercepts WndProc for input

4. **Direct Memory Access**:
   - No ReadProcessMemory/WriteProcessMemory needed
   - Direct pointer dereferencing
   - Faster than external methods
   - VirtualProtect for write protection

### Memory Offsets

Same as external version:

```cpp
LOCAL_PLAYER_OFFSET  = 0x2A5730
ENTITY_LIST_OFFSET   = 0x346C90
PLAYER_COUNT_OFFSET  = 0x346C9C
```

Update in `include/GameStructures.h` if needed.

---

## 🔒 Security Considerations

### Detection Risks

Internal cheats are **harder to detect** than external, but still detectable:

1. **Module Scanning**: Game can detect injected DLLs
2. **Hook Detection**: Anti-cheats scan for VTable hooks
3. **Memory Integrity**: Kernel drivers can detect modified memory
4. **Behavioral Analysis**: Inhuman patterns still detectable
5. **Thread Detection**: Unusual threads in game process

### Defense Mechanisms (For Developers)

- **Module verification**: Check loaded DLLs
- **VTable integrity checks**: Verify function pointers
- **Kernel-mode driver**: Monitor process memory
- **Code signing**: Only allow signed modules
- **Heartbeat system**: Detect suspended threads

---

## 🐛 Troubleshooting

### "Injection failed"
- ✅ Run injector as Administrator
- ✅ Disable antivirus temporarily
- ✅ Check if sauerbraten.exe is running
- ✅ Verify DLL path is correct

### "Game crashes on injection"
- ✅ Offsets might be wrong (update GameStructures.h)
- ✅ Game version incompatible
- ✅ Rebuild in Debug mode for error details
- ✅ Check DLL dependencies (use Dependency Walker)

### "Menu doesn't appear"
- ✅ Press INSERT key
- ✅ Check console output for D3D11 hook status
- ✅ Game might not be using DirectX11
- ✅ Try restarting game and re-injecting

### "Aimbot not working"
- ✅ Enable in menu
- ✅ Press aim key (default: Right Mouse)
- ✅ Check console for errors
- ✅ Verify offsets are correct

### "Cannot unload (END key)"
- ✅ Close menu first (INSERT)
- ✅ Wait a few seconds
- ✅ Check console for cleanup messages
- ✅ If stuck, restart game

---

## 🔄 Differences from External Version

| Feature | External | Internal (DLL) |
|---------|----------|----------------|
| **Process** | Separate .exe | Injected .dll |
| **Memory Access** | ReadProcessMemory | Direct pointers |
| **Speed** | Slower | Faster |
| **Overlay** | Separate window | DirectX hook |
| **Detection** | Easier to detect | Harder to detect |
| **Stability** | Safer | Can crash game |
| **Development** | Easier to debug | Harder to debug |
| **Unload** | Just close | Must unload safely |

---

## 📝 Development Notes

### Adding New Features

1. **Add feature to DirectMemory.h/cpp**
2. **Create feature class (e.g., ESPInternal.h/cpp)**
3. **Initialize in DllMain.cpp**
4. **Update in main loop**
5. **Add to Menu.cpp**
6. **Rebuild DLL**

### Debugging

Enable console in DllMain.cpp (already enabled):

```cpp
AllocConsole();
printf("[+] Debug message\n");
```

Use Visual Studio debugger:
1. Build Debug version
2. Attach to sauerbraten.exe
3. Set breakpoints
4. Inject DLL

---

## 🎓 Educational Value

This internal version demonstrates:

- **DLL injection** techniques (CreateRemoteThread)
- **DirectX11 hooking** (VTable hooking, Present hook)
- **Direct memory manipulation** (VirtualProtect, pointers)
- **Process injection** (LoadLibrary method)
- **WndProc hooking** (Input interception)
- **Safe DLL unloading** (FreeLibraryAndExitThread)
- **Thread management** in injected context

---

## ⚖️ Legal & Ethics

**IMPORTANT REMINDERS:**

- This is for **educational and authorized testing ONLY**
- **Never** use on public servers
- **Never** use without authorization
- Respect game developers and players
- Understand the legal implications
- Use for **learning** and **defense**, not offense

---

## 👨‍💻 Author

**0xTym**
- 📧 Email: vraquz@icloud.com
- 🐙 GitHub: [@0xTym](https://github.com/0xTym)
- 💼 Purpose: Security Research & Education

---

## 🙏 Acknowledgments

- [Dear ImGui](https://github.com/ocornut/imgui)
- [Cube 2: Sauerbraten](http://sauerbraten.org/)
- Game hacking community
- Security researchers

---

<p align="center">
  <strong>⚠️ INTERNAL VERSION - EXTREME CAUTION REQUIRED ⚠️</strong><br>
  <em>For authorized anticheat testing and security research only</em>
</p>
