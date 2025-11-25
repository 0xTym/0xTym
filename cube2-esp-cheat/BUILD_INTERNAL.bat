@echo off
echo ========================================
echo  Cube 2 Internal Cheat - Build Script
echo ========================================
echo.

cd /d "%~dp0"

REM Check for ImGui
if not exist "external\imgui\imgui.cpp" (
    echo [!] ImGui not found!
    echo [!] Running automatic download...
    echo.
    cd external
    call download_imgui.bat
    cd ..
    echo.
)

REM Create build directory
if not exist "build-internal" (
    echo [1/4] Creating build directory...
    mkdir build-internal
) else (
    echo [1/4] Build directory exists, cleaning...
    rmdir /s /q build-internal
    mkdir build-internal
)

cd build-internal

REM Copy CMakeLists
echo [2/4] Copying CMakeLists_Internal.txt...
copy ..\CMakeLists_Internal.txt CMakeLists.txt >nul

REM Generate Visual Studio solution
echo [3/4] Generating Visual Studio solution...
cmake . -G "Visual Studio 17 2022" -A x64

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo [!] CMake generation failed!
    echo [!] Make sure you have Visual Studio 2022 installed.
    echo [!] For VS 2019, edit this script and change the generator.
    pause
    exit /b 1
)

REM Build Release
echo [4/4] Building Release configuration...
cmake --build . --config Release

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo [!] Build failed!
    pause
    exit /b 1
)

echo.
echo ========================================
echo  Build Complete!
echo ========================================
echo.
echo Output files:
echo   - bin\Release\Cube2Internal.dll (Cheat DLL)
echo   - bin\Release\Cube2Injector.exe (Injector)
echo.
echo To use:
echo   1. Start Cube 2: Sauerbraten
echo   2. Run bin\Release\Cube2Injector.exe as Administrator
echo   3. Press INSERT in-game for menu
echo   4. Press END to unload
echo.
pause
