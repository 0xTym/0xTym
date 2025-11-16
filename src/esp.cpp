#include "esp.h"
#include "memory.h"
#include <cmath>
#include <string>

namespace ESP {
    Config config;
    static uintptr_t gameBase = 0;

    void Initialize() {
        // Get game base address (usually sauerbraten.exe or main executable)
        gameBase = Memory::GetModuleBase(nullptr);
    }

    PlayerEntity* GetLocalPlayer() {
        if (!gameBase) return nullptr;

        uintptr_t localPlayerPtr = Memory::Read<uintptr_t>(gameBase + Offsets::LocalPlayer);
        if (!localPlayerPtr) return nullptr;

        return reinterpret_cast<PlayerEntity*>(localPlayerPtr);
    }

    std::vector<PlayerEntity*> GetPlayers() {
        std::vector<PlayerEntity*> players;

        if (!gameBase) return players;

        // Read entity list and player count
        uintptr_t entityListPtr = Memory::Read<uintptr_t>(gameBase + Offsets::EntityList);
        int32_t playerCount = Memory::Read<int32_t>(gameBase + Offsets::PlayerCount);

        if (!entityListPtr || playerCount <= 0 || playerCount > 32) {
            return players;
        }

        // Read all players
        for (int i = 0; i < playerCount; ++i) {
            uintptr_t playerPtr = Memory::Read<uintptr_t>(entityListPtr + (i * sizeof(uintptr_t)));
            if (!playerPtr) continue;

            PlayerEntity* player = reinterpret_cast<PlayerEntity*>(playerPtr);
            if (player && player->IsValid()) {
                players.push_back(player);
            }
        }

        return players;
    }

    float GetDistance(const Vec3& from, const Vec3& to) {
        float dx = to.x - from.x;
        float dy = to.y - from.y;
        float dz = to.z - from.z;
        return sqrtf(dx * dx + dy * dy + dz * dz);
    }

    bool WorldToScreen(const Vec3& worldPos, Vec3& screenPos, float matrix[16], int viewport[4]) {
        Vec3 trans;
        trans.x = worldPos.x * matrix[0] + worldPos.y * matrix[4] + worldPos.z * matrix[8] + matrix[12];
        trans.y = worldPos.x * matrix[1] + worldPos.y * matrix[5] + worldPos.z * matrix[9] + matrix[13];
        trans.z = worldPos.x * matrix[2] + worldPos.y * matrix[6] + worldPos.z * matrix[10] + matrix[14];

        float w = worldPos.x * matrix[3] + worldPos.y * matrix[7] + worldPos.z * matrix[11] + matrix[15];

        if (w < 0.01f) {
            return false;
        }

        trans.x /= w;
        trans.y /= w;
        trans.z /= w;

        screenPos.x = (viewport[0] + (1.0f + trans.x) * viewport[2] / 2.0f);
        screenPos.y = (viewport[1] + (1.0f - trans.y) * viewport[3] / 2.0f);
        screenPos.z = trans.z;

        return true;
    }

    void DrawLine(const Vec3& from, const Vec3& to, float r, float g, float b) {
        glColor3f(r, g, b);
        glBegin(GL_LINES);
        glVertex2f(from.x, from.y);
        glVertex2f(to.x, to.y);
        glEnd();
    }

    void DrawBox(const Vec3& min, const Vec3& max) {
        glBegin(GL_LINE_LOOP);
        glVertex2f(min.x, min.y);
        glVertex2f(max.x, min.y);
        glVertex2f(max.x, max.y);
        glVertex2f(min.x, max.y);
        glEnd();
    }

    void DrawFilledRect(float x, float y, float w, float h, float r, float g, float b, float a) {
        glColor4f(r, g, b, a);
        glBegin(GL_QUADS);
        glVertex2f(x, y);
        glVertex2f(x + w, y);
        glVertex2f(x + w, y + h);
        glVertex2f(x, y + h);
        glEnd();
    }

