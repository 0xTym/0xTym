@echo off
echo ========================================
echo  ImGui Automatic Download Script
echo  For Cube 2 ESP Cheat Project
echo ========================================
echo.

cd /d "%~dp0"

if not exist "imgui" (
    echo [1/4] Downloading ImGui v1.90.0...
    powershell -Command "& {[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; Invoke-WebRequest -Uri 'https://github.com/ocornut/imgui/archive/refs/tags/v1.90.0.zip' -OutFile 'imgui.zip'}"

    if %ERRORLEVEL% NEQ 0 (
        echo ERROR: Failed to download ImGui!
        echo Please download manually from: https://github.com/ocornut/imgui/releases
        pause
        exit /b 1
    )

    echo [2/4] Extracting ImGui...
    powershell -Command "& {Expand-Archive -Path 'imgui.zip' -DestinationPath '.' -Force}"

    echo [3/4] Renaming folder...
    if exist "imgui-1.90.0" (
        rename "imgui-1.90.0" "imgui"
    )

    echo [4/4] Copying backend files...
    copy "imgui\backends\imgui_impl_dx11.cpp" "imgui\" >nul
    copy "imgui\backends\imgui_impl_dx11.h" "imgui\" >nul
    copy "imgui\backends\imgui_impl_win32.cpp" "imgui\" >nul
    copy "imgui\backends\imgui_impl_win32.h" "imgui\" >nul

    echo [5/4] Cleaning up...
    del "imgui.zip" >nul 2>&1

    echo.
    echo ========================================
    echo  ImGui successfully installed!
    echo ========================================
) else (
    echo ImGui already exists in external/imgui/
    echo If you want to reinstall, delete the imgui folder first.
)

echo.
echo You can now build the project with CMake.
echo.
pause
