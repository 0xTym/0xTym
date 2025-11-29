#pragma once
#include "entities.h"
#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <vector>

namespace ESP {
    // Configuration
    struct Config {
        bool enabled = true;
        bool showBox = true;
        bool showName = true;
        bool showHealth = true;
        bool showDistance = true;
        bool showSnaplines = true;

        float boxColor[3] = {1.0f, 0.0f, 0.0f};      // Red
        float teamColor[3] = {0.0f, 1.0f, 0.0f};     // Green
        float nameColor[3] = {1.0f, 1.0f, 1.0f};     // White
        float snaplineColor[3] = {1.0f, 1.0f, 0.0f}; // Yellow
    };

    extern Config config;

    // Initialize ESP
    void Initialize();

    // Main render function
    void Render();

    // Helper functions
    bool WorldToScreen(const Vec3& worldPos, Vec3& screenPos, float matrix[16], int viewport[4]);
    void DrawBox(const Vec3& min, const Vec3& max);
    void DrawLine(const Vec3& from, const Vec3& to, float r, float g, float b);
    void DrawString(float x, float y, const char* text, float r, float g, float b);
    void DrawFilledRect(float x, float y, float w, float h, float r, float g, float b, float a);

    // Get all players
    std::vector<PlayerEntity*> GetPlayers();
    PlayerEntity* GetLocalPlayer();

    // Calculate distance
    float GetDistance(const Vec3& from, const Vec3& to);
}
