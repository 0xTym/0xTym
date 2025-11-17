#include "../../include/internal/AimbotInternal.h"
#include <Windows.h>
#include <algorithm>
#include <cmath>

#define PI 3.14159265358979323846f
#define DEG2RAD(x) ((x) * PI / 180.0f)
#define RAD2DEG(x) ((x) * 180.0f / PI)

AimbotInternal::AimbotInternal(DirectMemory& mem)
    : memory(mem) {
}

AimbotInternal::~AimbotInternal() {
}

void AimbotInternal::Update() {
    using namespace CheatConfig;

    if (!g_Config.aimbot.enabled) {
        return;
    }

    // Check if aim key is pressed
    if (!IsKeyPressed(g_Config.aimbot.aimKey)) {
        return;
    }

    // Get local player
    Cube2::Player* localPlayer = memory.GetLocalPlayer();
    if (!localPlayer) {
        return;
    }

    // Find targets
    std::vector<Target> targets = FindTargets(localPlayer);
    if (targets.empty()) {
        return;
    }

    // Get best target
    Target* bestTarget = GetBestTarget(targets);
    if (bestTarget == nullptr) {
        return;
    }

    // Aim at target
    Cube2::Vector3 aimPos = g_Config.aimbot.targetHead ?
                             bestTarget->headPosition :
                             bestTarget->position;

    // Calculate aim angles
    float targetYaw, targetPitch;
    CalculateAngles(localPlayer->headPosition, aimPos, targetYaw, targetPitch);

    // Apply smoothing
    float newYaw, newPitch;
    if (g_Config.aimbot.smoothing) {
        SmoothAim(localPlayer->yaw, localPlayer->pitch,
                  targetYaw, targetPitch,
                  newYaw, newPitch);
    } else {
        newYaw = targetYaw;
        newPitch = targetPitch;
    }

    // Write new angles directly to local player
    memory.Write<float>(reinterpret_cast<uintptr_t>(&localPlayer->yaw), newYaw);
    memory.Write<float>(reinterpret_cast<uintptr_t>(&localPlayer->pitch), newPitch);
}

std::vector<AimbotInternal::Target> AimbotInternal::FindTargets(Cube2::Player* localPlayer) {
    using namespace CheatConfig;
    std::vector<Target> targets;

    // Get player count and entity list
    int32_t playerCount = memory.GetPlayerCount();
    if (playerCount <= 0 || playerCount > 32) {
        return targets;
    }

    Cube2::Player** entityList = memory.GetEntityList();
    if (!entityList) {
        return targets;
    }

    for (int i = 0; i < playerCount; i++) {
        Cube2::Player* player = entityList[i];
        if (!player || player == localPlayer) {
            continue;
        }

        // Skip invalid players
        if (player->health <= 0 || player->health > 100) {
            continue;
        }

        // Team check
        if (g_Config.aimbot.teamCheck && player->team == localPlayer->team && player->team != 0) {
            continue;
        }

        // Distance check
        float distance = localPlayer->position.Distance(player->position);
        if (distance > g_Config.aimbot.maxDistance) {
            continue;
        }

        // FOV check
        Cube2::Vector3 aimPos = g_Config.aimbot.targetHead ? player->headPosition : player->position;
        float fovDist = CalculateFOVDistance(localPlayer->headPosition, aimPos,
                                             localPlayer->yaw, localPlayer->pitch);

        if (fovDist > g_Config.aimbot.fov) {
            continue;
        }

        // Visibility check
        bool visible = true;
        if (g_Config.aimbot.visibleOnly) {
            visible = IsVisible(localPlayer, player->position);
        }

        // Add to targets
        Target target;
        target.player = player;
        target.position = player->position;
        target.headPosition = player->headPosition;
        target.distance = distance;
        target.fovDistance = fovDist;
        target.visible = visible;
        target.team = player->team;

        targets.push_back(target);
    }

    return targets;
}

AimbotInternal::Target* AimbotInternal::GetBestTarget(std::vector<Target>& targets) {
    if (targets.empty()) {
        return nullptr;
    }

    // Sort by FOV distance
    std::sort(targets.begin(), targets.end(), [](const Target& a, const Target& b) {
        return a.fovDistance < b.fovDistance;
    });

    for (auto& target : targets) {
        if (target.visible || !CheatConfig::g_Config.aimbot.visibleOnly) {
            return &target;
        }
    }

    return nullptr;
}

float AimbotInternal::CalculateFOVDistance(const Cube2::Vector3& from, const Cube2::Vector3& to,
                                           float yaw, float pitch) {
    float targetYaw, targetPitch;
    CalculateAngles(from, to, targetYaw, targetPitch);

    float deltaYaw = NormalizeAngle(targetYaw - yaw);
    float deltaPitch = NormalizeAngle(targetPitch - pitch);

    return sqrt(deltaYaw * deltaYaw + deltaPitch * deltaPitch);
}

void AimbotInternal::CalculateAngles(const Cube2::Vector3& from, const Cube2::Vector3& to,
                                     float& outYaw, float& outPitch) {
    Cube2::Vector3 delta;
    delta.x = to.x - from.x;
    delta.y = to.y - from.y;
    delta.z = to.z - from.z;

    float hyp = sqrt(delta.x * delta.x + delta.y * delta.y);

    outYaw = RAD2DEG(atan2(delta.y, delta.x));
    outPitch = RAD2DEG(atan2(delta.z, hyp));

    outYaw = NormalizeAngle(outYaw);
    outPitch = ClampAngle(outPitch);
}

void AimbotInternal::SmoothAim(float currentYaw, float currentPitch,
                               float targetYaw, float targetPitch,
                               float& outYaw, float& outPitch) {
    using namespace CheatConfig;

    float deltaYaw = NormalizeAngle(targetYaw - currentYaw);
    float deltaPitch = NormalizeAngle(targetPitch - currentPitch);

    float smoothFactor = g_Config.aimbot.smoothness;
    outYaw = currentYaw + (deltaYaw / smoothFactor);
    outPitch = currentPitch + (deltaPitch / smoothFactor);

    outYaw = NormalizeAngle(outYaw);
    outPitch = ClampAngle(outPitch);
}

bool AimbotInternal::IsVisible(Cube2::Player* localPlayer, const Cube2::Vector3& targetPos) {
    // Simplified visibility check
    return true;
}

bool AimbotInternal::IsKeyPressed(int key) {
    return (GetAsyncKeyState(key) & 0x8000) != 0;
}

float AimbotInternal::NormalizeAngle(float angle) {
    while (angle > 180.0f) angle -= 360.0f;
    while (angle < -180.0f) angle += 360.0f;
    return angle;
}

float AimbotInternal::ClampAngle(float angle) {
    if (angle > 89.0f) return 89.0f;
    if (angle < -89.0f) return -89.0f;
    return angle;
}
