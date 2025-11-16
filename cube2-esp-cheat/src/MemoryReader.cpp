#include "../include/MemoryReader.h"

MemoryReader::MemoryReader()
    : processHandle(nullptr), processId(0), baseAddress(0) {
}

MemoryReader::~MemoryReader() {
    DetachFromProcess();
}

bool MemoryReader::AttachToProcess(const std::wstring& processName) {
    processId = GetProcessIdByName(processName);
    if (processId == 0) {
        std::wcerr << L"Failed to find process: " << processName << std::endl;
        return false;
    }

    processHandle = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION, FALSE, processId);
    if (processHandle == nullptr) {
        std::wcerr << L"Failed to open process handle. Error code: " << GetLastError() << std::endl;
        return false;
    }

    baseAddress = GetModuleBaseAddress(processId, processName);
    if (baseAddress == 0) {
        std::wcerr << L"Failed to get base address for: " << processName << std::endl;
        CloseHandle(processHandle);
        processHandle = nullptr;
        return false;
    }

    std::wcout << L"Successfully attached to process: " << processName << std::endl;
    std::wcout << L"Process ID: " << processId << std::endl;
    std::wcout << L"Base Address: 0x" << std::hex << baseAddress << std::dec << std::endl;

    return true;
}

void MemoryReader::DetachFromProcess() {
    if (processHandle != nullptr) {
        CloseHandle(processHandle);
        processHandle = nullptr;
        processId = 0;
        baseAddress = 0;
    }
}

bool MemoryReader::ReadBuffer(uintptr_t address, void* buffer, size_t size) {
    SIZE_T bytesRead = 0;
    bool success = ReadProcessMemory(processHandle, (LPCVOID)address, buffer, size, &bytesRead);
    return success && bytesRead == size;
}

bool MemoryReader::WriteBuffer(uintptr_t address, void* buffer, size_t size) {
    SIZE_T bytesWritten = 0;
    bool success = WriteProcessMemory(processHandle, (LPVOID)address, buffer, size, &bytesWritten);
    return success && bytesWritten == size;
}

DWORD MemoryReader::GetProcessIdByName(const std::wstring& processName) {
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

uintptr_t MemoryReader::GetModuleBaseAddress(DWORD processId, const std::wstring& moduleName) {
    uintptr_t moduleBase = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);

    if (snapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    MODULEENTRY32W moduleEntry;
    moduleEntry.dwSize = sizeof(MODULEENTRY32W);

    if (Module32FirstW(snapshot, &moduleEntry)) {
        do {
            if (moduleName == moduleEntry.szModule) {
                moduleBase = (uintptr_t)moduleEntry.modBaseAddr;
                break;
            }
        } while (Module32NextW(snapshot, &moduleEntry));
    }

    CloseHandle(snapshot);
    return moduleBase;
}
