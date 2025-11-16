# ImGui Setup Instructions

This project uses Dear ImGui for the modern overlay menu system.

## Quick Setup

1. Download ImGui from: https://github.com/ocornut/imgui
2. Extract the following files to `cube2-esp-cheat/external/imgui/`:

### Required Core Files:
- `imgui.cpp`
- `imgui.h`
- `imgui_demo.cpp`
- `imgui_draw.cpp`
- `imgui_tables.cpp`
- `imgui_widgets.cpp`
- `imconfig.h`
- `imgui_internal.h`
- `imstb_rectpack.h`
- `imstb_textedit.h`
- `imstb_truetype.h`

### Required Backend Files (from backends/ folder):
- `backends/imgui_impl_dx11.cpp`
- `backends/imgui_impl_dx11.h`
- `backends/imgui_impl_win32.cpp`
- `backends/imgui_impl_win32.h`

## Automated Download (PowerShell)

Run this command in the `cube2-esp-cheat/external/` directory:

```powershell
# Download ImGui latest release
$version = "v1.90.0"
$url = "https://github.com/ocornut/imgui/archive/refs/tags/$version.zip"
Invoke-WebRequest -Uri $url -OutFile "imgui.zip"

# Extract
Expand-Archive -Path "imgui.zip" -DestinationPath "."
Rename-Item -Path "imgui-$($version.Substring(1))" -NewName "imgui"

# Copy backend files to main directory
Copy-Item "imgui/backends/imgui_impl_dx11.*" "imgui/"
Copy-Item "imgui/backends/imgui_impl_win32.*" "imgui/"

# Cleanup
Remove-Item "imgui.zip"
```

## Manual Download

1. Visit: https://github.com/ocornut/imgui/releases
2. Download the latest release (v1.90.0 or newer)
3. Extract all files from the root directory to `cube2-esp-cheat/external/imgui/`
4. Copy the DirectX11 and Win32 backend files from `backends/` to the imgui root folder

## Verify Installation

Your directory structure should look like:

```
cube2-esp-cheat/
└── external/
    └── imgui/
        ├── imgui.cpp
        ├── imgui.h
        ├── imgui_impl_dx11.cpp
        ├── imgui_impl_dx11.h
        ├── imgui_impl_win32.cpp
        ├── imgui_impl_win32.h
        └── [other ImGui files...]
```

## Build

Once ImGui is installed, the CMake build system will automatically include it.
