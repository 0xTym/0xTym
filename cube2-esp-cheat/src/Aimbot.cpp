#include "../include/Aimbot.h"
#include <Windows.h>
#include <algorithm>
#include <limits>

#define PI 3.14159265358979323846f
#define DEG2RAD(x) ((x) * PI / 180.0f)
#define RAD2DEG(x) ((x) * 180.0f / PI)

Aimbot::Aimbot(MemoryReader& reader)
    : memoryReader(reader), gameBase(reader.GetBaseAddress()) {
}

Aimbot::~Aimbot() {
}

void Aimbot::Update(const Cube2::Player& localPlayer) {
    using namespace CheatConfig;

    if (!g_Config.aimbot.enabled) {
        return;
    }

    // Check if aim key is pressed
    if (!IsKeyPressed(g_Config.aimbot.aimKey)) {
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
    CalculateAngles(localPlayer.headPosition, aimPos, targetYaw, targetPitch);

    // Apply smoothing
    float newYaw, newPitch;
    if (g_Config.aimbot.smoothing) {
        SmoothAim(localPlayer.yaw, localPlayer.pitch,
                  targetYaw, targetPitch,
                  newYaw, newPitch);
    } else {
        newYaw = targetYaw;
        newPitch = targetPitch;
    }

    // Write new angles to game memory
    uintptr_t localPlayerPtr = memoryReader.Read<uintptr_t>(gameBase + Cube2::LOCAL_PLAYER_OFFSET);
    if (localPlayerPtr != 0) {
        memoryReader.Write<float>(localPlayerPtr + 0x006C, newYaw);    // Yaw offset
        memoryReader.Write<float>(localPlayerPtr + 0x0070, newPitch);  // Pitch offset
    }
}

std::vector<Aimbot::Target> Aimbot::FindTargets(const Cube2::Player& localPlayer) {
    using namespace CheatConfig;
    std::vector<Target> targets;

    // Read player count
    int32_t playerCount = memoryReader.Read<int32_t>(gameBase + Cube2::PLAYER_COUNT_OFFSET);
    if (playerCount <= 0 || playerCount > 32) {
        return targets;
    }

    // Read entity list
    uintptr_t entityListPtr = memoryReader.Read<uintptr_t>(gameBase + Cube2::ENTITY_LIST_OFFSET);
    if (entityListPtr == 0) {
        return targets;
    }

    for (int i = 0; i < playerCount; i++) {
        uintptr_t playerPtr = memoryReader.Read<uintptr_t>(entityListPtr + (i * sizeof(uintptr_t)));
        if (playerPtr == 0) {
            continue;
        }

        Cube2::Player player = {};
        if (!memoryReader.Read(playerPtr, player)) {
            continue;
        }

        // Skip invalid players
        if (player.health <= 0 || player.health > 100) {
            continue;
        }

        // Team check
        if (g_Config.aimbot.teamCheck && player.team == localPlayer.team && player.team != 0) {
            continue;
        }

        // Distance check
        float distance = localPlayer.position.Distance(player.position);
        if (distance > g_Config.aimbot.maxDistance) {
            continue;
        }

        // FOV check
        Cube2::Vector3 aimPos = g_Config.aimbot.targetHead ? player.headPosition : player.position;
        float fovDist = CalculateFOVDistance(localPlayer.headPosition, aimPos,
                                             localPlayer.yaw, localPlayer.pitch);

        if (fovDist > g_Config.aimbot.fov) {
            continue;
        }

        // Visibility check
        bool visible = true;
        if (g_Config.aimbot.visibleOnly) {
            visible = IsVisible(localPlayer, player.position);
        }

        // Add to targets
        Target target;
        target.playerPtr = playerPtr;
        target.position = player.position;
        target.headPosition = player.headPosition;
        target.distance = distance;
        target.fovDistance = fovDist;
        target.visible = visible;
        target.team = player.team;

        targets.push_back(target);
    }

    return targets;
}

Aimbot::Target* Aimbot::GetBestTarget(std::vector<Target>& targets) {
    if (targets.empty()) {
        return nullptr;
    }

    // Sort by FOV distance (closest to crosshair)
    std::sort(targets.begin(), targets.end(), [](const Target& a, const Target& b) {
        return a.fovDistance < b.fovDistance;
    });

    // Return best target (prioritize visible)
    for (auto& target : targets) {
        if (target.visible || !CheatConfig::g_Config.aimbot.visibleOnly) {
            return &target;
        }
    }

    return nullptr;
}

float Aimbot::CalculateFOVDistance(const Cube2::Vector3& from, const Cube2::Vector3& to,
                                   float yaw, float pitch) {
    float targetYaw, targetPitch;
    CalculateAngles(from, to, targetYaw, targetPitch);

    // Calculate angular difference
    float deltaYaw = NormalizeAngle(targetYaw - yaw);
    float deltaPitch = NormalizeAngle(targetPitch - pitch);

    // Return combined FOV distance
    return sqrt(deltaYaw * deltaYaw + deltaPitch * deltaPitch);
}

void Aimbot::CalculateAngles(const Cube2::Vector3& from, const Cube2::Vector3& to,
                             float& outYaw, float& outPitch) {
    Cube2::Vector3 delta;
    delta.x = to.x - from.x;
    delta.y = to.y - from.y;
    delta.z = to.z - from.z;

    float hyp = sqrt(delta.x * delta.x + delta.y * delta.y);

    // Calculate yaw (horizontal angle)
    outYaw = RAD2DEG(atan2(delta.y, delta.x));

    // Calculate pitch (vertical angle)
    outPitch = RAD2DEG(atan2(delta.z, hyp));

    // Normalize
    outYaw = NormalizeAngle(outYaw);
    outPitch = ClampAngle(outPitch);
}

void Aimbot::SmoothAim(float currentYaw, float currentPitch,
                       float targetYaw, float targetPitch,
                       float& outYaw, float& outPitch) {
    using namespace CheatConfig;

    float deltaYaw = NormalizeAngle(targetYaw - currentYaw);
    float deltaPitch = NormalizeAngle(targetPitch - currentPitch);

    // Apply smoothing
    float smoothFactor = g_Config.aimbot.smoothness;
    outYaw = currentYaw + (deltaYaw / smoothFactor);
    outPitch = currentPitch + (deltaPitch / smoothFactor);

    // Normalize
    outYaw = NormalizeAngle(outYaw);
    outPitch = ClampAngle(outPitch);
}

bool Aimbot::IsVisible(const Cube2::Player& localPlayer, const Cube2::Vector3& targetPos) {
    // Simplified visibility check
    // In a real implementation, you would perform ray-casting through the game's world geometry
    // For now, we just return true (assume visible)
    return true;
}

bool Aimbot::IsKeyPressed(int key) {
    return (GetAsyncKeyState(key) & 0x8000) != 0;
}

float Aimbot::NormalizeAngle(float angle) {
    while (angle > 180.0f) angle -= 360.0f;
    while (angle < -180.0f) angle += 360.0f;
    return angle;
}

float Aimbot::ClampAngle(float angle) {
    if (angle > 89.0f) return 89.0f;
    if (angle < -89.0f) return -89.0f;
    return angle;
}
