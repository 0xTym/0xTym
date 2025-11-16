# External Dependencies

This directory contains setup scripts for external dependencies.

## ImGui Installation

ImGui is **required** to build this project. Choose one of the following methods:

### Method 1: Automatic Installation (Recommended)

#### Windows Batch Script:
```bash
cd external
download_imgui.bat
```

#### PowerShell Script:
```powershell
cd external
.\download_imgui.ps1
```

Or right-click on `download_imgui.ps1` → "Run with PowerShell"

### Method 2: Manual Installation

See `IMGUI_SETUP.md` for detailed manual installation instructions.

### Method 3: One-Liner (PowerShell)

From the `external/` directory:

```powershell
[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; Invoke-WebRequest -Uri 'https://github.com/ocornut/imgui/archive/refs/tags/v1.90.0.zip' -OutFile 'imgui.zip'; Expand-Archive -Path 'imgui.zip' -DestinationPath '.'; Rename-Item 'imgui-1.90.0' 'imgui'; Copy-Item 'imgui\backends\imgui_impl_dx11.*' 'imgui\'; Copy-Item 'imgui\backends\imgui_impl_win32.*' 'imgui\'; Remove-Item 'imgui.zip'
```

## Verification

After installation, verify that these files exist:

```
external/imgui/
├── imgui.cpp
├── imgui.h
├── imgui_impl_dx11.cpp
├── imgui_impl_dx11.h
├── imgui_impl_win32.cpp
└── imgui_impl_win32.h
```

## Troubleshooting

### "Cannot download ImGui"
- Check your internet connection
- Try downloading manually from: https://github.com/ocornut/imgui/releases/tag/v1.90.0

### "Access Denied" or execution policy errors (PowerShell)
Run PowerShell as Administrator and execute:
```powershell
Set-ExecutionPolicy -ExecutionPolicy RemoteSigned -Scope CurrentUser
```

Then try running the script again.

### CMake still complains about missing ImGui
1. Verify that `external/imgui/imgui.cpp` exists
2. Delete the `build/` folder and regenerate:
   ```bash
   rmdir /s /q build
   mkdir build
   cd build
   cmake .. -G "Visual Studio 17 2022" -A x64
   ```

## Next Steps

After installing ImGui, return to the main project directory and build:

```bash
cd ..
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022" -A x64
cmake --build . --config Release
```
