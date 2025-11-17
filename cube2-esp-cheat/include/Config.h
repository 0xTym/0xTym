#pragma once
#include <cstdint>

// Virtual Key Codes
#ifndef VK_LBUTTON
#define VK_LBUTTON        0x01
#define VK_RBUTTON        0x02
#define VK_MBUTTON        0x04
#define VK_XBUTTON1       0x05
#define VK_XBUTTON2       0x06
#define VK_SHIFT          0x10
#define VK_CONTROL        0x11
#define VK_MENU           0x12
#define VK_INSERT         0x2D
#define VK_DELETE         0x2E
#define VK_HOME           0x24
#define VK_END            0x23
#define VK_PRIOR          0x21
#define VK_NEXT           0x22
#endif

namespace CheatConfig {

    // ESP Settings
    struct ESPSettings {
        bool enabled = true;
        bool showBoxes = true;
        bool showNames = true;
        bool showHealth = true;
        bool showDistance = true;
        bool showSkeleton = false;
        float maxDistance = 500.0f;

        // Colors (RGB 0-255)
        float enemyColor[3] = {1.0f, 0.0f, 0.0f};      // Red
        float teamColor[3] = {0.0f, 1.0f, 0.0f};       // Green
        float boxThickness = 2.0f;
    };

    // Aimbot Settings
    struct AimbotSettings {
        bool enabled = false;
        bool smoothing = true;
        float smoothness = 5.0f;        // Higher = smoother (1-20)
        float fov = 90.0f;              // Field of view for aim assist
        bool targetHead = true;
        bool targetChest = false;
        bool visibleOnly = true;
        bool teamCheck = true;          // Don't aim at teammates
        int aimKey = VK_RBUTTON;        // Right mouse button
        float maxDistance = 300.0f;
    };

    // Wallhack Settings
    struct WallhackSettings {
        bool enabled = false;
        bool glowEnabled = false;
        bool chamEnabled = false;
        float glowBrightness = 1.5f;
        float enemyGlow[3] = {1.0f, 0.0f, 0.0f};       // Red
        float teamGlow[3] = {0.0f, 0.0f, 1.0f};        // Blue
    };

    // Misc Settings
    struct MiscSettings {
        bool noRecoil = false;
        bool noSpread = false;
        bool infiniteAmmo = false;
        bool rapidFire = false;
        bool speedHack = false;
        float speedMultiplier = 1.5f;
    };

    // Menu Settings
    struct MenuSettings {
        bool visible = true;
        int menuKey = VK_INSERT;
        float opacity = 0.95f;
        bool showFPS = true;
    };

    // Global config
    struct Config {
        ESPSettings esp;
        AimbotSettings aimbot;
        WallhackSettings wallhack;
        MiscSettings misc;
        MenuSettings menu;

        // Runtime state
        bool gameRunning = false;
        float deltaTime = 0.016f;
        int fps = 60;
    };

    // Global instance
    inline Config g_Config;

} // namespace CheatConfig
