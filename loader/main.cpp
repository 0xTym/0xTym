#include "loader.h"
#include <iostream>
#include <string>
#include <Windows.h>

void PrintBanner() {
    std::cout << R"(
 _____ _____ _____   _                    _
|  ___|  ___| ___ \ | |                  | |
| |__ | |__ | |_/ / | |     ___  __ _  __| | ___ _ __
|  __||  __||  __/  | |    / _ \/ _` |/ _` |/ _ \ '__|
| |___| |___| |     | |___| (_) | (_| | (_| |  __/ |
\____/\____/\_|     \_____/\___/ \__,_|\__,_|\___|_|

    Sauerbraten ESP Stealth Loader v1.0
    For Anti-Cheat Testing Only
    )" << "\n\n";
}

void PrintUsage(const char* programName) {
    std::cout << "Usage:\n";
    std::cout << "  " << programName << " <process_name> <dll_path> [options]\n\n";
    std::cout << "Options:\n";
    std::cout << "  -m, --manual      Use manual mapping (stealthier, default)\n";
    std::cout << "  -l, --loadlib     Use LoadLibrary method (faster, less stealth)\n";
    std::cout << "  -w, --wait        Wait for process if not found\n";
    std::cout << "  -h, --help        Show this help\n\n";
    std::cout << "Examples:\n";
    std::cout << "  " << programName << " sauerbraten.exe SauerbratenESP.dll\n";
    std::cout << "  " << programName << " sauerbraten.exe SauerbratenESP.dll --manual\n";
    std::cout << "  " << programName << " sauerbraten.exe SauerbratenESP.dll --wait\n\n";
}

int main(int argc, char* argv[]) {
    PrintBanner();

    // Parse arguments
    if (argc < 3) {
        PrintUsage(argv[0]);
        return 1;
    }

    std::string processName = argv[1];
    std::string dllPath = argv[2];

    bool useManualMap = true;  // Default to manual mapping (stealthier)
    bool waitForProcess = false;

    // Parse optional arguments
    for (int i = 3; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-l" || arg == "--loadlib") {
            useManualMap = false;
        } else if (arg == "-m" || arg == "--manual") {
            useManualMap = true;
        } else if (arg == "-w" || arg == "--wait") {
            waitForProcess = true;
        } else if (arg == "-h" || arg == "--help") {
            PrintUsage(argv[0]);
            return 0;
        }
    }

    // Check for admin rights
    if (!Loader::IsElevated()) {
        std::cout << "[!] WARNING: Not running as Administrator\n";
        std::cout << "[!] Injection may fail without elevated privileges\n";
        std::cout << "[!] Please run as Administrator\n\n";
        std::cout << "Continue anyway? (y/n): ";

        char choice;
        std::cin >> choice;
        if (choice != 'y' && choice != 'Y') {
            return 1;
        }
    }

    // Convert DLL path to absolute
    char fullPath[MAX_PATH];
    if (GetFullPathNameA(dllPath.c_str(), MAX_PATH, fullPath, NULL)) {
        dllPath = fullPath;
    }

    std::cout << "[*] Target Process: " << processName << "\n";
    std::cout << "[*] DLL Path: " << dllPath << "\n";
    std::cout << "[*] Injection Mode: " << (useManualMap ? "Manual Mapping (Stealth)" : "LoadLibrary (Standard)") << "\n\n";

    // Check if DLL exists
    if (GetFileAttributesA(dllPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
        std::cerr << "[!] DLL file not found: " << dllPath << "\n";
        std::cerr << "[!] Make sure the path is correct\n";
        return 1;
    }

    std::cout << "[+] DLL file verified\n";

    // Find process
    DWORD processId = 0;

    if (waitForProcess) {
        std::cout << "[*] Waiting for process to start...\n";
        while (processId == 0) {
            processId = Loader::FindProcessId(processName);
            if (processId == 0) {
                Sleep(1000);
                std::cout << ".";
            }
        }
        std::cout << "\n";
    } else {
        std::cout << "[*] Searching for process...\n";
        processId = Loader::FindProcessId(processName);
    }

    if (processId == 0) {
        std::cerr << "[!] Process not found: " << processName << "\n";
        std::cerr << "[!] Make sure the game is running\n";
        std::cerr << "[!] Or use --wait option to wait for the process\n";
        return 1;
    }

    std::cout << "[+] Process found! PID: " << processId << "\n";

    // Open process
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
    if (!hProcess) {
        std::cerr << "[!] Failed to open process\n";
        std::cerr << "[!] Error code: " << GetLastError() << "\n";
        std::cerr << "[!] Make sure you have sufficient privileges\n";
        return 1;
    }

    std::cout << "[+] Process opened successfully\n\n";

    // Inject DLL
    bool success = false;

    std::cout << "========================================\n";
    std::cout << "Starting injection...\n";
    std::cout << "========================================\n\n";

    if (useManualMap) {
        success = Loader::ManualMapInject(hProcess, dllPath);
    } else {
        success = Loader::LoadLibraryInject(hProcess, dllPath);
    }

    std::cout << "\n========================================\n";

    if (success) {
        std::cout << "[+] SUCCESS! DLL injected\n";
        std::cout << "========================================\n\n";
        std::cout << "[+] Check the game window for the ESP console\n";
        std::cout << "[+] Press INSERT to toggle ESP\n";
        std::cout << "[+] Press DELETE to unload\n\n";

        CloseHandle(hProcess);

        std::cout << "[*] Press any key to exit loader...\n";
        std::cin.ignore();
        std::cin.get();

        return 0;
    } else {
        std::cout << "[!] FAILED! Injection unsuccessful\n";
        std::cout << "========================================\n\n";
        std::cout << "Troubleshooting:\n";
        std::cout << "- Make sure you're running as Administrator\n";
        std::cout << "- Check if the DLL is built for correct architecture (32-bit)\n";
        std::cout << "- Verify the process is not protected by anti-cheat\n";
        std::cout << "- Try the other injection method (--loadlib or --manual)\n\n";

        CloseHandle(hProcess);

        std::cout << "[*] Press any key to exit...\n";
        std::cin.ignore();
        std::cin.get();

        return 1;
    }
}
