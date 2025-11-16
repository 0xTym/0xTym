@echo off
echo ========================================
echo Sauerbraten ESP Build Script
echo ========================================
echo.

REM Check if build directory exists
if exist build (
    echo [*] Build directory exists
    choice /C YN /M "Clean build directory"
    if errorlevel 2 goto :configure
    if errorlevel 1 (
        echo [*] Cleaning build directory...
        rmdir /s /q build
        echo [+] Cleaned
    )
)

:configure
echo.
echo [*] Creating build directory...
mkdir build
cd build

echo [*] Running CMake...
cmake .. -G "Visual Studio 17 2022" -A Win32

if errorlevel 1 (
    echo.
    echo [!] CMake failed!
    echo [!] Make sure you have:
    echo     - CMake installed
    echo     - Visual Studio 2022 installed
    echo.
    echo Trying Visual Studio 2019...
    cmake .. -G "Visual Studio 16 2019" -A Win32

    if errorlevel 1 (
        echo [!] Also failed with VS 2019
        echo [!] Please install Visual Studio 2022 or CMake
        pause
        exit /b 1
    )
)

echo.
echo [+] CMake configuration successful!
echo.

:build
echo [*] Building project...
echo.
cmake --build . --config Release

if errorlevel 1 (
    echo.
    echo [!] Build failed!
    echo [!] Check the error messages above
    pause
    exit /b 1
)

echo.
echo ========================================
echo [+] BUILD SUCCESSFUL!
echo ========================================
echo.
echo Output files:
echo   DLL:    build\bin\SauerbratenESP.dll
echo   Loader: build\bin\SauerbratenLoader.exe
echo.
echo Next steps:
echo   1. Launch Cube 2: Sauerbraten
echo   2. Run as Administrator:
echo      build\bin\SauerbratenLoader.exe sauerbraten.exe SauerbratenESP.dll
echo.

pause
exit /b 0
