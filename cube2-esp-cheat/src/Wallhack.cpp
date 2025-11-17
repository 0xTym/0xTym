#include "../include/Wallhack.h"

Wallhack::Wallhack(MemoryReader& reader)
    : memoryReader(reader), gameBase(reader.GetBaseAddress()) {
    renderSettings.originalRenderDistance = 0.0f;
    renderSettings.modified = false;
}

Wallhack::~Wallhack() {
    Disable();
}

void Wallhack::Enable() {
    using namespace CheatConfig;

    if (!g_Config.wallhack.enabled) {
        return;
    }

    if (g_Config.wallhack.glowEnabled) {
        ApplyGlowEffect();
    }

    if (g_Config.wallhack.chamEnabled) {
        ApplyChamEffect();
    }

    ModifyVisibilityFlags();
}

void Wallhack::Disable() {
    RestoreOriginalSettings();
}

void Wallhack::Update() {
    using namespace CheatConfig;

    if (g_Config.wallhack.enabled) {
        Enable();
    } else {
        if (renderSettings.modified) {
            Disable();
        }
    }
}

void Wallhack::ApplyGlowEffect() {
    using namespace CheatConfig;

    // Read player count
    int32_t playerCount = memoryReader.Read<int32_t>(gameBase + Cube2::PLAYER_COUNT_OFFSET);
    if (playerCount <= 0 || playerCount > 32) {
        return;
    }

    // Read entity list
    uintptr_t entityListPtr = memoryReader.Read<uintptr_t>(gameBase + Cube2::ENTITY_LIST_OFFSET);
    if (entityListPtr == 0) {
        return;
    }

    // Apply glow to all players
    for (int i = 0; i < playerCount; i++) {
        uintptr_t playerPtr = memoryReader.Read<uintptr_t>(entityListPtr + (i * sizeof(uintptr_t)));
        if (playerPtr == 0) {
            continue;
        }

        Cube2::Player player = {};
        if (!memoryReader.Read(playerPtr, player)) {
            continue;
        }

        if (player.health <= 0 || player.health > 100) {
            continue;
        }

        // Note: Actual glow implementation would require hooking into the renderer
        // or modifying specific render flags in the game's memory
        // This is a placeholder showing where glow modifications would go

        // Example: Set a hypothetical "glow flag" (offset would need to be found)
        // uintptr_t glowFlagOffset = 0x0400;  // Example offset
        // memoryReader.Write<uint8_t>(playerPtr + glowFlagOffset, 1);
    }
}

void Wallhack::ApplyChamEffect() {
    using namespace CheatConfig;

    // Chams (Chameleon models) typically involve modifying the rendering pipeline
    // to draw models with a specific color through walls

    // Read player count
    int32_t playerCount = memoryReader.Read<int32_t>(gameBase + Cube2::PLAYER_COUNT_OFFSET);
    if (playerCount <= 0 || playerCount > 32) {
        return;
    }

    uintptr_t entityListPtr = memoryReader.Read<uintptr_t>(gameBase + Cube2::ENTITY_LIST_OFFSET);
    if (entityListPtr == 0) {
        return;
    }

    // Apply chams to all players
    for (int i = 0; i < playerCount; i++) {
        uintptr_t playerPtr = memoryReader.Read<uintptr_t>(entityListPtr + (i * sizeof(uintptr_t)));
        if (playerPtr == 0) {
            continue;
        }

        Cube2::Player player = {};
        if (!memoryReader.Read(playerPtr, player)) {
            continue;
        }

        if (player.health <= 0 || player.health > 100) {
            continue;
        }

        // Note: Actual chams implementation requires renderer hooking
        // This is a placeholder for demonstration

        // Example: Modify render color (hypothetical offset)
        // uintptr_t colorOffset = 0x0420;
        // Choose color based on team
        // if (player.team == 1) {
        //     memoryReader.Write<float>(playerPtr + colorOffset, g_Config.wallhack.teamGlow[0]);
        //     memoryReader.Write<float>(playerPtr + colorOffset + 4, g_Config.wallhack.teamGlow[1]);
        //     memoryReader.Write<float>(playerPtr + colorOffset + 8, g_Config.wallhack.teamGlow[2]);
        // } else {
        //     memoryReader.Write<float>(playerPtr + colorOffset, g_Config.wallhack.enemyGlow[0]);
        //     memoryReader.Write<float>(playerPtr + colorOffset + 4, g_Config.wallhack.enemyGlow[1]);
        //     memoryReader.Write<float>(playerPtr + colorOffset + 8, g_Config.wallhack.enemyGlow[2]);
        // }
    }
}

void Wallhack::ModifyVisibilityFlags() {
    // Common wallhack technique: Modify visibility checks or render distance

    // Example approach 1: Increase render distance
    // uintptr_t renderDistanceOffset = 0x??????;  // Would need to be found
    // if (!renderSettings.modified) {
    //     renderSettings.originalRenderDistance = memoryReader.Read<float>(gameBase + renderDistanceOffset);
    //     renderSettings.modified = true;
    // }
    // memoryReader.Write<float>(gameBase + renderDistanceOffset, 99999.0f);

    // Example approach 2: Disable occlusion culling
    // uintptr_t occlusionCullingOffset = 0x??????;
    // memoryReader.Write<uint8_t>(gameBase + occlusionCullingOffset, 0);

    // Example approach 3: Force all entities visible
    int32_t playerCount = memoryReader.Read<int32_t>(gameBase + Cube2::PLAYER_COUNT_OFFSET);
    if (playerCount <= 0 || playerCount > 32) {
        return;
    }

    uintptr_t entityListPtr = memoryReader.Read<uintptr_t>(gameBase + Cube2::ENTITY_LIST_OFFSET);
    if (entityListPtr == 0) {
        return;
    }

    for (int i = 0; i < playerCount; i++) {
        uintptr_t playerPtr = memoryReader.Read<uintptr_t>(entityListPtr + (i * sizeof(uintptr_t)));
        if (playerPtr == 0) {
            continue;
        }

        // Example: Set visibility flag (offset would need to be found)
        // uintptr_t visibilityFlagOffset = 0x0350;
        // memoryReader.Write<uint8_t>(playerPtr + visibilityFlagOffset, 1);
    }

    renderSettings.modified = true;
}

void Wallhack::RestoreOriginalSettings() {
    if (!renderSettings.modified) {
        return;
    }

    // Restore original render distance if it was modified
    // if (renderSettings.originalRenderDistance != 0.0f) {
    //     uintptr_t renderDistanceOffset = 0x??????;
    //     memoryReader.Write<float>(gameBase + renderDistanceOffset, renderSettings.originalRenderDistance);
    // }

    renderSettings.modified = false;
}
