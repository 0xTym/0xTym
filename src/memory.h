#pragma once
#include <Windows.h>
#include <cstdint>

namespace Memory {
    // Get module base address
    inline uintptr_t GetModuleBase(const char* moduleName) {
        return reinterpret_cast<uintptr_t>(GetModuleHandleA(moduleName));
    }

    // Read memory at address
    template<typename T>
    T Read(uintptr_t address) {
        if (!address) return T{};
        return *reinterpret_cast<T*>(address);
    }

    // Write memory at address
    template<typename T>
    void Write(uintptr_t address, T value) {
        if (!address) return;
        *reinterpret_cast<T*>(address) = value;
    }

    // Read pointer chain
    inline uintptr_t ReadPointer(uintptr_t base, const std::vector<uintptr_t>& offsets) {
        uintptr_t address = base;
        for (size_t i = 0; i < offsets.size(); ++i) {
            address = Read<uintptr_t>(address);
            if (!address) return 0;
            if (i < offsets.size() - 1) {
                address += offsets[i];
            }
        }
        return address;
    }
}
