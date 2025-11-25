#pragma once
#include <cstdint>

namespace Cube2 {

    // Memory offsets
    constexpr uintptr_t LOCAL_PLAYER_OFFSET = 0x2A5730;
    constexpr uintptr_t ENTITY_LIST_OFFSET = 0x346C90;
    constexpr uintptr_t PLAYER_COUNT_OFFSET = 0x346C9C;

    // Vector3 structure for 3D positions
    struct Vector3 {
        float x, y, z;

        Vector3() : x(0), y(0), z(0) {}
        Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

        float Distance(const Vector3& other) const {
            float dx = x - other.x;
            float dy = y - other.y;
            float dz = z - other.z;
            return sqrt(dx * dx + dy * dy + dz * dz);
        }
    };

    // Player structure based on Cube 2 memory layout
    struct Player {
        char pad_0x0000[0x4];           // 0x0000
        Vector3 headPosition;            // 0x0004
        char pad_0x0010[0x24];          // 0x0010
        Vector3 position;                // 0x0034
        Vector3 velocity;                // 0x0040
        char pad_0x004C[0x20];          // 0x004C
        float yaw;                       // 0x006C
        float pitch;                     // 0x0070
        char pad_0x0074[0x54];          // 0x0074
        int32_t health;                  // 0x00C8
        int32_t armor;                   // 0x00CC
        char pad_0x00D0[0x1C];          // 0x00D0
        int32_t weaponAmmo[10];          // 0x00EC
        char pad_0x0114[0x30];          // 0x0114
        int32_t currentWeapon;           // 0x0144
        char pad_0x0148[0x70];          // 0x0148
        char name[16];                   // 0x01B8
        char pad_0x01C8[0x174];         // 0x01C8
        int32_t team;                    // 0x033C
        char pad_0x0340[0x20];          // 0x0340
    };

    // Entity list structure
    struct EntityList {
        Player** players;
        int32_t count;
    };

} // namespace Cube2
