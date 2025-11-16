#pragma once
#include <cstdint>

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
