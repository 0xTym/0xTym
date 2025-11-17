#pragma once
#include <cstdint>
#include <Windows.h>
#include "../GameStructures.h"

// Direct memory access for internal cheat
class DirectMemory {
private:
    uintptr_t moduleBase;

public:
    DirectMemory();
    ~DirectMemory();

    bool Initialize();
    uintptr_t GetModuleBase() const { return moduleBase; }

    // Direct memory read (no need for ReadProcessMemory)
    template<typename T>
    T Read(uintptr_t address) {
        if (!address) return T{};
        return *reinterpret_cast<T*>(address);
    }

    // Direct memory write
    template<typename T>
    bool Write(uintptr_t address, T value) {
        if (!address) return false;

        DWORD oldProtect;
        if (!VirtualProtect((LPVOID)address, sizeof(T), PAGE_EXECUTE_READWRITE, &oldProtect))
            return false;

        *reinterpret_cast<T*>(address) = value;

        VirtualProtect((LPVOID)address, sizeof(T), oldProtect, &oldProtect);
        return true;
    }

    // Read structure
    template<typename T>
    bool Read(uintptr_t address, T& buffer) {
        if (!address) return false;
        buffer = *reinterpret_cast<T*>(address);
        return true;
    }

    // Get local player
    Cube2::Player* GetLocalPlayer();

    // Get entity list
    Cube2::Player** GetEntityList();

    // Get player count
    int32_t GetPlayerCount();

private:
    uintptr_t GetModuleBaseAddress();
};
