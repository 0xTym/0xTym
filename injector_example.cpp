/*
 * Simple DLL Injector Example
 * For injecting SauerbratenESP.dll into Cube 2: Sauerbraten
 *
 * Compile: g++ injector_example.cpp -o injector.exe
 * Usage: injector.exe <process_name> <dll_path>
 * Example: injector.exe sauerbraten.exe SauerbratenESP.dll
 */

#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <string>

// Find process ID by name
DWORD GetProcessIdByName(const std::string& processName) {
    DWORD processId = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (snapshot != INVALID_HANDLE_VALUE) {
        PROCESSENTRY32 processEntry;
        processEntry.dwSize = sizeof(PROCESSENTRY32);

        if (Process32First(snapshot, &processEntry)) {
            do {
                if (processName == processEntry.szExeFile) {
                    processId = processEntry.th32ProcessID;
                    break;
                }
            } while (Process32Next(snapshot, &processEntry));
        }

        CloseHandle(snapshot);
    }

    return processId;
}

// Inject DLL into process
bool InjectDLL(DWORD processId, const std::string& dllPath) {
    // Open target process
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
    if (!hProcess) {
        std::cerr << "Failed to open process. Error: " << GetLastError() << std::endl;
        return false;
    }

    // Allocate memory in target process for DLL path
    LPVOID pRemoteMemory = VirtualAllocEx(hProcess, nullptr, dllPath.length() + 1,
                                          MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!pRemoteMemory) {
        std::cerr << "Failed to allocate memory. Error: " << GetLastError() << std::endl;
        CloseHandle(hProcess);
        return false;
    }

    // Write DLL path to allocated memory
    if (!WriteProcessMemory(hProcess, pRemoteMemory, dllPath.c_str(),
                           dllPath.length() + 1, nullptr)) {
        std::cerr << "Failed to write memory. Error: " << GetLastError() << std::endl;
        VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    // Get address of LoadLibraryA
    LPVOID pLoadLibrary = GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
    if (!pLoadLibrary) {
        std::cerr << "Failed to get LoadLibraryA address." << std::endl;
        VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    // Create remote thread to load DLL
    HANDLE hThread = CreateRemoteThread(hProcess, nullptr, 0,
                                       (LPTHREAD_START_ROUTINE)pLoadLibrary,
                                       pRemoteMemory, 0, nullptr);
    if (!hThread) {
        std::cerr << "Failed to create remote thread. Error: " << GetLastError() << std::endl;
        VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    // Wait for thread to finish
    WaitForSingleObject(hThread, INFINITE);

    // Cleanup
    VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
    CloseHandle(hThread);
    CloseHandle(hProcess);

    return true;
}

int main(int argc, char* argv[]) {
    std::cout << "========================================\n";
    std::cout << "  Sauerbraten ESP DLL Injector\n";
    std::cout << "========================================\n\n";

    if (argc < 3) {
        std::cout << "Usage: " << argv[0] << " <process_name> <dll_path>\n";
        std::cout << "Example: " << argv[0] << " sauerbraten.exe SauerbratenESP.dll\n";
        return 1;
    }

    std::string processName = argv[1];
    std::string dllPath = argv[2];

    // Convert DLL path to absolute path
    char fullPath[MAX_PATH];
    if (GetFullPathNameA(dllPath.c_str(), MAX_PATH, fullPath, nullptr)) {
        dllPath = fullPath;
    }

    std::cout << "[*] Target Process: " << processName << "\n";
    std::cout << "[*] DLL Path: " << dllPath << "\n\n";

    // Check if DLL exists
    if (GetFileAttributesA(dllPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
        std::cerr << "[!] DLL file not found: " << dllPath << "\n";
        return 1;
    }

    // Find process
    std::cout << "[*] Searching for process...\n";
    DWORD processId = GetProcessIdByName(processName);

    if (processId == 0) {
        std::cerr << "[!] Process not found: " << processName << "\n";
        std::cerr << "[!] Make sure the game is running.\n";
        return 1;
    }

    std::cout << "[+] Process found! PID: " << processId << "\n";

    // Inject DLL
    std::cout << "[*] Injecting DLL...\n";
    if (InjectDLL(processId, dllPath)) {
        std::cout << "[+] DLL injected successfully!\n";
        std::cout << "[+] Check the game for the console window.\n";
        std::cout << "\n[*] Press any key to exit...\n";
        std::cin.get();
        return 0;
    } else {
        std::cerr << "[!] DLL injection failed!\n";
        std::cerr << "[!] Make sure you have sufficient privileges (Run as Administrator).\n";
        std::cout << "\n[*] Press any key to exit...\n";
        std::cin.get();
        return 1;
    }
}
