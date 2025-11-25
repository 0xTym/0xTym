#include "../../include/internal/DirectMemory.h"

DirectMemory::DirectMemory() : moduleBase(0) {
}

DirectMemory::~DirectMemory() {
}

bool DirectMemory::Initialize() {
    moduleBase = GetModuleBaseAddress();
    return moduleBase != 0;
}

uintptr_t DirectMemory::GetModuleBaseAddress() {
    // Get the base address of the current module (sauerbraten.exe)
    return reinterpret_cast<uintptr_t>(GetModuleHandleW(nullptr));
}

Cube2::Player* DirectMemory::GetLocalPlayer() {
    uintptr_t localPlayerPtr = Read<uintptr_t>(moduleBase + Cube2::LOCAL_PLAYER_OFFSET);
    if (!localPlayerPtr) return nullptr;
    return reinterpret_cast<Cube2::Player*>(localPlayerPtr);
}

Cube2::Player** DirectMemory::GetEntityList() {
    uintptr_t entityListPtr = Read<uintptr_t>(moduleBase + Cube2::ENTITY_LIST_OFFSET);
    if (!entityListPtr) return nullptr;
    return reinterpret_cast<Cube2::Player**>(entityListPtr);
}

int32_t DirectMemory::GetPlayerCount() {
    return Read<int32_t>(moduleBase + Cube2::PLAYER_COUNT_OFFSET);
}
