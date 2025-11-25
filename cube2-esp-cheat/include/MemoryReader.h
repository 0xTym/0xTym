#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <string>
#include <iostream>

class MemoryReader {
private:
    HANDLE processHandle;
    DWORD processId;
    uintptr_t baseAddress;

public:
    MemoryReader();
    ~MemoryReader();

    bool AttachToProcess(const std::wstring& processName);
    void DetachFromProcess();

    template<typename T>
    T Read(uintptr_t address);

    template<typename T>
    bool Read(uintptr_t address, T& buffer);

    bool ReadBuffer(uintptr_t address, void* buffer, size_t size);

    template<typename T>
    bool Write(uintptr_t address, T value);

    bool WriteBuffer(uintptr_t address, void* buffer, size_t size);

    uintptr_t GetBaseAddress() const { return baseAddress; }
    bool IsAttached() const { return processHandle != nullptr; }
    DWORD GetProcessId() const { return processId; }

private:
    DWORD GetProcessIdByName(const std::wstring& processName);
    uintptr_t GetModuleBaseAddress(DWORD processId, const std::wstring& moduleName);
};

// Template implementations
template<typename T>
T MemoryReader::Read(uintptr_t address) {
    T buffer;
    ReadProcessMemory(processHandle, (LPCVOID)address, &buffer, sizeof(T), nullptr);
    return buffer;
}

template<typename T>
bool MemoryReader::Read(uintptr_t address, T& buffer) {
    SIZE_T bytesRead = 0;
    bool success = ReadProcessMemory(processHandle, (LPCVOID)address, &buffer, sizeof(T), &bytesRead);
    return success && bytesRead == sizeof(T);
}

template<typename T>
bool MemoryReader::Write(uintptr_t address, T value) {
    SIZE_T bytesWritten = 0;
    bool success = WriteProcessMemory(processHandle, (LPVOID)address, &value, sizeof(T), &bytesWritten);
    return success && bytesWritten == sizeof(T);
}