    void DrawString(float x, float y, const char* text, float r, float g, float b) {
        glColor3f(r, g, b);
        glRasterPos2f(x, y);

        // Simple text rendering using GLUT bitmap fonts
        // Note: This is basic and may need replacement with better text rendering
        while (*text) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *text);
            text++;
        }
    }

    void Render() {
        PlayerEntity* localPlayer = GetLocalPlayer();
        if (!localPlayer || !localPlayer->IsAlive()) {
            return;
        }

        std::vector<PlayerEntity*> players = GetPlayers();
        if (players.empty()) {
            return;
        }

        // Get matrices and viewport
        GLfloat modelView[16];
        GLfloat projection[16];
        GLint viewport[4];

        glGetFloatv(GL_MODELVIEW_MATRIX, modelView);
        glGetFloatv(GL_PROJECTION_MATRIX, projection);
        glGetIntegerv(GL_VIEWPORT, viewport);

        // Combine matrices
        float matrix[16];
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                matrix[i * 4 + j] = 0.0f;
                for (int k = 0; k < 4; ++k) {
                    matrix[i * 4 + j] += projection[i * 4 + k] * modelView[k * 4 + j];
                }
            }
        }

        // Setup orthographic projection for 2D drawing
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        glOrtho(0, viewport[2], viewport[3], 0, -1, 1);

        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glLineWidth(2.0f);

        // Screen center for snaplines
        Vec3 screenCenter = {static_cast<float>(viewport[2]) / 2.0f,
                            static_cast<float>(viewport[3]), 0.0f};

        // Render ESP for each player
        for (PlayerEntity* player : players) {
            if (!player || player == localPlayer || !player->IsAlive()) {
                continue;
            }

            // Get screen positions
            Vec3 headScreen, footScreen;
            Vec3 headPos = player->headPosition;
            Vec3 footPos = player->position;

            // Adjust head position (typically ~2 units above position)
            headPos.z += 2.0f;

            if (!WorldToScreen(footPos, footScreen, matrix, viewport) ||
                !WorldToScreen(headPos, headScreen, matrix, viewport)) {
                continue;
            }

            // Calculate box dimensions
            float height = footScreen.y - headScreen.y;
            float width = height / 2.5f;

            Vec3 boxMin = {headScreen.x - width / 2.0f, headScreen.y, 0.0f};
            Vec3 boxMax = {headScreen.x + width / 2.0f, footScreen.y, 0.0f};

            // Determine color based on team
            float* color = (player->team == localPlayer->team) ? config.teamColor : config.boxColor;

            // Draw snapline
            if (config.showSnaplines) {
                DrawLine(screenCenter, footScreen,
                        config.snaplineColor[0],
                        config.snaplineColor[1],
                        config.snaplineColor[2]);
            }

            // Draw box
            if (config.showBox) {
                glColor3f(color[0], color[1], color[2]);
                DrawBox(boxMin, boxMax);
            }

            // Draw health bar
            if (config.showHealth && player->health > 0) {
                float healthPercent = player->health / 100.0f;
                float barWidth = width;
                float barHeight = 4.0f;
                float barX = boxMin.x;
                float barY = boxMax.y + 2.0f;

                // Background
                DrawFilledRect(barX, barY, barWidth, barHeight, 0.2f, 0.2f, 0.2f, 0.8f);

                // Health bar (green to red based on health)
                float r = 1.0f - healthPercent;
                float g = healthPercent;
                DrawFilledRect(barX, barY, barWidth * healthPercent, barHeight, r, g, 0.0f, 0.9f);
            }

            // Draw name
            if (config.showName && player->name[0] != '\0') {
                float nameX = headScreen.x - width / 2.0f;
                float nameY = headScreen.y - 15.0f;

                char nameBuffer[32];
                snprintf(nameBuffer, sizeof(nameBuffer), "%s", player->name);

                // Note: DrawString needs GLUT or alternative text rendering
                // For production, use a proper font rendering library
                glColor3f(config.nameColor[0], config.nameColor[1], config.nameColor[2]);
                glRasterPos2f(nameX, nameY);
            }

            // Draw distance
            if (config.showDistance) {
                float distance = GetDistance(localPlayer->position, player->position);
                char distBuffer[32];
                snprintf(distBuffer, sizeof(distBuffer), "%.1fm", distance);

                float distX = headScreen.x - width / 2.0f;
                float distY = boxMax.y + 20.0f;

                glColor3f(1.0f, 1.0f, 1.0f);
                glRasterPos2f(distX, distY);
            }
        }

        // Restore state
        glEnable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);
        glLineWidth(1.0f);

        glPopMatrix();
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glMatrixMode(GL_MODELVIEW);
    }
}
