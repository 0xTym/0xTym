#pragma once
#include <cstdint>

// Vector3 structure for position
struct Vec3 {
    float x, y, z;
};

// Player entity structure for Cube 2: Sauerbraten
// This structure may need adjustment based on game version
struct PlayerEntity {
    char pad_0000[4];           // 0x0000
    Vec3 headPosition;          // 0x0004
    char pad_0010[36];          // 0x0010
    Vec3 position;              // 0x0034
    char pad_0040[12];          // 0x0040
    Vec3 velocity;              // 0x004C
    char pad_0058[68];          // 0x0058
    int32_t health;             // 0x009C
    int32_t armor;              // 0x00A0
    char pad_00A4[12];          // 0x00A4
    int32_t ammo;               // 0x00B0
    char pad_00B4[180];         // 0x00B4
    char name[16];              // 0x0168
    char pad_0178[248];         // 0x0178
    int32_t team;               // 0x0270
    char pad_0274[140];         // 0x0274

    bool IsValid() const {
        return health > 0 && health <= 100;
    }

    bool IsAlive() const {
        return health > 0;
    }
};

// Game offsets
namespace Offsets {
    constexpr uintptr_t LocalPlayer = 0x2A5730;
    constexpr uintptr_t EntityList = 0x346C90;
    constexpr uintptr_t PlayerCount = 0x346C9C;
}
