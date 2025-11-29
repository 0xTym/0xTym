#pragma once
#include <Windows.h>
#include <string>

namespace Loader {
    // Manual map injection (more stealthy than LoadLibrary)
    bool ManualMapInject(HANDLE hProcess, const std::string& dllPath);

    // Standard LoadLibrary injection (for comparison)
    bool LoadLibraryInject(HANDLE hProcess, const std::string& dllPath);

    // Find process by name
    DWORD FindProcessId(const std::string& processName);

    // Check if process is running with elevated privileges
    bool IsElevated();

    // Clean traces after injection
    void CleanInjectionTraces(HANDLE hProcess, LPVOID allocatedMemory);
}
