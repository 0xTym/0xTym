# Quick Start Guide

Follow these steps to build and run the Cube 2: Sauerbraten Advanced Cheat Suite.

## Prerequisites

- Windows 10/11 (64-bit)
- Visual Studio 2019 or 2022 (or MinGW-w64)
- CMake 3.15 or higher
- Administrator privileges

## Step-by-Step Installation

### 1. Download ImGui (Required!)

**Option A: Automatic (Recommended)**

Open PowerShell in the project directory and run:

```powershell
cd cube2-esp-cheat\external
.\download_imgui.ps1
```

Or use the batch file:

```cmd
cd cube2-esp-cheat\external
download_imgui.bat
```

**Option B: Manual**

See `external/README.md` for manual installation instructions.

### 2. Build the Project

**Using Visual Studio 2022:**

```cmd
cd cube2-esp-cheat
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```

**Using Visual Studio 2019:**

```cmd
cmake .. -G "Visual Studio 16 2019" -A x64
cmake --build . --config Release
```

**Using MinGW:**

```cmd
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build .
```

### 3. Locate the Executable

After building, find the executable at:

```
build\bin\Release\Cube2ESP.exe
```

### 4. Run the Cheat

1. **Start Cube 2: Sauerbraten** first
2. **Run Cube2ESP.exe as Administrator**:
   - Right-click on `Cube2ESP.exe`
   - Select "Run as administrator"
3. The cheat will automatically attach to the game
4. A GUI window will appear

### 5. Controls

| Key | Action |
|-----|--------|
| **INSERT** | Toggle menu on/off |
| **Right Mouse** | Activate aimbot (when enabled in menu) |

## Common Issues

### "ImGui not found" Error

If you see this during CMake configuration:

```
CMake Error: ImGui NOT FOUND!
```

**Solution:**
1. Navigate to `external` folder
2. Run `download_imgui.ps1` or `download_imgui.bat`
3. Delete the `build` folder
4. Run CMake again

### "Cannot find source file: imgui_impl_dx11.cpp"

**Solution:**
The ImGui backend files weren't copied correctly.

Run this in PowerShell from the `external` folder:

```powershell
Copy-Item "imgui\backends\imgui_impl_dx11.*" "imgui\" -Force
Copy-Item "imgui\backends\imgui_impl_win32.*" "imgui\" -Force
```

### "Access Denied" or "Execution Policy" Error

**Solution:**
Run PowerShell as Administrator and execute:

```powershell
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
```

Then try the download script again.

### CMake can't find Visual Studio

**Solution:**
Make sure you have Visual Studio 2019 or 2022 installed with C++ development tools.

Or specify the generator manually:

```cmd
cmake .. -G "Visual Studio 17 2022" -A x64
```

### "Failed to attach to process"

**Solution:**
1. Make sure Cube 2: Sauerbraten is running
2. Run Cube2ESP.exe as Administrator
3. Check that the process name is `sauerbraten.exe` in Task Manager

## Features Quick Reference

### Aimbot Tab
- Enable/Disable aimbot
- Smoothing (1-20, higher = smoother)
- FOV slider (field of view)
- Max distance
- Target selection (head/chest)
- Visibility checks
- Team check

### ESP Tab
- Show boxes, names, health, distance
- Skeleton rendering
- Color customization
- Distance filtering

### Wallhack Tab
- Glow effects
- Chams (colored models)
- Brightness adjustment
- Team-based colors

### Misc Tab
- No recoil
- No spread
- Infinite ammo
- Rapid fire
- Speed hack

### Settings Tab
- Menu opacity
- Show FPS
- Hotkey configuration

## Tips

**For testing aimbot:**
- Start with smoothing = 10
- FOV = 60 degrees
- Enable "Visible Only"
- Enable "Team Check"

**For obvious detection testing:**
- Disable smoothing
- FOV = 180 degrees
- Disable all checks

## Need Help?

See the main README.md for:
- Detailed documentation
- Troubleshooting guide
- Security considerations
- Educational resources

---

**Remember:** This tool is for educational and authorized testing only!
