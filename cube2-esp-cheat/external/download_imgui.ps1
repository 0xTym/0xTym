# ImGui Automatic Download Script
# For Cube 2 ESP Cheat Project

Write-Host "========================================" -ForegroundColor Cyan
Write-Host " ImGui Automatic Download Script" -ForegroundColor Cyan
Write-Host " For Cube 2 ESP Cheat Project" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Change to script directory
Set-Location $PSScriptRoot

# Check if imgui already exists
if (Test-Path "imgui") {
    Write-Host "ImGui already exists in external/imgui/" -ForegroundColor Yellow
    Write-Host "If you want to reinstall, delete the imgui folder first." -ForegroundColor Yellow
    Write-Host ""
    Read-Host "Press Enter to exit"
    exit 0
}

try {
    # Download ImGui
    Write-Host "[1/5] Downloading ImGui v1.90.0..." -ForegroundColor Green
    $url = "https://github.com/ocornut/imgui/archive/refs/tags/v1.90.0.zip"
    [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12
    Invoke-WebRequest -Uri $url -OutFile "imgui.zip" -UseBasicParsing

    # Extract
    Write-Host "[2/5] Extracting ImGui..." -ForegroundColor Green
    Expand-Archive -Path "imgui.zip" -DestinationPath "." -Force

    # Rename
    Write-Host "[3/5] Renaming folder..." -ForegroundColor Green
    if (Test-Path "imgui-1.90.0") {
        Rename-Item -Path "imgui-1.90.0" -NewName "imgui"
    }

    # Copy backend files
    Write-Host "[4/5] Copying backend files..." -ForegroundColor Green
    Copy-Item "imgui\backends\imgui_impl_dx11.cpp" "imgui\" -Force
    Copy-Item "imgui\backends\imgui_impl_dx11.h" "imgui\" -Force
    Copy-Item "imgui\backends\imgui_impl_win32.cpp" "imgui\" -Force
    Copy-Item "imgui\backends\imgui_impl_win32.h" "imgui\" -Force

    # Cleanup
    Write-Host "[5/5] Cleaning up..." -ForegroundColor Green
    Remove-Item "imgui.zip" -ErrorAction SilentlyContinue

    Write-Host ""
    Write-Host "========================================" -ForegroundColor Green
    Write-Host " ImGui successfully installed!" -ForegroundColor Green
    Write-Host "========================================" -ForegroundColor Green
    Write-Host ""
    Write-Host "You can now build the project with CMake." -ForegroundColor Cyan
    Write-Host ""

} catch {
    Write-Host ""
    Write-Host "ERROR: Failed to download/install ImGui!" -ForegroundColor Red
    Write-Host $_.Exception.Message -ForegroundColor Red
    Write-Host ""
    Write-Host "Please download manually from:" -ForegroundColor Yellow
    Write-Host "https://github.com/ocornut/imgui/releases" -ForegroundColor Yellow
    Write-Host ""
    Read-Host "Press Enter to exit"
    exit 1
}

Read-Host "Press Enter to exit"
