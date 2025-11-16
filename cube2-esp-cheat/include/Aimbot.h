#pragma once
#include "MemoryReader.h"
#include "GameStructures.h"
#include "Config.h"
#include <vector>
#include <cmath>

class Aimbot {
private:
    MemoryReader& memoryReader;
    uintptr_t gameBase;

    struct Target {
        uintptr_t playerPtr;
        Cube2::Vector3 position;
        Cube2::Vector3 headPosition;
        float distance;
        float fovDistance;
        bool visible;
        int team;
    };

public:
    Aimbot(MemoryReader& reader);
    ~Aimbot();

    void Update(const Cube2::Player& localPlayer);
    void AimAt(const Cube2::Vector3& targetPos, Cube2::Player& localPlayer);

private:
    std::vector<Target> FindTargets(const Cube2::Player& localPlayer);
    Target* GetBestTarget(std::vector<Target>& targets);

    float CalculateFOVDistance(const Cube2::Vector3& from, const Cube2::Vector3& to,
                               float yaw, float pitch);
    void CalculateAngles(const Cube2::Vector3& from, const Cube2::Vector3& to,
                         float& outYaw, float& outPitch);

    void SmoothAim(float currentYaw, float currentPitch,
                   float targetYaw, float targetPitch,
                   float& outYaw, float& outPitch);

    bool IsVisible(const Cube2::Player& localPlayer, const Cube2::Vector3& targetPos);
    bool IsKeyPressed(int key);

    // Math helpers
    float NormalizeAngle(float angle);
    float ClampAngle(float angle);
};
