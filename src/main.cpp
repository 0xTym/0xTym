#include <Windows.h>
#include <thread>
#include <iostream>
#include "hooks.h"
#include "esp.h"

// Console output for debugging
void InitializeConsole() {
    AllocConsole();
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONOUT$", "w", stderr);
    freopen_s(&fp, "CONIN$", "r", stdin);

    SetConsoleTitleA("Sauerbraten ESP - AntiCheat Testing");

    std::cout << "=================================\n";
    std::cout << "Sauerbraten ESP Internal Cheat\n";
    std::cout << "=================================\n";
    std::cout << "For AntiCheat Testing Purposes\n";
    std::cout << "=================================\n\n";
}

// Main thread
DWORD WINAPI MainThread(LPVOID lparam) {
    // Initialize console for debugging
    InitializeConsole();

    std::cout << "[+] Initializing ESP...\n";

    // Wait for game to load
    Sleep(2000);

    // Initialize ESP
    ESP::Initialize();
    std::cout << "[+] ESP initialized\n";

    // Install hooks
    if (Hooks::Initialize()) {
        std::cout << "[+] Hooks installed successfully\n";
    } else {
        std::cout << "[-] Failed to install hooks\n";
        return 1;
    }

    std::cout << "\n[+] ESP is now active!\n";
    std::cout << "\nControls:\n";
    std::cout << "  INSERT - Toggle ESP\n";
    std::cout << "  DELETE - Unload cheat\n";
    std::cout << "  HOME   - Toggle boxes\n";
    std::cout << "  END    - Toggle snaplines\n";
    std::cout << "\nConfiguration:\n";
    std::cout << "  ESP Enabled: " << (ESP::config.enabled ? "Yes" : "No") << "\n";
    std::cout << "  Show Boxes: " << (ESP::config.showBox ? "Yes" : "No") << "\n";
    std::cout << "  Show Names: " << (ESP::config.showName ? "Yes" : "No") << "\n";
    std::cout << "  Show Health: " << (ESP::config.showHealth ? "Yes" : "No") << "\n";
    std::cout << "  Show Distance: " << (ESP::config.showDistance ? "Yes" : "No") << "\n";
    std::cout << "  Show Snaplines: " << (ESP::config.showSnaplines ? "Yes" : "No") << "\n";

    // Main loop for input handling
    bool running = true;
    while (running) {
        // Toggle ESP
        if (GetAsyncKeyState(VK_INSERT) & 1) {
            ESP::config.enabled = !ESP::config.enabled;
            std::cout << "[*] ESP " << (ESP::config.enabled ? "Enabled" : "Disabled") << "\n";
        }

        // Toggle boxes
        if (GetAsyncKeyState(VK_HOME) & 1) {
            ESP::config.showBox = !ESP::config.showBox;
            std::cout << "[*] Boxes " << (ESP::config.showBox ? "Enabled" : "Disabled") << "\n";
        }

        // Toggle snaplines
        if (GetAsyncKeyState(VK_END) & 1) {
            ESP::config.showSnaplines = !ESP::config.showSnaplines;
            std::cout << "[*] Snaplines " << (ESP::config.showSnaplines ? "Enabled" : "Disabled") << "\n";
        }

        // Unload
        if (GetAsyncKeyState(VK_DELETE) & 1) {
            std::cout << "\n[*] Unloading...\n";
            running = false;
        }

        Sleep(100);
    }

    // Cleanup
    std::cout << "[*] Removing hooks...\n";
    Hooks::Shutdown();

    std::cout << "[+] Cheat unloaded successfully\n";
    Sleep(1000);

    FreeConsole();
    FreeLibraryAndExitThread(static_cast<HMODULE>(lparam), 0);
    return 0;
}

// DLL Entry Point
BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {
    switch (dwReason) {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(hModule);
            CreateThread(nullptr, 0, MainThread, hModule, 0, nullptr);
            break;

        case DLL_PROCESS_DETACH:
            Hooks::Shutdown();
            break;
    }
    return TRUE;
}
