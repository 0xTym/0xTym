#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include <string>

DWORD GetProcessIdByName(const std::wstring& processName) {
    DWORD pid = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (snapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    PROCESSENTRY32W processEntry;
    processEntry.dwSize = sizeof(PROCESSENTRY32W);

    if (Process32FirstW(snapshot, &processEntry)) {
        do {
            if (processName == processEntry.szExeFile) {
                pid = processEntry.th32ProcessID;
                break;
            }
        } while (Process32NextW(snapshot, &processEntry));
    }

    CloseHandle(snapshot);
    return pid;
}

bool InjectDLL(DWORD processId, const std::wstring& dllPath) {
    // Open target process
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId);
    if (!hProcess) {
        std::wcerr << L"[-] Failed to open process. Error: " << GetLastError() << std::endl;
        return false;
    }

    // Allocate memory in target process
    size_t dllPathSize = (dllPath.length() + 1) * sizeof(wchar_t);
    LPVOID pRemoteMemory = VirtualAllocEx(hProcess, nullptr, dllPathSize,
                                          MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

    if (!pRemoteMemory) {
        std::wcerr << L"[-] Failed to allocate memory. Error: " << GetLastError() << std::endl;
        CloseHandle(hProcess);
        return false;
    }

    // Write DLL path to target process
    if (!WriteProcessMemory(hProcess, pRemoteMemory, dllPath.c_str(), dllPathSize, nullptr)) {
        std::wcerr << L"[-] Failed to write DLL path. Error: " << GetLastError() << std::endl;
        VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    // Get LoadLibraryW address
    LPVOID pLoadLibraryW = (LPVOID)GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "LoadLibraryW");
    if (!pLoadLibraryW) {
        std::wcerr << L"[-] Failed to get LoadLibraryW address. Error: " << GetLastError() << std::endl;
        VirtualFreeEx(hProcess, pRemoteMemory, 0, MEM_RELEASE);
        CloseHandle(hProcess);
        return false;
    }

    // Create remote thread
    HANDLE hThread = CreateRemoteThread(hProcess, nullptr, 0,
                                        (LPTHREAD_START_ROUTINE)pLoadLibraryW,
                                        pRemoteMemory, 0, nullptr);

    if (!hThread) {
        std::wcerr << L"[-] Failed to create remote thread. Error: " << GetLastError() << std::endl;
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

int wmain(int argc, wchar_t* argv[]) {
    std::wcout << L"========================================" << std::endl;
    std::wcout << L" Cube 2: Sauerbraten DLL Injector" << std::endl;
    std::wcout << L" Internal Cheat v2.0" << std::endl;
    std::wcout << L"========================================" << std::endl;
    std::wcout << std::endl;

    // Get DLL path from command line or use default
    std::wstring dllPath;
    if (argc >= 2) {
        dllPath = argv[1];
    } else {
        // Get current directory
        wchar_t currentDir[MAX_PATH];
        GetCurrentDirectoryW(MAX_PATH, currentDir);
        dllPath = std::wstring(currentDir) + L"\\Cube2Internal.dll";
    }

    std::wcout << L"[+] DLL Path: " << dllPath << std::endl;

    // Check if DLL exists
    DWORD fileAttr = GetFileAttributesW(dllPath.c_str());
    if (fileAttr == INVALID_FILE_ATTRIBUTES) {
        std::wcerr << L"[-] DLL not found!" << std::endl;
        std::wcerr << L"[-] Expected: " << dllPath << std::endl;
        std::wcout << std::endl;
        std::wcout << L"Press Enter to exit..." << std::endl;
        std::wcin.get();
        return 1;
    }

    // Find target process
    std::wcout << L"[+] Searching for sauerbraten.exe..." << std::endl;

    DWORD processId = 0;
    while (processId == 0) {
        processId = GetProcessIdByName(L"sauerbraten.exe");
        if (processId == 0) {
            std::wcout << L"[-] Process not found. Waiting..." << std::endl;
            Sleep(2000);
        }
    }

    std::wcout << L"[+] Found process! PID: " << processId << std::endl;

    // Inject DLL
    std::wcout << L"[+] Injecting DLL..." << std::endl;

    if (InjectDLL(processId, dllPath)) {
        std::wcout << L"[+] DLL injected successfully!" << std::endl;
        std::wcout << std::endl;
        std::wcout << L"[+] Cheat loaded. Check game console for status." << std::endl;
        std::wcout << L"[+] Press INSERT in-game to toggle menu" << std::endl;
        std::wcout << L"[+] Press END in-game to unload cheat" << std::endl;
    } else {
        std::wcerr << L"[-] Failed to inject DLL!" << std::endl;
        std::wcerr << L"[-] Make sure you're running as Administrator" << std::endl;
    }

    std::wcout << std::endl;
    std::wcout << L"Press Enter to exit..." << std::endl;
    std::wcin.get();

    return 0;
}
