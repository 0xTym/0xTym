# Memory Offsets Guide

This document contains memory offsets for different versions of Cube 2: Sauerbraten.

## How to Find Offsets

If the provided offsets don't work for your game version, you'll need to find them yourself using a memory scanner like Cheat Engine.

### Finding Local Player Offset

1. Open Cheat Engine and attach to `sauerbraten.exe`
2. Start a game and note your health value
3. Search for your health value (usually 100)
4. Take damage and search for the new value
5. Repeat until you find the address
6. Use pointer scan to find a static offset from the base address
7. The offset from the base is your `localPlayerOffset`

### Finding Entity List Offset

1. Join a server with other players
2. Note the number of players (e.g., 4 players)
3. Search for the value 4 as a 4-byte integer
4. Have someone join or leave
5. Search for the new player count
6. Repeat to narrow down the address
7. The nearby memory should contain pointers to all player entities

### Finding Player Count Offset

This is usually very close to the entity list offset (often just +12 bytes).

## Known Offsets

### Sauerbraten 2020 Edition (Default)

```cpp
uintptr_t localPlayerOffset = 0x2A5730;
uintptr_t entityListOffset = 0x346C90;
uintptr_t playerCountOffset = 0x346C9C;
```

**Status**: ✅ Working
**Date Verified**: 2024-01-15
**Game Version**: 2020_12_21

### Sauerbraten 2013 Edition

```cpp
uintptr_t localPlayerOffset = 0x29A120;
uintptr_t entityListOffset = 0x33B480;
uintptr_t playerCountOffset = 0x33B48C;
```

**Status**: ⚠️ Untested
**Date Verified**: N/A
**Game Version**: 2013_02_03

## Player Entity Structure

The `PlayerEntity` structure may also vary between versions. Common fields:

```cpp
struct PlayerEntity {
    char pad_0000[4];           // 0x0000
    Vec3 headPosition;          // 0x0004 - Head position (X, Y, Z)
    char pad_0010[36];          // 0x0010
    Vec3 position;              // 0x0034 - Feet/body position
    char pad_0040[12];          // 0x0040
    Vec3 velocity;              // 0x004C - Movement velocity
    char pad_0058[68];          // 0x0058
    int32_t health;             // 0x009C - Current health (0-100)
    int32_t armor;              // 0x00A0 - Current armor (0-100)
    char pad_00A4[12];          // 0x00A4
    int32_t ammo;               // 0x00B0 - Current ammunition
    char pad_00B4[180];         // 0x00B4
    char name[16];              // 0x0168 - Player name (null-terminated)
    char pad_0178[248];         // 0x0178
    int32_t team;               // 0x0270 - Team number
    char pad_0274[140];         // 0x0274
};
```

### How to Verify Structure

1. Find a player entity in memory
2. Compare the health value location
3. Check if the name appears at the expected offset
4. Adjust padding if necessary

## Updating Offsets

To update offsets in the code:

1. Open `src/entities.h`
2. Modify the values in the `Offsets` namespace:

```cpp
namespace Offsets {
    constexpr uintptr_t LocalPlayer = 0xYOUR_OFFSET;
    constexpr uintptr_t EntityList = 0xYOUR_OFFSET;
    constexpr uintptr_t PlayerCount = 0xYOUR_OFFSET;
}
```

3. Rebuild the project

## ASLR (Address Space Layout Randomization)

Note: These offsets are relative to the base address of the main executable. The code automatically handles this by:

```cpp
uintptr_t gameBase = Memory::GetModuleBase(nullptr); // Gets base of main module
uintptr_t actualAddress = gameBase + offset;
```

## Version Detection Tips

- Check the game's "About" menu for version info
- Look at the executable's file properties
- Check the game's installation folder for version files
- The window title may contain version information

## Troubleshooting

### ESP Not Showing

- Offsets are incorrect for your game version
- Player entity structure has changed
- ASLR is changing base addresses (code should handle this)

### Game Crashes

- Player entity structure is wrong
- Reading invalid memory
- Hook conflicts with other mods

### False Positives

- Other entities being detected as players
- Invalid pointer chains
- Stale player data in memory

## Contributing

If you find working offsets for a different game version, please contribute:

1. Fork the repository
2. Update this file with the new offsets
3. Test thoroughly
4. Submit a pull request

Include:
- Game version number
- Date verified
- Testing notes
- Any structure changes

---

**Last Updated**: 2024-01-15
**Maintainer**: 0xTym
