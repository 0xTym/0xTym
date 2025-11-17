#pragma once
#include "DirectMemory.h"
#include "../Config.h"
#include "../GameStructures.h"
#include <vector>

class AimbotInternal {
private:
    DirectMemory& memory;

    struct Target {
        Cube2::Player* player;
        Cube2::Vector3 position;
        Cube2::Vector3 headPosition;
        float distance;
        float fovDistance;
        bool visible;
        int team;
    };

public:
    AimbotInternal(DirectMemory& mem);
    ~AimbotInternal();

    void Update();

private:
    std::vector<Target> FindTargets(Cube2::Player* localPlayer);
    Target* GetBestTarget(std::vector<Target>& targets);

    float CalculateFOVDistance(const Cube2::Vector3& from, const Cube2::Vector3& to,
                               float yaw, float pitch);
    void CalculateAngles(const Cube2::Vector3& from, const Cube2::Vector3& to,
                         float& outYaw, float& outPitch);

    void SmoothAim(float currentYaw, float currentPitch,
                   float targetYaw, float targetPitch,
                   float& outYaw, float& outPitch);

    bool IsVisible(Cube2::Player* localPlayer, const Cube2::Vector3& targetPos);
    bool IsKeyPressed(int key);

    // Math helpers
    float NormalizeAngle(float angle);
    float ClampAngle(float angle);
};
