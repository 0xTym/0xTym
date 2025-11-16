@echo off
REM Quick injection script for Sauerbraten ESP
REM Must be run as Administrator!

echo ========================================
echo Sauerbraten ESP Injector
echo ========================================
echo.

REM Check if running as admin
net session >nul 2>&1
if %errorLevel% neq 0 (
    echo [!] ERROR: Not running as Administrator
    echo [!] Please right-click and select "Run as Administrator"
    echo.
    pause
    exit /b 1
)

echo [+] Running with Administrator privileges
echo.

REM Check if files exist
if not exist "build\bin\SauerbratenESP.dll" (
    echo [!] ERROR: SauerbratenESP.dll not found!
    echo [!] Run build.bat first to compile the project
    echo.
    pause
    exit /b 1
)

if not exist "build\bin\SauerbratenLoader.exe" (
    echo [!] ERROR: SauerbratenLoader.exe not found!
    echo [!] Run build.bat first to compile the project
    echo.
    pause
    exit /b 1
)

echo [+] All files found
echo.

REM Ask for injection mode
echo Select injection mode:
echo   1. Manual Mapping (Stealth - Recommended)
echo   2. LoadLibrary (Fast - Less Stealth)
echo   3. Wait for Process + Manual Mapping
echo.
choice /C 123 /N /M "Enter choice (1-3): "

set INJECT_MODE=
if errorlevel 3 set INJECT_MODE=--manual --wait
if errorlevel 2 set INJECT_MODE=--loadlib
if errorlevel 1 set INJECT_MODE=--manual

echo.
echo [*] Starting injection...
echo [*] Mode: %INJECT_MODE%
echo.

cd build\bin
SauerbratenLoader.exe sauerbraten.exe SauerbratenESP.dll %INJECT_MODE%

echo.
echo ========================================
echo Script completed
echo ========================================
pause
