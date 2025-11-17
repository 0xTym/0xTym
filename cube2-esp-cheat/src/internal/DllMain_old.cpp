#include <Windows.h>
#include <thread>
#include "../../include/internal/DirectMemory.h"
#include "../../include/internal/AimbotInternal.h"
#include "../../include/internal/D3D11Hook.h"
#include "../../include/Config.h"

// Global instances
DirectMemory* g_Memory = nullptr;
AimbotInternal* g_Aimbot = nullptr;
bool g_Running = true;

// Main cheat thread
DWORD WINAPI MainThread(LPVOID lpParam) {
    // Allocate console for debugging (optional - comment out for production)
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    freopen_s(&f, "CONOUT$", "w", stderr);

    printf("[+] Cube 2: Sauerbraten Internal Cheat v2.0\n");
    printf("[+] DLL Injected Successfully!\n");
    printf("[+] Initializing...\n");

    // Initialize memory
    g_Memory = new DirectMemory();
    if (!g_Memory->Initialize()) {
        printf("[-] Failed to initialize memory!\n");
        return 1;
    }

    printf("[+] Module Base: 0x%p\n", (void*)g_Memory->GetModuleBase());

    // Initialize aimbot
    g_Aimbot = new AimbotInternal(*g_Memory);
    printf("[+] Aimbot initialized\n");

    // Initialize D3D11 Hook
    std::thread hookThread([]() {
        Sleep(2000); // Wait for game to initialize D3D11
        if (D3D11Hook::Initialize()) {
            printf("[+] D3D11 Hook initialized\n");
        } else {
            printf("[-] Failed to initialize D3D11 Hook\n");
        }
    });
    hookThread.detach();

    printf("[+] Initialization complete!\n");
    printf("[+] Press INSERT to toggle menu\n");
    printf("[+] Press END to unload cheat\n\n");

    // Main loop
    while (g_Running) {
        // Check for unload key
        if (GetAsyncKeyState(VK_END) & 0x8000) {
            printf("[!] Unloading cheat...\n");
            g_Running = false;
            break;
        }

        // Update aimbot
        if (g_Aimbot) {
            g_Aimbot->Update();
        }

        Sleep(1); // 1ms sleep to prevent CPU hammering
    }

    // Cleanup
    printf("[+] Cleaning up...\n");

    if (g_Aimbot) {
        delete g_Aimbot;
        g_Aimbot = nullptr;
    }

    if (g_Memory) {
        delete g_Memory;
        g_Memory = nullptr;
    }

    D3D11Hook::Shutdown();

    printf("[+] Cleanup complete. Unloading DLL...\n");
    Sleep(1000);

    // Free console
    fclose(stdout);
    fclose(stderr);
    FreeConsole();

    // Unload DLL
    FreeLibraryAndExitThread(static_cast<HMODULE>(lpParam), 0);
    return 0;
}

// DLL Entry Point
BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {
    switch (dwReason) {
        case DLL_PROCESS_ATTACH:
            // Disable DLL thread notifications
            DisableThreadLibraryCalls(hModule);

            // Create main thread
            CreateThread(nullptr, 0, MainThread, hModule, 0, nullptr);
            break;

        case DLL_PROCESS_DETACH:
            g_Running = false;
            break;
    }

    return TRUE;
}
